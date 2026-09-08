#include <twl.h>
#include <twl/dsp.h>
#include <twl/dsp/common/pipe.h>

#include "dsp_coff.h"

#define DSP_DPRINTF(...) ((void)0)

typedef BOOL (*DSPSectionEnumCallback)(DSPProcessContext *,
                                       const COFFFileHeader *,
                                       const COFFSectionTable *);

static DSPProcessContext *DSPiCurrentComponent = NULL;
static PMExitCallbackInfo DSPiShutdownCallbackInfo[1];
static BOOL DSPiShutdownCallbackIsRegistered = FALSE;
static PMSleepCallbackInfo DSPiPreSleepCallbackInfo[1];
static BOOL DSPiPreSleepCallbackIsRegistered = FALSE;

BOOL DSP_EnumSections(DSPProcessContext *context,
                      DSPSectionEnumCallback callback);

void DSP_StopDSPComponent(void) {
  DSPProcessContext *context = DSPiCurrentComponent;
  context->hookFactors = 0;
  DSP_SendData(DSP_PIPE_COMMAND_REGISTER, DSP_PIPE_FLAG_EXIT_OS);
  (void)DSP_RecvData(DSP_PIPE_COMMAND_REGISTER);
}

static void DSPi_ShutdownCallback(void *arg) {
  (void)arg;
  for (;;) {
    DSPProcessContext *context = DSP_FindProcess(NULL);
    if (!context) {
      break;
    }

    DSP_QuitProcess(context);
  }
}

static void DSPi_PreSleepCallback(void *arg) {
#pragma unused(arg)
  OS_TPanic("Could not sleep while DSP is processing.\n");
}

static void DSPi_MasterInterrupts(void) {
  DSPProcessContext *context = DSPiCurrentComponent;
  if (context) {

    for (;;) {

      int ready = (reg_DSP_SEM |
                   (((reg_DSP_PSTS >> REG_DSP_PSTS_RRI0_SHIFT) & 7) << 16));
      int factors = (ready & context->hookFactors);
      if (factors == 0) {
        break;
      } else {

        while (factors != 0) {
          int index = (int)MATH_CTZ((u32)factors);
          factors &= ~context->hookGroup[index];
          (*context->hookFunction[index])(context->hookUserdata[index]);
        }
      }
    }
  }
  OS_SetIrqCheckFlag(OS_IE_DSP);
}

static BOOL DSPi_ReadProcessImage(DSPProcessContext *context, int offset,
                                  void *buffer, int length) {
  return FS_SeekFile(context->image, offset, FS_SEEK_SET) &&
         (FS_ReadFile(context->image, buffer, length) == length);
}

static BOOL DSPi_CommitWram(DSPProcessContext *context, MIWramPos wram,
                            int segment, MIWramProc to) {
  BOOL retval = FALSE;
  int slot = DSP_GetProcessSlotFromSegment(context, wram, segment);

  if (!MI_IsWramSlotUsed(wram, slot) ||
      MI_FreeWramSlot(wram, slot, MI_WRAM_SIZE_32KB,
                      MI_GetWramBankMaster(wram, slot)) > 0) {
    void *physicalAddr;

    vu8 *bank = &(
        (vu8 *)((wram == MI_WRAM_B) ? REG_MBK_B0_ADDR : REG_MBK_C0_ADDR))[slot];
    if (to == MI_WRAM_ARM9) {
      *bank = (u8)((*bank & ~MI_WRAM_OFFSET_MASK_B) |
                   (slot << MI_WRAM_OFFSET_SHIFT_B));
    } else if (to == MI_WRAM_DSP) {
      *bank = (u8)((*bank & ~MI_WRAM_OFFSET_MASK_B) |
                   (segment << MI_WRAM_OFFSET_SHIFT_B));
    }

    physicalAddr = (void *)MI_AllocWramSlot(wram, slot, MI_WRAM_SIZE_32KB, to);
    if (physicalAddr != 0) {
      retval = TRUE;
    }
  }
  return retval;
}

static BOOL
DSPi_MapAndLoadProcessImageCallback(DSPProcessContext *context,
                                    const COFFFileHeader *header,
                                    const COFFSectionTable *section) {
  BOOL retval = TRUE;

  enum {
    placement_kind_max = 2, // { CODE, DATA }
    placement_code_page_max = 2,
    placement_data_page_max = 2,
    placement_max = placement_code_page_max + placement_data_page_max
  };
  MIWramPos wrams[placement_max];
  int addrs[placement_max];
  BOOL nolds[placement_max];
  int placement = 0;

  const char *name = (const char *)section->Name;
  char longname[128];
  if (*(u32 *)name == 0) {
    u32 stringtable =
        header->PointerToSymbolTable + 0x12 * header->NumberOfSymbols;
    if (!DSPi_ReadProcessImage(context,
                               (int)(stringtable + *(u32 *)&section->Name[4]),
                               longname, sizeof(longname))) {
      retval = FALSE;
      return retval;
    }
    name = longname;
  }

  if (STD_CompareString(name, "SDK_USING_OS@d0") == 0) {
    context->flags |= DSP_PROCESS_FLAG_USING_OS;
  }

  if ((section->s_flags & COFF_SECTION_ATTR_MAPPED_IN_CODE_MEMORY) != 0) {
    int baseaddr = (int)(section->s_paddr * 2);
    BOOL noload =
        ((section->s_flags & COFF_SECTION_ATTR_NOLOAD_FOR_CODE_MEMORY) != 0);
    if (STD_StrStr(name, "@c0") != NULL) {
      wrams[placement] = MI_WRAM_B;
      addrs[placement] = baseaddr + DSP_WRAM_SLOT_SIZE * 4 * 0;
      nolds[placement] = noload;
      ++placement;
    }
    if (STD_StrStr(name, "@c1") != NULL) {
      wrams[placement] = MI_WRAM_B;
      addrs[placement] = baseaddr + DSP_WRAM_SLOT_SIZE * 4 * 1;
      nolds[placement] = noload;
      ++placement;
    }
  }

  if ((section->s_flags & COFF_SECTION_ATTR_MAPPED_IN_DATA_MEMORY) != 0) {
    int baseaddr = (int)(section->s_vaddr * 2);
    BOOL noload =
        ((section->s_flags & COFF_SECTION_ATTR_NOLOAD_FOR_DATA_MEMORY) != 0);
    if (STD_StrStr(name, "@d0") != NULL) {
      wrams[placement] = MI_WRAM_C;
      addrs[placement] = baseaddr + DSP_WRAM_SLOT_SIZE * 4 * 0;
      nolds[placement] = noload;
      ++placement;
    }
    if (STD_StrStr(name, "@d1") != NULL) {
      wrams[placement] = MI_WRAM_C;
      addrs[placement] = baseaddr + DSP_WRAM_SLOT_SIZE * 4 * 1;
      nolds[placement] = noload;
      ++placement;
    }
  }

  {
    int i;
    for (i = 0; i < placement; ++i) {
      MIWramPos wram = wrams[i];
      int dstofs = addrs[i];
      int length = (int)section->s_size;
      int srcofs = (int)section->s_scnptr;

      while (length > 0) {

        int ceil = MATH_ROUNDUP(dstofs + 1, DSP_WRAM_SLOT_SIZE);
        int curlen = MATH_MIN(length, ceil - dstofs);
        BOOL newmapped = FALSE;

        if (DSP_GetProcessSlotFromSegment(context, wram,
                                          dstofs / DSP_WRAM_SLOT_SIZE) == -1) {
          int segment = (dstofs / DSP_WRAM_SLOT_SIZE);
          u16 *slots = (wram == MI_WRAM_B) ? &context->slotB : &context->slotC;
          int *segbits = (wram == MI_WRAM_B) ? &context->segmentCode
                                             : &context->segmentData;
          int *map = (wram == MI_WRAM_B) ? context->slotOfSegmentCode
                                         : context->slotOfSegmentData;
          int slot = (int)MATH_CountTrailingZeros((u32)*slots);
          if (slot >= MI_WRAM_B_MAX_NUM) {
            retval = FALSE;
            break;
          } else {
            newmapped = TRUE;
            map[segment] = slot;
            *slots &= ~(1 << slot);
            *segbits |= (1 << segment);
            if (!DSPi_CommitWram(context, wram, segment, MI_WRAM_ARM9)) {
              retval = FALSE;
              break;
            }
          }
          MI_CpuFillFast(DSP_ConvertProcessAddressFromDSP(
                             context, wram, segment * (DSP_WRAM_SLOT_SIZE / 2)),
                         0, DSP_WRAM_SLOT_SIZE);
        }

        if (nolds[i]) {
          DSP_DPRINTF("$%04X (noload)\n", dstofs);
        } else {

          u8 *dstbuf =
              (u8 *)DSP_ConvertProcessAddressFromDSP(context, wram, dstofs / 2);
          if (!DSPi_ReadProcessImage(context, srcofs, dstbuf, curlen)) {
            retval = FALSE;
            break;
          }
          DSP_DPRINTF("$%04X -> mem:%08X\n", dstofs, dstbuf);
        }
        srcofs += curlen;
        dstofs += curlen;
        length -= curlen;
      }
    }
  }

  return retval;
}

void DSP_InitProcessContext(DSPProcessContext *context, const char *name) {
  int i;
  int segment;
  MI_CpuFill8(context, 0, sizeof(*context));
  context->next = NULL;
  context->flags = 0;
  (void)STD_CopyString(context->name, name ? name : "");
  context->image = NULL;

  context->segmentCode = 0;
  context->segmentData = 0;

  for (segment = 0; segment < MI_WRAM_B_MAX_NUM; ++segment) {
    context->slotOfSegmentCode[segment] = -1;
  }
  for (segment = 0; segment < MI_WRAM_C_MAX_NUM; ++segment) {
    context->slotOfSegmentData[segment] = -1;
  }

  context->hookFactors = 0;
  for (i = 0; i < DSP_HOOK_MAX; ++i) {
    context->hookFunction[i] = NULL;
    context->hookUserdata[i] = NULL;
    context->hookGroup[i] = 0;
  }
}

BOOL DSP_EnumSections(DSPProcessContext *context,
                      DSPSectionEnumCallback callback) {
  BOOL retval = FALSE;

  COFFFileHeader header[1];
  if (DSPi_ReadProcessImage(context, 0, header, sizeof(header))) {
    int base = (int)(sizeof(header) + header->SizeOfOptionalHeader);
    int index;
    for (index = 0; index < header->NumberOfSections; ++index) {
      COFFSectionTable section[1];
      if (!DSPi_ReadProcessImage(context, (int)(base + index * sizeof(section)),
                                 section, (int)sizeof(section))) {
        break;
      }

      if (((section->s_flags & COFF_SECTION_ATTR_BLOCK_HEADER) == 0) &&
          (section->s_size != 0)) {
        if (callback && !(*callback)(context, header, section)) {
          break;
        }
      }
    }
    retval = (index >= header->NumberOfSections);
  }
  return retval;
}

u32 DSP_AttachProcessMemory(DSPProcessContext *context, MIWramPos wram,
                            int slots) {
  u32 retval = 0;
  int *segbits =
      (wram == MI_WRAM_B) ? &context->segmentCode : &context->segmentData;
  int *map = (wram == MI_WRAM_B) ? context->slotOfSegmentCode
                                 : context->slotOfSegmentData;

  int need = (int)MATH_CountPopulation((u32)slots);
  u32 region = (u32)((1 << need) - 1);
  u32 space = (u32)(~*segbits & 0xFF);
  int segment = 0;
  for (segment = 0; segment < MI_WRAM_B_MAX_NUM; ++segment) {

    if ((((segment ^ (segment + need - 1)) & 4) == 0) &&
        (((space >> segment) & region) == region)) {

      retval = (u32)(DSP_ADDR_TO_DSP(DSP_WRAM_SLOT_SIZE) * segment);
      while (slots) {
        int slot = (int)MATH_CountTrailingZeros((u32)slots);
        map[segment] = slot;
        slots &= ~(1 << slot);
        *segbits |= (1 << segment);
        segment += 1;
      }
      break;
    }
  }
  return retval;
}

void DSP_DetachProcessMemory(DSPProcessContext *context, MIWramPos wram,
                             int slots) {
  int *segbits =
      (wram == MI_WRAM_B) ? &context->segmentCode : &context->segmentData;
  int *map = (wram == MI_WRAM_B) ? context->slotOfSegmentCode
                                 : context->slotOfSegmentData;
  int segment;
  for (segment = 0; segment < MI_WRAM_B_MAX_NUM; ++segment) {
    if ((((1 << segment) & *segbits) != 0) &&
        (((1 << map[segment]) & slots) != 0)) {
      *segbits &= ~(1 << segment);
      map[segment] = -1;
    }
  }
}

BOOL DSP_SwitchProcessMemory(DSPProcessContext *context, MIWramPos wram,
                             u32 address, u32 length, MIWramProc to) {
  address = DSP_ADDR_TO_ARM(address);
  length = DSP_ADDR_TO_ARM(MATH_MAX(length, 1));
  {
    int *segbits =
        (wram == MI_WRAM_B) ? &context->segmentCode : &context->segmentData;
    int lower = (int)(address / DSP_WRAM_SLOT_SIZE);
    int upper = (int)((address + length - 1) / DSP_WRAM_SLOT_SIZE);
    int segment;
    for (segment = lower; segment <= upper; ++segment) {
      if ((*segbits & (1 << segment)) != 0) {
        if (!DSPi_CommitWram(context, wram, segment, to)) {
          return FALSE;
        }
      }
    }
  }
  return TRUE;
}

static BOOL DSP_MapAndLoadProcessImage(DSPProcessContext *context,
                                       FSFile *image, int slotB, int slotC) {
  BOOL retval = FALSE;
  const u32 dspMemSize =
      DSP_ADDR_TO_DSP(DSP_WRAM_SLOT_SIZE) * MI_WRAM_B_MAX_NUM;

  context->image = image;
  context->slotB = (u16)slotB;
  context->slotC = (u16)slotC;

  if (DSP_EnumSections(context, DSPi_MapAndLoadProcessImageCallback)) {
    DC_FlushRange((const void *)MI_GetWramMapStart_B(), MI_WRAM_B_SIZE);
    DC_FlushRange((const void *)MI_GetWramMapStart_C(), MI_WRAM_C_SIZE);

    if (DSP_SwitchProcessMemory(context, MI_WRAM_B, 0, dspMemSize,
                                MI_WRAM_DSP) &&
        DSP_SwitchProcessMemory(context, MI_WRAM_C, 0, dspMemSize,
                                MI_WRAM_DSP)) {
      retval = TRUE;
    }
  }
  return retval;
}

void DSP_SetProcessHook(DSPProcessContext *context, int factors,
                        DSPHookFunction function, void *userdata) {
  OSIntrMode bak_cpsr = OS_DisableInterrupts();
  int i;
  for (i = 0; i < DSP_HOOK_MAX; ++i) {
    int bit = (1 << i);
    if ((bit & factors) != 0) {
      context->hookFunction[i] = function;
      context->hookUserdata[i] = userdata;
      context->hookGroup[i] = factors;
    }
  }
  {

    int modrep = (((factors >> 16) & 0x7) << REG_DSP_PCFG_PRIE0_SHIFT);
    int modsem = ((factors >> 0) & 0xFFFF);
    int currep = reg_DSP_PCFG;
    int cursem = reg_DSP_PMASK;
    if (function != NULL) {
      reg_DSP_PCFG = (u16)(currep | modrep);
      reg_DSP_PMASK = (u16)(cursem & ~modsem);
      context->hookFactors |= factors;
    } else {
      reg_DSP_PCFG = (u16)(currep & ~modrep);
      reg_DSP_PMASK = (u16)(cursem | modsem);
      context->hookFactors &= ~factors;
    }
  }
  (void)OS_RestoreInterrupts(bak_cpsr);
}

SDK_WEAK_SYMBOL void DSP_HookPostStartProcess(void) {}

BOOL DSP_ExecuteProcess(DSPProcessContext *context, FSFile *image, int slotB,
                        int slotC) {
  BOOL retval = FALSE;
  if (!FS_IsAvailable()) {
    OS_TWarning("FS is not initialized yet.\n");
    FS_Init(FS_DMA_NOT_USE);
  }

  DSP_InitPipe();
  OS_SetIrqFunction(OS_IE_DSP, DSPi_MasterInterrupts);
  DSP_SetProcessHook(context, DSP_HOOK_SEMAPHORE_(15) | DSP_HOOK_REPLY_(2),
                     (DSPHookFunction)DSP_HookPipeNotification, NULL);
  (void)OS_EnableIrqMask(OS_IE_DSP);

  if (!DSP_MapAndLoadProcessImage(context, image, slotB, slotC)) {
    OS_TWarning("you should check wram\n");
  } else {
    OSIntrMode bak_cpsr = OS_DisableInterrupts();

    DSPProcessContext **pp = &DSPiCurrentComponent;
    for (pp = &DSPiCurrentComponent; *pp && (*pp != context);
         pp = &(*pp)->next) {
    }
    *pp = context;
    context->image = NULL;

    if (!DSPiShutdownCallbackIsRegistered) {
      PM_SetExitCallbackInfo(DSPiShutdownCallbackInfo, DSPi_ShutdownCallback,
                             NULL);
      PMi_InsertPostExitCallbackEx(DSPiShutdownCallbackInfo,
                                   PM_CALLBACK_PRIORITY_DSP);
      DSPiShutdownCallbackIsRegistered = TRUE;
    }

    if (!DSPiPreSleepCallbackIsRegistered) {
      PM_SetSleepCallbackInfo(DSPiPreSleepCallbackInfo, DSPi_PreSleepCallback,
                              NULL);
      PMi_InsertPreSleepCallbackEx(DSPiPreSleepCallbackInfo,
                                   PM_CALLBACK_PRIORITY_DSP);
      DSPiPreSleepCallbackIsRegistered = TRUE;
    }

    DSP_PowerOn();
    DSP_ResetOffEx((u16)(context->hookFactors >> 16));
    DSP_MaskSemaphore((u16) ~(context->hookFactors >> 0));

    DSP_HookPostStartProcess();

    if ((context->flags & DSP_PROCESS_FLAG_USING_OS) != 0) {
      u16 id;
      for (id = 0; id < 3; ++id) {
        vu16 dummy;
        while (dummy = DSP_RecvDataCore(id), dummy != 1) {
        }
      }
    }

    DSPi_MasterInterrupts();
    retval = TRUE;
    (void)OS_RestoreInterrupts(bak_cpsr);
  }
  return retval;
}

void DSP_QuitProcess(DSPProcessContext *context) {
  OSIntrMode bak_cpsr;

  DSP_StopDSPComponent();

  bak_cpsr = OS_DisableInterrupts();

  DSP_ResetOn();
  DSP_PowerOff();

  (void)OS_DisableIrqMask(OS_IE_DSP);
  OS_SetIrqFunction(OS_IE_DSP, NULL);

  (void)MI_FreeWram(MI_WRAM_B, MI_WRAM_DSP);
  (void)MI_FreeWram(MI_WRAM_C, MI_WRAM_DSP);
  {

    DSPProcessContext **pp = &DSPiCurrentComponent;
    for (pp = &DSPiCurrentComponent; *pp; pp = &(*pp)->next) {
      if (*pp == context) {
        *pp = (*pp)->next;
        break;
      }
    }
    context->next = NULL;
  }
  (void)context;
  (void)OS_RestoreInterrupts(bak_cpsr);

  PM_DeletePreSleepCallback(DSPiPreSleepCallbackInfo);
  DSPiPreSleepCallbackIsRegistered = FALSE;
}

DSPProcessContext *DSP_FindProcess(const char *name) {
  DSPProcessContext *ptr = NULL;
  OSIntrMode bak_cpsr = OS_DisableInterrupts();
  ptr = DSPiCurrentComponent;
  if (name) {
    for (; ptr && (STD_CompareString(ptr->name, name) != 0); ptr = ptr->next) {
    }
  }
  (void)OS_RestoreInterrupts(bak_cpsr);
  return ptr;
}

void DSP_ReadProcessPipe(DSPProcessContext *context, int port, void *buffer,
                         u32 length) {
  DSPPipe input[1];
  (void)DSP_LoadPipe(input, port, DSP_PIPE_INPUT);
  DSP_ReadPipe(input, buffer, (DSPByte)length);
  (void)context;
}

void DSP_WriteProcessPipe(DSPProcessContext *context, int port,
                          const void *buffer, u32 length) {
  DSPPipe output[1];
  (void)DSP_LoadPipe(output, port, DSP_PIPE_OUTPUT);
  DSP_WritePipe(output, buffer, (DSPByte)length);
  (void)context;
}

#if defined(DSP_SUPPORT_OBSOLETE_LOADER)

static BOOL DSPi_MapProcessSegmentCallback(DSPProcessContext *context,
                                           const COFFFileHeader *header,
                                           const COFFSectionTable *section) {
  (void)header;

  if ((section->s_flags & COFF_SECTION_ATTR_MAPPED_IN_CODE_MEMORY) != 0) {
    u32 addr = DSP_ADDR_TO_ARM(section->s_paddr);
    int lower = (int)(addr / DSP_WRAM_SLOT_SIZE);
    int upper = (int)((addr + section->s_size - 1) / DSP_WRAM_SLOT_SIZE);
    int segment;
    for (segment = lower; segment <= upper; ++segment) {
      context->segmentCode |= (1 << segment);
    }
  } else if ((section->s_flags & COFF_SECTION_ATTR_MAPPED_IN_DATA_MEMORY) !=
             0) {
    u32 addr = DSP_ADDR_TO_ARM(section->s_vaddr);
    int lower = (int)(addr / DSP_WRAM_SLOT_SIZE);
    int upper = (int)((addr + section->s_size - 1) / DSP_WRAM_SLOT_SIZE);
    int segment;
    for (segment = lower; segment <= upper; ++segment) {
      context->segmentData |= (1 << segment);
    }
  }
  return TRUE;
}

static BOOL DSPi_MapProcessSlotDefault(DSPProcessContext *context, int slotB,
                                       int slotC) {
  BOOL retval = TRUE;
  int segment;
  for (segment = 0; segment < MI_WRAM_B_MAX_NUM; ++segment) {
    if ((context->segmentCode & (1 << segment)) != 0) {
      int slot = (int)MATH_CountTrailingZeros((u32)slotB);
      if (slot >= MI_WRAM_B_MAX_NUM) {
        retval = FALSE;
        break;
      }
      context->slotOfSegmentCode[segment] = slot;
      slotB &= ~(1 << slot);
    }
  }
  for (segment = 0; segment < MI_WRAM_C_MAX_NUM; ++segment) {
    if ((context->segmentData & (1 << segment)) != 0) {
      int slot = (int)MATH_CountTrailingZeros((u32)slotC);
      if (slot >= MI_WRAM_C_MAX_NUM) {
        retval = FALSE;
        break;
      }
      context->slotOfSegmentData[segment] = slot;
      slotC &= ~(1 << slot);
    }
  }
  return retval;
}

static BOOL DSP_IsProcessMemoryReady(DSPProcessContext *context) {
  BOOL retval = TRUE;
  int segment;
  for (segment = 0; segment < MI_WRAM_B_MAX_NUM; ++segment) {
    if ((context->segmentCode & (1 << segment)) != 0) {
      int slot = context->slotOfSegmentCode[segment];
      if (MI_IsWramSlotUsed(MI_WRAM_B, slot)) {
        OS_TWarning("slot:%d for DSP:%05X is now used by someone.\n", slot,
                    segment * DSP_WRAM_SLOT_SIZE);
        retval = FALSE;
        break;
      }
    }
  }
  for (segment = 0; segment < MI_WRAM_C_MAX_NUM; ++segment) {
    if ((context->segmentData & (1 << segment)) != 0) {
      int slot = context->slotOfSegmentData[segment];
      if (MI_IsWramSlotUsed(MI_WRAM_C, slot)) {
        OS_TWarning("slot:%d for DSP:%05X is now used by someone.\n", slot,
                    segment * DSP_WRAM_SLOT_SIZE);
        retval = FALSE;
        break;
      }
    }
  }
  return retval;
}

static BOOL DSPi_LoadProcessImageCallback(DSPProcessContext *context,
                                          const COFFFileHeader *header,
                                          const COFFSectionTable *section) {
  BOOL retval = TRUE;
  MIWramPos wram = MI_WRAM_A;
  int dstofs = 0;
  BOOL noload = FALSE;
  (void)header;

  if ((section->s_flags & COFF_SECTION_ATTR_MAPPED_IN_CODE_MEMORY) != 0) {
    wram = MI_WRAM_B;
    dstofs = (int)(section->s_paddr * 2);
    if ((section->s_flags & COFF_SECTION_ATTR_NOLOAD_FOR_CODE_MEMORY) != 0) {
      noload = TRUE;
    }
  }

  else if ((section->s_flags & COFF_SECTION_ATTR_MAPPED_IN_DATA_MEMORY) != 0) {
    wram = MI_WRAM_C;
    dstofs = (int)(section->s_vaddr * 2);
    if ((section->s_flags & COFF_SECTION_ATTR_NOLOAD_FOR_DATA_MEMORY) != 0) {
      noload = TRUE;
    }
  }

  if (wram != MI_WRAM_A) {

    if (noload) {
      DSP_DPRINTF("$%04X (noload)\n", dstofs);
    } else {

      int length = (int)section->s_size;
      int srcofs = (int)section->s_scnptr;
      while (length > 0) {

        int ceil = MATH_ROUNDUP(dstofs + 1, DSP_WRAM_SLOT_SIZE);
        int curlen = MATH_MIN(length, ceil - dstofs);

        u8 *dstbuf =
            (u8 *)DSP_ConvertProcessAddressFromDSP(context, wram, dstofs / 2);
        if (!DSPi_ReadProcessImage(context, srcofs, dstbuf, length)) {
          retval = FALSE;
          break;
        }
        DSP_DPRINTF("$%04X -> mem:%08X\n", dstofs, dstbuf);
        srcofs += curlen;
        dstofs += curlen;
        length -= curlen;
      }
    }
  }
  return retval;
}

void DSP_MapProcessSegment(DSPProcessContext *context) {
  (void)DSP_EnumSections(context, DSPi_MapProcessSegmentCallback);
}

BOOL DSP_LoadProcessImage(DSPProcessContext *context) {
  BOOL retval = FALSE;

  if (DSP_IsProcessMemoryReady(context)) {
    const u32 dspMemSize =
        DSP_ADDR_TO_DSP(DSP_WRAM_SLOT_SIZE) * MI_WRAM_B_MAX_NUM;

    if (DSP_SwitchProcessMemory(context, MI_WRAM_B, 0, dspMemSize,
                                MI_WRAM_ARM9) &&
        DSP_SwitchProcessMemory(context, MI_WRAM_C, 0, dspMemSize,
                                MI_WRAM_ARM9)) {

      if (DSP_EnumSections(context, DSPi_LoadProcessImageCallback)) {
        DC_FlushRange((const void *)MI_GetWramMapStart_B(), MI_WRAM_B_SIZE);
        DC_FlushRange((const void *)MI_GetWramMapStart_C(), MI_WRAM_C_SIZE);

        if (DSP_SwitchProcessMemory(context, MI_WRAM_B, 0, dspMemSize,
                                    MI_WRAM_DSP) &&
            DSP_SwitchProcessMemory(context, MI_WRAM_C, 0, dspMemSize,
                                    MI_WRAM_DSP)) {
          retval = TRUE;
        }
      }
    }
  }
  return retval;
}

BOOL DSP_StartupProcess(DSPProcessContext *context, FSFile *image, int slotB,
                        int slotC,
                        BOOL (*slotMapper)(DSPProcessContext *, int, int)) {
  BOOL retval = FALSE;
  if (!slotMapper) {
    slotMapper = DSPi_MapProcessSlotDefault;
  }
  if (!FS_IsAvailable()) {
    OS_TWarning("FS is not initialized yet.\n");
    FS_Init(FS_DMA_NOT_USE);
  }
  context->image = image;
  DSP_MapProcessSegment(context);
  if (!(*slotMapper)(context, slotB, slotC) || !DSP_LoadProcessImage(context)) {
    OS_TWarning("you should check wram\n");
  } else {
    retval = TRUE;
  }
  context->image = NULL;
  return retval;
}

#endif

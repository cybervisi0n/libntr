#include <twl.h>
#include <twl/dsp.h>

#include <dsp_coff.h>
#include "dsp_process.h"

#define reg_CFG_DSP_RST *(vu8 *)REG_RST_ADDR

#define REG_DSP_PCFG_RRIE_MASK                                                 \
  (REG_DSP_PCFG_PRIE0_MASK | REG_DSP_PCFG_PRIE1_MASK | REG_DSP_PCFG_PRIE2_MASK)
#define REG_DSP_PCFG_RRIE_SHIFT REG_DSP_PCFG_PRIE0_SHIFT
#define REG_DSP_PSTS_RCOMIM_SHIFT REG_DSP_PSTS_RCOMIM0_SHIFT
#define REG_DSP_PSTS_RRI_SHIFT REG_DSP_PSTS_RRI0_SHIFT

#define DSP_DPRINTF(...) ((void)0)

typedef struct DSPData {
  u16 send;
  u16 reserve1;
  u16 recv;
  u16 reserve2;
} DSPData;

static volatile DSPData *const dspData = (DSPData *)REG_COM0_ADDR;

void DSP_PowerOnCore(void) // DSP_Init
{
  SCFG_ResetDSP();             // Confirms reset of DSP block
  SCFG_SupplyClockToDSP(TRUE); // Power on for DSP block
  OS_SpinWaitSysCycles(2);     // Wait 8 cycles @ 134 MHz
  SCFG_ReleaseResetDSP();      // Release reset of DSP block
  DSP_ResetOnCore();           // Set reset of DSP core
}

void DSP_PowerOffCore(void) // DSP_End
{
  SCFG_ResetDSP();              // Sets reset of DSP block
  SCFG_SupplyClockToDSP(FALSE); // Power off for DSP block
}

void DSP_ResetOnCore(void) {
  OS_SpinWaitSysCycles(4);
  if ((reg_DSP_PCFG & REG_DSP_PCFG_DSPR_MASK) == 0) {
    reg_DSP_PCFG |= REG_DSP_PCFG_DSPR_MASK;
    OS_SpinWaitSysCycles(4);
    while (reg_DSP_PSTS & REG_DSP_PSTS_PRST_MASK) {
    }
  }
}

void DSP_ResetOffCore(void) {
  OS_SpinWaitSysCycles(4);
  while (reg_DSP_PSTS & REG_DSP_PSTS_PRST_MASK) {
  }
  DSP_ResetInterfaceCore(); // Initialize DSP-A9IF
  OS_SpinWaitSysCycles(4);
  reg_DSP_PCFG &= ~REG_DSP_PCFG_DSPR_MASK;
}

void DSP_ResetOffExCore(u16 bitmap) {
  SDK_ASSERT(bitmap >= 0 && bitmap <= 7);

  OS_SpinWaitSysCycles(4);
  while (reg_DSP_PSTS & REG_DSP_PSTS_PRST_MASK) {
  }
  DSP_ResetInterfaceCore(); // Initialize DSP-A9IF
  OS_SpinWaitSysCycles(4);
  reg_DSP_PCFG |= (bitmap) << REG_DSP_PCFG_RRIE_SHIFT;
  OS_SpinWaitSysCycles(4);
  reg_DSP_PCFG &= ~REG_DSP_PCFG_DSPR_MASK;
}

void DSP_ResetInterfaceCore(void) {
  OS_SpinWaitSysCycles(4);
  if (reg_DSP_PCFG & REG_DSP_PCFG_DSPR_MASK) {
    u16 dummy;
    reg_DSP_PCFG &= ~REG_DSP_PCFG_RRIE_MASK;
    reg_DSP_PSEM = 0;
    reg_DSP_PCLEAR = 0xFFFF;
    dummy = dspData[0].recv;
    dummy = dspData[1].recv;
    dummy = dspData[2].recv;
  }
}

void DSP_EnableRecvDataInterruptCore(u32 dataNo) {
  SDK_ASSERT(dataNo >= 0 && dataNo <= 2);
  OS_SpinWaitSysCycles(4);
  reg_DSP_PCFG |= (1 << dataNo) << REG_DSP_PCFG_RRIE_SHIFT;
}

void DSP_DisableRecvDataInterruptCore(u32 dataNo) {
  SDK_ASSERT(dataNo >= 0 && dataNo <= 2);
  OS_SpinWaitSysCycles(4);
  reg_DSP_PCFG &= ~((1 << dataNo) << REG_DSP_PCFG_RRIE_SHIFT);
}

BOOL DSP_SendDataIsEmptyCore(u32 dataNo) {
  SDK_ASSERT(dataNo >= 0 && dataNo <= 2);
  OS_SpinWaitSysCycles(4);
  return (reg_DSP_PSTS & ((1 << dataNo) << REG_DSP_PSTS_RCOMIM_SHIFT)) ? FALSE
                                                                       : TRUE;
}

BOOL DSP_RecvDataIsReadyCore(u32 dataNo) {
  SDK_ASSERT(dataNo >= 0 && dataNo <= 2);
  OS_SpinWaitSysCycles(4);
  return (reg_DSP_PSTS & ((1 << dataNo) << REG_DSP_PSTS_RRI_SHIFT)) ? TRUE
                                                                    : FALSE;
}

void DSP_SendDataCore(u32 dataNo, u16 data) {
  SDK_ASSERT(dataNo >= 0 && dataNo <= 2);
  OS_SpinWaitSysCycles(4);
  while (reg_DSP_PSTS & ((1 << dataNo) << REG_DSP_PSTS_RCOMIM_SHIFT)) {
  }
  dspData[dataNo].send = data;
}

u16 DSP_RecvDataCore(u32 dataNo) {
  SDK_ASSERT(dataNo >= 0 && dataNo <= 2);
  OS_SpinWaitSysCycles(4);
  while (!(reg_DSP_PSTS & ((1 << dataNo) << REG_DSP_PSTS_RRI_SHIFT))) {
  }
  return dspData[dataNo].recv;
}

void DSP_EnableFifoInterruptCore(DSPFifoIntr type) {
  OS_SpinWaitSysCycles(4);
  reg_DSP_PCFG |= type;
}

void DSP_DisableFifoInterruptCore(DSPFifoIntr type) {
  OS_SpinWaitSysCycles(4);
  reg_DSP_PCFG &= ~type;
}

void DSP_SendFifoExCore(DSPFifoMemSel memsel, u16 dest, const u16 *src,
                        int size, u16 flags) {
  OSIntrMode bak = OS_DisableInterrupts();
  u16 incmode =
      (u16)((flags & DSP_FIFO_FLAG_DEST_FIX) ? 0 : REG_DSP_PCFG_AIM_MASK);

  OS_SpinWaitSysCycles(4);
  reg_DSP_PCFG = (u16)((reg_DSP_PCFG &
                        ~(REG_DSP_PCFG_MEMSEL_MASK | REG_DSP_PCFG_AIM_MASK)) |
                       memsel | incmode);
  reg_DSP_PADR = dest;

  if (flags & DSP_FIFO_FLAG_SRC_FIX) {
    while (size-- > 0) {
      OS_SpinWaitSysCycles(4);
      while (reg_DSP_PSTS & REG_DSP_PSTS_WFFI_MASK) {
      }
      reg_DSP_PDATA = *src;
    }
  } else {
    while (size-- > 0) {
      OS_SpinWaitSysCycles(4);
      while (reg_DSP_PSTS & REG_DSP_PSTS_WFFI_MASK) {
      }
      reg_DSP_PDATA = *src++;
    }
  }
  (void)OS_RestoreInterrupts(bak);
}

void DSP_RecvFifoExCore(DSPFifoMemSel memsel, u16 *dest, u16 src, int size,
                        u16 flags) {
  OSIntrMode bak = OS_DisableInterrupts();
  DSPFifoRecvLength len;
  u16 incmode =
      (u16)((flags & DSP_FIFO_FLAG_SRC_FIX) ? 0 : REG_DSP_PCFG_AIM_MASK);

  SDK_ASSERT(memsel != DSP_FIFO_MEMSEL_PROGRAM);

  switch (flags & DSP_FIFO_FLAG_RECV_MASK) {
  case DSP_FIFO_FLAG_RECV_UNIT_2B:
    len = DSP_FIFO_RECV_2B;
    size = 1;
    break;
  case DSP_FIFO_FLAG_RECV_UNIT_16B:
    len = DSP_FIFO_RECV_16B;
    size = 8;
    break;
  case DSP_FIFO_FLAG_RECV_UNIT_32B:
    len = DSP_FIFO_RECV_32B;
    size = 16;
    break;
  default:
    len = DSP_FIFO_RECV_CONTINUOUS;
    break;
  }

  reg_DSP_PADR = src;
  OS_SpinWaitSysCycles(4);
  reg_DSP_PCFG =
      (u16)((reg_DSP_PCFG & ~(REG_DSP_PCFG_MEMSEL_MASK | REG_DSP_PCFG_DRS_MASK |
                              REG_DSP_PCFG_AIM_MASK)) |
            memsel | len | incmode | REG_DSP_PCFG_RS_MASK);

  if (flags & DSP_FIFO_FLAG_DEST_FIX) {
    while (size-- > 0) {
      OS_SpinWaitSysCycles(4);
      while ((reg_DSP_PSTS & REG_DSP_PSTS_RFNEI_MASK) == 0) {
      }
      *dest = reg_DSP_PDATA;
    }
  } else {
    while (size-- > 0) {
      OS_SpinWaitSysCycles(4);
      while ((reg_DSP_PSTS & REG_DSP_PSTS_RFNEI_MASK) == 0) {
      }
      *dest++ = reg_DSP_PDATA;
    }
  }
  OS_SpinWaitSysCycles(4);
  reg_DSP_PCFG &= ~REG_DSP_PCFG_RS_MASK;
  (void)OS_RestoreInterrupts(bak);
}

void DSP_SetCommandRegCore(u32 regNo, u16 data) {
  SDK_ASSERT(regNo >= 0 && regNo <= 2);
  OS_SpinWaitSysCycles(4);
  dspData[regNo].send = data;
}

u16 DSP_GetReplyRegCore(u32 regNo) {
  SDK_ASSERT(regNo >= 0 && regNo <= 2);
  OS_SpinWaitSysCycles(4);
  return dspData[regNo].recv;
}

void DSP_SetSemaphoreCore(u16 mask) { reg_DSP_PSEM = mask; }

u16 DSP_GetSemaphoreCore(void) {
  OS_SpinWaitSysCycles(4);
  return reg_DSP_SEM;
}

void DSP_ClearSemaphoreCore(u16 mask) { reg_DSP_PCLEAR = mask; }

void DSP_MaskSemaphoreCore(u16 mask) { reg_DSP_PMASK = mask; }

BOOL DSP_CheckSemaphoreRequestCore(void) {
  return (reg_DSP_PSTS & REG_DSP_PSTS_PSEMI_MASK) >> REG_DSP_PSTS_PSEMI_SHIFT;
}

#if defined(DSP_SUPPORT_OBSOLETE_LOADER)

static BOOL DSPi_MapProcessSlotAsStraight(DSPProcessContext *context, int slotB,
                                          int slotC) {
  int segment;
  for (segment = 0; segment < MI_WRAM_B_MAX_NUM; ++segment) {
    if (context->segmentCode & (1 << segment) != 0) {
      int slot = segment;
      if ((slotB & (1 << slot)) == 0) {
        return FALSE;
      }
      context->slotOfSegmentCode[segment] = slot;
    }
  }
  for (segment = 0; segment < MI_WRAM_C_MAX_NUM; ++segment) {
    if (context->segmentData & (1 << segment) != 0) {
      int slot = segment;
      if ((slotC & (1 << slot)) == 0) {
        return FALSE;
      }
      context->slotOfSegmentData[segment] = slot;
    }
  }
  return TRUE;
}

BOOL DSP_LoadFileAutoCore(const void *image) {

  FSFile memfile[1];
  if (DSPi_CreateMemoryFile(memfile, image)) {
    DSPProcessContext context[1];
    DSP_InitProcessContext(context, NULL);
    return DSP_StartupProcess(context, memfile, 0xFF, 0xFF,
                              DSPi_MapProcessSlotAsStraight);
  }
  return FALSE;
}

#endif

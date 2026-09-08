#ifndef TWL_DSP_PROCESS_H_
#define TWL_DSP_PROCESS_H_

#ifdef SDK_TWL

#include <twl/dsp.h>
#include <twl/dsp/common/byteaccess.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DSP_WRAM_SLOT_SIZE (32 * 1024)

#define DSP_HOOK_SEMAPHORE_(id) (1 << (id))
#define DSP_HOOK_REPLY_(id) (1 << (16 + (id)))
#define DSP_HOOK_MAX (16 + 3)

#define DSP_PROCESS_FLAG_USING_OS 0x0001

typedef void (*DSPHookFunction)(void *userdata);

typedef struct DSPProcessContext {

  struct DSPProcessContext *next;
  char name[15 + 1];

  FSFile *image;
  u16 slotB;
  u16 slotC;
  int flags;

  int segmentCode;
  int segmentData;

  int slotOfSegmentCode[8];
  int slotOfSegmentData[8];

  int hookFactors;
  DSPHookFunction hookFunction[DSP_HOOK_MAX];
  void *hookUserdata[DSP_HOOK_MAX];
  int hookGroup[DSP_HOOK_MAX];
} DSPProcessContext;

void DSP_StopDSPComponent(void);

void DSP_InitProcessContext(DSPProcessContext *context, const char *name);

BOOL DSP_ExecuteProcess(DSPProcessContext *context, FSFile *image, int slotB,
                        int slotC);

void DSP_QuitProcess(DSPProcessContext *context);

DSPProcessContext *DSP_FindProcess(const char *name);

SDK_INLINE int DSP_GetProcessSlotFromSegment(const DSPProcessContext *context,
                                             MIWramPos wram, int segment) {
  return (wram == MI_WRAM_B) ? context->slotOfSegmentCode[segment]
                             : context->slotOfSegmentData[segment];
}

SDK_INLINE void *
DSP_ConvertProcessAddressFromDSP(const DSPProcessContext *context,
                                 MIWramPos wram, int address) {
  int segment = (address / (DSP_WRAM_SLOT_SIZE / 2));
  int mod = (address - segment * (DSP_WRAM_SLOT_SIZE / 2));
  int slot = DSP_GetProcessSlotFromSegment(context, wram, segment);
  SDK_ASSERT((slot >= 0) && (slot < MI_WRAM_B_MAX_NUM));
  return (u8 *)MI_GetWramMapStart(wram) + slot * DSP_WRAM_SLOT_SIZE + mod * 2;
}

u32 DSP_AttachProcessMemory(DSPProcessContext *context, MIWramPos wram,
                            int slots);

void DSP_DetachProcessMemory(DSPProcessContext *context, MIWramPos wram,
                             int slots);

BOOL DSP_SwitchProcessMemory(DSPProcessContext *context, MIWramPos wram,
                             u32 address, u32 length, MIWramProc to);

void DSP_SetProcessHook(DSPProcessContext *context, int factors,
                        DSPHookFunction function, void *userdata);

SDK_INLINE BOOL DSPi_CreateMemoryFile(FSFile *memfile, const void *image) {
  if (!FS_IsAvailable()) {
    OS_TWarning("FS is not initialized yet.\n");
    FS_Init(FS_DMA_NOT_USE);
  }
  FS_InitFile(memfile);
  return FS_CreateFileFromMemory(memfile, (void *)image,
                                 (u32)(16 * 1024 * 1024));
}

void DSP_ReadProcessPipe(DSPProcessContext *context, int port, void *buffer,
                         u32 length);

void DSP_WriteProcessPipe(DSPProcessContext *context, int port,
                          const void *buffer, u32 length);

#if defined(DSP_SUPPORT_OBSOLETE_LOADER)

void DSP_MapProcessSegment(DSPProcessContext *context);

BOOL DSP_LoadProcessImage(DSPProcessContext *context);

BOOL DSP_StartupProcess(DSPProcessContext *context, FSFile *image, int slotB,
                        int slotC,
                        BOOL (*slotMapper)(DSPProcessContext *, int, int));

#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // SDK_TWL

#endif // TWL_DSP_PROCESS_H_

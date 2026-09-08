#ifndef TWL_DSP_IF_H_
#define TWL_DSP_IF_H_

#include <twl/types.h>
#include <twl/hw/ARM9/ioreg_DSP.h>
#include <nitro/os/common/emulator.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  DSP_FIFO_MEMSEL_DATA = (0x0 << REG_DSP_PCFG_MEMSEL_SHIFT),
  DSP_FIFO_MEMSEL_MMIO = (0x1 << REG_DSP_PCFG_MEMSEL_SHIFT),
  DSP_FIFO_MEMSEL_PROGRAM = (0x5 << REG_DSP_PCFG_MEMSEL_SHIFT)
} DSPFifoMemSel;

typedef enum {
  DSP_FIFO_RECV_2B = (0x0 << REG_DSP_PCFG_DRS_SHIFT),
  DSP_FIFO_RECV_16B = (0x1 << REG_DSP_PCFG_DRS_SHIFT),
  DSP_FIFO_RECV_32B = (0x2 << REG_DSP_PCFG_DRS_SHIFT),
  DSP_FIFO_RECV_CONTINUOUS = (0x3 << REG_DSP_PCFG_DRS_SHIFT)
} DSPFifoRecvLength;

typedef enum {
  DSP_FIFO_INTR_SEND_EMPTY = REG_DSP_PCFG_WFEIE_MASK,
  DSP_FIFO_INTR_SEND_FULL = REG_DSP_PCFG_WFFIE_MASK,
  DSP_FIFO_INTR_RECV_NOT_EMPTY = REG_DSP_PCFG_RFNEIE_MASK,
  DSP_FIFO_INTR_RECV_FULL = REG_DSP_PCFG_RFFIE_MASK
} DSPFifoIntr;

typedef enum {
  DSP_FIFO_FLAG_SRC_INC = (0UL << 0),
  DSP_FIFO_FLAG_SRC_FIX = (1UL << 0),

  DSP_FIFO_FLAG_DEST_INC = (0UL << 1),
  DSP_FIFO_FLAG_DEST_FIX = (1UL << 1),

  DSP_FIFO_FLAG_RECV_UNIT_CONTINUOUS = (0UL << 8),
  DSP_FIFO_FLAG_RECV_UNIT_2B = (1UL << 8),
  DSP_FIFO_FLAG_RECV_UNIT_16B = (2UL << 8),
  DSP_FIFO_FLAG_RECV_UNIT_32B = (3UL << 8),
  DSP_FIFO_FLAG_RECV_MASK = (3UL << 8)
} DSPFifoFlag;

#define DSP_SUPPORT_OBSOLETE_LOADER

void DSP_PowerOnCore(void);
SDK_INLINE void DSP_PowerOn(void) {
  if (OS_IsRunOnTwl() == TRUE) {
    DSP_PowerOnCore();
  }
}

void DSP_PowerOffCore(void);
SDK_INLINE void DSP_PowerOff(void) {
  if (OS_IsRunOnTwl() == TRUE) {
    DSP_PowerOffCore();
  }
}

void DSP_ResetOnCore(void);
SDK_INLINE void DSP_ResetOn(void) {
  if (OS_IsRunOnTwl() == TRUE) {
    DSP_ResetOnCore();
  }
}

void DSP_ResetOffCore(void);
SDK_INLINE void DSP_ResetOff(void) {
  if (OS_IsRunOnTwl() == TRUE) {
    DSP_ResetOffCore();
  }
}

void DSP_ResetOffExCore(u16 bitmap);
SDK_INLINE void DSP_ResetOffEx(u16 bitmap) {
  if (OS_IsRunOnTwl() == TRUE) {
    DSP_ResetOffExCore(bitmap);
  }
}

void DSP_ResetInterfaceCore(void);
SDK_INLINE void DSP_ResetInterface(void) {
  if (OS_IsRunOnTwl() == TRUE) {
    DSP_ResetInterfaceCore();
  }
}

void DSP_EnableRecvDataInterruptCore(u32 dataNo);
SDK_INLINE void DSP_EnableRecvDataInterrupt(u32 dataNo) {
  if (OS_IsRunOnTwl() == TRUE) {
    DSP_EnableRecvDataInterruptCore(dataNo);
  }
}

void DSP_DisableRecvDataInterruptCore(u32 dataNo);
SDK_INLINE void DSP_DisableRecvDataInterrupt(u32 dataNo) {
  if (OS_IsRunOnTwl() == TRUE) {
    DSP_DisableRecvDataInterruptCore(dataNo);
  }
}

BOOL DSP_SendDataIsEmptyCore(u32 dataNo);
SDK_INLINE BOOL DSP_SendDataIsEmpty(u32 dataNo) {
  if (OS_IsRunOnTwl() == TRUE) {
    return DSP_SendDataIsEmptyCore(dataNo);
  }
  return FALSE;
}

BOOL DSP_RecvDataIsReadyCore(u32 dataNo);
SDK_INLINE BOOL DSP_RecvDataIsReady(u32 dataNo) {
  if (OS_IsRunOnTwl() == TRUE) {
    return DSP_RecvDataIsReadyCore(dataNo);
  }
  return FALSE;
}

void DSP_SendDataCore(u32 dataNo, u16 data);
SDK_INLINE void DSP_SendData(u32 dataNo, u16 data) {
  if (OS_IsRunOnTwl() == TRUE) {
    DSP_SendDataCore(dataNo, data);
  }
}

u16 DSP_RecvDataCore(u32 dataNo);
SDK_INLINE u16 DSP_RecvData(u32 dataNo) {
  if (OS_IsRunOnTwl() == TRUE) {
    return DSP_RecvDataCore(dataNo);
  }
  return 0;
}

void DSP_EnableFifoInterruptCore(DSPFifoIntr type);
SDK_INLINE void DSP_EnableFifoInterrupt(DSPFifoIntr type) {
  if (OS_IsRunOnTwl() == TRUE) {
    DSP_EnableFifoInterruptCore(type);
  }
}

void DSP_DisableFifoInterruptCore(DSPFifoIntr type);
SDK_INLINE void DSP_DisableFifoInterrupt(DSPFifoIntr type) {
  if (OS_IsRunOnTwl() == TRUE) {
    DSP_DisableFifoInterruptCore(type);
  }
}

void DSP_SendFifoExCore(DSPFifoMemSel memsel, u16 dest, const u16 *src,
                        int size, u16 flags);
SDK_INLINE void DSP_SendFifoEx(DSPFifoMemSel memsel, u16 dest, const u16 *src,
                               int size, u16 flags) {
  if (OS_IsRunOnTwl() == TRUE) {
    DSP_SendFifoExCore(memsel, dest, src, size, flags);
  }
}

static inline void DSP_SendFifo(DSPFifoMemSel memsel, u16 dest, const u16 *src,
                                int size) {
  if (OS_IsRunOnTwl() == TRUE) {
    DSP_SendFifoExCore(memsel, dest, src, size, 0);
  }
}

void DSP_RecvFifoExCore(DSPFifoMemSel memsel, u16 *dest, u16 src, int size,
                        u16 flags);
SDK_INLINE void DSP_RecvFifoEx(DSPFifoMemSel memsel, u16 *dest, u16 src,
                               int size, u16 flags) {
  if (OS_IsRunOnTwl() == TRUE) {
    DSP_RecvFifoExCore(memsel, dest, src, size, flags);
  }
}

static inline void DSP_RecvFifo(DSPFifoMemSel memsel, u16 *dest, u16 src,
                                int size) {
  if (OS_IsRunOnTwl() == TRUE) {
    DSP_RecvFifoExCore(memsel, dest, src, size, 0);
  }
}

void DSP_SetCommandRegCore(u32 regNo, u16 data);
SDK_INLINE void DSP_SetCommandReg(u32 regNo, u16 data) {
  if (OS_IsRunOnTwl() == TRUE) {
    DSP_SetCommandRegCore(regNo, data);
  }
}

u16 DSP_GetReplyRegCore(u32 regNo);
SDK_INLINE u16 DSP_GetReplyReg(u32 regNo) {
  if (OS_IsRunOnTwl() == TRUE) {
    return DSP_GetReplyRegCore(regNo);
  }
  return 0;
}

void DSP_SetSemaphoreCore(u16 mask);
SDK_INLINE void DSP_SetSemaphore(u16 mask) {
  if (OS_IsRunOnTwl() == TRUE) {
    DSP_SetSemaphoreCore(mask);
  }
}

u16 DSP_GetSemaphoreCore(void);
SDK_INLINE u16 DSP_GetSemaphore(void) {
  if (OS_IsRunOnTwl() == TRUE) {
    return DSP_GetSemaphoreCore();
  }
  return 0;
}

void DSP_ClearSemaphoreCore(u16 mask);
SDK_INLINE void DSP_ClearSemaphore(u16 mask) {
  if (OS_IsRunOnTwl() == TRUE) {
    DSP_ClearSemaphoreCore(mask);
  }
}

void DSP_MaskSemaphoreCore(u16 mask);
SDK_INLINE void DSP_MaskSemaphore(u16 mask) {
  if (OS_IsRunOnTwl() == TRUE) {
    DSP_MaskSemaphoreCore(mask);
  }
}

BOOL DSP_CheckSemaphoreRequestCore(void);
SDK_INLINE BOOL DSP_CheckSemaphoreRequest(void) {
  if (OS_IsRunOnTwl() == TRUE) {
    return DSP_CheckSemaphoreRequestCore();
  }
  return FALSE;
}

#if defined(DSP_SUPPORT_OBSOLETE_LOADER)

BOOL DSP_LoadFileAutoCore(const void *image);
SDK_INLINE BOOL DSP_LoadFileAuto(const void *image) {
  if (OS_IsRunOnTwl() == TRUE) {
    return DSP_LoadFileAutoCore(image);
  }
  return FALSE;
}

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TWL_DSP_IF_H_ */

#ifdef SDK_ARM9
#include <twl.h>
#include "../include/mi_ndma.h"

#include <twl/ltdmain_begin.h>

#define MIi_GX_LENGTH_ONCE (118 * sizeof(u32)) //(byte)

typedef struct {
  volatile BOOL isBusy;

  u32 ndmaNo;
  u32 src;
  u32 length;
  MINDmaCallback callback;
  void *arg;

  GXFifoIntrCond fifoCond;
  void (*fifoFunc)(void);
} MIiGXNDmaParams;

static MIiGXNDmaParams MIi_GXNDmaParams = {FALSE};

static void MIi_FIFOCallback(void);
static void MIi_NDMACallback(void *);
static void MIi_NDMAFastCallback(void *);

void MI_SendNDmaGXCommand(u32 ndmaNo, const void *src, u32 commandLength) {
  vu32 *ndmaCntp;
  u32 leftLength = commandLength;
  u32 currentSrc = (u32)src;
  OSIntrMode enabled;

  MIi_ASSERT_DMANO(ndmaNo);

  if (leftLength == 0) {
    return;
  }

  enabled = OS_DisableInterrupts();
  ndmaCntp = (vu32 *)MI_NDMA_REGADDR(ndmaNo, MI_NDMA_REG_CNT_WOFFSET);

  while (*ndmaCntp & REG_MI_NDMA0CNT_E_MASK) {
  }

  while (leftLength > 0) {
    u32 length =
        (leftLength > MIi_GX_LENGTH_ONCE) ? MIi_GX_LENGTH_ONCE : leftLength;

    MIi_NDma_withConfig_Dev(MIi_NDMA_TYPE_GXCOPY, ndmaNo,
                            (const void *)currentSrc, (void *)REG_GXFIFO_ADDR,
                            0 /*not used*/, length, &MIi_NDmaConfig[ndmaNo],
                            MI_NDMA_TIMING_GXFIFO, MI_NDMA_ENABLE);

    leftLength -= length;
    currentSrc += length;
  }

  while (*ndmaCntp & REG_MI_NDMA0CNT_E_MASK) {
  }
  (void)OS_RestoreInterrupts(enabled);
}

void MI_SendNDmaGXCommandAsync(u32 ndmaNo, const void *src, u32 commandLength,
                               MINDmaCallback callback, void *arg) {
  MIi_ASSERT_DMANO(ndmaNo);

  if (commandLength == 0) {
    MIi_CallCallback(callback, arg);
    return;
  }

  while (MIi_GXNDmaParams.isBusy) {
  }

  while (!(G3X_GetCommandFifoStatus() & GX_FIFOSTAT_UNDERHALF)) {
  }

  MIi_GXNDmaParams.isBusy = TRUE;
  MIi_GXNDmaParams.ndmaNo = ndmaNo;
  MIi_GXNDmaParams.src = (u32)src;
  MIi_GXNDmaParams.length = commandLength;
  MIi_GXNDmaParams.callback = callback;
  MIi_GXNDmaParams.arg = arg;

  MI_WaitNDma(ndmaNo);
  {
    OSIntrMode enabled = OS_DisableInterrupts();

    MIi_GXNDmaParams.fifoCond =
        (GXFifoIntrCond)((reg_G3X_GXSTAT & REG_G3X_GXSTAT_FI_MASK) >>
                         REG_G3X_GXSTAT_FI_SHIFT);
    MIi_GXNDmaParams.fifoFunc = OS_GetIrqFunction(OS_IE_GXFIFO);

    G3X_SetFifoIntrCond(GX_FIFOINTR_COND_UNDERHALF);
    OS_SetIrqFunction(OS_IE_GXFIFO, MIi_FIFOCallback);
    (void)OS_EnableIrqMask(OS_IE_GXFIFO);

    MIi_FIFOCallback();

    (void)OS_RestoreInterrupts(enabled);
  }
}

static void MIi_FIFOCallback(void) {
  u32 length;
  u32 src;

  if (MIi_GXNDmaParams.length == 0) {
    return;
  }

  length = (MIi_GXNDmaParams.length >= MIi_GX_LENGTH_ONCE)
               ? MIi_GX_LENGTH_ONCE
               : MIi_GXNDmaParams.length;
  src = MIi_GXNDmaParams.src;

  MIi_GXNDmaParams.length -= length;
  MIi_GXNDmaParams.src += length;

  if (MIi_GXNDmaParams.length == 0) {
    (void)OS_DisableIrqMask(OS_IE_GXFIFO);
  }
  (void)OS_ResetRequestIrqMask(OS_IE_GXFIFO);

  MIi_NDmaAsync((MIi_GXNDmaParams.length == 0) ? MIi_NDMA_TYPE_GXCOPY_IF
                                               : MIi_NDMA_TYPE_GXCOPY,
                MIi_GXNDmaParams.ndmaNo, (const void *)src,
                (void *)REG_GXFIFO_ADDR, 0 /*not used*/, length,
                (MIi_GXNDmaParams.length == 0) ? MIi_NDMACallback : NULL, NULL,
                MI_NDMA_ENABLE);
}

static void MIi_NDMACallback(void *) {
  (void)OS_DisableIrqMask(OS_IE_GXFIFO);

  G3X_SetFifoIntrCond(MIi_GXNDmaParams.fifoCond);
  OS_SetIrqFunction(OS_IE_GXFIFO, MIi_GXNDmaParams.fifoFunc);

  MIi_GXNDmaParams.isBusy = FALSE;

  MIi_CallCallback(MIi_GXNDmaParams.callback, MIi_GXNDmaParams.arg);
}

void MI_SendNDmaGXCommandFast(u32 ndmaNo, const void *src, u32 commandLength) {
  vu32 *ndmaCntp;
  OSIntrMode enabled;

  MIi_ASSERT_DMANO(ndmaNo);

  if (commandLength == 0) {
    return;
  }

  enabled = OS_DisableInterrupts();
  ndmaCntp = (vu32 *)MI_NDMA_REGADDR(ndmaNo, MI_NDMA_REG_CNT_WOFFSET);

  while (*ndmaCntp & REG_MI_NDMA0CNT_E_MASK) {
  }

  MIi_NDmaAsync(MIi_NDMA_TYPE_GXCOPY, ndmaNo, (const void *)src,
                (void *)REG_GXFIFO_ADDR, 0 /*not used*/, commandLength, NULL,
                NULL, MI_NDMA_ENABLE);

  while (*ndmaCntp & REG_MI_NDMA0CNT_E_MASK) {
  }
  (void)OS_RestoreInterrupts(enabled);
}

void MI_SendNDmaGXCommandAsyncFast(u32 ndmaNo, const void *src,
                                   u32 commandLength, MINDmaCallback callback,
                                   void *arg) {
  MIi_ASSERT_DMANO(ndmaNo);

  if (commandLength == 0) {
    MIi_CallCallback(callback, arg);
    return;
  }

  while (MIi_GXNDmaParams.isBusy /*volatile valiable */) {
  }

  MIi_GXNDmaParams.isBusy = TRUE;
  MIi_GXNDmaParams.ndmaNo = ndmaNo;
  MIi_GXNDmaParams.callback = callback;
  MIi_GXNDmaParams.arg = arg;

  MI_WaitNDma(ndmaNo);

  MIi_NDmaAsync(MIi_NDMA_TYPE_GXCOPY_IF, ndmaNo, (const void *)src,
                (void *)REG_GXFIFO_ADDR, 0 /*not used*/, commandLength,
                MIi_NDMAFastCallback, NULL, MI_NDMA_ENABLE);
}

static void MIi_NDMAFastCallback(void *) {
  MIi_GXNDmaParams.isBusy = FALSE;

  MIi_CallCallback(MIi_GXNDmaParams.callback, MIi_GXNDmaParams.arg);
}

#endif // SDK_ARM9

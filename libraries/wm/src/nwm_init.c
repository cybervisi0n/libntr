#include <twl.h>
#include "nwm_common_private.h"
#ifdef SDK_TWL
#include "wm_common.h"
#else
#include "nwm_arm9_private.h"
#include <nitro/wm.h>
#include <nitro/os/ARM9/protectionRegion.h>
#endif

u8 nwmInitialized = 0;

NWMRetCode NWMi_InitCore(void *sysBuf, u32 bufSize, u8 dmaNo);
static BOOL NwmCheckEnableFlag(void);

NWMRetCode NWM_Init(void *sysBuf, u32 bufSize, u8 dmaNo) {
  if (NWMi_CheckEnableFlag() ==
      FALSE) // Return an error if the TWL System Settings are configured not to
             // use wireless
  {
    return NWM_RETCODE_NWM_DISABLE;
  }

  return NWMi_InitCore(sysBuf, bufSize, dmaNo);
}

NWMRetCode NWMi_InitCore(void *sysBuf, u32 bufSize, u8 dmaNo) {
  OSIntrMode e;
  extern NWMArm9Buf *nwm9buf;

  SDK_COMPILER_ASSERT(sizeof(NWMArm9Buf) <= NWM_ARM9NWM_BUF_SIZE);
  SDK_COMPILER_ASSERT(sizeof(NWMArm7Buf) <= NWM_ARM7NWM_BUF_SIZE);
  SDK_COMPILER_ASSERT(sizeof(NWMStatus) <= NWM_STATUS_BUF_SIZE);
  SDK_ASSERT(bufSize >= NWM_SYSTEM_BUF_SIZE);

  e = OS_DisableInterrupts();

#ifdef SDK_TWL

  if (WM_CheckInitialized() == WM_ERRCODE_SUCCESS) {
    NWM_WARNING("WM has already been initialized.\n");
    (void)OS_RestoreInterrupts(e);
    return NWM_RETCODE_ILLEGAL_STATE;
  }
#endif

  if (nwmInitialized) {
    NWM_WARNING("NWM  has already been initialized.\n");
    (void)OS_RestoreInterrupts(e);
    return NWM_RETCODE_ILLEGAL_STATE; // Initialization complete
  }

  if (sysBuf == NULL) {
    NWM_WARNING("Parameter \"sysBuf\" must not be NULL.\n");
    (void)OS_RestoreInterrupts(e);
    return NWM_RETCODE_INVALID_PARAM;
  }

  if (dmaNo > MI_DMA_MAX_NUM) {
    NWM_WARNING("Parameter \"dmaNo\" is over %d.\n", MI_DMA_MAX_NUM);
    (void)OS_RestoreInterrupts(e);
    return NWM_RETCODE_INVALID_PARAM;
  }

  if ((u32)sysBuf & 0x01f) {
    NWM_WARNING("Parameter \"sysBuf\" must be 32-byte aligned.\n");
    (void)OS_RestoreInterrupts(e);
    return NWM_RETCODE_INVALID_PARAM;
  }

  PXI_Init();
  if (!PXI_IsCallbackReady(PXI_FIFO_TAG_WMW, PXI_PROC_ARM7)) {
    NWM_WARNING("NWM library on ARM7 side is not ready yet.\n");
    (void)OS_RestoreInterrupts(e);
    return NWM_RETCODE_NWM_DISABLE;
  }

  DC_InvalidateRange(sysBuf, bufSize);
  MI_DmaClear32(dmaNo, sysBuf, bufSize);
  nwm9buf = (NWMArm9Buf *)sysBuf;
  DC_StoreRange(&nwm9buf, sizeof(nwm9buf));
  nwm9buf->NWM7 = (NWMArm7Buf *)((u32)nwm9buf + NWM_ARM9NWM_BUF_SIZE);
  nwm9buf->status = (NWMStatus *)((u32)(nwm9buf->NWM7) + NWM_ARM7NWM_BUF_SIZE);
  nwm9buf->fifo7to9 = (u32 *)((u32)(nwm9buf->status) + NWM_STATUS_BUF_SIZE);

  NWMi_ClearFifoRecvFlag();

  nwm9buf->dmaNo = dmaNo;

  {
    s32 i;

    OS_InitMessageQueue(&nwm9buf->apibufQ.q, nwm9buf->apibufQ.fifo,
                        NWM_APIBUF_NUM);
    for (i = 0; i < NWM_APIBUF_NUM; i++) {

      *((u16 *)(nwm9buf->apibuf[i])) = NWM_API_REQUEST_ACCEPTED;
#ifdef SDK_BUILD_ARM
      DC_StoreRange(nwm9buf->apibuf[i], sizeof(u16));
      (void)OS_SendMessage(&nwm9buf->apibufQ.q, &nwm9buf->apibuf[i][0],
                           OS_MESSAGE_BLOCK);
#endif
    }
  }

  PXI_SetFifoRecvCallback(PXI_FIFO_TAG_WMW, NWMi_ReceiveFifo9);

  MI_CpuClear8(&nwm9buf->binData, sizeof(NWMBinaryData));

  nwmInitialized = 1;
  nwm9buf->status->state =
      NWM_STATE_INITIALIZED; // As an exception, set the initial state here
                             // alone (this was originally done by the ARM7)

  DC_StoreRange(nwm9buf, sizeof(NWMArm9Buf));
  DC_StoreRange(&nwmInitialized, sizeof(u8));
  (void)OS_RestoreInterrupts(e);

  return NWM_RETCODE_SUCCESS;
}

NWMRetCode NWMi_CheckInitialized(void) {

  if (!nwmInitialized) {

    return NWM_RETCODE_ILLEGAL_STATE;
  }
  return NWM_RETCODE_SUCCESS;
}

BOOL NWMi_CheckEnableFlag(void) {
  if (OS_IsAvailableWireless() == TRUE &&
      OS_IsForceDisableWireless() == FALSE) {
    return TRUE;
  }
  return FALSE;
}

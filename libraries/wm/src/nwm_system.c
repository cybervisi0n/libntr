#include <twl.h>
#include "nwm_arm9_private.h"
#include "nwm_common_private.h"

NWMArm9Buf *nwm9buf = NULL;
static PMSleepCallbackInfo
    sleepCbInfo; // Sleep callback information to register with the PM library

#ifdef NWM_SUPPORT_HWRESET
PMExitCallbackInfo hwResetCbInfo;
#endif

static void NwmSleepCallback(void *);

NWMArm9Buf *NWMi_GetSystemWork(void) { return nwm9buf; }

void NWMi_ClearFifoRecvFlag(void) {
  NWMArm7Buf *w7b;

  if (nwm9buf == NULL) {
    return;
  }

  w7b = nwm9buf->NWM7;

  DC_InvalidateRange(&w7b->callbackSyncFlag, 1);

  if (w7b->callbackSyncFlag & NWM_EXCEPTION_CB_MASK) {

    w7b->callbackSyncFlag &= ~NWM_EXCEPTION_CB_MASK;

    DC_StoreRange(&w7b->callbackSyncFlag, 1);
  }
}

#ifdef SDK_PORT
void NWMi_ReceiveFifo9(PXIFifoTag tag, u64 fifo_buf_adr, BOOL err)
#else
void NWMi_ReceiveFifo9(PXIFifoTag tag, u32 fifo_buf_adr, BOOL err)
#endif
{
#pragma unused(tag)
  NWMCallback *pCallback = (NWMCallback *)fifo_buf_adr;
  NWMCallbackFunc callback;
  NWMArm9Buf *w9b = nwm9buf;

  if (w9b == NULL) {
    return;
  }

  if (err) {
    NWM_WARNING("NWM9 FIFO receive error. :%d\n", err);
    return;
  }

  if (!fifo_buf_adr) {
    NWM_WARNING("NWM9 FIFO receive error.(NULL address) :%d\n", err);
    return;
  }

  if (pCallback->apiid >= NWM_APIID_ASYNC_KIND_MAX) {
    NWM_WARNING("Receive Unknown APIID(%d)\n", pCallback->apiid);
    return;
  }

  DC_InvalidateRange(w9b->fifo7to9, NWM_APIFIFO_BUF_SIZE);
  DC_InvalidateRange(w9b->status, NWM_STATUS_BUF_SIZE);
  if ((u32)pCallback != (u32)(w9b->fifo7to9)) {
    DC_InvalidateRange(pCallback, NWM_APIFIFO_BUF_SIZE);
  }

  NWM_DPRINTF("APIID%04x\n", pCallback->apiid);

  if ((pCallback->apiid == NWM_APIID_LOAD_DEVICE &&
       pCallback->retcode != NWM_RETCODE_SUCCESS) ||
      (pCallback->apiid == NWM_APIID_UNLOAD_DEVICE &&
       pCallback->retcode == NWM_RETCODE_SUCCESS) ||
      pCallback->apiid == NWM_APIID_INSTALL_FIRMWARE) {
    NWMi_DeleteSleepCallback();
  }

  {
    NWMSendFrameCallback *pSfcb = (NWMSendFrameCallback *)pCallback;
    NWMRetCode result = NWM_RETCODE_FAILED;

    if (pCallback->apiid == NWM_APIID_SEND_FRAME) {
      if (pCallback->retcode != NWM_RETCODE_INDICATION &&
          NULL != pSfcb->callback) {
        NWM_DPRINTF("Execute CallbackFunc APIID 0x%04x\n", pCallback->apiid);
        (pSfcb->callback)((void *)pCallback);
      }
    }
  }

  if (pCallback->apiid == NWM_APIID_SEND_FRAME) {
    NWMSendFrameCallback *pSfcb = (NWMSendFrameCallback *)pCallback;

  } else {

#ifdef NWM_SUPPORT_HWRESET
    if (pCallback->apiid == NWM_APIID_UNLOAD_DEVICE ||
        pCallback->apiid == NWM_APIID_INSTALL_FIRMWARE ||
        (pCallback->apiid == NWM_APIID_LOAD_DEVICE &&
         pCallback->retcode != NWM_RETCODE_SUCCESS)) {

      PM_DeletePostExitCallback(&hwResetCbInfo);
    }
#endif

    callback = w9b->callbackTable[pCallback->apiid];

    if (pCallback->apiid == NWM_APIID_START_SCAN) {
      NWMStartScanCallback *psscb = (NWMStartScanCallback *)pCallback;

      DC_InvalidateRange(psscb->bssDesc[0], psscb->allBssDescSize);
    }

    if (NULL != callback) {
      NWM_DPRINTF("Execute CallbackFunc APIID 0x%04x\n", pCallback->apiid);
      (callback)((void *)pCallback);
    }
  }

  MI_CpuClear8(pCallback, NWM_APIFIFO_BUF_SIZE);
  DC_StoreRange(pCallback, NWM_APIFIFO_BUF_SIZE);
  if (w9b) { // NWM might be terminated after callback
    NWMi_ClearFifoRecvFlag();
  }
}

u16 NWM_GetState(void) {
  NWMStatus *nwmStatus;
  NWMArm9Buf *sys = NWMi_GetSystemWork();
  u16 state = NWM_STATE_NONE;

  if (sys) {
    nwmStatus = sys->status;
    DC_InvalidateRange(nwmStatus, 2);
    state = nwmStatus->state;
  }

  return state;
}

NWMRetCode NWMi_CheckState(s32 paramNum, ...) {
  NWMRetCode result;
  u16 now;
  u32 temp;
  va_list vlist;
  NWMArm9Buf *sys = NWMi_GetSystemWork();

  SDK_NULL_ASSERT(sys);

  result = NWMi_CheckInitialized();
  NWM_CHECK_RESULT(result);

  DC_InvalidateRange(&(sys->status->state), 2);
  now = sys->status->state;

  result = NWM_RETCODE_ILLEGAL_STATE;
  va_start(vlist, paramNum);
  for (; paramNum; paramNum--) {
    temp = va_arg(vlist, u32);
    if (temp == now) {
      result = NWM_RETCODE_SUCCESS;
    }
  }
  va_end(vlist);

  if (result == NWM_RETCODE_ILLEGAL_STATE) {
    NWM_WARNING("New WM state is \"0x%04x\" now. So can't execute request.\n",
                now);
  }

  return result;
}

u16 NWM_GetAllowedChannel(void) {
  NWMArm9Buf *sys = NWMi_GetSystemWork();

  return sys->status->allowedChannel;
}

u16 NWM_CalcLinkLevel(s16 rssi) {

  if (1) /* Infra Structure Mode */
  {
    if (rssi < NWM_RSSI_INFRA_LINK_LEVEL_1) {
      return WM_LINK_LEVEL_0;
    }
    if (rssi < NWM_RSSI_INFRA_LINK_LEVEL_2) {
      return WM_LINK_LEVEL_1;
    }
    if (rssi < NWM_RSSI_INFRA_LINK_LEVEL_3) {
      return WM_LINK_LEVEL_2;
    }

    return WM_LINK_LEVEL_3;
  } else if (0) /*Ad Hoc Mode*/
  {
    if (rssi < NWM_RSSI_ADHOC_LINK_LEVEL_1) {
      return WM_LINK_LEVEL_0;
    }
    if (rssi < NWM_RSSI_ADHOC_LINK_LEVEL_2) {
      return WM_LINK_LEVEL_1;
    }
    if (rssi < NWM_RSSI_ADHOC_LINK_LEVEL_3) {
      return WM_LINK_LEVEL_2;
    }
    return WM_LINK_LEVEL_3;
  }
}

u16 NWM_GetDispersionScanPeriod(u16 scanType) {
  u8 mac[6];
  u16 ret;
  s32 i;

  OS_GetMacAddress(mac);
  for (i = 0, ret = 0; i < 6; i++) {
    ret += mac[i];
  }
  ret += OS_GetVBlankCount();
  ret *= 13;

  if (scanType == NWM_SCANTYPE_ACTIVE) {
    ret = (u16)(NWM_DEFAULT_ACTIVE_SCAN_PERIOD + (ret % 10));
  } else /* An unknown scan type is treated in the same way as a passive scan */
  {
    ret = (u16)(NWM_DEFAULT_PASSIVE_SCAN_PERIOD + (ret % 10));
  }
  return ret;
}

void NWMi_RegisterSleepCallback(void) {
  PM_SetSleepCallbackInfo(&sleepCbInfo, NwmSleepCallback, NULL);
  PMi_InsertPreSleepCallbackEx(&sleepCbInfo, PM_CALLBACK_PRIORITY_NWM);
}

void NWMi_DeleteSleepCallback(void) { PM_DeletePreSleepCallback(&sleepCbInfo); }

static void NwmSleepCallback(void *) {

  OS_TPanic("Could not sleep during wireless communications.");
}

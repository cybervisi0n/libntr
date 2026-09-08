#include <nitro.h>

#include <nitro/wxc/common.h>
#include <nitro/wxc/driver.h>

static void WXCi_RecoverWmApiFailure(WXCDriverWork *, WMApiid id,
                                     WMErrCode err);
static BOOL WXCi_CheckWmApiResult(WXCDriverWork *driver, WMApiid id,
                                  WMErrCode err);
static BOOL WXCi_CheckWmCallbackResult(WXCDriverWork *, void *arg);
static void WXCi_ErrorQuit(WXCDriverWork *);

static void WXCi_InitProc(void *arg);        /* (end) -> STOP             */
static void WXCi_StartProc(void *arg);       /* STOP -> IDLE              */
static void WXCi_StopProc(void *arg);        /* IDLE -> STOP              */
static void WXCi_EndProc(void *arg);         /* STOP  -> (end)            */
static void WXCi_ResetProc(void *arg);       /* (any) -> IDLE             */
static void WXCi_StartParentProc(void *arg); /* IDLE  -> MP_PARENT        */
static void WXCi_StartChildProc(void *arg);  /* IDLE  -> MP_CHILD         */
static void WXCi_ScanProc(void *arg);        /* IDLE -> SCAN -> IDLE      */
static void WXCi_MeasureProc(void *arg);     /* IDLE -> (measure) -> IDLE */

static void WXCi_OnStateChanged(WXCDriverWork *, WXCDriverState state,
                                void *arg);

static void WXCi_IndicateCallback(void *arg);
static void WXCi_PortCallback(void *arg);
static void WXCi_MPSendCallback(void *arg);
static void WXCi_ParentIndicate(void *arg);
static void WXCi_ChildIndicate(void *arg);

#define VERSION_TO_INT(major, minor, relstep)                                  \
  (((major) << 24) | ((minor) << 16) | ((relstep) << 0))
#if VERSION_TO_INT(SDK_VERSION_MAJOR, SDK_VERSION_MINOR,                       \
                   SDK_VERSION_RELSTEP) < VERSION_TO_INT(3, 0, 20100)
#define WM_STATECODE_DISCONNECTED_FROM_MYSELF ((WMStateCode)26)
#endif

static WXCDriverWork *work;

static void WXCi_ErrorQuit(WXCDriverWork *driver) {

  if (driver->state == WXC_DRIVER_STATE_BUSY) {
    driver->state = driver->target;
  }
  WXCi_End(driver);
}

static void WXCi_RecoverWmApiFailure(WXCDriverWork *driver, WMApiid id,
                                     WMErrCode err) {

  if (err == WM_ERRCODE_SUCCESS) {
    return;
  }

  switch (id) {
  default:

    OS_TWarning("WXC library error (unknown WM API : %d)\n", id);
    WXCi_ErrorQuit(driver);
    break;

  case WM_APIID_ENABLE:
  case WM_APIID_POWER_ON:
  case WM_APIID_POWER_OFF:
  case WM_APIID_DISABLE:
  case WM_APIID_RESET:

    WXCi_ErrorQuit(driver);
    break;

  case WM_APIID_MEASURE_CHANNEL:
  case WM_APIID_SET_P_PARAM:
  case WM_APIID_START_PARENT:
  case WM_APIID_START_SCAN_EX:
  case WM_APIID_END_SCAN:
  case WM_APIID_DISCONNECT:

    WXCi_ErrorQuit(driver);
    break;

  case WM_APIID_START_MP:
    driver->state = WXC_DRIVER_STATE_CHILD; // Cancel BUSY
    WXCi_ErrorQuit(driver);
    break;

  case WM_APIID_START_CONNECT:

    if ((err == WM_ERRCODE_FAILED) || (err == WM_ERRCODE_NO_ENTRY) ||
        (err == WM_ERRCODE_OVER_MAX_ENTRY)) {

      driver->state = WXC_DRIVER_STATE_CHILD;
      WXCi_Reset(driver);
    }

    else {
      WXCi_ErrorQuit(driver);
    }
    break;

  case WM_APIID_INDICATION:

    WXCi_ErrorQuit(driver);
    break;

  case WM_APIID_SET_MP_DATA:
  case WM_APIID_PORT_SEND:

    break;
  }
}

static BOOL WXCi_CheckWmApiResult(WXCDriverWork *driver, WMApiid id,
                                  WMErrCode err) {
  BOOL ret = WXC_CheckWmApiResult(id, err);

  if (!ret) {
    WXCi_RecoverWmApiFailure(driver, id, err);
  }
  return ret;
}

static BOOL WXCi_CheckWmCallbackResult(WXCDriverWork *driver, void *arg) {
  BOOL ret = WXC_CheckWmCallbackResult(arg);

  if (!ret) {
    const WMCallback *cb = (const WMCallback *)arg;
    WXCi_RecoverWmApiFailure(driver, (WMApiid)cb->apiid,
                             (WMErrCode)cb->errcode);
  }
  return ret;
}

static inline u32 WXCi_CallDriverEvent(WXCDriverWork *driver,
                                       WXCDriverEvent event, void *arg) {
  u32 result = 0;
  if (driver->callback) {
    result = (*driver->callback)(event, arg);
  }
  return result;
}

static void WXCi_CallSendEvent(WXCDriverWork *driver) {
  if (!driver->send_busy) {

    const u16 max_length =
        (u16)((driver->own_aid == 0)
                  ? driver->parent_param->parentMaxSize
                  : driver->target_bss->gameInfo.childMaxSize);
    WXCPacketInfo packet;
    packet.bitmap = driver->peer_bitmap;
    packet.length = max_length;
    packet.buffer = driver->current_send_buf;

    (void)WXCi_CallDriverEvent(driver, WXC_DRIVER_EVENT_DATA_SEND, &packet);

    if ((packet.length <= max_length) &&
        ((driver->state == WXC_DRIVER_STATE_PARENT) ||
         (driver->state == WXC_DRIVER_STATE_CHILD))) {
      WMErrCode ret;
      ret = WM_SetMPDataToPort(WXCi_MPSendCallback, (u16 *)packet.buffer,
                               packet.length, packet.bitmap, WXC_DEFAULT_PORT,
                               WXC_DEFAULT_PORT_PRIO);
      driver->send_busy =
          WXCi_CheckWmApiResult(driver, WM_APIID_SET_MP_DATA, ret);
    }
  }
}

static void WXCi_OnStateChanged(WXCDriverWork *driver, WXCDriverState state,
                                void *arg) {
  driver->state = state;

  if (driver->target == state) {
    switch (state) {
    case WXC_DRIVER_STATE_END:
      (void)WXCi_CallDriverEvent(driver, WXC_DRIVER_EVENT_STATE_END, NULL);
      break;

    case WXC_DRIVER_STATE_STOP:
      (void)WXCi_CallDriverEvent(driver, WXC_DRIVER_EVENT_STATE_STOP, NULL);
      break;

    case WXC_DRIVER_STATE_IDLE:
      (void)WXCi_CallDriverEvent(driver, WXC_DRIVER_EVENT_STATE_IDLE, NULL);
      break;

    case WXC_DRIVER_STATE_PARENT:
      driver->send_busy = FALSE;
      (void)WXCi_CallDriverEvent(driver, WXC_DRIVER_EVENT_STATE_PARENT, NULL);
      break;

    case WXC_DRIVER_STATE_CHILD:
      driver->send_busy = FALSE;
      (void)WXCi_CallDriverEvent(driver, WXC_DRIVER_EVENT_STATE_CHILD, NULL);

      driver->peer_bitmap |= (u16)(1 << 0);
      (void)WXCi_CallDriverEvent(driver, WXC_DRIVER_EVENT_CONNECTED, arg);
      WXCi_CallSendEvent(driver);
      break;
    }
  }

  else {
    switch (state) {

    case WXC_DRIVER_STATE_END:
      WXCi_InitProc(NULL);
      break;

    case WXC_DRIVER_STATE_STOP:
      switch (driver->target) {
      case WXC_DRIVER_STATE_END:
        WXCi_EndProc(NULL);
        break;
      case WXC_DRIVER_STATE_IDLE:
      case WXC_DRIVER_STATE_PARENT:
      case WXC_DRIVER_STATE_CHILD:
        WXCi_StartProc(NULL);
        break;
      }
      break;

    case WXC_DRIVER_STATE_IDLE:
      switch (driver->target) {
      case WXC_DRIVER_STATE_END:
      case WXC_DRIVER_STATE_STOP:
        WXCi_StopProc(NULL);
        break;
      case WXC_DRIVER_STATE_PARENT:
        driver->need_measure_channel = TRUE;
        if (driver->need_measure_channel) {
          WXCi_MeasureProc(NULL);
        }
        break;
      case WXC_DRIVER_STATE_CHILD:
        WXCi_ScanProc(NULL);
        break;
      }
      break;

    case WXC_DRIVER_STATE_PARENT:
    case WXC_DRIVER_STATE_CHILD:
      WXCi_ResetProc(NULL);
      break;
    }
  }
}

static void WXCi_MPSendCallback(void *arg) {
  WXCDriverWork *const driver = work;

  (void)WXCi_CheckWmCallbackResult(driver, arg);
  driver->send_busy = FALSE;
  if (driver->peer_bitmap != 0) {
    WXCi_CallSendEvent(driver);
  }
}

static void WXCi_IndicateCallback(void *arg) {

  WXCDriverWork *const driver = work;
  WMIndCallback *cb = (WMIndCallback *)arg;
  if (cb->errcode == WM_ERRCODE_FIFO_ERROR) {
    WXC_DRIVER_LOG("WM_ERRCODE_FIFO_ERROR Indication!\n");

    driver->target = WXC_DRIVER_STATE_ERROR;
    driver->state = WXC_DRIVER_STATE_ERROR;
  }
}

static void WXCi_PortCallback(void *arg) {
  WXCDriverWork *const driver = work;

  if (WXCi_CheckWmCallbackResult(driver, arg)) {
    WMPortRecvCallback *cb = (WMPortRecvCallback *)arg;
    switch (cb->state) {
    case WM_STATECODE_PORT_RECV: {
      WXCPacketInfo packet;
      packet.bitmap = (u16)(1 << cb->aid);
      packet.length = cb->length;
      packet.buffer = (u8 *)cb->data;
      (void)WXCi_CallDriverEvent(driver, WXC_DRIVER_EVENT_DATA_RECV, &packet);
    } break;
    case WM_STATECODE_CONNECTED:
      break;
    case WM_STATECODE_DISCONNECTED_FROM_MYSELF:
    case WM_STATECODE_DISCONNECTED:
      WXC_DRIVER_LOG("disconnected(%02X-=%02X)\n", driver->peer_bitmap,
                     (1 << cb->aid));
      driver->peer_bitmap &= (u16) ~(1 << cb->aid);
      (void)WXCi_CallDriverEvent(driver, WXC_DRIVER_EVENT_DISCONNECTED,
                                 (void *)(1 << cb->aid));
      break;
    }
  }
}

static void WXCi_InitProc(void *arg) {
  WXCDriverWork *const driver = work;
  WMCallback *cb = (WMCallback *)arg;

  if (!arg || WXCi_CheckWmCallbackResult(driver, arg)) {
    WMErrCode wmResult = WM_ERRCODE_SUCCESS;

    if (!arg) {

      driver->need_measure_channel = TRUE;
      driver->state = WXC_DRIVER_STATE_BUSY;
      wmResult = WM_Init(driver->wm_work, driver->wm_dma);
      (void)WXCi_CheckWmApiResult(driver, WM_APIID_INITIALIZE, wmResult);
      wmResult = WM_Enable(WXCi_InitProc);
      (void)WXCi_CheckWmApiResult(driver, WM_APIID_ENABLE, wmResult);
    }

    else if (cb->apiid == WM_APIID_ENABLE) {

      wmResult = WM_SetIndCallback(WXCi_IndicateCallback);
      if (WXCi_CheckWmApiResult(driver, WM_APIID_INDICATION, wmResult)) {

        wmResult =
            WM_SetPortCallback(WXC_DEFAULT_PORT, WXCi_PortCallback, NULL);
        if (WXCi_CheckWmApiResult(driver, WM_APIID_PORT_SEND, wmResult)) {
          WXCi_OnStateChanged(driver, WXC_DRIVER_STATE_STOP, NULL);
        }
      }
    }
  }
}

static void WXCi_StartProc(void *arg) {
  WXCDriverWork *const driver = work;
  WMCallback *cb = (WMCallback *)arg;

  if (!arg || WXCi_CheckWmCallbackResult(driver, arg)) {
    WMErrCode wmResult = WM_ERRCODE_SUCCESS;

    if (!arg) {
      driver->state = WXC_DRIVER_STATE_BUSY;
      wmResult = WM_PowerOn(WXCi_StartProc);
      (void)WXCi_CheckWmApiResult(driver, WM_APIID_POWER_ON, wmResult);
    }

    else if (cb->apiid == WM_APIID_POWER_ON) {
      WXCi_OnStateChanged(driver, WXC_DRIVER_STATE_IDLE, NULL);
    }
  }
}

static void WXCi_StopProc(void *arg) {
  WXCDriverWork *const driver = work;
  WMCallback *cb = (WMCallback *)arg;

  if (!arg || WXCi_CheckWmCallbackResult(driver, arg)) {
    WMErrCode wmResult = WM_ERRCODE_SUCCESS;

    if (!arg) {
      driver->state = WXC_DRIVER_STATE_BUSY;
      wmResult = WM_PowerOff(WXCi_StopProc);
      (void)WXCi_CheckWmApiResult(driver, WM_APIID_POWER_OFF, wmResult);
    }

    else if (cb->apiid == WM_APIID_POWER_OFF) {
      WXCi_OnStateChanged(driver, WXC_DRIVER_STATE_STOP, NULL);
    }
  }
}

static void WXCi_EndProc(void *arg) {
  WXCDriverWork *const driver = work;
  WMCallback *cb = (WMCallback *)arg;

  if (!arg || WXCi_CheckWmCallbackResult(driver, arg)) {
    WMErrCode wmResult = WM_ERRCODE_SUCCESS;

    if (!arg) {
      driver->state = WXC_DRIVER_STATE_BUSY;
      wmResult = WM_Disable(WXCi_EndProc);
      (void)WXCi_CheckWmApiResult(driver, WM_APIID_DISABLE, wmResult);
    }

    else if (cb->apiid == WM_APIID_DISABLE) {

      wmResult = WM_Finish();
      if (WXCi_CheckWmApiResult(driver, WM_APIID_END, wmResult)) {
        work = NULL;
        WXCi_OnStateChanged(driver, WXC_DRIVER_STATE_END, NULL);
      }
    }
  }
}

static void WXCi_ResetProc(void *arg) {
  WXCDriverWork *const driver = work;
  WMCallback *cb = (WMCallback *)arg;

  if (!arg || WXCi_CheckWmCallbackResult(driver, arg)) {
    WMErrCode wmResult = WM_ERRCODE_SUCCESS;

    if (!arg) {
      driver->state = WXC_DRIVER_STATE_BUSY;
      wmResult = WM_Reset(WXCi_ResetProc);
      (void)WXCi_CheckWmApiResult(driver, WM_APIID_RESET, wmResult);
    }

    else if (cb->apiid == WM_APIID_RESET) {
      driver->own_aid = 0;
      WXCi_OnStateChanged(driver, WXC_DRIVER_STATE_IDLE, NULL);
    }
  }
}

static void WXCi_ParentIndicate(void *arg) {
  WXCDriverWork *const driver = work;
  WMStartParentCallback *cb = (WMStartParentCallback *)arg;

  if (cb->state == WM_STATECODE_PARENT_START) {
    WXCi_StartParentProc(arg);
  }

  else if (cb->errcode == WM_ERRCODE_SUCCESS) {
    switch (cb->state) {
    case WM_STATECODE_PARENT_START:
      break;
    case WM_STATECODE_CONNECTED: {
      BOOL mp_start = (driver->peer_bitmap == 0);
      WXC_DRIVER_LOG("connected(%02X+=%02X)\n", driver->peer_bitmap,
                     (1 << cb->aid));
      driver->peer_bitmap |= (u16)(1 << cb->aid);
      (void)WXCi_CallDriverEvent(driver, WXC_DRIVER_EVENT_CONNECTED, cb);

      if (mp_start) {
        WXCi_CallSendEvent(driver);
      }
    } break;
    case WM_STATECODE_DISCONNECTED_FROM_MYSELF:
    case WM_STATECODE_DISCONNECTED:

      break;
    case WM_STATECODE_BEACON_SENT:
      (void)WXCi_CallDriverEvent(driver, WXC_DRIVER_EVENT_BEACON_SEND,
                                 driver->parent_param);
      break;
    }
  }
}

static void WXCi_StartParentProc(void *arg) {
  WXCDriverWork *const driver = work;
  WMCallback *cb = (WMCallback *)arg;

  if (!arg || WXCi_CheckWmCallbackResult(driver, arg)) {
    WMErrCode wmResult = WM_ERRCODE_SUCCESS;

    if (!arg) {
      driver->state = WXC_DRIVER_STATE_BUSY;

      driver->parent_param->channel = driver->current_channel;
      driver->parent_param->tgid = WM_GetNextTgid();
      WXC_DRIVER_LOG("start parent. (%2dch, TGID=%02X, GGID=%04X)\n",
                     driver->current_channel, driver->parent_param->tgid,
                     driver->parent_param->ggid);
      wmResult =
          WM_SetParentParameter(WXCi_StartParentProc, driver->parent_param);
      (void)WXCi_CheckWmApiResult(driver, WM_APIID_SET_P_PARAM, wmResult);
    }

    else if (cb->apiid == WM_APIID_SET_P_PARAM) {

      wmResult = WM_StartParent(WXCi_ParentIndicate);
      (void)WXCi_CheckWmApiResult(driver, WM_APIID_START_PARENT, wmResult);
    }

    else if (cb->apiid == WM_APIID_START_PARENT) {

      driver->own_aid = 0;
      driver->peer_bitmap = 0;
      wmResult = WM_StartMP(WXCi_StartParentProc, (u16 *)driver->mp_recv_work,
                            driver->recv_size_max, (u16 *)driver->mp_send_work,
                            driver->send_size_max,
                            (u16)(driver->parent_param->CS_Flag ? 0 : 1));
      (void)WXCi_CheckWmApiResult(driver, WM_APIID_START_MP, wmResult);
    }

    else if (cb->apiid == WM_APIID_START_MP) {
      WMStartMPCallback *cb = (WMStartMPCallback *)arg;
      switch (cb->state) {
      case WM_STATECODE_MP_START:
        WXCi_OnStateChanged(driver, WXC_DRIVER_STATE_PARENT, NULL);
        break;
      }
    }
  }
}

static void WXCi_ChildIndicate(void *arg) {
  WXCDriverWork *const driver = work;

  if (WXCi_CheckWmCallbackResult(driver, arg)) {
    WMStartConnectCallback *cb = (WMStartConnectCallback *)arg;
    switch (cb->state) {
    case WM_STATECODE_CONNECT_START:
    case WM_STATECODE_BEACON_LOST:
      break;

    case WM_STATECODE_CONNECTED:

      if (driver->state != WXC_DRIVER_STATE_CHILD) {
        WXCi_StartChildProc(arg);
      }
      break;

    case WM_STATECODE_DISCONNECTED_FROM_MYSELF:
    case WM_STATECODE_DISCONNECTED:

      if (driver->state != WXC_DRIVER_STATE_BUSY) {
        driver->target = WXC_DRIVER_STATE_PARENT;
        WXCi_ResetProc(NULL);
      } else {
        driver->target = WXC_DRIVER_STATE_IDLE;
      }
      break;

    default:
      WXCi_ErrorQuit(driver);
      break;
    }
  }
}

static void WXCi_StartChildProc(void *arg) {
  WXCDriverWork *const driver = work;
  WMCallback *cb = (WMCallback *)arg;

  if (!arg || WXCi_CheckWmCallbackResult(driver, arg)) {
    WMErrCode wmResult = WM_ERRCODE_SUCCESS;

    if (!arg) {

      u8 ssid_bak[WM_SIZE_BSSID];
      MI_CpuCopy8(driver->target_bss->ssid, ssid_bak, sizeof(ssid_bak));
      (void)WXCi_CallDriverEvent(driver, WXC_DRIVER_EVENT_CONNECTING,
                                 driver->target_bss);
      MI_CpuCopy8(driver->target_bss->ssid + 8, driver->ssid,
                  WM_SIZE_CHILD_SSID);
      MI_CpuCopy8(ssid_bak, driver->target_bss->ssid, sizeof(ssid_bak));

      driver->state = WXC_DRIVER_STATE_BUSY;
      wmResult =
          WM_StartConnect(WXCi_ChildIndicate, driver->target_bss, driver->ssid);
      (void)WXCi_CheckWmApiResult(driver, WM_APIID_START_CONNECT, wmResult);
    }

    else if (cb->apiid == WM_APIID_START_CONNECT) {
      WMStartConnectCallback *cb = (WMStartConnectCallback *)arg;

      driver->own_aid = cb->aid;
      wmResult = WM_StartMP(WXCi_StartChildProc, (u16 *)driver->mp_recv_work,
                            driver->recv_size_max, (u16 *)driver->mp_send_work,
                            driver->send_size_max,
                            (u16)(driver->parent_param->CS_Flag ? 0 : 1));
      (void)WXCi_CheckWmApiResult(driver, WM_APIID_START_MP, wmResult);
    }

    else if (cb->apiid == WM_APIID_START_MP) {
      WMStartMPCallback *cb = (WMStartMPCallback *)arg;
      switch (cb->state) {
      case WM_STATECODE_MP_START:
        WXCi_OnStateChanged(driver, WXC_DRIVER_STATE_CHILD, cb);
        break;
      }
    }
  }
}

static void WXCi_MeasureProc(void *arg) {
  WXCDriverWork *const driver = work;
  WMMeasureChannelCallback *cb = (WMMeasureChannelCallback *)arg;
  u16 channel = 0;

  if (!arg) {
    driver->state = WXC_DRIVER_STATE_BUSY;
    driver->current_channel = 0;
    driver->measure_ratio_min = WXC_MAX_RATIO + 1;
  } else if (WXCi_CheckWmCallbackResult(driver, cb))

  {
    channel = cb->channel;

    if (driver->measure_ratio_min > cb->ccaBusyRatio) {
      driver->measure_ratio_min = cb->ccaBusyRatio;
      driver->current_channel = channel;
    }

    if (channel == (32 - MATH_CountLeadingZeros(WM_GetAllowedChannel()))) {
      driver->need_measure_channel = FALSE;

      WXCi_StartParentProc(NULL);
      return;
    }
  }

  else {
    driver->need_measure_channel = FALSE;
  }

  if (driver->need_measure_channel) {

    const u16 WH_MEASURE_TIME = 30;

    const u16 WH_MEASURE_CS_OR_ED = 3;

    const u16 WH_MEASURE_ED_THRESHOLD = 17;
    WMErrCode ret;

    channel = WXC_GetNextAllowedChannel(channel);
    ret = WM_MeasureChannel(WXCi_MeasureProc, WH_MEASURE_CS_OR_ED,
                            WH_MEASURE_ED_THRESHOLD, channel, WH_MEASURE_TIME);
    (void)WXCi_CheckWmApiResult(driver, WM_APIID_MEASURE_CHANNEL, ret);
  }
}

static void WXCi_ScanProc(void *arg) {
  WXCDriverWork *const driver = work;
  WMCallback *cb = (WMCallback *)arg;

  static u16 scan_channelList;

  if (!arg || WXCi_CheckWmCallbackResult(driver, arg)) {
    WMErrCode wmResult = WM_ERRCODE_SUCCESS;

    if (!arg) {
      driver->state = WXC_DRIVER_STATE_BUSY;
      driver->scan_found_num = 0;

      driver->scan_param->scanBuf = (WMBssDesc *)driver->scan_buf;
      driver->scan_param->scanBufSize = WM_SIZE_SCAN_EX_BUF;

      scan_channelList = WM_GetAllowedChannel();
      driver->scan_param->channelList =
          (u16)MATH_GetLeastSignificantBit(scan_channelList);
      scan_channelList = (u16)(scan_channelList ^
                               MATH_GetLeastSignificantBit(scan_channelList));

      driver->scan_param->maxChannelTime = WXC_SCAN_TIME_MAX;
      MI_CpuFill8(driver->scan_param->bssid, 0xFF,
                  sizeof(driver->scan_param->bssid));
      driver->scan_param->scanType = WM_SCANTYPE_PASSIVE;
      driver->scan_param->ssidLength = 0;
      MI_CpuFill8(driver->scan_param->ssid, 0xFF,
                  sizeof(driver->scan_param->ssid));
      wmResult = WM_StartScanEx(WXCi_ScanProc, driver->scan_param);
      (void)WXCi_CheckWmApiResult(driver, WM_APIID_START_SCAN_EX, wmResult);
    }

    else if (cb->apiid == WM_APIID_START_SCAN_EX) {
      WMStartScanExCallback *cb = (WMStartScanExCallback *)arg;

      if (cb->state == WM_STATECODE_PARENT_FOUND) {
        DC_InvalidateRange(driver->scan_buf, WM_SIZE_SCAN_EX_BUF);
        driver->scan_found_num = cb->bssDescCount;
      }
      wmResult = WM_EndScan(WXCi_ScanProc);
      (void)WXCi_CheckWmApiResult(driver, WM_APIID_END_SCAN, wmResult);
    }

    else if (cb->apiid == WM_APIID_END_SCAN) {
      BOOL ret = FALSE;

      if (driver->target == WXC_DRIVER_STATE_CHILD) {
        int i;
        const u8 *scan_buf = driver->scan_buf;

        WXC_DRIVER_LOG("found:%d beacons\n", driver->scan_found_num);
        for (i = 0; i < driver->scan_found_num; ++i) {
          const WMBssDesc *p_desc = (const WMBssDesc *)scan_buf;
          const int len = p_desc->length * 2;
          BOOL is_valid;
          is_valid = WM_IsValidGameBeacon(p_desc);
          WXC_DRIVER_LOG("   GGID=%08X(%2dch:%3dBYTE)\n",
                         is_valid ? p_desc->gameInfo.ggid : 0xFFFFFFFF,
                         p_desc->channel, len);
          if (is_valid) {

            ret = (BOOL)WXCi_CallDriverEvent(
                driver, WXC_DRIVER_EVENT_BEACON_RECV, (void *)p_desc);
            if (ret) {
              WXC_DRIVER_LOG("     -> matched!\n");
              MI_CpuCopy8(p_desc, driver->target_bss, sizeof(WMBssDesc));
              break;
            }
          }
          scan_buf += MATH_ROUNDUP(len, 4);
        }

        if ((ret == FALSE) &&
            (MATH_GetLeastSignificantBit(scan_channelList) != 0)) {
          driver->scan_found_num = 0;
          driver->scan_param->channelList =
              (u16)MATH_GetLeastSignificantBit(scan_channelList);
          scan_channelList =
              (u16)(scan_channelList ^
                    MATH_GetLeastSignificantBit(scan_channelList));
          wmResult = WM_StartScanEx(WXCi_ScanProc, driver->scan_param);
          (void)WXCi_CheckWmApiResult(driver, WM_APIID_START_SCAN_EX, wmResult);
          return;
        }
      }

      if (ret) {
        WXCi_StartChildProc(NULL);
      }

      else {
        if (driver->target == WXC_DRIVER_STATE_CHILD) {
          driver->target = WXC_DRIVER_STATE_IDLE;
        }
        WXCi_OnStateChanged(driver, WXC_DRIVER_STATE_IDLE, NULL);
      }
    }
  }
}

void WXC_InitDriver(WXCDriverWork *driver, WMParentParam *pp,
                    WXCDriverEventFunc func, u32 dma) {

  {
    OSIntrMode bak_cpsr = OS_EnableInterrupts();
    (void)WM_GetNextTgid();
    (void)OS_RestoreInterrupts(bak_cpsr);
  }

  work = driver;
  MI_CpuClear32(driver, sizeof(WXCDriverWork));
  driver->own_aid = 0;
  driver->send_busy = TRUE;
  driver->callback = func;
  driver->wm_dma = (u16)dma;
  driver->send_size_max = (u16)sizeof(driver->mp_send_work);
  driver->recv_size_max = (u16)sizeof(driver->mp_recv_work);
  driver->state = WXC_DRIVER_STATE_END;
  driver->parent_param = pp;
  driver->parent_param->entryFlag = 1;
  driver->parent_param->beaconPeriod = WXC_BEACON_PERIOD;
  driver->parent_param->channel = 1;
}

void WXC_SetDriverTarget(WXCDriverWork *driver, WXCDriverState target) {
  driver->target = target;

  if ((driver->state != WXC_DRIVER_STATE_BUSY) &&
      (driver->state != driver->target)) {
    WXCi_OnStateChanged(driver, driver->state, NULL);
  }
}

void WXC_SetDriverSsid(WXCDriverWork *driver, const void *buffer, u32 length) {
  length = (u32)MATH_MIN(length, WM_SIZE_CHILD_SSID);
  MI_CpuCopy8(buffer, driver->ssid, length);
  MI_CpuFill8(driver->ssid + length, 0x00, (u32)(WM_SIZE_CHILD_SSID - length));
}

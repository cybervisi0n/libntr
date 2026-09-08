#include <nitro.h>

#include <nitro/wxc.h>

typedef struct WXCWork {
  u32 wm_dma;
  WXCCallback system_callback;
  WXCScheduler scheduler;
  WXCUserInfo user_info[WM_NUM_MAX_CHILD + 1];
  BOOL stopping;
  u8 beacon_count; /* Number of beacon sends */
  u8 padding[3];
  WMParentParam parent_param ATTRIBUTE_ALIGN(32);
  WXCDriverWork driver ATTRIBUTE_ALIGN(32);
  WXCProtocolContext protocol[1] ATTRIBUTE_ALIGN(32);
} WXCWork;

SDK_STATIC_ASSERT(sizeof(WXCWork) <= WXC_WORK_SIZE);

static WXCStateCode state = WXC_STATE_END;
static WXCWork *work = NULL;

static inline void WXCi_ChangeState(WXCStateCode stat, void *arg) {
  OSIntrMode bak_cpsr = OS_DisableInterrupts();
  {
    state = stat;
    if (work->system_callback) {
      (*work->system_callback)(state, arg);
    }
  }
  (void)OS_RestoreInterrupts(bak_cpsr);
}

static void WXCi_SeekNextBlock(void) {

  WXCProtocolRegistry *target = WXC_FindNextBlock(
      work->protocol, work->protocol->current_block, 0, FALSE);
  if (!target) {

    target = WXC_FindNextBlock(work->protocol, NULL, 0, TRUE);
  }

  if (target != NULL) {
    WXC_SetCurrentBlock(work->protocol, target);
    work->parent_param.ggid = target->ggid;
  }
}

static u32 WXCi_DriverEventCallback(WXCDriverEvent event, void *arg) {
  u32 result = 0;

  switch (event) {
  case WXC_DRIVER_EVENT_BEACON_SEND:

    if (!work->stopping && (WXC_GetStateCode() == WXC_STATE_ACTIVE)) {
      WXC_BeaconSendHook(work->protocol, &work->parent_param);

      if (work->driver.peer_bitmap == 0) {
        if (++work->beacon_count > WXC_PARENT_BEACON_SEND_COUNT_OUT) {
          work->beacon_count = 0;

          if (WXCi_UpdateScheduler(&work->scheduler)) {
          }

          else {
            WXCi_StartChild(&work->driver);
          }
        }
      }
    }
    break;

  case WXC_DRIVER_EVENT_BEACON_RECV:

  {
    WXCBeaconFoundCallback cb;

    cb.bssdesc = (const WMBssDesc *)arg;
    cb.matched = WXC_BeaconRecvHook(work->protocol, cb.bssdesc);

    (*work->system_callback)(WXC_STATE_BEACON_RECV, &cb);
    result = (u32)cb.matched;

    if (result) {
      work->parent_param.ggid = WXC_GetCurrentBlock(work->protocol)->ggid;
    }
  } break;

  case WXC_DRIVER_EVENT_STATE_END:

    work->stopping = FALSE;
    WXCi_ChangeState(WXC_STATE_END, work);
    break;

  case WXC_DRIVER_EVENT_STATE_STOP:

    work->stopping = FALSE;
    WXCi_ChangeState(WXC_STATE_READY, NULL);
    break;

  case WXC_DRIVER_EVENT_STATE_IDLE:

    if (WXC_GetStateCode() != WXC_STATE_ACTIVE) {
      WXCi_End(&work->driver);
    }

    else if (work->stopping) {
      WXCi_Stop(&work->driver);
    }

    else if (WXCi_UpdateScheduler(&work->scheduler)) {

      WXCi_SeekNextBlock();

      WXC_BeaconSendHook(work->protocol, &work->parent_param);

      WXCi_StartParent(&work->driver);
    } else {
      WXCi_StartChild(&work->driver);
    }
    break;

  case WXC_DRIVER_EVENT_STATE_PARENT:
  case WXC_DRIVER_EVENT_STATE_CHILD:

    if (WXC_GetStateCode() != WXC_STATE_ACTIVE) {
      WXCi_End(&work->driver);
    }

    else if (work->stopping) {
      WXCi_Stop(&work->driver);
    }
    work->beacon_count = 0;

    {
      WXCUserInfo *p_user = &work->user_info[work->driver.own_aid];
      p_user->aid = work->driver.own_aid;
      OS_GetMacAddress(p_user->macAddress);
    }
    break;

  case WXC_DRIVER_EVENT_CONNECTING:

  {
    WMBssDesc *bss = (WMBssDesc *)arg;
    WXC_CallPreConnectHook(work->protocol, bss, &bss->ssid[8]);
  } break;

  case WXC_DRIVER_EVENT_CONNECTED:

  {
    const WXCProtocolRegistry *block = WXC_GetCurrentBlock(work->protocol);
    WMStartParentCallback *cb = (WMStartParentCallback *)arg;
    const BOOL is_parent = WXC_IsParentMode();
    const u16 parent_size =
        (u16)(is_parent ? work->parent_param.parentMaxSize
                        : WXCi_GetParentBssDesc(&work->driver)
                              ->gameInfo.parentMaxSize);
    const u16 child_size =
        (u16)(is_parent ? work->parent_param.childMaxSize
                        : WXCi_GetParentBssDesc(&work->driver)
                              ->gameInfo.childMaxSize);
    const u16 aid = (u16)(is_parent ? cb->aid : 0);
    WXCUserInfo *p_user = &work->user_info[aid];
    const BOOL is_valid_block =
        (!work->stopping && (WXC_GetStateCode() == WXC_STATE_ACTIVE) && block);

    p_user->aid = aid;
    if (is_parent) {
      WM_CopyBssid16(cb->macAddress, p_user->macAddress);
    } else {
      const WMBssDesc *p_bss = WXCi_GetParentBssDesc(&work->driver);
      WM_CopyBssid16(p_bss->bssid, p_user->macAddress);
    }

    WXC_ResetSequence(work->protocol, parent_size, child_size);

    if (is_valid_block) {

      if ((block->send.buffer != NULL) && (block->recv.buffer != NULL)) {
        WXC_AddBlockSequence(work->protocol, block->send.buffer,
                             block->send.length, block->recv.buffer,
                             block->recv.length);
      }
    }

    WXC_ConnectHook(work->protocol, (u16)(1 << aid));

    if (is_valid_block) {
      (*work->system_callback)(WXC_STATE_CONNECTED, p_user);
    }

  } break;

  case WXC_DRIVER_EVENT_DISCONNECTED:

    WXC_DisconnectHook(work->protocol, (u16)(u32)arg);

    if (!WXC_GetUserBitmap()) {
      if ((WXC_GetStateCode() == WXC_STATE_ACTIVE) ||
          (WXC_GetStateCode() == WXC_STATE_ENDING) ||
          !WXC_GetCurrentBlock(work->protocol)) {
        work->beacon_count = 0;
        WXCi_Reset(&work->driver);
      }
    }
    break;

  case WXC_DRIVER_EVENT_DATA_SEND:

    WXC_PacketSendHook(work->protocol, (WXCPacketInfo *)arg);
    break;

  case WXC_DRIVER_EVENT_DATA_RECV:

    (void)WXC_PacketRecvHook(work->protocol, (const WXCPacketInfo *)arg);

    if (!work->protocol->current_block->impl->IsExecuting(work->protocol)) {
      WXCi_Reset(&work->driver);
    }
    break;

  default:
    OS_TPanic("unchecked event implementations!");
    break;
  }

  return result;
}

void WXC_Init(void *work_mem, WXCCallback callback, u32 dma) {
  OSIntrMode bak_cpsr = OS_DisableInterrupts();

  static BOOL initialized = FALSE;
  if (!initialized) {
    WXC_InstallProtocolImpl(WXCi_GetProtocolImplCommon());
    WXC_InstallProtocolImpl(WXCi_GetProtocolImplWPB());
    WXC_InstallProtocolImpl(WXCi_GetProtocolImplWXC());
    initialized = TRUE;
  }
  {
    if (WXC_GetStateCode() == WXC_STATE_END) {
      if (((u32)work_mem & 31) != 0) {
        OS_TPanic("WXC work buffer must be 32-byte aligned!");
      }
      work = (WXCWork *)work_mem;
      MI_CpuClear32(work, sizeof(WXCWork));
      work->wm_dma = dma;
      work->system_callback = callback;
      WXCi_InitScheduler(&work->scheduler);
      WXC_InitProtocol(work->protocol);

      work->parent_param.maxEntry =
          1; /* Currently only supports 1 to 1 connections */
      work->parent_param.parentMaxSize = WXC_PACKET_SIZE_MAX;
      work->parent_param.childMaxSize = WXC_PACKET_SIZE_MAX;
      work->parent_param.CS_Flag = 1; /* Continuous transmission */
      WXC_InitDriver(&work->driver, &work->parent_param,
                     WXCi_DriverEventCallback, work->wm_dma);
      WXCi_ChangeState(WXC_STATE_READY, NULL);
    }
  }
  (void)OS_RestoreInterrupts(bak_cpsr);
}

BOOL WXC_SetParentParameter(u16 sendSize, u16 recvSize, u16 maxEntry) {
  BOOL ret = FALSE;

  if (maxEntry > 1) {
    OS_TWarning("unsupported maxEntry. (must be 1)\n");
  }

  else if ((sendSize < WXC_PACKET_SIZE_MIN) ||
           (sendSize > WXC_PACKET_SIZE_MAX) ||
           (recvSize < WXC_PACKET_SIZE_MIN) ||
           (recvSize > WXC_PACKET_SIZE_MAX)) {
    OS_TWarning("packet size is out of range.\n");
  }

  else {

    int usec =
        330 + 4 * (sendSize + 38) + maxEntry * (112 + 4 * (recvSize + 32));
    const int max_time = 5600;
    if (usec >= max_time) {
      OS_TWarning("specified MP setting takes %d[usec]. (should be lower than "
                  "%d[usec])\n",
                  usec, max_time);
    } else {
      work->parent_param.parentMaxSize = sendSize;
      work->parent_param.childMaxSize = recvSize;
      ret = TRUE;
    }
  }
  return ret;
}

void WXC_Start(void) {
  OSIntrMode bak_cpsr = OS_DisableInterrupts();
  {
    SDK_ASSERT(WXC_GetStateCode() != WXC_STATE_END);

    if (WXC_GetStateCode() == WXC_STATE_READY) {
      WXCi_Reset(&work->driver);
      WXCi_ChangeState(WXC_STATE_ACTIVE, NULL);
    }
  }
  (void)OS_RestoreInterrupts(bak_cpsr);
}

void WXC_Stop(void) {
  OSIntrMode bak_cpsr = OS_DisableInterrupts();
  {
    SDK_ASSERT(WXC_GetStateCode() != WXC_STATE_END);

    switch (WXC_GetStateCode()) {

    case WXC_STATE_ACTIVE:
      if (!work->stopping) {
        work->stopping = TRUE;

        if (WXC_GetUserBitmap() == 0) {
          WXCi_Stop(&work->driver);
        }
      }
      break;

    case WXC_STATE_READY:
    case WXC_STATE_ENDING:
    case WXC_STATE_END:

      break;
    }
  }
  (void)OS_RestoreInterrupts(bak_cpsr);
}

void WXC_End(void) {
  OSIntrMode bak_cpsr = OS_DisableInterrupts();
  {
    switch (WXC_GetStateCode()) {

    case WXC_STATE_READY:
    case WXC_STATE_ACTIVE:
      WXCi_ChangeState(WXC_STATE_ENDING, NULL);

      if (WXC_GetUserBitmap() == 0) {
        WXCi_End(&work->driver);
      }
      break;

    case WXC_STATE_ENDING:
    case WXC_STATE_END:

      break;
    }
  }
  (void)OS_RestoreInterrupts(bak_cpsr);
}

WXCStateCode WXC_GetStateCode(void) { return state; }

BOOL WXC_IsParentMode(void) { return WXCi_IsParentMode(&work->driver); }

const WMParentParam *WXC_GetParentParam(void) { return &work->parent_param; }

const WMBssDesc *WXC_GetParentBssDesc(void) {
  return WXCi_GetParentBssDesc(&work->driver);
}

u16 WXC_GetUserBitmap(void) {
  u16 bitmap = work->driver.peer_bitmap;
  if (bitmap != 0) {
    bitmap = (u16)(bitmap | (1 << work->driver.own_aid));
  }
  return bitmap;
}

u32 WXC_GetCurrentGgid(void) {
  u32 ggid = 0;
  {
    OSIntrMode bak_cpsr = OS_DisableInterrupts();
    WXCProtocolRegistry *current = WXC_GetCurrentBlock(work->protocol);
    if (current) {
      ggid = current->ggid;

      if (ggid == WXC_GGID_COMMON_ANY) {
        ggid = work->protocol->target_ggid;
      }
    }
    (void)OS_RestoreInterrupts(bak_cpsr);
  }
  return ggid;
}

u16 WXC_GetOwnAid(void) { return work->driver.own_aid; }

const WXCUserInfo *WXC_GetUserInfo(u16 aid) {
  const WXCUserInfo *ret = NULL;

  {
    OSIntrMode bak_cpsr = OS_DisableInterrupts();
    if ((aid <= WM_NUM_MAX_CHILD) &&
        ((WXC_GetUserBitmap() & (1 << aid)) != 0)) {
      ret = &work->user_info[aid];
    }
    (void)OS_RestoreInterrupts(bak_cpsr);
  }
  return ret;
}

void WXC_SetChildMode(BOOL enable) {
  SDK_ASSERT(WXC_GetStateCode() != WXC_STATE_ACTIVE);
  WXCi_SetChildMode(&work->scheduler, enable);
}

void WXC_AddData(const void *send_buf, u32 send_size, void *recv_buf,
                 u32 recv_max) {
  WXC_AddBlockSequence(work->protocol, send_buf, send_size, recv_buf, recv_max);
}

void WXC_RegisterDataEx(u32 ggid, WXCCallback callback, u8 *send_ptr,
                        u32 send_size, u8 *recv_ptr, u32 recv_size,
                        const char *type) {
  OSIntrMode bak_cpsr = OS_DisableInterrupts();

  WXCProtocolRegistry *same_ggid =
      WXC_FindNextBlock(work->protocol, NULL, ggid, TRUE);
  if (same_ggid != NULL) {
    OS_TWarning("already registered same GGID!");
  }

  else {
    WXCProtocolRegistry *p_data =
        WXC_FindNextBlock(work->protocol, NULL, 0, TRUE);

    if (!p_data) {
      OS_TPanic("no more memory to register data!");
    } else {

      WXCProtocolImpl *impl = WXC_FindProtocolImpl(type);
      if (!impl) {
        OS_TPanic("unknown protocol \"%s\"!", type);
      }
      WXC_InitProtocolRegistry(p_data, ggid, callback, impl);
      WXC_SetInitialExchangeBuffers(p_data, send_ptr, send_size, recv_ptr,
                                    recv_size);
    }
  }

  (void)OS_RestoreInterrupts(bak_cpsr);
}

void WXC_SetInitialData(u32 ggid, u8 *send_ptr, u32 send_size, u8 *recv_ptr,
                        u32 recv_size) {
  OSIntrMode bak_cpsr = OS_DisableInterrupts();

  WXCProtocolRegistry *target =
      WXC_FindNextBlock(work->protocol, NULL, ggid, TRUE);
  if (target) {
    WXC_SetInitialExchangeBuffers(target, send_ptr, send_size, recv_ptr,
                                  recv_size);
  }
  (void)OS_RestoreInterrupts(bak_cpsr);
}

void WXC_UnregisterData(u32 ggid) {
  OSIntrMode bak_cpsr = OS_DisableInterrupts();

  WXCProtocolRegistry *target =
      WXC_FindNextBlock(work->protocol, NULL, ggid, TRUE);

  if (!target) {
    target = WXC_FindNextBlock(work->protocol, NULL,
                               (u32)(ggid | WXC_GGID_COMMON_BIT), TRUE);
  }
  if (target) {

    if ((WXC_GetUserBitmap() != 0) &&
        (target == WXC_GetCurrentBlock(work->protocol))) {
      OS_TWarning("specified data is now using.");
    } else {

      target->ggid = 0;
      target->callback = NULL;
      target->send.buffer = NULL;
      target->send.length = 0;
      target->recv.buffer = NULL;
      target->recv.length = 0;
    }
  }
  (void)OS_RestoreInterrupts(bak_cpsr);
}

void WXCi_SetSsid(const void *buffer, u32 length) {
  OSIntrMode bak_cpsr = OS_DisableInterrupts();
  WXC_SetDriverSsid(&work->driver, buffer, length);
  (void)OS_RestoreInterrupts(bak_cpsr);
}

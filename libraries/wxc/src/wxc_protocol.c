#include <nitro.h>

#include <nitro/wxc/common.h>
#include <nitro/wxc/protocol.h>

static WXCProtocolImpl *impl_list;

void WXC_InitProtocol(WXCProtocolContext *protocol) {
  protocol->current_block = NULL;
  MI_CpuClear32(protocol, sizeof(protocol));
}

void WXC_InstallProtocolImpl(WXCProtocolImpl *impl) {
  WXCProtocolImpl *p = impl_list;
  if (!p) {
    impl_list = impl;
    impl->next = NULL;
  } else {
    for (;; p = p->next) {
      if (p == impl) {
        break;
      } else if (!p->next) {
        p->next = impl;
        impl->next = NULL;
        break;
      }
    }
  }
}

WXCProtocolImpl *WXC_FindProtocolImpl(const char *name) {
  WXCProtocolImpl *p = impl_list;
  for (; p; p = p->next) {
    if (STD_CompareString(name, p->name) == 0) {
      break;
    }
  }
  return p;
}

void WXC_ResetSequence(WXCProtocolContext *protocol, u16 send_max,
                       u16 recv_max) {
  WXC_GetCurrentBlockImpl(protocol)->Init(protocol, send_max, recv_max);
}

void WXC_AddBlockSequence(WXCProtocolContext *protocol, const void *send_buf,
                          u32 send_size, void *recv_buf, u32 recv_max) {
  int result;
  result = WXC_GetCurrentBlockImpl(protocol)->AddData(
      protocol, send_buf, send_size, recv_buf, recv_max);
  SDK_TASSERTMSG(result, "sequence is now busy.");
}

WXCProtocolRegistry *WXC_FindNextBlock(WXCProtocolContext *protocol,
                                       const WXCProtocolRegistry *from,
                                       u32 ggid, BOOL match) {
  WXCProtocolRegistry *target;

  if (!from) {
    from = &protocol->data_array[WXC_REGISTER_DATA_MAX - 1];
  }

  target = (WXCProtocolRegistry *)from;
  for (;;) {
    BOOL eq;

    if (++target >= &protocol->data_array[WXC_REGISTER_DATA_MAX]) {
      target = &protocol->data_array[0];
    }

    eq = (target->ggid == ggid);
    if ((match && eq) || (!match && !eq)) {
      break;
    }

    if (target == from) {
      target = NULL;
      break;
    }
  }
  return target;
}

void WXC_BeaconSendHook(WXCProtocolContext *protocol, WMParentParam *p_param) {
  WXC_GetCurrentBlockImpl(protocol)->BeaconSend(protocol, p_param);
}

BOOL WXC_BeaconRecvHook(WXCProtocolContext *protocol, const WMBssDesc *p_desc) {
  BOOL ret = FALSE;

  u32 ggid = p_desc->gameInfo.ggid;
  if (ggid != 0) {

    WXCProtocolRegistry *found = NULL;
    int i;
    for (i = 0; i < WXC_REGISTER_DATA_MAX; ++i) {
      WXCProtocolRegistry *p = &protocol->data_array[i];

      if (p->ggid == ggid) {
        found = p;
        break;
      }

      else if (WXC_IsCommonGgid(ggid) && WXC_IsCommonGgid(p->ggid)) {

        const BOOL is_target_any = (ggid == WXC_GGID_COMMON_PARENT);
        const BOOL is_current_any = (p->ggid == WXC_GGID_COMMON_ANY);
        if (is_target_any) {
          if (!is_current_any) {
            ggid = p->ggid;
            found = p;
            break;
          }
        } else {
          if (is_current_any) {
            found = p;
            break;
          }
        }
      }
    }

    if (found) {
      ret = found->impl->BeaconRecv(protocol, p_desc);

      if (ret) {
        WXC_SetCurrentBlock(protocol, found);
        protocol->target_ggid = ggid;
      }
    }
  }
  return ret;
}

void WXC_PacketSendHook(WXCProtocolContext *protocol, WXCPacketInfo *packet) {
  WXC_GetCurrentBlockImpl(protocol)->PacketSend(protocol, packet);
}

BOOL WXC_PacketRecvHook(WXCProtocolContext *protocol,
                        const WXCPacketInfo *packet) {
  int ret = FALSE;

  ret = WXC_GetCurrentBlockImpl(protocol)->PacketRecv(protocol, packet);

  if (ret) {

    WXCCallback callback = protocol->current_block->callback;
    if (callback) {
      (*callback)(WXC_STATE_EXCHANGE_DONE, &protocol->recv);
    }
  }

  return ret;
}

void WXC_ConnectHook(WXCProtocolContext *protocol, u16 bitmap) {
#pragma unused(protocol)
#pragma unused(bitmap)
}

void WXC_DisconnectHook(WXCProtocolContext *protocol, u16 bitmap) {
#pragma unused(protocol)
#pragma unused(bitmap)
}

void WXC_CallPreConnectHook(WXCProtocolContext *protocol, WMBssDesc *p_desc,
                            u8 *ssid) {
  WXCProtocolImpl *const impl = WXC_GetCurrentBlockImpl(protocol);
  if (impl->PreConnectHook) {
    impl->PreConnectHook(protocol, p_desc, ssid);
  }
}

void WXC_InitProtocolRegistry(WXCProtocolRegistry *p_data, u32 ggid,
                              WXCCallback callback, WXCProtocolImpl *impl) {
  p_data->ggid = ggid;
  p_data->callback = callback;
  p_data->impl = impl;
  p_data->send.buffer = NULL;
  p_data->send.length = 0;
  p_data->recv.buffer = NULL;
  p_data->recv.length = 0;
}

void WXC_SetInitialExchangeBuffers(WXCProtocolRegistry *p_data, u8 *send_ptr,
                                   u32 send_size, u8 *recv_ptr, u32 recv_size) {
  p_data->send.buffer = send_ptr;
  p_data->send.length = (u32)send_size;

  p_data->recv.buffer = recv_ptr;
  p_data->recv.length = (u32)recv_size;
}

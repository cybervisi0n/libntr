#include <nitro.h>

#include <nitro/wxc/common.h>
#include <nitro/wxc/protocol.h>
#include <nitro/wxc/wxc_protocol_impl_wxc.h>

static void WXCi_BeaconSendHook(WXCProtocolContext *protocol,
                                WMParentParam *p_param);
static BOOL WXCi_BeaconRecvHook(WXCProtocolContext *protocol,
                                const WMBssDesc *p_desc);
static void WXCi_PacketSendHook(WXCProtocolContext *protocol,
                                WXCPacketInfo *packet);
static BOOL WXCi_PacketRecvHook(WXCProtocolContext *protocol,
                                const WXCPacketInfo *packet);
static void WXCi_InitSequence(WXCProtocolContext *protocol, u16 send_max,
                              u16 recv_max);
static BOOL WXCi_AddData(WXCProtocolContext *protocol, const void *send_buf,
                         u32 send_size, void *recv_buf, u32 recv_max);
static BOOL WXCi_IsExecuting(WXCProtocolContext *protocol);

static WXCImplWorkWxc impl_wxc_work;

static WXCProtocolImpl impl_wxc = {
    "WXC",
    WXCi_BeaconSendHook,
    WXCi_BeaconRecvHook,
    NULL,
    WXCi_PacketSendHook,
    WXCi_PacketRecvHook,
    WXCi_InitSequence,
    WXCi_AddData,
    WXCi_IsExecuting,
    &impl_wxc_work,
};

WXCProtocolImpl *WXCi_GetProtocolImplWXC(void) { return &impl_wxc; }

void WXCi_BeaconSendHook(WXCProtocolContext *protocol, WMParentParam *p_param) {
#pragma unused(protocol)
#pragma unused(p_param)
}

BOOL WXCi_BeaconRecvHook(WXCProtocolContext *protocol,
                         const WMBssDesc *p_desc) {
#pragma unused(protocol)
#pragma unused(p_desc)

  return TRUE;
}

void WXCi_PacketSendHook(WXCProtocolContext *protocol, WXCPacketInfo *packet) {
  WXCImplWorkWxc *wxc_work = WXC_GetCurrentBlockImpl(protocol)->impl_work;
  WXCBlockHeader *p_header = (WXCBlockHeader *)packet->buffer;

  WXC_PACKET_LOG("--SEND:ACK=(%3d,%d,%04X),REQ=(%3d,%d,%04X)\n",
                 wxc_work->ack.phase, wxc_work->ack.command,
                 wxc_work->ack.index, wxc_work->req.phase,
                 wxc_work->req.command, wxc_work->req.index);

  p_header->req = wxc_work->req;
  p_header->ack = wxc_work->ack;

  if (wxc_work->ack.phase == wxc_work->req.phase) {
    u8 *p_body = packet->buffer + sizeof(WXCBlockHeader);

    switch (wxc_work->ack.command) {
    case WXC_BLOCK_COMMAND_INIT:

      WXC_PACKET_LOG("       INIT(%6d)\n", protocol->send.length);
      *(u16 *)(p_body + 0) = (u16)protocol->send.length;
      *(u16 *)(p_body + 2) = protocol->send.checksum;
      break;
    case WXC_BLOCK_COMMAND_SEND:

    {
      int offset = (wxc_work->ack.index * wxc_work->send_unit);
      u32 len = (u32)(protocol->send.length - offset);
      if (len > wxc_work->send_unit) {
        len = wxc_work->send_unit;
      }
      MI_CpuCopy8((const u8 *)protocol->send.buffer + offset, p_body, len);
    } break;
    }
  }

  packet->length =
      (u16)MATH_ROUNDUP(sizeof(WXCBlockHeader) + wxc_work->send_unit, 2);
}

static void WXCi_MergeBlockData(WXCProtocolContext *protocol, int index,
                                const void *src) {
  WXCImplWorkWxc *wxc_work = WXC_GetCurrentBlockImpl(protocol)->impl_work;
  if (index < wxc_work->recv_total) {
    u32 *bmp = wxc_work->recv_bitmap_buf + (index >> 5);
    u32 bit = (u32)(1 << (index & 31));
    if ((*bmp & bit) == 0) {
      int offset = (index * wxc_work->recv_unit);
      u32 len = (u32)(protocol->recv.length - offset);
      if (len > wxc_work->recv_unit) {
        len = wxc_work->recv_unit;
      }

      if (protocol->recv.buffer != NULL) {
        MI_CpuCopy8(src, (u8 *)protocol->recv.buffer + offset, len);
      }
      *bmp |= bit;

      if (--wxc_work->recv_rest == 0) {
        wxc_work->req.command = WXC_BLOCK_COMMAND_DONE;
      }

      else {
        int i;

        int count = wxc_work->recent_index[0];
        int last_count = count;
        if (last_count >= wxc_work->recv_total) {
          last_count = (int)wxc_work->recv_total - 1;
        }
        for (;;) {

          if (++count >= wxc_work->recv_total) {
            count = 0;
          }

          if (count == last_count) {
            count = wxc_work->recent_index[WXC_RECENT_SENT_LIST_MAX - 1];
            break;
          }

          if ((*(wxc_work->recv_bitmap_buf + (count >> 5)) &
               (1 << (count & 31))) == 0) {

            for (i = 0; i < WXC_RECENT_SENT_LIST_MAX; ++i) {
              if (count == wxc_work->recent_index[i]) {
                break;
              }
            }
            if (i >= WXC_RECENT_SENT_LIST_MAX) {
              break;
            }
          }
        }

        for (i = WXC_RECENT_SENT_LIST_MAX; --i > 0;) {
          wxc_work->recent_index[i] = wxc_work->recent_index[i - 1];
        }
        wxc_work->recent_index[0] = (u16)count;
        wxc_work->req.index = wxc_work->recent_index[0];
      }
    }
  }
}

BOOL WXCi_PacketRecvHook(WXCProtocolContext *protocol,
                         const WXCPacketInfo *packet) {
  WXCImplWorkWxc *wxc_work = WXC_GetCurrentBlockImpl(protocol)->impl_work;
  int ret = FALSE;

  WXCBlockHeader *p_header = (WXCBlockHeader *)packet->buffer;

  if (packet->length >= wxc_work->recv_unit) {
    WXC_PACKET_LOG("--RECV:REQ=(%3d,%d,%04X),ACK=(%3d,%d,%04X)\n",
                   p_header->req.phase, p_header->req.command,
                   p_header->req.index, p_header->ack.phase,
                   p_header->ack.command, p_header->ack.index);

    if (p_header->req.phase == wxc_work->req.phase) {
      wxc_work->ack = p_header->req;
    }

    if (p_header->ack.phase == wxc_work->req.phase) {
      u8 *p_body = packet->buffer + sizeof(WXCBlockHeader);

      switch (p_header->ack.command) {
      case WXC_BLOCK_COMMAND_QUIT:

        wxc_work->executing = FALSE;
        break;
      case WXC_BLOCK_COMMAND_INIT:

        protocol->recv.length = *(u16 *)(p_body + 0);
        protocol->recv.checksum = *(u16 *)(p_body + 2);
        wxc_work->recv_total =
            (u16)((protocol->recv.length + wxc_work->recv_unit - 1) /
                  wxc_work->recv_unit);
        wxc_work->recv_rest = wxc_work->recv_total;
        wxc_work->req.index = 0;

        if (wxc_work->recv_total > 0) {
          wxc_work->req.command = WXC_BLOCK_COMMAND_SEND;
        } else {
          wxc_work->req.command = WXC_BLOCK_COMMAND_DONE;
        }
        WXC_PACKET_LOG("       INIT(%6d)\n", protocol->recv.length);
        break;
      case WXC_BLOCK_COMMAND_SEND:

        WXCi_MergeBlockData(protocol, p_header->ack.index, p_body);
        break;
      }
    }

    if ((p_header->ack.phase == wxc_work->req.phase) &&
        (p_header->ack.command == WXC_BLOCK_COMMAND_DONE) &&
        (wxc_work->ack.command == WXC_BLOCK_COMMAND_DONE)) {

      ++wxc_work->req.phase;

      wxc_work->req.command = WXC_BLOCK_COMMAND_QUIT;
      ret = TRUE;
    }
  }
  return ret;
}

void WXCi_InitSequence(WXCProtocolContext *protocol, u16 send_max,
                       u16 recv_max) {
  WXCImplWorkWxc *wxc_work = WXC_GetCurrentBlockImpl(protocol)->impl_work;

  protocol->send.buffer = NULL;
  protocol->send.length = 0;
  protocol->send.checksum = 0;
  protocol->recv.buffer = NULL;
  protocol->recv.length = 0;
  protocol->recv.buffer_max = 0;

  wxc_work->send_unit = (u16)(send_max - sizeof(WXCBlockHeader));
  wxc_work->recv_unit = (u16)(recv_max - sizeof(WXCBlockHeader));

  {
    int i;
    for (i = 0; i < WXC_RECENT_SENT_LIST_MAX; ++i) {
      wxc_work->recent_index[i] = 0;
    }
  }

  wxc_work->req.phase = 0;
  wxc_work->recv_total = 0;

  wxc_work->req.command = WXC_BLOCK_COMMAND_QUIT;

  wxc_work->ack.phase = 0;
  wxc_work->ack.command = WXC_BLOCK_COMMAND_IDLE;

  MI_CpuClear32(wxc_work->recv_bitmap_buf, sizeof(wxc_work->recv_bitmap_buf));

  wxc_work->executing = TRUE;
}

BOOL WXCi_AddData(WXCProtocolContext *protocol, const void *send_buf,
                  u32 send_size, void *recv_buf, u32 recv_max) {
  WXCImplWorkWxc *wxc_work = WXC_GetCurrentBlockImpl(protocol)->impl_work;

  if (wxc_work->req.command == WXC_BLOCK_COMMAND_QUIT) {
    wxc_work->req.command = WXC_BLOCK_COMMAND_INIT;

    protocol->send.buffer = (void *)send_buf;
    protocol->send.length = (u16)send_size;
    protocol->send.checksum = MATH_CalcChecksum8(send_buf, send_size);

    protocol->recv.buffer = recv_buf;
    protocol->recv.buffer_max = (u16)recv_max;
    MI_CpuClear32(wxc_work->recv_bitmap_buf, sizeof(wxc_work->recv_bitmap_buf));
    return TRUE;
  } else {
    return FALSE;
  }
}

BOOL WXCi_IsExecuting(WXCProtocolContext *protocol) {
  WXCImplWorkWxc *wxc_work = WXC_GetCurrentBlockImpl(protocol)->impl_work;

  return wxc_work->executing;
}

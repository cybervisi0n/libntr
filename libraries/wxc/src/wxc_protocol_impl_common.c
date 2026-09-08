#include <nitro.h>

#include <nitro/wxc/common.h>
#include <nitro/wxc/protocol.h>

typedef struct WXCCommonProtocolBeaconFormat {
  u32 send_length; /* Send data size (fixed for each game) */
} WXCCommonProtocolBeaconFormat;

typedef struct WXCCommonProtocolSsidFormat {
  char header[4]; /* Version information "WXC1" */
  u32 ggid;       /* GGID to declare from child device */
} WXCCommonProtocolSsidFormat;

typedef struct PacketCommandFormat {
  u16 segment : 1; /* always 0 */
  u16 ack : 1;
  u16 turn : 1;
  u16 phase : 5;
  u16 checksum : 8;
  u16 recv_arg;
  u16 send_arg;
} PacketCommandFormat;

typedef struct PacketSegmentFormat {
  u16 segment : 1; /* always 1 */
  u16 recv_arg : 15;
  u16 turn : 1;
  u16 send_arg : 15;
  u8 buffer[0];
} PacketSegmentFormat;

typedef struct WXCCommonProtocolImplContext {

  WXCCommonProtocolBeaconFormat beacon_format;

  u32 parent_send;
  u32 child_send;
  BOOL executing;

  int req_phase;
  u32 req_turn;
  int ack_phase;

  u32 own_packet;
  u32 peer_packet;
  u32 req_index;
  u32 ack_index;
  u32 recv_count;
  u32 recv_total;
  u32 recv_bitset[WXC_MAX_DATA_SIZE / (8 * 4) + 1];

#if defined(DEBUG_CALCSPEED)

  OSTick tick_count;
#endif
} WXCCommonProtocolImplContext;

static void WXCi_BeaconSendHook(WXCProtocolContext *protocol,
                                WMParentParam *p_param);
static BOOL WXCi_BeaconRecvHook(WXCProtocolContext *protocol,
                                const WMBssDesc *p_desc);
static void WXCi_PreConnectHook(WXCProtocolContext *protocol,
                                const WMBssDesc *p_desc, u8 *ssid);
static void WXCi_PacketSendHook(WXCProtocolContext *protocol,
                                WXCPacketInfo *packet);
static BOOL WXCi_PacketRecvHook(WXCProtocolContext *protocol,
                                const WXCPacketInfo *packet);
static void WXCi_InitSequence(WXCProtocolContext *protocol, u16 send_max,
                              u16 recv_max);
static BOOL WXCi_AddData(WXCProtocolContext *protocol, const void *send_buf,
                         u32 send_size, void *recv_buf, u32 recv_max);
static BOOL WXCi_IsExecuting(WXCProtocolContext *protocol);

enum { PHASE_IDLE, PHASE_QUIT, PHASE_INIT, PHASE_DATA, PHASE_DONE };

static WXCCommonProtocolImplContext work_common ATTRIBUTE_ALIGN(32);

static WXCProtocolImpl impl_common = {
    "COMMON",
    WXCi_BeaconSendHook,
    WXCi_BeaconRecvHook,
    WXCi_PreConnectHook,
    WXCi_PacketSendHook,
    WXCi_PacketRecvHook,
    WXCi_InitSequence,
    WXCi_AddData,
    WXCi_IsExecuting,
    &work_common,
};

void WXCi_BeaconSendHook(WXCProtocolContext *protocol, WMParentParam *p_param) {

  WXCCommonProtocolImplContext *work =
      (WXCCommonProtocolImplContext *)WXC_GetCurrentBlockImpl(protocol)
          ->impl_work;
  WXCProtocolRegistry *const reg = WXC_GetCurrentBlock(protocol);

  work->beacon_format.send_length = reg->send.length;
  p_param->userGameInfo = (u16 *)&work->beacon_format;
  p_param->userGameInfoLength = sizeof(work->beacon_format);
}

BOOL WXCi_BeaconRecvHook(WXCProtocolContext *protocol,
                         const WMBssDesc *p_desc) {
#pragma unused(protocol)
#pragma unused(p_desc)

  return TRUE;
}

void WXCi_PreConnectHook(WXCProtocolContext *protocol, const WMBssDesc *p_desc,
                         u8 ssid[WM_SIZE_CHILD_SSID]) {
#pragma unused(p_desc)

  WXCCommonProtocolSsidFormat *p = (WXCCommonProtocolSsidFormat *)ssid;
  WXCProtocolRegistry *const reg = WXC_GetCurrentBlock(protocol);
  p->header[0] = 'W';
  p->header[1] = 'X';
  p->header[2] = 'C';
  p->header[3] = '1';
  p->ggid = reg->ggid;
}

void WXCi_InitSequence(WXCProtocolContext *protocol, u16 send_max,
                       u16 recv_max) {
  WXCCommonProtocolImplContext *work =
      (WXCCommonProtocolImplContext *)WXC_GetCurrentBlockImpl(protocol)
          ->impl_work;

  protocol->send.buffer = NULL;
  protocol->send.length = 0;
  protocol->send.buffer_max = 0;
  protocol->send.checksum = 0;
  protocol->recv.buffer = NULL;
  protocol->recv.length = 0;
  protocol->recv.buffer_max = 0;
  protocol->recv.checksum = 0;

  work->parent_send = send_max;
  work->child_send = recv_max;

  work->req_phase = PHASE_QUIT;
  work->req_turn = 0;
  work->ack_phase = PHASE_IDLE;

  work->executing = TRUE;
}

BOOL WXCi_AddData(WXCProtocolContext *protocol, const void *send_buf,
                  u32 send_size, void *recv_buf, u32 recv_max) {
  BOOL retval = FALSE;

  WXCCommonProtocolImplContext *work =
      (WXCCommonProtocolImplContext *)WXC_GetCurrentBlockImpl(protocol)
          ->impl_work;

  if (work->req_phase == PHASE_QUIT) {
    retval = TRUE;
    protocol->send.buffer = (void *)send_buf;
    protocol->send.buffer_max = (u16)send_size;
    protocol->send.length = (u16)send_size;
    protocol->send.checksum = MATH_CalcChecksum8(send_buf, send_size);
    protocol->recv.buffer = recv_buf;
    protocol->recv.buffer_max = (u16)recv_max;

    work->req_phase = PHASE_INIT;
    work->ack_phase = PHASE_IDLE;
#if defined(DEBUG_CALCSPEED)
    if (OS_IsTickAvailable()) {
      work->tick_count = OS_GetTick();
    }
#endif
  }
  return retval;
}

BOOL WXCi_IsExecuting(WXCProtocolContext *protocol) {
  WXCCommonProtocolImplContext *work =
      (WXCCommonProtocolImplContext *)WXC_GetCurrentBlockImpl(protocol)
          ->impl_work;
  return work->executing;
}

void WXCi_PacketSendHook(WXCProtocolContext *protocol, WXCPacketInfo *packet) {
  WXCCommonProtocolImplContext *work =
      (WXCCommonProtocolImplContext *)WXC_GetCurrentBlockImpl(protocol)
          ->impl_work;

  if ((work->req_phase == PHASE_DATA) || (work->req_phase == PHASE_DONE)) {
    PacketSegmentFormat *format = (PacketSegmentFormat *)packet->buffer;
    format->segment = TRUE;
    format->turn = work->req_turn;
    format->recv_arg = work->req_index;
    format->send_arg = work->ack_index;
    if (work->ack_index != 0x7FFF) {
      u32 segment = work->own_packet;
      u32 offset = segment * work->ack_index;
      u32 rest = protocol->send.length - offset;
      MI_CpuCopy8((u8 *)protocol->send.buffer + offset, format->buffer,
                  MATH_MIN(segment, rest));
    }
    WXC_PACKET_LOG("--- [send] %d:F:%04X:%04X\n", format->turn,
                   format->recv_arg, format->send_arg);
  } else {
    PacketCommandFormat *format = (PacketCommandFormat *)packet->buffer;
    format->segment = FALSE;
    format->ack = (u8)(work->ack_phase == work->req_phase);
    format->turn = work->req_turn;
    format->phase = (u8)work->req_phase;
    format->checksum = (u8)protocol->send.checksum;
    format->recv_arg = (u16)protocol->recv.buffer_max;
    format->send_arg = (u16)protocol->send.length;
    WXC_PACKET_LOG("--- [send] %d:%d:%04X:%04X\n", format->turn, format->phase,
                   format->recv_arg, format->send_arg);
  }
}

BOOL WXCi_PacketRecvHook(WXCProtocolContext *protocol,
                         const WXCPacketInfo *packet) {
  BOOL retval = FALSE;

  WXCCommonProtocolImplContext *work =
      (WXCCommonProtocolImplContext *)WXC_GetCurrentBlockImpl(protocol)
          ->impl_work;

  PacketCommandFormat format;
  BOOL ack = FALSE;
  BOOL turn = !work->req_turn;
  u32 recv_arg;
  u32 send_arg;
  u16 checksum = 0;

  if (packet->length < sizeof(format)) {
    OS_TWarning("not enough length.\n");
    return FALSE;
  }
  MI_CpuCopy8(packet->buffer, &format, sizeof(format));

  if (format.segment) {
    PacketSegmentFormat segment;
    MI_CpuCopy8(packet->buffer, &segment, sizeof(segment));
    turn = segment.turn;
    recv_arg = segment.recv_arg;
    send_arg = segment.send_arg;
    work->ack_phase = ((recv_arg == 0x7FFF) && (send_arg == 0x7FFF))
                          ? PHASE_DONE
                          : PHASE_DATA;
    WXC_PACKET_LOG("---                            [recv] %d:F:%04X:%04X\n",
                   segment.turn, segment.recv_arg, segment.send_arg);
  } else {
    work->ack_phase = format.phase;
    ack = format.ack;
    turn = format.turn;
    recv_arg = format.recv_arg;
    send_arg = format.send_arg;
    checksum = format.checksum;
    WXC_PACKET_LOG("---                            [recv] %d:%d:%04X:%04X\n",
                   format.turn, format.phase, format.recv_arg, format.send_arg);
  }

  if (turn != work->req_turn) {
    if (work->req_phase == PHASE_DONE) {
      retval = TRUE;
    }
  }

  else {
    switch (work->ack_phase) {
    case PHASE_INIT:
      if (work->req_phase != PHASE_INIT) {
        break;
      }

      protocol->send.buffer_max = recv_arg;
      protocol->recv.checksum = checksum;
      protocol->recv.length = send_arg;

      if ((protocol->send.length > protocol->send.buffer_max) ||
          (protocol->recv.length > protocol->recv.buffer_max)) {
        OS_TWarning("not enough buffer length for data exchange.\n");
        work->req_phase = PHASE_QUIT;
      } else if (!ack) {
        break;
      }

    case PHASE_DATA:

      if (work->req_phase == PHASE_INIT) {
        work->req_phase = PHASE_DATA;
        if (packet->length == work->child_send) {
          work->own_packet = work->parent_send;
          work->peer_packet = work->child_send;
        } else {
          work->own_packet = work->child_send;
          work->peer_packet = work->parent_send;
        }
        work->own_packet -= sizeof(PacketSegmentFormat);
        work->peer_packet -= sizeof(PacketSegmentFormat);
        work->req_index = 0;
        work->ack_index = 0;
        work->recv_count = 0;
        work->recv_total =
            (u32)((protocol->recv.length + work->peer_packet - 1) /
                  work->peer_packet);
        MI_CpuFill32(work->recv_bitset, 0x00, sizeof(work->recv_bitset));
      }
      if (work->ack_phase != PHASE_DATA) {
        break;
      }

      if (work->req_phase == PHASE_DATA) {

        work->ack_index = recv_arg;

        if (send_arg != 0x7FFF) {
          u32 pos = (send_arg >> 5UL);
          u32 bit = (send_arg & 31UL);
          if ((work->recv_bitset[pos] & (1 << bit)) == 0) {
            const u8 *argument = &packet->buffer[sizeof(PacketSegmentFormat)];
            u32 segment = work->peer_packet;
            u32 offset = segment * send_arg;
            u32 rest = protocol->recv.length - offset;
            MI_CpuCopy8(argument, (u8 *)protocol->recv.buffer + offset,
                        MATH_MIN(segment, rest));
            work->recv_bitset[pos] |= (1 << bit);
            ++work->recv_count;
          }
        }

        if (work->recv_count < work->recv_total) {
          u32 index = work->req_index;
          u32 pos = (index >> 5UL);
          u32 bit = (index & 31UL);
          for (;;) {
            if (++bit >= 32) {
              ++pos, bit = 0;
              for (; work->recv_bitset[pos] == (u32)~0; ++pos) {
              }
              bit = MATH_CTZ((u32)~work->recv_bitset[pos]);
            }
            if ((pos << 5UL) + bit >= work->recv_total) {
              pos = 0, bit = 0;
            }
            if ((work->recv_bitset[pos] & (1 << bit)) == 0) {
              break;
            }
          }
          work->req_index = (pos << 5UL) + bit;
        }

        else {
          work->req_index = 0x7FFF;

          if (work->ack_index == 0x7FFF) {
            work->req_phase = PHASE_DONE;
          }
        }
      }
      break;
    case PHASE_DONE:
      if (work->req_phase == PHASE_DATA) {
        work->ack_index = recv_arg;

        if ((work->ack_index == 0x7FFF) && (work->req_index == 0x7FFF)) {
          work->req_phase = PHASE_DONE;
        }
      }
      if (work->req_phase == PHASE_DONE) {
        retval = TRUE;
      }
      break;
    case PHASE_QUIT:

      work->req_phase = PHASE_QUIT;

      if (ack) {
        work->executing = FALSE;
      }
      break;
    case PHASE_IDLE:
    default:

      break;
    }
  }
  if (retval) {
#if defined(SDK_DEBUG)

    u8 checksum =
        MATH_CalcChecksum8(protocol->recv.buffer, protocol->recv.length);
    if (protocol->recv.checksum != checksum) {
      OS_TWarning("checksum error!\n");
    }
#endif
#if defined(DEBUG_CALCSPEED)

    if (OS_IsTickAvailable()) {
      OSTick diff = OS_GetTick() - work->tick_count;
      u64 milli = OS_TicksToMilliSeconds(diff);
      u64 length = MATH_MAX(protocol->send.length, protocol->recv.length);
      WXC_PACKET_LOG("data-exchange : %d[B/s]\n", (length * 1000) / milli);
    }
#endif
    work->req_turn = !work->req_turn;
    work->req_phase = PHASE_QUIT;
  }

  return retval;
}

WXCProtocolImpl *WXCi_GetProtocolImplCommon(void) { return &impl_common; }

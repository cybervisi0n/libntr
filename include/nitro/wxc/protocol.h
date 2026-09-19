#ifndef NITRO_WXC_PROTOCOL_H_
#define NITRO_WXC_PROTOCOL_H_

#include <nitro.h>
#include <nitro/wxc/common.h>

#define WXC_BLOCK_COMMAND_IDLE 0 /* State with request or response */
#define WXC_BLOCK_COMMAND_INIT 1 /* Send/receive data size and checksum */
#define WXC_BLOCK_COMMAND_SEND 2 /* Send/receive data block */
#define WXC_BLOCK_COMMAND_STOP 3 /* Send/receive cancel */
#define WXC_BLOCK_COMMAND_DONE 4 /* Send/receive completion */
#define WXC_BLOCK_COMMAND_QUIT 5 /* Close communication request (disconnect)   \
                                  */

#define WXC_PACKET_BUFFRER_MIN                                                 \
  (int)(WXC_PACKET_SIZE_MIN - sizeof(WXCBlockHeader))

#define WXC_MAX_DATA_SIZE (32 * 1024)
#define WXC_RECV_BITSET_SIZE                                                   \
  ((((WXC_MAX_DATA_SIZE + WXC_PACKET_BUFFRER_MIN - 1) /                        \
     WXC_PACKET_BUFFRER_MIN) +                                                 \
    31) /                                                                      \
   32)

#define WXC_RECENT_SENT_LIST_MAX 2

#define WXC_REGISTER_DATA_MAX 16

typedef struct WXCBlockInfo {
  u8 phase;   /* Block data exchange phase */
  u8 command; /* Command in the current phase */
  u16 index;  /* Required sequence number */
} WXCBlockInfo;

typedef struct WXCBlockHeader {
  WXCBlockInfo req; /* Request from transmission source */
  WXCBlockInfo ack; /* Response to the data destination */
} WXCBlockHeader;

SDK_STATIC_ASSERT(sizeof(WXCBlockHeader) == 8);

typedef struct WXCBlockDataFormat {
  void *buffer;   /* Pointer to a buffer */
  u32 length;     /* Data size */
  u32 buffer_max; /* Buffer size (match with 'length' for send data) */
  u16 checksum;   /* Checksum (use MATH_CalcChecksum8()) */
  u8 padding[2];
} WXCBlockDataFormat;

struct WXCProtocolContext;

typedef struct WXCProtocolImpl {

  const char *name;

  void (*BeaconSend)(struct WXCProtocolContext *, WMParentParam *);
  BOOL (*BeaconRecv)(struct WXCProtocolContext *, const WMBssDesc *);

  void (*PreConnectHook)(struct WXCProtocolContext *, const WMBssDesc *,
                         u8 ssid[WM_SIZE_CHILD_SSID]);

  void (*PacketSend)(struct WXCProtocolContext *, WXCPacketInfo *);
  BOOL (*PacketRecv)(struct WXCProtocolContext *, const WXCPacketInfo *);

  void (*Init)(struct WXCProtocolContext *, u16, u16);

  BOOL (*AddData)(struct WXCProtocolContext *, const void *, u32, void *, u32);

  BOOL (*IsExecuting)(struct WXCProtocolContext *);

  void *impl_work;

  struct WXCProtocolImpl *next;
} WXCProtocolImpl;

typedef struct WXCProtocolRegistry {

  struct WXCProtocolRegistry *next;

  u32 ggid;
  WXCCallback callback;
  WXCProtocolImpl *impl;

  WXCBlockDataFormat send; /* Send buffer */
  WXCBlockDataFormat recv; /* Receive buffer */
} WXCProtocolRegistry;

typedef struct WXCProtocolContext {
  WXCBlockDataFormat send; /* Send buffer */
  WXCBlockDataFormat recv; /* Receive buffer */

  WXCProtocolRegistry *current_block;
  WXCProtocolRegistry data_array[WXC_REGISTER_DATA_MAX];
  u32 target_ggid;
} WXCProtocolContext;

#ifdef __cplusplus
extern "C" {
#endif

void WXC_InitProtocol(WXCProtocolContext *protocol);

void WXC_InstallProtocolImpl(WXCProtocolImpl *impl);

WXCProtocolImpl *WXC_FindProtocolImpl(const char *name);

void WXC_ResetSequence(WXCProtocolContext *protocol, u16 send_max,
                       u16 recv_max);

void WXC_AddBlockSequence(WXCProtocolContext *protocol, const void *send_buf,
                          u32 send_size, void *recv_buf, u32 recv_max);

static inline WXCProtocolRegistry *
WXC_GetCurrentBlock(WXCProtocolContext *protocol) {
  return protocol->current_block;
}

static inline WXCProtocolImpl *
WXC_GetCurrentBlockImpl(WXCProtocolContext *protocol) {
  return protocol->current_block->impl;
}

static inline void WXC_SetCurrentBlock(WXCProtocolContext *protocol,
                                       WXCProtocolRegistry *target) {
  protocol->current_block = target;
}

WXCProtocolRegistry *WXC_FindNextBlock(WXCProtocolContext *protocol,
                                       const WXCProtocolRegistry *from,
                                       u32 ggid, BOOL match);

void WXC_BeaconSendHook(WXCProtocolContext *protocol, WMParentParam *p_param);

BOOL WXC_BeaconRecvHook(WXCProtocolContext *protocol, const WMBssDesc *p_desc);

void WXC_PacketSendHook(WXCProtocolContext *protocol, WXCPacketInfo *packet);

BOOL WXC_PacketRecvHook(WXCProtocolContext *protocol,
                        const WXCPacketInfo *packet);

void WXC_ConnectHook(WXCProtocolContext *protocol, u16 bitmap);

void WXC_DisconnectHook(WXCProtocolContext *protocol, u16 bitmap);

void WXC_CallPreConnectHook(WXCProtocolContext *protocol, WMBssDesc *p_desc,
                            u8 *ssid);

void WXC_InitProtocolRegistry(WXCProtocolRegistry *p_data, u32 ggid,
                              WXCCallback callback, WXCProtocolImpl *impl);

void WXC_SetInitialExchangeBuffers(WXCProtocolRegistry *p_data, u8 *send_ptr,
                                   u32 send_size, u8 *recv_ptr, u32 recv_size);

WXCProtocolImpl *WXCi_GetProtocolImplCommon(void);
WXCProtocolImpl *WXCi_GetProtocolImplWPB(void);
WXCProtocolImpl *WXCi_GetProtocolImplWXC(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NITRO_WXC_PROTOCOL_H_ */

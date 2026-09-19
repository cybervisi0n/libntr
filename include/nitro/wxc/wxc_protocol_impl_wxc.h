#ifndef NITRO_WXC_PROTOCOL_IMPL_WXC_H_
#define NITRO_WXC_PROTOCOL_IMPL_WXC_H_

#include <nitro.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  WXCBlockInfo req; /* Request from self */
  WXCBlockInfo ack; /* Response to the target */

  u32 recv_total; /* Total received packet */
  u32 recv_rest;  /* Remaining receive packet */

  u32 recv_bitmap_buf[WXC_RECV_BITSET_SIZE];

  u16 send_unit; /* Unit size of send packet */
  u16 recv_unit; /* Unit size of receive packet */

  u16 recent_index[WXC_RECENT_SENT_LIST_MAX];

  BOOL executing;
} WXCImplWorkWxc;

extern WXCProtocolImpl wxc_protocol_impl_wxc;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NITRO_WXC_PROTOCOL_IMPL_WXC_H_ */

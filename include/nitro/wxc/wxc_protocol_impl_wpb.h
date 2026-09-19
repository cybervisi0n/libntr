#ifndef NITRO_WXC_PROTOCOL_IMPL_WPB_H_
#define NITRO_WXC_PROTOCOL_IMPL_WPB_H_

#include <nitro.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PASS_PACKET_SIZE 512
#define PASS_BUFFER_SIZE (PASS_PACKET_SIZE - 4)
#define WPBC_PARENT_DATA_SIZE_MAX PASS_PACKET_SIZE
#define WPBC_CHILD_DATA_SIZE_MAX PASS_PACKET_SIZE

#define REQUEST_DONE 0xffff
#define REQUEST_NONE 0xfffe
#define REQUEST_BYE 0xfffd

#define HARDWARE_BUFFER_DUMMY_COUNT 3

#define MAX_PATTERN 4
#define MAX_SEQ 4

typedef struct {
  int size;
  int total_count;
  const u8 *user_send_data;
} PassData;

typedef struct {
  u16 req_count;
  u16 res_count;
  u8 buf[PASS_BUFFER_SIZE];
} PassBuffer;

typedef struct {
  u8 *user_recv_buffer;
  BOOL reset_done;
  BOOL send_done;
  BOOL recv_done;
  int hardware_buffer_count;
  PassBuffer send_buf;
  PassBuffer recv_buf;
  int pre_send_count;
  u32 recv_bitmap; /* Loop in mod32 */
  int recv_bitmap_index;
} PassCtrl;

typedef struct {

  PassCtrl pass_ctrl;

  PassData my_pass_data;

  u32 recv_bitmap_buf[WXC_RECV_BITSET_SIZE];

  u16 send_unit; /* Unit size of send packet */
  u16 recv_unit; /* Unit size of receive packet */

  u16 recent_index[WXC_RECENT_SENT_LIST_MAX];

  BOOL executing;
} WXCImplWorkWpb;

extern WXCProtocolImpl wxc_protocol_impl_wpb;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NITRO_WXC_PROTOCOL_IMPL_WPB_H_ */

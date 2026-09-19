#ifndef LIBRARIES_NWM_ARM9_NWM_COMMON_PRIVATE_H__
#define LIBRARIES_NWM_ARM9_NWM_COMMON_PRIVATE_H__

#if SDK_VERSION_MAJOR != 5
#error This file is for SDK 5 only!
#endif

#ifdef SDK_TWL
#include <twl.h>
#else
#include <nitro.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  u32 callbackSyncFlag;
} NWMArm7Buf;

typedef struct {
  u32 apiActiveFlag[1];
  u32 state;
  u32 allowedChannel;
} NWMStatus;

typedef struct {
  int dummy;
} NWMQoSParam;

typedef struct {
  int dummy;
} NWMBinaryData;

typedef struct {
  NWMArm7Buf *NWM7;
  NWMStatus *status;
  u32 *fifo7to9;
  NWMCallbackFunc callbackTable[10];
  NWMCallbackFunc recvCallbackTable[10];
  struct {
    OSMessageQueue q;
    void *fifo;
  } apibufQ;
  u8 *apibuf;
  u8 *binData;
  u32 dmaNo;

} NWMArm9Buf;

void NWMi_DeleteSleepCallback(void);

#define NWM_WARNING SDK_WARNING

#define NWM_APIFIFO_BUF_SIZE 10
#define NWM_APIBUF_NUM 1
#define NWM_API_REQUEST_ACCEPTED WM_API_REQUEST_ACCEPTED
#define NWM_ARM9NWM_BUF_SIZE 10
#define NWM_ARM7NWM_BUF_SIZE 10
#define NWM_STATUS_BUF_SIZE 10
#define NWM_EXCEPTION_CB_MASK 0
#define NWM_DPRINTF

#ifdef __cplusplus
}
#endif

#endif

#ifndef NITRO_WXC_DRIVER_H_
#define NITRO_WXC_DRIVER_H_

#include <nitro.h>

#define WXC_DEFAULT_PORT 4
#define WXC_DEFAULT_PORT_PRIO 2

#define WXC_SCAN_TIME_MAX (WXC_BEACON_PERIOD + 20)

#define WXC_MAX_RATIO 100

typedef enum {

  WXC_DRIVER_EVENT_STATE_END,

  WXC_DRIVER_EVENT_STATE_IDLE,

  WXC_DRIVER_EVENT_STATE_STOP,

  WXC_DRIVER_EVENT_STATE_PARENT,

  WXC_DRIVER_EVENT_STATE_CHILD,

  WXC_DRIVER_EVENT_BEACON_SEND,

  WXC_DRIVER_EVENT_BEACON_RECV,

  WXC_DRIVER_EVENT_DATA_SEND,

  WXC_DRIVER_EVENT_DATA_RECV,

  WXC_DRIVER_EVENT_CONNECTING,

  WXC_DRIVER_EVENT_CONNECTED,

  WXC_DRIVER_EVENT_DISCONNECTED,

  WXC_DRIVER_EVENT_MAX
} WXCDriverEvent;

typedef enum WXCDriverState {
  WXC_DRIVER_STATE_END,    /* Before initialization (driver = NULL) */
  WXC_DRIVER_STATE_BUSY,   /* State in transition */
  WXC_DRIVER_STATE_STOP,   /* Stable in STOP state */
  WXC_DRIVER_STATE_IDLE,   /* Stable in IDLE state */
  WXC_DRIVER_STATE_PARENT, /* Stable in MP_PARENT state */
  WXC_DRIVER_STATE_CHILD,  /* Stable in MP_CHILD state */
  WXC_DRIVER_STATE_ERROR   /* Error without automatic recovery */
} WXCDriverState;

typedef u32 (*WXCDriverEventFunc)(WXCDriverEvent event, void *arg);

typedef struct WXCDriverWork {

  u8 wm_work[WM_SYSTEM_BUF_SIZE] ATTRIBUTE_ALIGN(32);
  u8 mp_send_work[WM_SIZE_MP_PARENT_SEND_BUFFER(WM_SIZE_MP_DATA_MAX,
                                                FALSE)] ATTRIBUTE_ALIGN(32);
  u8 mp_recv_work[WM_SIZE_MP_PARENT_RECEIVE_BUFFER(
      WM_SIZE_MP_DATA_MAX, WM_NUM_MAX_CHILD, FALSE)] ATTRIBUTE_ALIGN(32);
  u8 current_send_buf[WM_SIZE_MP_DATA_MAX] ATTRIBUTE_ALIGN(32);
  u16 wm_dma;          /* WM DMA channel */
  u16 current_channel; /* Current channel (Measure/Start) */
  u16 own_aid;         /* This system's AID */
  u16 peer_bitmap;     /* Bitmap of connection peers */
  u16 send_size_max;   /* MP send size */
  u16 recv_size_max;   /* MP send size */
  BOOL send_busy;      /* Waiting for previous MP to complete   */

  WXCDriverState state;
  WXCDriverState target;
  WXCDriverEventFunc callback;

  WMParentParam *parent_param;
  BOOL need_measure_channel;
  int measure_ratio_min;

  int scan_found_num;
  u8 padding1[20];
  WMBssDesc target_bss[1] ATTRIBUTE_ALIGN(32);
  u8 scan_buf[WM_SIZE_SCAN_EX_BUF] ATTRIBUTE_ALIGN(32);
  WMScanExParam scan_param[1] ATTRIBUTE_ALIGN(32);
  u8 ssid[24];
  u8 padding2[4];

} WXCDriverWork;

#ifdef __cplusplus
extern "C" {
#endif

void WXC_InitDriver(WXCDriverWork *driver, WMParentParam *pp,
                    WXCDriverEventFunc func, u32 dma);

void WXC_SetDriverTarget(WXCDriverWork *driver, WXCDriverState target);

static inline WXCDriverState WXC_GetDriverState(const WXCDriverWork *driver) {
  return driver->state;
}

void WXC_SetDriverSsid(WXCDriverWork *driver, const void *buffer, u32 length);

static inline WXCDriverState WXC_GetDriverTarget(const WXCDriverWork *driver) {
  return driver->target;
}

static inline BOOL WXCi_IsParentMode(const WXCDriverWork *driver) {
  return (driver->state == WXC_DRIVER_STATE_PARENT);
}

static inline const WMBssDesc *
WXCi_GetParentBssDesc(const WXCDriverWork *driver) {
  return driver->target_bss;
}

static inline void WXCi_Stop(WXCDriverWork *driver) {
  WXC_SetDriverTarget(driver, WXC_DRIVER_STATE_STOP);
}

static inline void WXCi_StartParent(WXCDriverWork *driver) {
  WXC_SetDriverTarget(driver, WXC_DRIVER_STATE_PARENT);
}

static inline void WXCi_StartChild(WXCDriverWork *driver) {
  WXC_SetDriverTarget(driver, WXC_DRIVER_STATE_CHILD);
}

static inline void WXCi_Reset(WXCDriverWork *driver) {
  WXC_SetDriverTarget(driver, WXC_DRIVER_STATE_IDLE);
}

static inline void WXCi_End(WXCDriverWork *driver) {
  WXC_SetDriverTarget(driver, WXC_DRIVER_STATE_END);
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NITRO_WXC_DRIVER_H_ */

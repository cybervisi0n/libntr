#ifndef NITRO_WXC_COMMON_H_
#define NITRO_WXC_COMMON_H_

#include <nitro.h>

#if !defined(WXC_DEBUG_OUTPUT)
#define WXC_DEBUG_OUTPUT 0
#endif

#define WXC_PACKET_LOG WXC_DEBUG_LOG
#define WXC_DRIVER_LOG WXC_DEBUG_LOG

#if defined(SDK_COMPILER_ASSERT)
#define SDK_STATIC_ASSERT SDK_COMPILER_ASSERT
#else
#define SDK_STATIC_ASSERT(expr)                                                \
  extern void sdk_compiler_assert##__LINE__(char is[(expr) ? +1 : -1])
#endif

typedef enum {

  WXC_STATE_END,    /* End process completed by WXC_End function */
  WXC_STATE_ENDING, /* After WXC_End function and currently running end process
                     */
  WXC_STATE_READY,  /* After WXC_Init function and before WXC_Start function */
  WXC_STATE_ACTIVE, /* After WXC_Start function and wireless communication is
                       enabled */

  WXC_STATE_CONNECTED,     /* Child connected (only for Parent side. argument is
                              WMStartParentCallback) */
  WXC_STATE_EXCHANGE_DONE, /* Data exchange completed (argument is the user
                              specified receive buffer) */
  WXC_STATE_BEACON_RECV    /* Received beacon (argument is the
                              WXCBeaconFoundCallback function) */
} WXCStateCode;

#define WXC_BEACON_PERIOD 90 /* [ms] */
#define WXC_PARENT_BEACON_SEND_COUNT_OUT (900 / WXC_BEACON_PERIOD)

#define WXC_PACKET_SIZE_MIN 20
#define WXC_PACKET_SIZE_MAX WM_SIZE_MP_DATA_MAX

#define WXC_GGID_COMMON_BIT 0x80000000UL
#define WXC_GGID_COMMON_ANY (u32)(WXC_GGID_COMMON_BIT | 0x00000000UL)
#define WXC_GGID_COMMON_PARENT (u32)(WXC_GGID_COMMON_BIT | 0x00400120UL)

typedef void (*WXCCallback)(WXCStateCode stat, void *arg);

typedef struct WXCUserInfo {
  u16 aid;
  u8 macAddress[6];
} WXCUserInfo;

typedef struct WXCPacketInfo {

  u8 *buffer;

  u16 length;

  u16 bitmap;
} WXCPacketInfo;

typedef struct WXCBeaconFoundCallback {

  BOOL matched;

  const WMBssDesc *bssdesc;
} WXCBeaconFoundCallback;

#ifdef __cplusplus
extern "C" {
#endif

#if (WXC_DEBUG_OUTPUT == 1)
void WXC_DEBUG_LOG(const char *format, ...);
#else
#define WXC_DEBUG_LOG(...) ((void)0)
#endif

const char *WXC_GetWmApiName(WMApiid id);

const char *WXC_GetWmErrorName(WMErrCode err);

BOOL WXC_CheckWmApiResult(WMApiid id, WMErrCode err);

BOOL WXC_CheckWmCallbackResult(void *arg);

u16 WXC_GetNextAllowedChannel(int ch);

SDK_INLINE
BOOL WXC_IsCommonGgid(u32 ggid) { return ((ggid & WXC_GGID_COMMON_BIT) != 0); }

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NITRO_WXC_COMMON_H_ */

#ifndef NITRO_WXC_H_
#define NITRO_WXC_H_

#if SDK_VERSION_MAJOR != 5
#error This file is for SDK 5 only!
#endif

#include <nitro.h>

#include <nitro/wxc/common.h>
#include <nitro/wxc/protocol.h>
#include <nitro/wxc/driver.h>
#include <nitro/wxc/scheduler.h>

#include <nitro/wxc/wxc_protocol_impl_wxc.h>

#define WXC_WORK_SIZE 0xA000

#ifdef __cplusplus
extern "C" {
#endif

void WXC_Init(void *work, WXCCallback callback, u32 dma);

void WXC_Start(void);

void WXC_Stop(void);

void WXC_End(void);

WXCStateCode WXC_GetStateCode(void);

BOOL WXC_IsParentMode(void);

const WMParentParam *WXC_GetParentParam(void);

const WMBssDesc *WXC_GetParentBssDesc(void);

u16 WXC_GetUserBitmap(void);

u32 WXC_GetCurrentGgid(void);

u16 WXC_GetOwnAid(void);

const WXCUserInfo *WXC_GetUserInfo(u16 aid);

void WXC_SetChildMode(BOOL enable);

BOOL WXC_SetParentParameter(u16 sendSize, u16 recvSize, u16 maxEntry);

void WXC_RegisterDataEx(u32 ggid, WXCCallback callback, u8 *send_ptr,
                        u32 send_size, u8 *recv_ptr, u32 recv_size,
                        const char *type);

#define WXC_RegisterData(...)                                                  \
  SDK_OBSOLETE_FUNCTION(                                                       \
      "WXC_RegisterData() is obsolete. use WXC_RegisterCommonData()")

static inline void WXC_RegisterCommonData(u32 ggid, WXCCallback callback,
                                          u8 *send_ptr, u32 send_size,
                                          u8 *recv_ptr, u32 recv_size) {
  WXC_RegisterDataEx((u32)(ggid | WXC_GGID_COMMON_BIT), callback, send_ptr,
                     send_size, recv_ptr, recv_size, "COMMON");
}

void WXC_SetInitialData(u32 ggid, u8 *send_ptr, u32 send_size, u8 *recv_ptr,
                        u32 recv_size);

void WXC_AddData(const void *send_buf, u32 send_size, void *recv_buf,
                 u32 recv_max);

void WXC_UnregisterData(u32 ggid);

void WXCi_SetSsid(const void *buffer, u32 length);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NITRO_WXC_H_ */

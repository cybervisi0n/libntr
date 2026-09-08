#ifndef TWL_RTC_ARM9_API_EX_H_
#define TWL_RTC_ARM9_API_EX_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/rtc/common/type.h>

RTCResult RTCi_GetCounterAsync(u32 *count, RTCCallback callback, void *arg);
RTCResult RTCi_GetCounter(u32 *count);
RTCResult RTCi_GetFoutAsync(u16 *fout, RTCCallback callback, void *arg);
RTCResult RTCi_GetFout(u16 *fout);
RTCResult RTCi_SetFoutAsync(const u16 *fout, RTCCallback callback, void *arg);
RTCResult RTCi_SetFout(const u16 *fout);

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* TWL_RTC_ARM9_API_EX_H_ */

#ifndef NITRO_RTC_ARM9_SWCLOCK_H_
#define NITRO_RTC_ARM9_SWCLOCK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/types.h>

RTCResult RTC_InitSWClock(void);

OSTick RTC_GetSWClockTick(void);

RTCResult RTC_GetLastSyncSWClockResult(void);

RTCResult RTC_GetDateTimeExFromSWClock(RTCDate *date, RTCTimeEx *time);

RTCResult RTC_SyncSWClock(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NITRO_RTC_ARM9_SWCLOCK_H_ */

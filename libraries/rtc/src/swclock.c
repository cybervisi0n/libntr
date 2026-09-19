#include <nitro/os.h>
#include <nitro/rtc.h>
#include <nitro/spi/ARM9/pm.h>

static u16 rtcSWClockInitialized;  // Tick initialized verify flag
static OSTick rtcSWClockBootTicks; // Boot-time tick-converted RTC value
static RTCResult
    rtcLastResultOfSyncSWClock; // RTCResult at final synchronization
static PMSleepCallbackInfo
    rtcSWClockSleepCbInfo; // Callback information when recovering from sleep

static void RtcGetDateTimeExFromSWClock(RTCDate *date, RTCTimeEx *time);
static void RtcSleepCallbackForSyncSWClock(void *args);

RTCResult RTC_InitSWClock(void) {
  SDK_ASSERT(OS_IsTickAvailable());

  if (rtcSWClockInitialized) {
    return RTC_RESULT_SUCCESS;
  }

  (void)RTC_SyncSWClock();

  PM_SetSleepCallbackInfo(&rtcSWClockSleepCbInfo,
                          RtcSleepCallbackForSyncSWClock, NULL);
  PM_AppendPostSleepCallback(&rtcSWClockSleepCbInfo);

  rtcSWClockInitialized = TRUE;

  return rtcLastResultOfSyncSWClock;
}

OSTick RTC_GetSWClockTick(void) {
  if (rtcLastResultOfSyncSWClock == RTC_RESULT_SUCCESS) {
    return OS_GetTick() + rtcSWClockBootTicks;
  } else {
    return 0;
  }
}

RTCResult RTC_GetLastSyncSWClockResult(void) {
  return rtcLastResultOfSyncSWClock;
}

RTCResult RTC_GetDateTimeExFromSWClock(RTCDate *date, RTCTimeEx *time) {
  SDK_NULL_ASSERT(date);
  SDK_NULL_ASSERT(time);

  RtcGetDateTimeExFromSWClock(date, time);

  return rtcLastResultOfSyncSWClock;
}

RTCResult RTC_SyncSWClock(void) {
  RTCDate currentDate;
  RTCTime currentTime;

  rtcLastResultOfSyncSWClock = RTC_GetDateTime(&currentDate, &currentTime);

  rtcSWClockBootTicks = OS_SecondsToTicks(RTC_ConvertDateTimeToSecond(
                            &currentDate, &currentTime)) -
                        OS_GetTick();

  return rtcLastResultOfSyncSWClock;
}

static void RtcGetDateTimeExFromSWClock(RTCDate *date, RTCTimeEx *time) {
  OSTick currentTicks;
  s64 currentSWClockSeconds;

  currentTicks = RTC_GetSWClockTick();
  currentSWClockSeconds = (s64)OS_TicksToSeconds(currentTicks);

  RTC_ConvertSecondToDateTime(date, (RTCTime *)time, currentSWClockSeconds);

  time->millisecond = (u32)(OS_TicksToMilliSeconds(currentTicks) % 1000);
}

#define RTC_SWCLOCK_SYNC_RETRY_INTERVAL 1 // Units are milliseconds
static void RtcSleepCallbackForSyncSWClock(void *args) {
#pragma unused(args)
  for (;;) {
    (void)RTC_SyncSWClock();

    if (rtcLastResultOfSyncSWClock != RTC_RESULT_BUSY &&
        rtcLastResultOfSyncSWClock != RTC_RESULT_SEND_ERROR) {
      break;
    }

    OS_TWarning("RTC_SyncSWClock() failed at sleep callback. Retry... \n");
    OS_Sleep(RTC_SWCLOCK_SYNC_RETRY_INTERVAL);
  }
}

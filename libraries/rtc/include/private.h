#ifndef TWL_LIBRARIES_RTC_ARM9_PRIVATE_H_
#define TWL_LIBRARIES_RTC_ARM9_PRIVATE_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/rtc/ARM9/api.h>

typedef enum RTCLock {
  RTC_LOCK_OFF = 0, /* Unlock status */
  RTC_LOCK_ON,      /* Lock status */
  RTC_LOCK_MAX
} RTCLock;

typedef enum RTCSequence {
  RTC_SEQ_GET_DATE = 0,      /* Sequence for getting dates */
  RTC_SEQ_GET_TIME,          /* Sequence for getting times */
  RTC_SEQ_GET_DATETIME,      /* Sequence for getting dates and times */
  RTC_SEQ_SET_DATE,          /* Sequence for setting dates */
  RTC_SEQ_SET_TIME,          /* Sequence for setting times */
  RTC_SEQ_SET_DATETIME,      /* Sequence for setting dates and times */
  RTC_SEQ_GET_ALARM1_STATUS, /* Sequence for getting alarm 1's status */
  RTC_SEQ_GET_ALARM2_STATUS, /* Sequence for getting alarm 2's status */
  RTC_SEQ_GET_ALARM_PARAM,   /* Sequence for getting alarm setting values */
  RTC_SEQ_SET_ALARM1_STATUS, /* Sequence for changing alarm 1's status */
  RTC_SEQ_SET_ALARM2_STATUS, /* Sequence for changing alarm 2's status */
  RTC_SEQ_SET_ALARM1_PARAM, /* Sequence for changing alarm 1's setting values */
  RTC_SEQ_SET_ALARM2_PARAM, /* Sequence for changing alarm 2's setting values */
  RTC_SEQ_SET_HOUR_FORMAT,  /* Sequence for changing the format of the time
                               notation */
  RTC_SEQ_SET_REG_STATUS2,  /* Sequence for status 2 register writes */
  RTC_SEQ_SET_REG_ADJUST,   /* Sequence for adjust register writes */
  RTC_SEQ_GET_COUNTER,      /* Sequence to get the up counter value */
  RTC_SEQ_GET_FOUT,         /* Sequence to get the FOUT value */
  RTC_SEQ_SET_FOUT,         /* Sequence to change the FOUT value */
  RTC_SEQ_MAX
} RTCSequence;

typedef struct RTCWork {
  u32 lock;               /* Exclusive lock */
  RTCCallback callback;   /* For saving an asynchronous callback function */
  void *buffer[2];        /* For storing asynchronous function parameters */
  void *callbackArg;      /* For saving arguments to the callback function */
  u32 sequence;           /* For controlling continuous processing mode */
  u32 index;              /* For controlling continuous processing status */
  RTCInterrupt interrupt; /* For saving the function called when an alarm
                             notification is sent */
  RTCResult
      commonResult; /* For saving asynchronous function processing results */

} RTCWork;

RTCWork *RTCi_GetSysWork(void);
RTCResult RTCEXi_CommonCallback(void);

RTCResult RTCEXi_GetCounterAsync(u32 *count, RTCCallback callback, void *arg);
RTCResult RTCEXi_GetCounter(u32 *count);
RTCResult RTCEXi_GetFoutAsync(u16 *fout, RTCCallback callback, void *arg);
RTCResult RTCEXi_GetFout(u16 *fout);
RTCResult RTCEXi_SetFoutAsync(const u16 *fout, RTCCallback callback, void *arg);
RTCResult RTCEXi_SetFout(const u16 *fout);

BOOL RTCEXi_ReadRawCounterAsync(void);
BOOL RTCEXi_ReadRawFoutAsync(void);
BOOL RTCEXi_WriteRawFoutAsync(void);
BOOL RTCEXi_ReadRawAlarmEx1Async(void);
BOOL RTCEXi_WriteRawAlarmEx1Async(void);
BOOL RTCEXi_ReadRawAlarmEx2Async(void);
BOOL RTCEXi_WriteRawAlarmEx2Async(void);

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* TWL_LIBRARIES_RTC_ARM9_PRIVATE_H_ */

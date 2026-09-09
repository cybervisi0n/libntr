#ifndef NITRO_LIBRARIES_CARD_EVENT_H__
#define NITRO_LIBRARIES_CARD_EVENT_H__

#include <nitro.h>

#if SDK_VERSION_MAJOR != 5
#error This file is for SDK version 5 only!
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CARDEventListener {
  OSEvent event[1];
  OSVAlarm valarm[1];
  BOOL (*Condition)(void *);
  void *userdata;
  u16 lockID;
  u8 padding[2];
} CARDEventListener;

static void CARDi_PeekEventListener(void *arg) {
  CARDEventListener *el = (CARDEventListener *)arg;
  if (el->Condition(el->userdata)) {
    OS_SignalEvent(el->event, 0x00000001UL);
  } else {
    OS_SetVAlarm(el->valarm, HW_LCD_HEIGHT, OS_VALARM_DELAY_MAX,
                 CARDi_PeekEventListener, el);
  }
}

SDK_INLINE void CARDi_InitEventListener(CARDEventListener *el) {
  OS_InitEvent(el->event);
  OS_CreateVAlarm(el->valarm);
}

SDK_INLINE void CARDi_SetEventListener(CARDEventListener *el,
                                       BOOL (*condition)(void *),
                                       void *userdata) {
  el->Condition = condition;
  el->userdata = userdata;
  CARDi_PeekEventListener(el);
}

SDK_INLINE void CARDi_WaitForEvent(CARDEventListener *el) {
  (void)OS_WaitEventEx(el->event, 0x00000001UL, OS_EVENT_MODE_AND,
                       0x00000001UL);
}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // NITRO_LIBRARIES_CARD_EVENT_H__

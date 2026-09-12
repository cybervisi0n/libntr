#ifndef NITRO_OS_EVENT_H_
#define NITRO_OS_EVENT_H_

#include <nitro/os/common/thread.h>

#if SDK_VERSION_MAJOR != 5
#error This file is for SDK version 5 only!
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum { OS_EVENT_MODE_AND = 0, OS_EVENT_MODE_OR = 1 } OSEventMode;

typedef struct {
  vu32 flag;
  OSThreadQueue queue;
} OSEvent;

void OS_InitEvent(OSEvent *event);

u32 OS_WaitEvent(OSEvent *event, u32 pattern, OSEventMode mode);

u32 OS_WaitEventEx(OSEvent *event, u32 pattern, OSEventMode mode, u32 clearBit);

static inline u32 OS_WaitEvent_And(OSEvent *event, u32 pattern) {
  return OS_WaitEventEx(event, pattern, OS_EVENT_MODE_AND, 0);
}
static inline u32 OS_WaitEvent_Or(OSEvent *event, u32 pattern) {
  return OS_WaitEventEx(event, pattern, OS_EVENT_MODE_OR, 0);
}
static inline u32 OS_WaitEventEx_And(OSEvent *event, u32 pattern,
                                     u32 clearBit) {
  return OS_WaitEventEx(event, pattern, OS_EVENT_MODE_AND, clearBit);
}
static inline u32 OS_WaitEventEx_Or(OSEvent *event, u32 pattern, u32 clearBit) {
  return OS_WaitEventEx(event, pattern, OS_EVENT_MODE_OR, clearBit);
}

void OS_SignalEvent(OSEvent *event, u32 setPattern);

void OS_ClearEvent(OSEvent *event, u32 clearBit);

void OS_ClearAllEvent(OSEvent *event);

u32 OS_PollEvent(OSEvent *event, u32 pattern, OSEventMode mode);

u32 OS_PollEventEx(OSEvent *event, u32 pattern, OSEventMode mode, u32 clearBit);

static inline u32 OS_PollEvent_And(OSEvent *event, u32 pattern) {
  return OS_PollEventEx(event, pattern, OS_EVENT_MODE_AND, 0);
}
static inline u32 OS_PollEvent_Or(OSEvent *event, u32 pattern) {
  return OS_PollEventEx(event, pattern, OS_EVENT_MODE_OR, 0);
}
static inline u32 OS_PollEventEx_And(OSEvent *event, u32 pattern,
                                     u32 clearBit) {
  return OS_PollEventEx(event, pattern, OS_EVENT_MODE_AND, clearBit);
}
static inline u32 OS_PollEventEx_Or(OSEvent *event, u32 pattern, u32 clearBit) {
  return OS_PollEventEx(event, pattern, OS_EVENT_MODE_OR, clearBit);
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

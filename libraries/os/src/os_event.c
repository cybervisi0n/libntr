#ifdef SDK_TWL
#include <twl/memorymap.h>
#else
#include <nitro/memorymap.h>
#endif

#include <nitro/os.h>

#if defined(SDK_TCM_APPLY) && defined(SDK_ARM9)
#include <nitro/itcm_begin.h>
#endif

void OS_InitEvent(OSEvent *event) {
  OS_InitThreadQueue(&event->queue);
  event->flag = 0;
}

u32 OS_WaitEvent(OSEvent *event, u32 pattern, OSEventMode mode) {
  return OS_WaitEventEx(event, pattern, mode, 0);
}

u32 OS_WaitEventEx(OSEvent *event, u32 pattern, OSEventMode mode,
                   u32 clearBit) {
  u32 retval = 0;
  OSIntrMode enable = OS_DisableInterrupts();

  SDK_ASSERT(event);

  switch (mode) {
  case OS_EVENT_MODE_AND:
    while ((event->flag & pattern) != pattern) {
      OS_SleepThread(&event->queue);
    }
    retval = event->flag;
    break;
  case OS_EVENT_MODE_OR:
    while ((event->flag & pattern) == 0) {
      OS_SleepThread(&event->queue);
    }
    retval = event->flag;
    break;
  }

  if (retval) {
    event->flag &= ~clearBit;
  }

  (void)OS_RestoreInterrupts(enable);
  return retval;
}

void OS_SignalEvent(OSEvent *event, u32 setPattern) {
  OSIntrMode enable = OS_DisableInterrupts();
  SDK_ASSERT(event);

  if (setPattern) {
    event->flag |= setPattern;
    OS_WakeupThread(&event->queue);
  }

  (void)OS_RestoreInterrupts(enable);
}

void OS_ClearAllEvent(OSEvent *event) {
  SDK_ASSERT(event);
  event->flag = 0;
}

void OS_ClearEvent(OSEvent *event, u32 clearBit) {
  OSIntrMode enable = OS_DisableInterrupts();
  SDK_ASSERT(event);

  event->flag &= ~clearBit;

  (void)OS_RestoreInterrupts(enable);
}

u32 OS_PollEvent(OSEvent *event, u32 pattern, OSEventMode mode) {
  return OS_PollEventEx(event, pattern, mode, 0);
}

u32 OS_PollEventEx(OSEvent *event, u32 pattern, OSEventMode mode,
                   u32 clearBit) {
  u32 retval = 0;
  OSIntrMode enable = OS_DisableInterrupts();

  SDK_ASSERT(event);

  switch (mode) {
  case OS_EVENT_MODE_AND:
    if ((event->flag & pattern) == pattern) {
      retval = event->flag;
    }
    break;
  case OS_EVENT_MODE_OR:
    if (event->flag & pattern) {
      retval = event->flag;
    }
    break;
  }

  if (retval) {
    event->flag &= ~clearBit;
  }

  (void)OS_RestoreInterrupts(enable);
  return retval;
}

#if defined(SDK_TCM_APPLY) && defined(SDK_ARM9)
#include <nitro/itcm_end.h>
#endif

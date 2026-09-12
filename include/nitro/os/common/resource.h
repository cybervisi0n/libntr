#ifndef NITRO_OS_RESOURCE_H_
#define NITRO_OS_RESOURCE_H_

#if SDK_VERSION_MAJOR != 5
#error This file is for SDK version 5 only!
#endif

#ifndef SDK_ASM
#include <nitro/types.h>

#include <nitro/os/common/thread.h>
#include <nitro/os/common/alarm.h>
#include <nitro/os/common/arena.h>
#include <nitro/os/common/valarm.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  BOOL threadResourceFlag;
  BOOL alarmResourceFlag;
  BOOL valarmResourceFlag;
  BOOL arenaResourceFlag;

  OSThreadResource threadResource;
  OSAlarmResource alarmResource;
  OSVAlarmResource valarmResource;
  OSArenaResource arenaResource;
} OSResource;

extern BOOL OS_GetAllResources(OSResource *resource);

#endif /* SDK_ASM */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

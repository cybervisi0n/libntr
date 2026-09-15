#include <nitro/os/common/resource.h>
#include <nitro/os/common/system.h>

BOOL OS_GetAllResources(OSResource *resource) {
  OSIntrMode enabled = OS_DisableInterrupts();
  BOOL flag = TRUE;
  SDK_ASSERT(resource != NULL);

  if (!(resource->threadResourceFlag =
            OS_GetThreadResource(&resource->threadResource))) {
    flag = FALSE;
  }

  if (!(resource->alarmResourceFlag =
            OS_GetAlarmResource(&resource->alarmResource))) {
    flag = FALSE;
  }

  if (!(resource->valarmResourceFlag =
            OS_GetVAlarmResource(&resource->valarmResource))) {
    flag = FALSE;
  }

  if (!(resource->arenaResourceFlag =
            OS_GetArenaResource(&resource->arenaResource))) {
    flag = FALSE;
  }

  (void)OS_RestoreInterrupts(enabled);
  return flag;
}

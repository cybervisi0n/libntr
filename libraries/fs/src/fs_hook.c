#include <nitro/fs/archive.h>
#include <nitro/fs/hook.h>
#include <nitro/card/common.h>
#include <nitro/std/string.h>
#ifdef SDK_TWL
#include <twl/fatfs/common/types.h>
#endif // SDK_TWL

typedef struct FSEventHookContext {
  FSEventHook *chain;
  CARDHookContext hook[1];
} FSEventHookContext;

static FSEventHookContext FSiEventHookStaticWork[1];

static void FSi_SystemEvents(void *userdata, CARDEvent event, void *argument) {
  FSEventHookContext *context = (FSEventHookContext *)userdata;
  FSArchive *target = NULL;
  FSEvent fsevent = FS_EVENT_NONE;
  (void)argument;

  if (event == CARD_EVENT_SLOTRESET) {
    target = FS_FindArchive("rom", 3);
    fsevent = FS_EVENT_MEDIA_INSERTED;
  } else if (event == CARD_EVENT_PULLEDOUT) {
    target = FS_FindArchive("rom", 3);
    fsevent = FS_EVENT_MEDIA_REMOVED;
  }
#ifdef SDK_TWL
  else if (event == FATFS_EVENT_SD_INSERTED) {
    target = FS_FindArchive("sdmc", 4);
    fsevent = FS_EVENT_MEDIA_INSERTED;
  } else if (event == FATFS_EVENT_SD_REMOVED) {
    target = FS_FindArchive("sdmc", 4);
    fsevent = FS_EVENT_MEDIA_REMOVED;
  }
#endif // SDK_TWL
  if (fsevent != FS_EVENT_NONE) {
    FSEventHook **pp = &context->chain;
    while (*pp != NULL) {
      FSEventHook *hook = *pp;
      if ((hook->arc == target) && (hook->callback != NULL)) {
        (*hook->callback)(hook->userdata, fsevent, argument);
      }
      if (*pp == hook) {
        pp = &(*pp)->next;
      }
    }
  }
}

void FS_RegisterEventHook(const char *arcname, FSEventHook *hook,
                          FSEventFunction callback, void *userdata) {
  FSEventHookContext *context = FSiEventHookStaticWork;
  OSIntrMode bak = OS_DisableInterrupts();
  FSArchive *arc = FS_FindArchive(arcname, STD_GetStringLength(arcname));
  if (arc) {

    if (!context->hook->callback) {
      CARDi_RegisterHook(context->hook, FSi_SystemEvents, context);
    }
    hook->callback = callback;
    hook->userdata = userdata;
    hook->arc = arc;
    hook->next = context->chain;
    context->chain = hook;
  }
  (void)OS_RestoreInterrupts(bak);
}

void FS_UnregisterEventHook(FSEventHook *hook) {
  FSEventHookContext *context = FSiEventHookStaticWork;
  OSIntrMode bak = OS_DisableInterrupts();
  FSEventHook **pp;
  for (pp = &context->chain; *pp; pp = &(*pp)->next) {
    if (*pp == hook) {
      *pp = (*pp)->next;
      break;
    }
  }
  (void)OS_RestoreInterrupts(bak);
}

#ifndef NITRO_FS_HOOK_H_
#define NITRO_FS_HOOK_H_

#include <nitro/fs/archive.h>

#if SDK_VERSION_MAJOR != 5
#error This file is for SDK version 5 only!
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef u32 FSEvent;
#define FS_EVENT_NONE 0x00000000
#define FS_EVENT_MEDIA_REMOVED 0x00000001
#define FS_EVENT_MEDIA_INSERTED 0x00000002

typedef void (*FSEventFunction)(void *userdata, FSEvent event, void *argument);
typedef struct FSEventHook {
  struct FSEventHook *next;
  FSEventFunction callback;
  void *userdata;
  FSArchive *arc;
} FSEventHook;

void FS_RegisterEventHook(const char *arc, FSEventHook *hook,
                          FSEventFunction callback, void *userdata);

void FS_UnregisterEventHook(FSEventHook *hook);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NITRO_FS_HOOK_H_ */

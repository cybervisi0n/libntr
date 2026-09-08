#ifndef TWL_NA_ARM9_ARCHIVE_H_
#define TWL_NA_ARM9_ARCHIVE_H_

#include <twl/misc.h>
#include <twl/types.h>
#include <nitro/os/common/emulator.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum NATitleArchive {
  NA_TITLE_ARCHIVE_DATAPUB,
  NA_TITLE_ARCHIVE_DATAPRV,
  NA_TITLE_ARCHIVE_MAX
} NATitleArchive;

FSResult NAi_LoadOtherTitleArchive(const char *initialCode,
                                   NATitleArchive archive);
FSResult NAi_UnloadOtherTitleArchive(void);
FSResult NAi_FormatTitleArchive(NATitleArchive archive);

static inline FSResult NA_LoadOtherTitleArchive(const char *initialCode,
                                                NATitleArchive archive) {
  return OS_IsRunOnTwl() ? NAi_LoadOtherTitleArchive(initialCode, archive)
                         : FS_RESULT_UNSUPPORTED;
}
static inline FSResult NA_UnloadOtherTitleArchive(void) {
  return OS_IsRunOnTwl() ? NAi_UnloadOtherTitleArchive()
                         : FS_RESULT_UNSUPPORTED;
}
static inline FSResult NA_FormatTitleArchive(NATitleArchive archive) {
  return OS_IsRunOnTwl() ? NAi_FormatTitleArchive(archive)
                         : FS_RESULT_UNSUPPORTED;
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TWL_NA_ARM9_ARCHIVE_H_ */

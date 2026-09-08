#ifndef TWL_NA_ARM9_VERDATA_H_
#define TWL_NA_ARM9_VERDATA_H_

#include <twl/misc.h>
#include <twl/types.h>
#include <nitro/os/common/emulator.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NA_VERSION_DATA_WORK_SIZE (40 * 1024)

BOOL NAi_LoadVersionDataArchive(void *pWork, u32 workSize);
BOOL NAi_UnloadVersionDataArchive(void);
s32 NAi_DecodeVersionData(const void *src, u32 srcSize, void *dst, u32 dstSize);

static inline BOOL NA_LoadVersionDataArchive(void *pWork, u32 workSize) {
  return OS_IsRunOnTwl() ? NAi_LoadVersionDataArchive(pWork, workSize) : FALSE;
}
static inline BOOL NA_UnloadVersionDataArchive(void) {
  return OS_IsRunOnTwl() ? NAi_UnloadVersionDataArchive() : FALSE;
}
static inline s32 NA_DecodeVersionData(const void *src, u32 srcSize, void *dst,
                                       u32 dstSize) {
  return OS_IsRunOnTwl() ? NAi_DecodeVersionData(src, srcSize, dst, dstSize)
                         : -4;
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TWL_NA_ARM9_VERDATA_H_ */

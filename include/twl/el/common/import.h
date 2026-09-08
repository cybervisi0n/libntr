#ifndef TWL_COMMON_IMPORT_H_
#define TWL_COMMON_IMPORT_H_

#include <twl/el.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  void *fp;
  const char *fn;
} ELImportEntry;

#ifdef SDK_DEBUG
extern const char *ELi_LastFunctionName;

SDK_INLINE void ELi_SetLastFunctionName(const char *fn) {
  ELi_LastFunctionName = fn;
}
#else
#define ELi_SetLastFunctionName(fn) (void)(fn);
#endif

void ELi_UnresolvedFunc(void);
void EL_LoadImportTable(ELDlld dlld, ELImportEntry entries[], u32 count);
void EL_UnloadImportTable(ELImportEntry entries[], u32 count);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*TWL_COMMON_IMPORT_H_*/

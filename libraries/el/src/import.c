#include <twl/el/common/import.h>

const char *ELi_LastFunctionName = NULL;

void ELi_UnresolvedFunc(void) {
  const char *const message = "An unresolved function is called!";
#ifdef SDK_DEBUG
  OS_TPanic("%s: %s()", message,
            ELi_LastFunctionName ? ELi_LastFunctionName : "unknown");
#else
  OS_TPanic("%s", message);
#endif
}

void EL_LoadImportTable(ELDlld dlld, ELImportEntry entries[], u32 count) {
  void *fp;
  u32 i;
  for (i = 0; i < count; ++i) {
    fp = EL_GetGlobalAdr(dlld, entries[i].fn);
    if (fp) {
      entries[i].fp = fp;
    } else {
      OS_TWarning("EL_GetGlobalAdr failed: %s", entries[i].fn);
    }
  }
}

void EL_UnloadImportTable(ELImportEntry entries[], u32 count) {
  u32 i;
  for (i = 0; i < count; ++i) {
    entries[i].fp = ELi_UnresolvedFunc;
  }
}

#include <twl.h>
#include <twl/na/ARM9/sub_banner.h>
#include <twl/na/ARM9/shared_font.h>

#include <twl/ltdmain_begin.h>

SDK_WEAK_SYMBOL BOOL NAi_InitSharedFont(void) { return FALSE; }
SDK_WEAK_SYMBOL int NAi_GetSharedFontTableSize(void) { return -1; }
SDK_WEAK_SYMBOL BOOL NAi_LoadSharedFontTable(void *pBuffer) {
#pragma unused(pBuffer)
  return FALSE;
}
SDK_WEAK_SYMBOL int NAi_GetSharedFontSize(NASharedFontIndex index) {
#pragma unused(index)
  return -1;
}
SDK_WEAK_SYMBOL BOOL NAi_LoadSharedFont(NASharedFontIndex index,
                                        void *pBuffer) {
#pragma unused(index)
#pragma unused(pBuffer)
  return FALSE;
}

SDK_WEAK_SYMBOL BOOL NAi_SaveSubBanner(const NASubBanner *banner) {
#pragma unused(banner)
  return FALSE;
}
SDK_WEAK_SYMBOL BOOL NAi_DeleteSubBanner(NASubBanner *workBuffer) {
#pragma unused(workBuffer)
  return FALSE;
}

#include <twl/ltdmain_end.h>

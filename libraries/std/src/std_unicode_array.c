#include <nitro.h>

#if defined(SDK_ARM9) || !defined(SDK_NITRO)

#if defined(SDK_ARM9)
#define STD_UNICODE_STATIC_IMPLEMENTATION
#endif

#if defined(STD_UNICODE_STATIC_IMPLEMENTATION)

#include "sjis2unicode.h"
#include "unicode2sjis.h"

SDK_WEAK_SYMBOL const u8 *STD_Unicode2SjisArray = unicode2sjis_array;
SDK_WEAK_SYMBOL const u16 *STD_Sjis2UnicodeArray = sjis2unicode_array;
#endif

#endif // defined(SDK_ARM9) || !defined(SDK_NITRO)

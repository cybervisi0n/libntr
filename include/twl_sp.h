#ifndef TWL_SP_H
#define TWL_SP_H

#if SDK_VERSION_MAJOR != 5
#error This file is only supported on SDK version 5
#endif

#ifndef SDK_ARM7
#define SDK_ARM7
#endif
#ifdef SDK_ARM9
#undef SDK_ARM9
#endif

#include <twl.h>
#endif
#ifndef LIBRARIES_WM_ARM9_WM_COMMON_H__
#define LIBRARIES_WM_ARM9_WM_COMMON_H__

#if SDK_VERSION_MAJOR != 5
#error This file is for SDK version 5 only!
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SDK_TWL
#include "nwm_arm9_private.h"
#endif
#include "wm_arm9_private.h"
#include <nitro/os.h>

extern WMErrCode WM_CheckInitialized();
#ifdef SDK_TWL
extern NWMRetCode NWM_CheckInitialized();
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LIBRARIES_WM_ARM9_WM_COMMON_H__ */

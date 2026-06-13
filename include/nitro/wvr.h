#ifndef NITRO_WVR_H_
#define NITRO_WVR_H_

#ifdef  __cplusplus
extern "C" {
#endif

#ifdef SDK_ARM9
    #include <nitro/wvr/common/wvr_common.h>
    #include <nitro/wvr/ARM9/wvr.h>
#else
    #ifdef SDK_ARM7
    #include <nitro/wvr/common/wvr_common.h>
    #include <nitro/wvr/ARM7/wvr_sp.h>
    #endif
#endif

#ifdef SDK_PORT
    #include <nitro/wvr/common/wvr_common.h>
    #include <nitro/wvr/ARM9/wvr.h>
#endif

#ifdef  __cplusplus
}
#endif

#endif

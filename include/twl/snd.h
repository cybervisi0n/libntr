#ifndef TWL_SND_H_
#define TWL_SND_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/snd.h>
#include <twl/snd/common/sndex_common.h>

#if defined(SDK_ARM9) || defined(SDK_PORT)
#include <twl/snd/ARM9/sndex.h>
#else // SDK_ARM7
#include <twl/snd/ARM7/sndex_api.h>
#endif

#ifdef __cplusplus
} // extern "C"
#endif
#endif // TWL_SND_H_

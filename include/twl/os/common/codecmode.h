#ifndef TWL_OS_COMMON_CODECMODE_H_
#define TWL_OS_COMMON_CODECMODE_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <twl/types.h>
#if defined(SDK_ARM9) || defined(SDK_PORT)
#include <twl/memorymap.h>
#else
#include <twl/memorymap_sp.h>
#endif

typedef enum { OS_CODECMODE_NITRO = 0, OS_CODECMODE_TWL = 1 } OSCodecMode;

#define OS_IsCodecTwlMode OSi_IsCodecTwlMode

#ifdef SDK_TWL
BOOL OSi_IsCodecTwlMode(void);
#else // SDK_NITRO
#define OSi_IsCodecTwlMode() (FALSE)
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* TWL_OS_COMMON_CODECMODE_H_ */

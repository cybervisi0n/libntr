#ifndef NITRO_OS_PXI_H_
#define NITRO_OS_PXI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/misc.h>
#include <nitro/types.h>
#include <nitro/pxi.h>

BOOL OS_IsResetOccurred(void);

#ifdef SDK_TWL

BOOL OSi_IsTerminatePxiOccurred(void);

#endif // SDK_TWL

#ifdef SDK_TWL
void OSi_SetTerminatePxiOccurred(void);
#else // SDK_NITRO
#define OSi_SetTerminatePxiOccurred() ((void)0)
#endif // SDK_NITRO

#ifdef SDK_PORT
void OSi_CommonCallback(PXIFifoTag tag, u64 data, BOOL err);
#else
void OSi_CommonCallback(PXIFifoTag tag, u32 data, BOOL err);
#endif
void OSi_SendToPxi(u16 data);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

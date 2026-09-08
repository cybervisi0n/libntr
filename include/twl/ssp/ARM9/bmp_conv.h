#ifndef TWL_SSP_BMP_H_
#define TWL_SSP_BMP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <twl/types.h>

typedef enum { SSP_BMP_YUV422 = 1, SSP_BMP_RGB555 } SSPConvertBmpType;

u32 SSP_YUV422ToRGB888b(const void *src, void *dst, u16 width, u16 height);

u32 SSP_RGB555ToRGB888b(const void *src, void *dst, u16 width, u16 height);

u32 SSP_GetRGB888Size(u16 width, u16 height);

void SSP_CreateBmpHeader(u8 *dst, u16 width, u16 height);

u32 SSP_GetBmpHeaderSize();

u32 SSP_GetBmpFileSize(u16 width, u16 height);

u32 SSP_CreateBmpFile(const void *src, void *dst, u16 width, u16 height,
                      SSPConvertBmpType type);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

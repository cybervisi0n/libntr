#ifndef NITRO_MI_SECURE_UNCOMPRESSION_H__
#define NITRO_MI_SECURE_UNCOMPRESSION_H__

#if SDK_VERSION_MAJOR != 5
#error This file is for SDK version 5 only!
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/types.h>

#define MI_ERR_SUCCESS 0
#define MI_ERR_UNSUPPORTED -1
#define MI_ERR_SRC_SHORTAGE -2
#define MI_ERR_SRC_REMAINDER -3
#define MI_ERR_DEST_OVERRUN -4
#define MI_ERR_ILLEGAL_TABLE -5

s32 MI_SecureUncompressAny(const void *srcp, u32 srcSize, void *destp,
                           u32 dstSize);

s32 MI_SecureUncompressRL(const void *srcp, u32 srcSize, void *destp,
                          u32 dstSize);

s32 MI_SecureUncompressLZ(const void *srcp, u32 srcSize, void *destp,
                          u32 dstSize);

s32 MI_SecureUncompressHuffman(const void *srcp, u32 srcSize, void *destp,
                               u32 dstSize);

s32 MI_SecureUnfilterDiff(register const void *srcp, u32 srcSize,
                          register void *destp, u32 dstSize);

s32 MI_SecureUncompressBLZ(const void *srcp, u32 srcSize, u32 dstSize);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

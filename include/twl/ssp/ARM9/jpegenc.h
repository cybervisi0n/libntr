#ifndef TWL_SSP_JPEGENC_H_
#define TWL_SSP_JPEGENC_H_

#include <twl/ssp/common/ssp_jpeg_type.h>
#include <twl/ssp/ARM9/exifenc.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <twl/types.h>

u32 SSP_GetJpegEncoderBufferSize(u32 width, u32 height, u32 sampling,
                                 u32 option);

u32 SSP_StartJpegEncoder(const void *src, u8 *dst, u32 limit, u8 *wrk,
                         u32 width, u32 height, u32 quality, u32 sampling,
                         u32 option);

static inline u32 SSP_StartJpegEncoderEx(const void *src, u8 *dst, u32 limit,
                                         u8 *wrk, u32 width, u32 height,
                                         u32 quality, u32 sampling, u32 option,
                                         BOOL sign) {
  u32 result;
  BOOL old_sign;

  old_sign = SSP_SetJpegEncoderSignMode(sign);
  result = SSP_StartJpegEncoder(src, dst, limit, wrk, width, height, quality,
                                sampling, option);
  (void)SSP_SetJpegEncoderSignMode(old_sign);

  return result;
}

BOOL SSP_ConvertJpegEncodeData(const void *src, u8 *wrk, u32 width, u32 height,
                               u32 sampling, u32 option);

u32 SSP_StartJpegEncoderWithEncodeData(u8 *dst, u32 limit, u8 *wrk, u32 width,
                                       u32 height, u32 quality, u32 sampling,
                                       u32 option);

static inline u32 SSP_StartJpegEncoderWithEncodeDataEx(u8 *dst, u32 limit,
                                                       u8 *wrk, u32 width,
                                                       u32 height, u32 quality,
                                                       u32 sampling, u32 option,
                                                       BOOL sign) {
  u32 result;
  BOOL old_sign;

  old_sign = SSP_SetJpegEncoderSignMode(sign);
  result = SSP_StartJpegEncoderWithEncodeData(dst, limit, wrk, width, height,
                                              quality, sampling, option);
  (void)SSP_SetJpegEncoderSignMode(old_sign);

  return result;
}

u32 SSP_GetJpegEncoderLiteBufferSize(u32 option);

u32 SSP_StartJpegEncoderLite(const void *src, u8 *dst, u32 limit, u8 *wrk,
                             u32 width, u32 height, u32 quality, u32 sampling,
                             u32 option);

static inline u32 SSP_StartJpegEncoderLiteEx(const void *src, u8 *dst,
                                             u32 limit, u8 *wrk, u32 width,
                                             u32 height, u32 quality,
                                             u32 sampling, u32 option,
                                             BOOL sign) {
  u32 result;
  BOOL old_sign;

  old_sign = SSP_SetJpegEncoderSignMode(sign);
  result = SSP_StartJpegEncoderLite(src, dst, limit, wrk, width, height,
                                    quality, sampling, option);
  (void)SSP_SetJpegEncoderSignMode(old_sign);

  return result;
}

u32 SSP_GetJpegEncoderFastBufferSize(u32 option);

u32 SSP_StartJpegEncoderFast(const void *src, u8 *dst, u32 limit, u8 *wrk,
                             u32 width, u32 height, u32 quality, u32 sampling,
                             u32 option);

u32 SSP_StartJpegEncoderFastEx(const void *src, u8 *dst, u32 limit, u8 *wrk,
                               u32 width, u32 height, u32 quality, u32 sampling,
                               u32 option, BOOL sign);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

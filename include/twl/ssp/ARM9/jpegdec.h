#ifndef TWL_SSP_JPEGDEC_H_
#define TWL_SSP_JPEGDEC_H_

#include <twl/ssp/common/ssp_jpeg_type.h>
#include <twl/ssp/ARM9/exifdec.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <twl/types.h>

typedef enum SSPJpegDecoderErrorCode {
  SSP_JPEG_DECODER_OK = 0,                  // Success
  SSP_JPEG_DECODER_ERROR_ARGUMENT = (-1),   // There is a mistake in an argument
                                            // other than the 'option' argument
  SSP_JPEG_DECODER_ERROR_WORK_ALIGN = (-2), // pCtx is not in 4-byte alignment
  SSP_JPEG_DECODER_ERROR_OPTION =
      (-3), // There is a mistake in the 'option' argument
  SSP_JPEG_DECODER_ERROR_SIGN = (-10),

  SSP_JPEG_DECODER_ERROR_WIDTH_HEIGHT =
      (-20), // Either the width or the height exceeds the specified maximum
             // value. The actual size is in pCtx->width and pCtx->height

  SSP_JPEG_DECODER_ERROR_EXIF_0 = (-30), // Could not process data

  SSP_JPEG_DECODER_ERROR_MARKER_COMBINATION = (-50), // Could not process data

  SSP_JPEG_DECODER_ERROR_SOI = (-60), // Could not process data (or marker)
  SSP_JPEG_DECODER_ERROR_SOF = (-61),
  SSP_JPEG_DECODER_ERROR_SOF_BLOCK_ID = (-62),
  SSP_JPEG_DECODER_ERROR_DHT = (-63),
  SSP_JPEG_DECODER_ERROR_SOS = (-64),
  SSP_JPEG_DECODER_ERROR_DQT = (-65),
  SSP_JPEG_DECODER_ERROR_DRI = (-66),

  SSP_JPEG_DECODER_ERROR_UNDERRUN_0 = (-90), // Could not process data
  SSP_JPEG_DECODER_ERROR_UNDERRUN_1 = (-91),
  SSP_JPEG_DECODER_ERROR_UNDERRUN_2 = (-92),
  SSP_JPEG_DECODER_ERROR_UNDERRUN_3 = (-93),
  SSP_JPEG_DECODER_ERROR_UNDERRUN_4 = (-94),
  SSP_JPEG_DECODER_ERROR_UNDERRUN_5 = (-95),

  SSP_JPEG_DECODER_ERROR_RANGE_0 = (-110), // Could not process data
  SSP_JPEG_DECODER_ERROR_RANGE_1 = (-111),
  SSP_JPEG_DECODER_ERROR_RANGE_2 = (-112),
  SSP_JPEG_DECODER_ERROR_RANGE_3 = (-113),
  SSP_JPEG_DECODER_ERROR_RANGE_4 = (-114),
  SSP_JPEG_DECODER_ERROR_RANGE_5 = (-115),

  SSP_JPEG_DECODER_ERROR_HLB_0 = (-120), // Could not process data

  SSP_JPEG_DECODER_ERROR_INTERNAL_CTX_SIZE =
      (-128) // Internal error. Normally, this does not occur
} SSPJpegDecoderErrorCode;

typedef struct SSPJpegDecoderFastContext {
  u8 *pSrc;
  u32 inputDataSize;
  void *pDst;
  u32 option;
  u16 maxWidth; // Maximum allowable image width (less than 65536). The argument
                // maxWidth of the SSP_StartJpegDecoderFast function is copied
                // here
  u16 maxHeight; // Maximum allowable image height (less than 65536). The
                 // argument maxHeight of the SSP_StartJpegDecoderFast function
                 // is copied here
  u16 width;     // Actual image width
  u16 height;    // Actual image height
  SSPJpegDecoderErrorCode errorCode; // No error if 0
  u8 reserved[4];
  u32 work[0x7f8];
} SSPJpegDecoderFastContext;

BOOL SSP_CheckJpegDecoderSign(u8 *data, u32 size);

BOOL SSP_StartJpegDecoder(u8 *data, u32 size, void *dst, s16 *width,
                          s16 *height, u32 option);

static inline BOOL SSP_StartJpegDecoderEx(u8 *data, u32 size, void *dst,
                                          s16 *width, s16 *height, u32 option,
                                          BOOL sign) {
  BOOL result;
  BOOL old_sign;

  old_sign = SSP_SetJpegDecoderSignMode(sign);
  result = SSP_StartJpegDecoder(data, size, dst, width, height, option);
  (void)SSP_SetJpegDecoderSignMode(old_sign);

  return result;
}

static inline BOOL SSP_ExtractJpegDecoderExif(u8 *data, u32 size) {
  return SSP_StartJpegDecoder(data, size, 0, 0, 0, SSP_JPEG_EXIF);
}

BOOL SSP_StartJpegDecoderFast(SSPJpegDecoderFastContext *pCtx, u8 *data,
                              u32 size, void *dst, u32 maxWidth, u32 maxHeight,
                              u32 option);

BOOL SSP_StartJpegDecoderFastEx(SSPJpegDecoderFastContext *pCtx, u8 *data,
                                u32 size, void *dst, u32 maxWidth,
                                u32 maxHeight, u32 option, BOOL sign);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

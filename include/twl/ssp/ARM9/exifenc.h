#ifndef TWL_SSP_EXIFENC_H_
#define TWL_SSP_EXIFENC_H_

#include <twl/ssp/common/ssp_jpeg_type.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <twl/types.h>

void SSP_SetJpegEncoderMakerNoteEx(SSPJpegMakernote tag, const u8 *buffer,
                                   u32 size);

static inline void SSP_SetJpegEncoderMakerNote(const u8 *buffer, u32 size) {
  SSP_SetJpegEncoderMakerNoteEx(SSP_MAKERNOTE_PHOTO, buffer, size);
}

void SSP_SetJpegEncoderDateTime(const u8 *buffer);

BOOL SSP_GetDateTime(RTCDate *date, RTCTime *time);

BOOL SSP_SetJpegEncoderDateTimeNow(void);

BOOL SSP_SetJpegEncoderSignMode(BOOL mode);

u32 SSP_ExifEncode(u8 *l_dst, u32 width, u32 height, const u8 *thumb_src,
                   u32 thumb_size);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

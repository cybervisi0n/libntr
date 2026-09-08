#ifndef TWL_SSP_EXIFDEC_H_
#define TWL_SSP_EXIFDEC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <twl/types.h>

u16 SSP_GetJpegDecoderDateTime(u8 *buffer);

u16 SSP_GetJpegDecoderSoftware(char *buffer);

u8 *SSP_GetJpegDecoderMakerNoteAddrEx(SSPJpegMakernote tag);

u16 SSP_GetJpegDecoderMakerNoteSizeEx(SSPJpegMakernote tag);

static inline u8 *SSP_GetJpegDecoderMakerNoteAddr(void) {
  return SSP_GetJpegDecoderMakerNoteAddrEx(SSP_MAKERNOTE_PHOTO);
}

static inline u16 SSP_GetJpegDecoderMakerNoteSize(void) {
  return SSP_GetJpegDecoderMakerNoteSizeEx(SSP_MAKERNOTE_PHOTO);
}

BOOL SSP_SetJpegDecoderSignMode(BOOL mode);

s32 SSP_ExifDecode(u8 *src, u32 src_size, int *cur, int option);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

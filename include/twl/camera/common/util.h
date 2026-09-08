#ifndef TWL_CAMERA_UTIL_H_
#define TWL_CAMERA_UTIL_H_

#include <twl/types.h>
#include <twl/camera/common/types.h>
#include <nitro/os/common/emulator.h>

#ifdef __cplusplus
extern "C" {
#endif

u16 CAMERA_SizeToWidthCore(CAMERASize size);
SDK_INLINE u16 CAMERA_SizeToWidth(CAMERASize size) {
  if (OS_IsRunOnTwl() == TRUE) {
    return CAMERA_SizeToWidthCore(size);
  }
  return 0;
}

u16 CAMERA_SizeToHeightCore(CAMERASize size);
SDK_INLINE u16 CAMERA_SizeToHeight(CAMERASize size) {
  if (OS_IsRunOnTwl() == TRUE) {
    return CAMERA_SizeToHeightCore(size);
  }
  return 0;
}

u32 CAMERA_SizeToPixelsCore(CAMERASize size);
SDK_INLINE u32 CAMERA_SizeToPixels(CAMERASize size) {
  if (OS_IsRunOnTwl() == TRUE) {
    return CAMERA_SizeToPixelsCore(size);
  }
  return 0;
}

CAMERASize CAMERA_WidthHeightToSizeCore(u16 width, u16 height);
SDK_INLINE CAMERASize CAMERA_WidthHeightToSize(u16 width, u16 height) {
  if (OS_IsRunOnTwl() == TRUE) {
    return CAMERA_WidthHeightToSizeCore(width, height);
  }
  return (CAMERASize)0;
}

CAMERASize CAMERA_WidthToSizeCore(u16 width);
SDK_INLINE CAMERASize CAMERA_WidthToSize(u16 width) {
  if (OS_IsRunOnTwl() == TRUE) {
    return CAMERA_WidthToSizeCore(width);
  }
  return (CAMERASize)0;
}

CAMERASize CAMERA_HeightToSizeCore(u16 height);
SDK_INLINE CAMERASize CAMERA_HeightToSize(u16 height) {
  if (OS_IsRunOnTwl() == TRUE) {
    return CAMERA_HeightToSizeCore(height);
  }
  return (CAMERASize)0;
}

CAMERASize CAMERA_PixelsToSizeCore(u32 pixels);
SDK_INLINE CAMERASize CAMERA_PixelsToSize(u32 pixels) {
  if (OS_IsRunOnTwl() == TRUE) {
    return CAMERA_PixelsToSizeCore(pixels);
  }
  return (CAMERASize)0;
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TWL_CAMERA_UTIL_H_ */

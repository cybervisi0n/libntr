#include <twl.h>
#include <twl/camera.h>

u16 CAMERA_SizeToWidthCore(CAMERASize size) {
  switch (size) {
  case CAMERA_SIZE_VGA:
    return 640;
  case CAMERA_SIZE_QVGA:
    return 320;
  case CAMERA_SIZE_QQVGA:
    return 160;
  case CAMERA_SIZE_CIF:
    return 352;
  case CAMERA_SIZE_QCIF:
    return 176;
  case CAMERA_SIZE_DS_LCD:
    return 256;
  case CAMERA_SIZE_DS_LCDx4:
    return 512;
  }
  return 0;
}

u16 CAMERA_SizeToHeightCore(CAMERASize size) {
  switch (size) {
  case CAMERA_SIZE_VGA:
    return 480;
  case CAMERA_SIZE_QVGA:
    return 240;
  case CAMERA_SIZE_QQVGA:
    return 120;
  case CAMERA_SIZE_CIF:
    return 288;
  case CAMERA_SIZE_QCIF:
    return 144;
  case CAMERA_SIZE_DS_LCD:
    return 192;
  case CAMERA_SIZE_DS_LCDx4:
    return 384;
  }
  return 0;
}

u32 CAMERA_SizeToPixelsCore(CAMERASize size) {
  switch (size) {
  case CAMERA_SIZE_VGA:
    return 640 * 480;
  case CAMERA_SIZE_QVGA:
    return 320 * 240;
  case CAMERA_SIZE_QQVGA:
    return 160 * 120;
  case CAMERA_SIZE_CIF:
    return 352 * 288;
  case CAMERA_SIZE_QCIF:
    return 176 * 144;
  case CAMERA_SIZE_DS_LCD:
    return 256 * 192;
  case CAMERA_SIZE_DS_LCDx4:
    return 512 * 384;
  }
  return 0;
}

CAMERASize CAMERA_WidthHeightToSizeCore(u16 width, u16 height) {
  int size;
  for (size = 0; size < CAMERA_SIZE_MAX; size++) {
    if (width == CAMERA_SizeToWidthCore((CAMERASize)size)) {
      if (height == CAMERA_SizeToHeightCore((CAMERASize)size)) {
        break;
      }
      return CAMERA_SIZE_MAX;
    }
  }
  return (CAMERASize)size;
}

CAMERASize CAMERA_WidthToSizeCore(u16 width) {
  int size;
  for (size = 0; size < CAMERA_SIZE_MAX; size++) {
    if (width == CAMERA_SizeToWidthCore((CAMERASize)size)) {
      break;
    }
  }
  return (CAMERASize)size;
}

CAMERASize CAMERA_HeightToSizeCore(u16 height) {
  int size;
  for (size = 0; size < CAMERA_SIZE_MAX; size++) {
    if (height == CAMERA_SizeToHeightCore((CAMERASize)size)) {
      break;
    }
  }
  return (CAMERASize)size;
}

CAMERASize CAMERA_PixelsToSizeCore(u32 pixels) {
  int size;
  for (size = 0; size < CAMERA_SIZE_MAX; size++) {
    if (pixels == CAMERA_SizeToPixelsCore((CAMERASize)size)) {
      break;
    }
  }
  return (CAMERASize)size;
}

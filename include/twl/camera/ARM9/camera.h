#ifndef TWL_CAMERA_CAMERA_H_
#define TWL_CAMERA_CAMERA_H_

#include <twl/misc.h>
#include <twl/types.h>
#include <twl/hw/ARM9/ioreg.h>

#include <twl/camera/ARM9/camera_api.h>

#include <nitro/math.h>

#define CAMERA_GET_MAX_LINES(width) MATH_MIN((1024 / (width)), 16)
#define CAMERA_GET_LINE_BYTES(width) ((width) << 1)
#define CAMERA_GET_FRAME_BYTES(width, height)                                  \
  (CAMERA_GET_LINE_BYTES(width) * (height))

#ifdef __cplusplus
extern "C" {
#endif

typedef enum { CAMERA_OUTPUT_YUV, CAMERA_OUTPUT_RGB } CAMERAOutput;

typedef enum {
  CAMERA_INTR_VSYNC_NONE = (0 << REG_CAM_CNT_IREQVS_SHIFT),
  CAMERA_INTR_VSYNC_NEGATIVE_EDGE = (2 << REG_CAM_CNT_IREQVS_SHIFT),
  CAMERA_INTR_VSYNC_POSITIVE_EDGE = (3 << REG_CAM_CNT_IREQVS_SHIFT)
} CAMERAIntrVsync;

void CAMERA_ResetCore(void);
SDK_INLINE void CAMERA_Reset(void) {
  if (OS_IsRunOnTwl() == TRUE) {
    CAMERA_ResetCore();
  }
}

BOOL CAMERA_IsBusyCore(void);
SDK_INLINE BOOL CAMERA_IsBusy(void) {
  if (OS_IsRunOnTwl() == TRUE) {
    return CAMERA_IsBusyCore();
  }
  return FALSE;
}

void CAMERA_StartCaptureCore(void);
SDK_INLINE void CAMERA_StartCapture(void) {
  if (OS_IsRunOnTwl() == TRUE) {
    CAMERA_StartCaptureCore();
  }
}

void CAMERA_StopCaptureCore(void);
SDK_INLINE void CAMERA_StopCapture(void) {
  if (OS_IsRunOnTwl() == TRUE) {
    CAMERA_StopCaptureCore();
  }
}

void CAMERA_SetTrimmingParamsCenterCore(u16 destWidth, u16 destHeight,
                                        u16 srcWidth, u16 srcHeight);
SDK_INLINE void CAMERA_SetTrimmingParamsCenter(u16 destWidth, u16 destHeight,
                                               u16 srcWidth, u16 srcHeight) {
  if (OS_IsRunOnTwl() == TRUE) {
    CAMERA_SetTrimmingParamsCenterCore(destWidth, destHeight, srcWidth,
                                       srcHeight);
  }
}

void CAMERA_SetTrimmingParamsCore(u16 x1, u16 y1, u16 x2, u16 y2);
SDK_INLINE void CAMERA_SetTrimmingParams(u16 x1, u16 y1, u16 x2, u16 y2) {
  if (OS_IsRunOnTwl() == TRUE) {
    CAMERA_SetTrimmingParamsCore(x1, y1, x2, y2);
  }
}

void CAMERA_GetTrimmingParamsCore(u16 *x1, u16 *y1, u16 *x2, u16 *y2);
SDK_INLINE void CAMERA_GetTrimmingParams(u16 *x1, u16 *y1, u16 *x2, u16 *y2) {
  if (OS_IsRunOnTwl() == TRUE) {
    CAMERA_GetTrimmingParamsCore(x1, y1, x2, y2);
  }
}

void CAMERA_SetTrimmingCore(BOOL enabled);
SDK_INLINE void CAMERA_SetTrimming(BOOL enabled) {
  if (OS_IsRunOnTwl() == TRUE) {
    CAMERA_SetTrimmingCore(enabled);
  }
}

BOOL CAMERA_IsTrimmingCore(void);
SDK_INLINE BOOL CAMERA_IsTrimming(void) {
  if (OS_IsRunOnTwl() == TRUE) {
    return CAMERA_IsTrimmingCore();
  }
  return FALSE;
}

void CAMERA_SetOutputFormatCore(CAMERAOutput output);
SDK_INLINE void CAMERA_SetOutputFormat(CAMERAOutput output) {
  if (OS_IsRunOnTwl() == TRUE) {
    CAMERA_SetOutputFormatCore(output);
  }
}

CAMERAOutput CAMERA_GetOutputFormatCore(void);
SDK_INLINE CAMERAOutput CAMERA_GetOutputFormat(void) {
  if (OS_IsRunOnTwl() == TRUE) {
    return CAMERA_GetOutputFormatCore();
  }
  return CAMERA_OUTPUT_YUV;
}

BOOL CAMERA_GetErrorStatusCore(void);
SDK_INLINE BOOL CAMERA_GetErrorStatus(void) {
  if (OS_IsRunOnTwl() == TRUE) {
    return CAMERA_GetErrorStatusCore();
  }
  return FALSE;
}

void CAMERA_ClearBufferCore(void);
SDK_INLINE void CAMERA_ClearBuffer(void) {
  if (OS_IsRunOnTwl() == TRUE) {
    CAMERA_ClearBufferCore();
  }
}

void CAMERA_SetTransferLinesCore(int lines);
SDK_INLINE void CAMERA_SetTransferLines(int lines) {
  if (OS_IsRunOnTwl() == TRUE) {
    CAMERA_SetTransferLinesCore(lines);
  }
}

int CAMERA_GetTransferLinesCore(void);
SDK_INLINE int CAMERA_GetTransferLines(void) {
  if (OS_IsRunOnTwl() == TRUE) {
    return CAMERA_GetTransferLinesCore();
  }
  return 0;
}

int CAMERA_GetMaxLinesRoundCore(u16 width, u16 height);
SDK_INLINE int CAMERA_GetMaxLinesRound(u16 width, u16 height) {
  if (OS_IsRunOnTwl() == TRUE) {
    return CAMERA_GetMaxLinesRoundCore(width, height);
  }
  return 0;
}

u32 CAMERA_GetBytesAtOnceCore(u16 width);
SDK_INLINE u32 CAMERA_GetBytesAtOnce(u16 width) {
  if (OS_IsRunOnTwl() == TRUE) {
    return CAMERA_GetBytesAtOnceCore(width);
  }
  return 0;
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

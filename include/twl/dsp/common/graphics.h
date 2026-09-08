#ifndef TWL_DSP_GRAPHICS_H_
#define TWL_DSP_GRAPHICS_H_

#include <twl/dsp/common/pipe.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef DSPWord DSPGraphicsScalingMode;
#define DSP_GRAPHICS_SCALING_MODE_N_NEIGHBOR (DSPGraphicsScalingMode)0x0001
#define DSP_GRAPHICS_SCALING_MODE_BILINEAR (DSPGraphicsScalingMode)0x0002
#define DSP_GRAPHICS_SCALING_MODE_BICUBIC (DSPGraphicsScalingMode)0x0003

#define DSP_GRAPHICS_SCALING_MODE_NPARTSHRINK (DSPGraphicsScalingMode)0x000A

#define DSP_GRAPHICS_COM_REGISTER                                              \
  0 // Register number for processing content notifications
#define DSP_GRAPHICS_REP_REGISTER                                              \
  1 // Register number for processing result notifications

typedef enum DspState {
  DSP_STATE_FAIL = 0x0000,
  DSP_STATE_SUCCESS = 0x0001

} DSPSTATE;

typedef enum _GraphicsFuncID {
  DSP_G_FUNCID_SCALING = 0x0001,
  DSP_G_FUNCID_YUV2RGB,

  DSP_FUNCID_NUM
} DSPGraphicsFuncID;

typedef struct _Yuv2RgbParam {
  u32 size;
  u32 src;
  u32 dst;
} DSPYuv2RgbParam;

typedef struct _ScalingParam {
  u32 src;
  u32 dst;
  u16 mode;
  u16 img_width;
  u16 img_height;
  u16 rate_w;
  u16 rate_h;
  u16 block_x;
  u16 block_y;
  u16 width;
  u16 height;
  u16 pad[1]; // This makes the size of _ScalingParam a multiple of 4
} DSPScalingParam;

typedef void (*DSP_GraphicsCallback)(
    void); // User-specified callback function to invoke when processing is
           // complete

#ifdef SDK_TWL

#define DSP_CALC_SCALING_SIZE(value, ratio)                                    \
  ((u32)(value * (u32)(ratio * 1000) / 1000))

#define DSP_CALC_SCALING_SIZE_FX(value, ratio)                                 \
  ((u32)(value * (u32)(ratio * 1000 / 4096.0f) / 1000))

static DSP_GraphicsCallback callBackFunc; // Callback function to invoke when
                                          // asynchronous processing is complete

static volatile BOOL isBusy; // TRUE if some process is running on the DSP
static volatile BOOL
    isAsync; // TRUE if some asynchronous process is running on the DSP

void DSPi_OpenStaticComponentGraphicsCore(FSFile *file);
BOOL DSPi_LoadGraphicsCore(FSFile *file, int slotB, int slotC);
void DSPi_UnloadGraphicsCore(void);
BOOL DSPi_ConvertYuvToRgbCore(const void *src, void *dst, u32 size,
                              DSP_GraphicsCallback callback, BOOL async);
BOOL DSPi_ScalingCore(const void *src, void *dst, u16 img_width, u16 img_height,
                      f32 rw, f32 ry, DSPGraphicsScalingMode mode, u16 x, u16 y,
                      u16 width, u16 height, DSP_GraphicsCallback callback,
                      BOOL async);
BOOL DSPi_ScalingFxCore(const void *src, void *dst, u16 img_width,
                        u16 img_height, fx32 rw, fx32 ry,
                        DSPGraphicsScalingMode mode, u16 x, u16 y, u16 width,
                        u16 height, DSP_GraphicsCallback callback, BOOL async);

static inline void DSP_OpenStaticComponentGraphics(FSFile *file) {
  if (OS_IsRunOnTwl()) {
    DSPi_OpenStaticComponentGraphicsCore(file);
  }
}

static inline BOOL DSP_LoadGraphics(FSFile *file, int slotB, int slotC) {
  if (OS_IsRunOnTwl() == TRUE) {
    return DSPi_LoadGraphicsCore(file, slotB, slotC);
  }
  return FALSE;
}

static inline void DSP_UnloadGraphics(void) {
  if (OS_IsRunOnTwl()) {
    DSPi_UnloadGraphicsCore();
  }
}

static inline BOOL DSP_ConvertYuvToRgb(const void *src, void *dst, u32 size) {
  if (OS_IsRunOnTwl() && !isBusy) {
    return DSPi_ConvertYuvToRgbCore(src, dst, size, NULL, FALSE);
  }
  return FALSE;
}

static inline BOOL DSP_ConvertYuvToRgbAsync(const void *src, void *dst,
                                            u32 size,
                                            DSP_GraphicsCallback callback) {
  if (OS_IsRunOnTwl() && !isBusy) {
    return DSPi_ConvertYuvToRgbCore(src, dst, size, callback, TRUE);
  }
  return FALSE;
}

static inline BOOL DSP_Scaling(const void *src, void *dst, u16 img_width,
                               u16 img_height, f32 rx, f32 ry,
                               DSPGraphicsScalingMode mode) {
  if (OS_IsRunOnTwl() && !isBusy) {
    return DSPi_ScalingCore(src, dst, img_width, img_height, rx, ry, mode, 0, 0,
                            img_width, img_height, NULL, FALSE);
  }
  return FALSE;
}

static inline BOOL DSP_ScalingFx(const void *src, void *dst, u16 img_width,
                                 u16 img_height, fx32 rx, fx32 ry,
                                 DSPGraphicsScalingMode mode) {
  if (OS_IsRunOnTwl() && !isBusy) {
    return DSPi_ScalingFxCore(src, dst, img_width, img_height, rx, ry, mode, 0,
                              0, img_width, img_height, NULL, FALSE);
  }
  return FALSE;
}

static inline BOOL DSP_ScalingAsync(const void *src, void *dst, u16 img_width,
                                    u16 img_height, f32 rx, f32 ry,
                                    DSPGraphicsScalingMode mode,
                                    DSP_GraphicsCallback callback) {
  if (OS_IsRunOnTwl() && !isBusy) {
    return DSPi_ScalingCore(src, dst, img_width, img_height, rx, ry, mode, 0, 0,
                            img_width, img_height, callback, TRUE);
  }
  return FALSE;
}

static inline BOOL DSP_ScalingFxAsync(const void *src, void *dst, u16 img_width,
                                      u16 img_height, fx32 rx, fx32 ry,
                                      DSPGraphicsScalingMode mode,
                                      DSP_GraphicsCallback callback) {
  if (OS_IsRunOnTwl() && !isBusy) {
    return DSPi_ScalingFxCore(src, dst, img_width, img_height, rx, ry, mode, 0,
                              0, img_width, img_height, callback, TRUE);
  }
  return FALSE;
}

static inline BOOL DSP_ScalingEx(const void *src, void *dst, u16 img_width,
                                 u16 img_height, f32 rx, f32 ry,
                                 DSPGraphicsScalingMode mode, u16 x, u16 y,
                                 u16 width, u16 height) {
  if (OS_IsRunOnTwl() && !isBusy) {
    return DSPi_ScalingCore(src, dst, img_width, img_height, rx, ry, mode, x, y,
                            width, height, NULL, FALSE);
  }
  return FALSE;
}

static inline BOOL DSP_ScalingFxEx(const void *src, void *dst, u16 img_width,
                                   u16 img_height, fx32 rx, fx32 ry,
                                   DSPGraphicsScalingMode mode, u16 x, u16 y,
                                   u16 width, u16 height) {
  if (OS_IsRunOnTwl() && !isBusy) {
    return DSPi_ScalingFxCore(src, dst, img_width, img_height, rx, ry, mode, x,
                              y, width, height, NULL, FALSE);
  }
  return FALSE;
}

static inline BOOL DSP_ScalingAsyncEx(const void *src, void *dst, u16 img_width,
                                      u16 img_height, f32 rx, f32 ry,
                                      DSPGraphicsScalingMode mode, u16 x, u16 y,
                                      u16 width, u16 height,
                                      DSP_GraphicsCallback callback) {
  if (OS_IsRunOnTwl() && !isBusy) {
    return DSPi_ScalingCore(src, dst, img_width, img_height, rx, ry, mode, x, y,
                            width, height, callback, TRUE);
  }
  return FALSE;
}

static inline BOOL DSP_ScalingFxAsyncEx(const void *src, void *dst,
                                        u16 img_width, u16 img_height, fx32 rx,
                                        fx32 ry, DSPGraphicsScalingMode mode,
                                        u16 x, u16 y, u16 width, u16 height,
                                        DSP_GraphicsCallback callback) {
  if (OS_IsRunOnTwl() && !isBusy) {
    return DSPi_ScalingFxCore(src, dst, img_width, img_height, rx, ry, mode, x,
                              y, width, height, callback, TRUE);
  }
  return FALSE;
}

static inline f32 DSP_CalcScalingFactorF32(const u16 src_size,
                                           const u16 dst_size) {

  return (dst_size / (f32)src_size + 0.0009f);
}

static inline fx32 DSP_CalcScalingFactorFx32(const u16 src_size,
                                             const u16 dst_size) {
  return FX_F32_TO_FX32(dst_size / (f32)src_size + 0.0009f);
}

#endif // SDK_TWL

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TWL_DSP_GRAPHICS_H_ */

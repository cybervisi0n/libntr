#include <twl.h>
#include <twl/camera.h>

#include "camera_intr.h"

#define SYNC_TYPE (0 << REG_CAM_MCNT_SYNC_SHIFT)  // 1 if low active
#define RCLK_TYPE (0 << REG_CAM_MCNT_IRCLK_SHIFT) // 1 if negative edge

#define RESET_ON (SYNC_TYPE | RCLK_TYPE)
#define RESET_OFF                                                              \
  (REG_CAM_MCNT_VIO_MASK | REG_CAM_MCNT_RST_MASK | SYNC_TYPE |                 \
   RCLK_TYPE) // RST is only for TS-X2

static inline void CAMERAi_Wait(u32 clocks) {
  OS_SpinWaitSysCycles(clocks << 1);
}

void CAMERA_ResetCore(void) {
  reg_SCFG_CLK |= REG_SCFG_CLK_CAMHCLK_MASK; // Reliably turn on the power

  reg_CAM_MCNT = RESET_ON; // Hardware reset
  CAMERAi_Wait(15);
  reg_SCFG_CLK |=
      REG_SCFG_CLK_CAMCKI_MASK; // Provide a clock to the camera module for
                                // reading internal ROM code
  CAMERAi_Wait(15);
  reg_CAM_MCNT = RESET_OFF; // Cancel the reset state
  CAMERAi_Wait(4100);       // Wait for internal ROM code to be read

  reg_SCFG_CLK &= ~REG_SCFG_CLK_CAMCKI_MASK; // Disable CAM_CKI
}

BOOL CAMERA_IsBusyCore(void) {
  return (reg_CAM_CNT & REG_CAM_CNT_E_MASK) >> REG_CAM_CNT_E_SHIFT;
}

void CAMERA_StartCaptureCore(void) {
  OSIntrMode old = OS_DisableInterrupts();
  reg_CAM_CNT |= REG_CAM_CNT_E_MASK;
  (void)OS_RestoreInterrupts(old);
}

void CAMERA_StopCaptureCore(void) {
  OSIntrMode old = OS_DisableInterrupts();
  reg_CAM_CNT &= ~REG_CAM_CNT_E_MASK;
  (void)OS_RestoreInterrupts(old);
}

void CAMERA_SetTrimmingParamsCenterCore(u16 destWidth, u16 destHeight,
                                        u16 srcWidth, u16 srcHeight) {
  if ((destWidth > srcWidth) ||
      (destHeight > srcHeight)) // Trimming range is outside original image
  {
    return;
  }

  destWidth -= 2;
  destHeight -= 1;
  reg_CAM_SOFS = REG_CAM_SOFS_FIELD((srcHeight - destHeight) >> 1,
                                    (srcWidth - destWidth) >> 1);
  reg_CAM_EOFS = REG_CAM_EOFS_FIELD((srcHeight + destHeight) >> 1,
                                    (srcWidth + destWidth) >> 1);
}

void CAMERA_SetTrimmingParamsCore(u16 x1, u16 y1, u16 x2, u16 y2) {
  if ((x1 > x2) || (y1 > y2)) // The ending position offset is smaller than the
                              // starting position offset
  {
    return;
  }

  reg_CAM_SOFS = REG_CAM_SOFS_FIELD(y1, x1);
  reg_CAM_EOFS = REG_CAM_EOFS_FIELD(y2 - 1, x2 - 2);
}

void CAMERA_GetTrimmingParamsCore(u16 *x1, u16 *y1, u16 *x2, u16 *y2) {
  *x1 =
      (u16)((reg_CAM_SOFS & REG_CAM_SOFS_HOFS_MASK) >> REG_CAM_SOFS_HOFS_SHIFT);
  *y1 =
      (u16)((reg_CAM_SOFS & REG_CAM_SOFS_VOFS_MASK) >> REG_CAM_SOFS_VOFS_SHIFT);
  *x2 = (u16)(((reg_CAM_EOFS & REG_CAM_EOFS_HOFS_MASK) >>
               REG_CAM_EOFS_HOFS_SHIFT) +
              2);
  *y2 = (u16)(((reg_CAM_EOFS & REG_CAM_EOFS_VOFS_MASK) >>
               REG_CAM_EOFS_VOFS_SHIFT) +
              1);
}

void CAMERA_SetTrimmingCore(BOOL enabled) {
  OSIntrMode old = OS_DisableInterrupts();
  u16 value = reg_CAM_CNT;
  reg_CAM_CNT = (u16)(enabled ? (value | REG_CAM_CNT_T_MASK)
                              : (value & ~REG_CAM_CNT_T_MASK));
  (void)OS_RestoreInterrupts(old);
}

BOOL CAMERA_IsTrimmingCore(void) {
  return ((reg_CAM_CNT & REG_CAM_CNT_T_MASK) >> REG_CAM_CNT_T_SHIFT);
}

void CAMERA_SetOutputFormatCore(CAMERAOutput output) {
  OSIntrMode old = OS_DisableInterrupts();
  u16 value = reg_CAM_CNT;
  switch (output) {
  case CAMERA_OUTPUT_YUV:
    reg_CAM_CNT = (u16)(value & ~REG_CAM_CNT_F_MASK);
    break;
  case CAMERA_OUTPUT_RGB:
    reg_CAM_CNT = (u16)(value | REG_CAM_CNT_F_MASK);
    break;
  }
  (void)OS_RestoreInterrupts(old);
}

CAMERAOutput CAMERA_GetOutputFormatCore(void) {
  return (reg_CAM_CNT & REG_CAM_CNT_F_MASK) ? CAMERA_OUTPUT_RGB
                                            : CAMERA_OUTPUT_YUV;
}

BOOL CAMERA_GetErrorStatusCore(void) {
  return (reg_CAM_CNT & REG_CAM_CNT_ERR_MASK) >> REG_CAM_CNT_ERR_SHIFT;
}

void CAMERA_ClearBufferCore(void) {
  OSIntrMode old = OS_DisableInterrupts();
  reg_CAM_CNT |= REG_CAM_CNT_CL_MASK;
  (void)OS_RestoreInterrupts(old);
}

void CAMERA_SetMasterInterruptCore(BOOL enabled) {
  OSIntrMode old = OS_DisableInterrupts();
  u16 value = reg_CAM_CNT;
  reg_CAM_CNT = (u16)(enabled ? (value | REG_CAM_CNT_IREQI_MASK)
                              : (value & ~REG_CAM_CNT_IREQI_MASK));
  (void)OS_RestoreInterrupts(old);
}

BOOL CAMERA_GetMasterInterruptCore(void) {
  return ((reg_CAM_CNT & REG_CAM_CNT_IREQI_MASK) >> REG_CAM_CNT_IREQI_SHIFT);
}

void CAMERA_SetVsyncInterruptCore(CAMERAIntrVsync type) {
  OSIntrMode old = OS_DisableInterrupts();
  reg_CAM_CNT = (u16)((reg_CAM_CNT & ~REG_CAM_CNT_IREQVS_MASK) | type);
  (void)OS_RestoreInterrupts(old);
}

CAMERAIntrVsync CAMERA_GetVsyncInterruptCore(void) {
  return (CAMERAIntrVsync)(reg_CAM_CNT & REG_CAM_CNT_IREQVS_MASK);
}

void CAMERA_SetBufferErrorInterruptCore(BOOL enabled) {
  OSIntrMode old = OS_DisableInterrupts();
  u16 value = reg_CAM_CNT;
  reg_CAM_CNT = (u16)(enabled ? (value | REG_CAM_CNT_IREQBE_MASK)
                              : (value & ~REG_CAM_CNT_IREQBE_MASK));
  (void)OS_RestoreInterrupts(old);
}

BOOL CAMERA_GetBufferErrorInterruptCore(void) {
  return ((reg_CAM_CNT & REG_CAM_CNT_IREQBE_MASK) >> REG_CAM_CNT_IREQBE_SHIFT);
}

void CAMERA_SetTransferLinesCore(int lines) {
  if (lines >= 1 && lines <= 16) {
    OSIntrMode old = OS_DisableInterrupts();
    u16 bits = (u16)((lines - 1) << REG_CAM_CNT_TL_SHIFT);
    reg_CAM_CNT = (u16)((reg_CAM_CNT & ~REG_CAM_CNT_TL_MASK) | bits);
    (void)OS_RestoreInterrupts(old);
  }
}

int CAMERA_GetTransferLinesCore(void) {
  return (((reg_CAM_CNT & REG_CAM_CNT_TL_MASK) >> REG_CAM_CNT_TL_SHIFT) + 1);
}

int CAMERA_GetMaxLinesRoundCore(u16 width, u16 height) {
  int lines;

  for (lines = CAMERA_GET_MAX_LINES(width); lines > 1; lines--) {
    if (height % lines == 0) {
      return lines;
    }
  }
  return 1;
}

u32 CAMERA_GetBytesAtOnceCore(u16 width) {
  return (u32)(CAMERA_GET_LINE_BYTES(width) * CAMERA_GetTransferLinesCore());
}

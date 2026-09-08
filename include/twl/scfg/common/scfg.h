#ifndef TWL_COMMON_SCFG_H_
#define TWL_COMMON_SCFG_H_

#include <twl/misc.h>
#include <twl/types.h>

#include <nitro/os/common/system.h>
#include <twl/hw/common/mmap_wramEnv.h>
#include <twl/hw/common/mmap_shared.h>
#if defined(SDK_ARM9) || defined(SDK_PORT)
#include <twl/hw/ARM9/ioreg.h>
#else // SDK_ARM7
#include <twl/hw/ARM7/ioreg.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  SCFG_SYSTEM_ROM_FOR_TWL = (0 << REG_SCFG_A9ROM_RSEL_SHIFT),
  SCFG_SYSTEM_ROM_FOR_NITRO = (1 << REG_SCFG_A9ROM_RSEL_SHIFT)
} SCFGSystemRomType;

#define SCFG_SYSTEM_ROM_MASK (REG_SCFG_A9ROM_RSEL_MASK)

typedef enum {
  SCFG_SECURE_ROM_ACCESSIBLE = (0 << REG_SCFG_A9ROM_SEC_SHIFT),
  SCFG_SECURE_ROM_INACCESSIBLE = (1 << REG_SCFG_A9ROM_SEC_SHIFT)
} SCFGSecureRomAccessible;

#define SCFG_SECURE_ROM_ACCESSIBLE_MASK (REG_SCFG_A9ROM_SEC_MASK)

#define SCFG_CLOCK_SUPPLY_WRAM (REG_SCFG_CLK_WRAMHCLK_MASK)
#if defined(SDK_ARM9) || defined(SDK_PORT)
#define SCFG_CAMERA_CKI_FLAG (REG_SCFG_CLK_CAMCKI_MASK)
#define SCFG_CLOCK_SUPPLY_CAMERA (REG_SCFG_CLK_CAMHCLK_MASK)
#define SCFG_CLOCK_SUPPLY_DSP (REG_SCFG_CLK_DSPHCLK_MASK)

typedef enum {
  SCFG_CPU_SPEED_1X = (0 << REG_SCFG_CLK_CPUSPD_SHIFT),
  SCFG_CPU_SPEED_2X = (1 << REG_SCFG_CLK_CPUSPD_SHIFT)
} SCFGCpuSpeed;

#define SCFG_CPU_SPEED_MASK (REG_SCFG_CLK_CPUSPD_MASK)
#endif

#if defined(SDK_ARM9) || defined(SDK_PORT)
#define SCFG_DSP_RESET_SET (0 << REG_SCFG_RST_DSPRSTB_SHIFT)
#define SCFG_DSP_RESET_RELEASE (1 << REG_SCFG_RST_DSPRSTB_SHIFT)
#define SCFG_DSP_RESET_MASK (REG_SCFG_RST_DSPRSTB_MASK)
#endif

#define SCFG_CONFIG_ENABLE (1 << REG_SCFG_EXT_CFG_SHIFT)
#define SCFG_CONFIG_DISABLE (0 << REG_SCFG_EXT_CFG_SHIFT)
#define SCFG_CONFIG_ENABLE_MASK (REG_SCFG_EXT_CFG_MASK)

#define SCFG_WRAM_ACCESS (1 << REG_SCFG_EXT_WRAM_SHIFT) // read only in ARM9

#if defined(SDK_ARM9) || defined(SDK_PORT)
#define SCFG_DSP_ACCESS (1 << REG_SCFG_EXT_DSP_SHIFT)
#define SCFG_CAMERA_ACCESS (1 << REG_SCFG_EXT_CAM_SHIFT)
#define SCFG_NDMA_ACCESS (1 << REG_SCFG_EXT_DMAC_SHIFT)
#endif

#define SCFG_EXPANDED_VRAM (1 << REG_SCFG_EXT_VRAM_SHIFT)
#define SCFG_EXPANDED_LCDC (1 << REG_SCFG_EXT_LCDC_SHIFT)
#define SCFG_EXPANDED_INTC (1 << REG_SCFG_EXT_INTC_SHIFT)

typedef enum {
  SCFG_PSRAM_BOUNDARY_4MB = (0 << REG_SCFG_EXT_PSRAM_SHIFT),
  SCFG_PSRAM_BOUNDARY_4MB_2 = (1 << REG_SCFG_EXT_PSRAM_SHIFT), // treated as 4MB
  SCFG_PSRAM_BOUNDARY_16MB = (2 << REG_SCFG_EXT_PSRAM_SHIFT),
  SCFG_PSRAM_BOUNDARY_32MB = (3 << REG_SCFG_EXT_PSRAM_SHIFT)
} SCFGPsramBoundary;

#define SCFG_PSRAM_BOUNDARY_MASK (REG_SCFG_EXT_PSRAM_MASK)

#define SCFG_FIXED_CARD (1 << REG_SCFG_EXT_MC_SHIFT)
#if defined(SDK_ARM9) || defined(SDK_PORT)
#define SCFG_FIXED_DIVIDER (1 << REG_SCFG_EXT_DIV_SHIFT)
#define SCFG_FIXED_2DENGINE (1 << REG_SCFG_EXT_G2DE_SHIFT)
#define SCFG_FIXED_RENDERER (1 << REG_SCFG_EXT_REN_SHIFT)
#define SCFG_FIXED_GEOMETRY (1 << REG_SCFG_EXT_GEO_SHIFT)
#endif
#define SCFG_FIXED_DMAC (1 << REG_SCFG_EXT_DMA_SHIFT)

typedef enum {
  SCFG_CARD_DETECT_MODE_0 = 0,
  SCFG_CARD_DETECT_MODE_1 = 1,
  SCFG_CARD_DETECT_MODE_2 = 2,
  SCFG_CARD_DETECT_MODE_3 = 3
} SCFGCardDetectMode;

#define SCFG_CARD_DETECT_MODE_MASK 3

typedef enum {
  SCFG_CARD_DETECT_CHATTERING_LOW = 0,
  SCFG_CARD_DETECT_CHATTERING_HIGH = 1
} SCFGCardDetectChattering;

#define SCFG_CARD_DETECT_CHATTERING_MASK 1

#define SCFG_OP_PRODUCT 0
#define SCFG_OP_A9_A7_DEV 1
#define SCFG_OP_A9_DEV 2
#define SCFG_OP_PROM 3

#define SCFGi_CHANGEBIT_8(addr, flag, eval, orval)                             \
  do {                                                                         \
    *(u8 *)(addr) = (u8)(((*(u8 *)(addr)) & (0xff ^ (flag))) |                 \
                         ((eval) ? (flag) : 0) | orval);                       \
  } while (0)

#define SCFGi_CHANGEBIT_16(addr, flag, eval, orval)                            \
  do {                                                                         \
    *(u16 *)(addr) = (u16)(((*(u16 *)(addr)) & (0xffff ^ (flag))) |            \
                           ((eval) ? (flag) : 0) | orval);                     \
  } while (0)

#define SCFGi_CHANGEBIT_32(addr, flag, eval, orval)                            \
  do {                                                                         \
    *(u32 *)(addr) = (u32)(((*(u32 *)(addr)) & (0xffffffff ^ (flag))) |        \
                           ((eval) ? (flag) : 0) | orval);                     \
  } while (0)

#define SCFGi_CLEARBIT_8(addr, flag) SCFGi_CHANGEBIT_8((addr), (flag), 0, 0)
#define SCFGi_CLEARBIT_16(addr, flag) SCFGi_CHANGEBIT_16((addr), (flag), 0, 0)
#define SCFGi_CLEARBIT_32(addr, flag) SCFGi_CHANGEBIT_32((addr), (flag), 0, 0)

#define SCFG_PXI_COMMAND_MASK 0x03f00000
#define SCFG_PXI_COMMAND_SHIFT 20
#define SCFG_PXI_ORDINAL_MASK 0x000f0000
#define SCFG_PXI_ORDINAL_SHIFT 16
#define SCFG_PXI_DATA_MASK 0x0000ffff
#define SCFG_PXI_DATA_SHIFT 0

typedef enum {
  SCFGi_PXI_COMMAND_READ = 1,
  SCFGi_PXI_COMMAND_READ_OP = 2
} SCFGPxiCommand;

typedef void (*SCFGCallback)(u64, void *);

#if defined(SDK_ARM9) || defined(SDK_PORT)

static inline SCFGSystemRomType SCFG_GetSystemRomType(void) {
  return (*(u32 *)(HW_SYS_CONF_BUF + HWi_WSYS08_OFFSET) &
          HWi_WSYS08_ROM_ARM9RSEL_MASK)
             ? SCFG_SYSTEM_ROM_FOR_NITRO
             : SCFG_SYSTEM_ROM_FOR_TWL;
}

#define SCFG_GetSystemRomType_ARM9() SCFG_GetSystemRomType()

static inline BOOL SCFG_IsSecureRomAccessible(void) {
  return (*(u32 *)(HW_SYS_CONF_BUF + HWi_WSYS08_OFFSET) &
          HWi_WSYS08_ROM_ARM9SEC_MASK)
             ? FALSE
             : TRUE;
}

#define SCFG_IsSecureRomAccessible_ARM9() SCFG_IsSecureRomAccessible()

static inline void SCFG_SetCameraCKIClock(BOOL sw) {
  SCFGi_CHANGEBIT_16(&reg_SCFG_CLK, SCFG_CAMERA_CKI_FLAG, sw, 0);
}

static inline BOOL SCFG_IsCameraCKIClockEnable(void) {
  return (BOOL)((reg_SCFG_CLK & SCFG_CAMERA_CKI_FLAG) ? TRUE : FALSE);
}

static inline BOOL SCFG_IsClockSuppliedToWram(void) {
  return (BOOL)((reg_SCFG_CLK & SCFG_CLOCK_SUPPLY_WRAM) ? TRUE : FALSE);
}

static inline void SCFG_SupplyClockToCamera(BOOL sw) {
  SCFGi_CHANGEBIT_16(&reg_SCFG_CLK, SCFG_CLOCK_SUPPLY_CAMERA, sw, 0);
}

static inline BOOL SCFG_IsClockSuppliedToCamera(void) {
  return (BOOL)((reg_SCFG_CLK & SCFG_CLOCK_SUPPLY_CAMERA) ? TRUE : FALSE);
}

static inline void SCFG_SupplyClockToDSP(BOOL sw) {
  SCFGi_CHANGEBIT_16(&reg_SCFG_CLK, SCFG_CLOCK_SUPPLY_DSP, sw, 0);
}

static inline BOOL SCFG_IsClockSuppliedToDSP(void) {
  return (BOOL)((reg_SCFG_CLK & SCFG_CLOCK_SUPPLY_DSP) ? TRUE : FALSE);
}

void SCFG_SetCpuSpeed(SCFGCpuSpeed cpuSpeed);

static inline SCFGCpuSpeed SCFG_GetCpuSpeed(void) {
  return (SCFGCpuSpeed)(reg_SCFG_CLK & SCFG_CPU_SPEED_MASK);
}

static inline void SCFGi_SendResetToDSP(BOOL sw) {
  SCFGi_CHANGEBIT_16(&reg_SCFG_RST, SCFG_DSP_RESET_MASK, sw, 0);
}

static inline void SCFG_ResetDSP(void) { SCFGi_SendResetToDSP(FALSE); }

static inline void SCFG_ReleaseResetDSP(void) { SCFGi_SendResetToDSP(TRUE); }

static inline BOOL SCFG_IsDSPReset(void) {
  return (BOOL)((reg_SCFG_RST & SCFG_DSP_RESET_MASK) ? FALSE : TRUE);
}

static inline void SCFG_SetConfigBlockInaccessible(void) {
  SCFGi_CHANGEBIT_32(&reg_SCFG_EXT, SCFG_CONFIG_ENABLE_MASK, 0,
                     SCFG_CONFIG_DISABLE);
}

static inline BOOL SCFG_IsConfigBlockAccessible(void) {
  return (BOOL)((reg_SCFG_EXT & SCFG_CONFIG_ENABLE_MASK) ? TRUE : FALSE);
}

static inline BOOL SCFG_IsWramAccessible(void) {
  return (BOOL)((reg_SCFG_EXT & SCFG_WRAM_ACCESS) ? TRUE : FALSE);
}

#define SCFG_IsWRAMAccessible SCFG_IsWramAccessible

static inline void SCFG_SetDSPAccessible(BOOL sw) {
  SCFGi_CHANGEBIT_32(&reg_SCFG_EXT, SCFG_DSP_ACCESS, sw, 0);
}

static inline BOOL SCFG_IsDSPAccessible(void) {
  return (BOOL)((reg_SCFG_EXT & SCFG_DSP_ACCESS) ? TRUE : FALSE);
}

static inline void SCFG_SetCameraAccessible(BOOL sw) {
  SCFGi_CHANGEBIT_32(&reg_SCFG_EXT, SCFG_CAMERA_ACCESS, sw, 0);
}

static inline BOOL SCFG_IsCameraAccessible(void) {
  return (BOOL)((reg_SCFG_EXT & SCFG_CAMERA_ACCESS) ? TRUE : FALSE);
}

static inline void SCFG_SetNDmaAccessible(BOOL sw) {
  SCFGi_CHANGEBIT_32(&reg_SCFG_EXT, SCFG_NDMA_ACCESS, sw, 0);
}

static inline BOOL SCFG_IsNDmaAccessible(void) {
  return (BOOL)((reg_SCFG_EXT & SCFG_NDMA_ACCESS) ? TRUE : FALSE);
}

static inline void SCFG_SetIntcExpanded(BOOL sw) {
  SCFGi_CHANGEBIT_32(&reg_SCFG_EXT, SCFG_EXPANDED_INTC, sw, 0);
}

static inline BOOL SCFG_IsIntcExpanded(void) {
  return (BOOL)((reg_SCFG_EXT & SCFG_EXPANDED_INTC) ? TRUE : FALSE);
}

static inline void SCFG_SetLCDCExpanded(BOOL sw) {
  SCFGi_CHANGEBIT_32(&reg_SCFG_EXT, SCFG_EXPANDED_LCDC, sw, 0);

  SCFGi_CHANGEBIT_32(HW_SYS_CONF_BUF + HWi_WSYS04_OFFSET,
                     HWi_WSYS04_EXT_LCDC_MASK, sw, 0);
}

static inline BOOL SCFG_IsLCDCExpanded(void) {
  return (BOOL)((reg_SCFG_EXT & SCFG_EXPANDED_LCDC) ? TRUE : FALSE);
}

static inline void SCFG_SetVramExpanded(BOOL sw) {
  SCFGi_CHANGEBIT_32(&reg_SCFG_EXT, SCFG_EXPANDED_VRAM, sw, 0);

  SCFGi_CHANGEBIT_32(HW_SYS_CONF_BUF + HWi_WSYS04_OFFSET,
                     HWi_WSYS04_EXT_VRAM_MASK, sw, 0);
}

#define SCFG_SetVRAMExpanded SCFG_SetVramExpanded

static inline BOOL SCFG_IsVramExpanded(void) {
  return (BOOL)((reg_SCFG_EXT & SCFG_EXPANDED_VRAM) ? TRUE : FALSE);
}

#define SCFG_IsVRAMExpanded SCFG_IsVramExpanded

static inline void SCFG_SetPsramBoundary(SCFGPsramBoundary boundary) {
  SCFGi_CHANGEBIT_32(&reg_SCFG_EXT, SCFG_PSRAM_BOUNDARY_MASK, 0, boundary);

  SCFGi_CHANGEBIT_32(HW_SYS_CONF_BUF + HWi_WSYS04_OFFSET,
                     HWi_WSYS04_EXT_PSRAM_MASK, 0, boundary);
}

static inline SCFGPsramBoundary SCFG_GetPsramBoundary(void) {
  return (SCFGPsramBoundary)(reg_SCFG_EXT & SCFG_PSRAM_BOUNDARY_MASK);
}

static inline void SCFG_SetCardFixed(BOOL sw) {
  SCFGi_CHANGEBIT_32(&reg_SCFG_EXT, SCFG_FIXED_CARD, sw, 0);

  SCFGi_CHANGEBIT_32(HW_SYS_CONF_BUF + HWi_WSYS04_OFFSET,
                     HWi_WSYS04_EXT_MC_MASK, sw, 0);
}

static inline BOOL SCFG_IsCardFixed(void) {
  return (BOOL)((reg_SCFG_EXT & SCFG_FIXED_CARD) ? TRUE : FALSE);
}

static inline void SCFG_SetDividerFixed(BOOL sw) {
  SCFGi_CHANGEBIT_32(&reg_SCFG_EXT, SCFG_FIXED_DIVIDER, sw, 0);
}

static inline BOOL SCFG_IsDividerFixed(void) {
  return (BOOL)((reg_SCFG_EXT & SCFG_FIXED_DIVIDER) ? TRUE : FALSE);
}

static inline void SCFG_Set2DEngineFixed(BOOL sw) {
  SCFGi_CHANGEBIT_32(&reg_SCFG_EXT, SCFG_FIXED_2DENGINE, sw, 0);
}

static inline BOOL SCFG_Is2DEngineFixed(void) {
  return (BOOL)((reg_SCFG_EXT & SCFG_FIXED_2DENGINE) ? TRUE : FALSE);
}

static inline void SCFG_SetRendererFixed(BOOL sw) {
  SCFGi_CHANGEBIT_32(&reg_SCFG_EXT, SCFG_FIXED_RENDERER, sw, 0);
}

static inline BOOL SCFG_IsRendererFixed(void) {
  return (BOOL)((reg_SCFG_EXT & SCFG_FIXED_RENDERER) ? TRUE : FALSE);
}

static inline void SCFG_SetGeometryFixed(BOOL sw) {
  SCFGi_CHANGEBIT_32(&reg_SCFG_EXT, SCFG_FIXED_GEOMETRY, sw, 0);
}

static inline BOOL SCFG_IsGeometryFixed(void) {
  return (BOOL)((reg_SCFG_EXT & SCFG_FIXED_GEOMETRY) ? TRUE : FALSE);
}

static inline void SCFG_SetDmacFixed(BOOL sw) {
  SCFGi_CHANGEBIT_32(&reg_SCFG_EXT, SCFG_FIXED_DMAC, sw, 0);
}

static inline BOOL SCFG_IsDmacFixed(void) {
  return (BOOL)((reg_SCFG_EXT & SCFG_FIXED_DMAC) ? TRUE : FALSE);
}

static inline SCFGCardDetectMode SCFG_GetCardDetectMode(int slot) {
#pragma unused(slot)
  int shift = 2;
  return (
      SCFGCardDetectMode)((reg_MI_MC & (SCFG_CARD_DETECT_MODE_MASK << shift)) >>
                          shift);
}

static inline SCFGCardDetectChattering
SCFG_GetCardDetectForChattering(int slot) {
#pragma unused(slot)
  int shift = 0;
  return (
      SCFGCardDetectChattering)((reg_MI_MC &
                                 (SCFG_CARD_DETECT_CHATTERING_MASK << shift)) >>
                                shift);
}
#endif

void SCFG_Init(void);

BOOL SCFG_ReadFuseDataAsync(SCFGCallback callback, void *arg);

u64 SCFG_ReadFuseData(void);

#if defined(SDK_ARM9) || defined(SDK_PORT)

BOOL SCFG_ReadBondingOptionAsync(SCFGCallback callback, void *arg);

u16 SCFG_ReadBondingOption(void);
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

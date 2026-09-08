#ifndef NITRO_HW_ARM9_IOREG_CFG_H_
#define NITRO_HW_ARM9_IOREG_CFG_H_

#ifndef SDK_ASM
#include <twl/types.h>
#include <nitro/hw/ARM9/mmap_global.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define REG_A9ROM_OFFSET 0x4000
#define REG_A9ROM_ADDR (HW_REG_BASE + REG_A9ROM_OFFSET)
#define reg_CFG_A9ROM (*(REGType8v *)REG_A9ROM_ADDR)

#define REG_CLK_OFFSET 0x4004
#define REG_CLK_ADDR (HW_REG_BASE + REG_CLK_OFFSET)
#define reg_CFG_CLK (*(REGType16v *)REG_CLK_ADDR)

#define REG_DSP_RST_OFFSET 0x4006
#define REG_DSP_RST_ADDR (HW_REG_BASE + REG_DSP_RST_OFFSET)
#define reg_CFG_DSP_RST (*(REGType8v *)REG_DSP_RST_ADDR)

#define REG_DS_MDFY_OFFSET 0x4008
#define REG_DS_MDFY_ADDR (HW_REG_BASE + REG_DS_MDFY_OFFSET)
#define reg_CFG_DS_MDFY (*(REGType8v *)REG_DS_MDFY_ADDR)

#define REG_DS_EX_OFFSET 0x4009
#define REG_DS_EX_ADDR (HW_REG_BASE + REG_DS_EX_OFFSET)
#define reg_CFG_DS_EX (*(REGType8v *)REG_DS_EX_ADDR)

#define REG_TWL_EX_OFFSET 0x400a
#define REG_TWL_EX_ADDR (HW_REG_BASE + REG_TWL_EX_OFFSET)
#define reg_CFG_TWL_EX (*(REGType16v *)REG_TWL_EX_ADDR)

#define REG_CFG_A9ROM_NTR_SHIFT 1
#define REG_CFG_A9ROM_NTR_SIZE 1
#define REG_CFG_A9ROM_NTR_MASK 0x02

#define REG_CFG_A9ROM_SEC_SHIFT 0
#define REG_CFG_A9ROM_SEC_SIZE 1
#define REG_CFG_A9ROM_SEC_MASK 0x01

#ifndef SDK_ASM
#define REG_CFG_A9ROM_FIELD(ntr, sec)                                          \
  (u8)(((u32)(ntr) << REG_CFG_A9ROM_NTR_SHIFT) |                               \
       ((u32)(sec) << REG_CFG_A9ROM_SEC_SHIFT))
#endif

#define REG_CFG_CLK_CAM_CKI_SHIFT 8
#define REG_CFG_CLK_CAM_CKI_SIZE 1
#define REG_CFG_CLK_CAM_CKI_MASK 0x0100

#define REG_CFG_CLK_WRAM_SHIFT 7
#define REG_CFG_CLK_WRAM_SIZE 1
#define REG_CFG_CLK_WRAM_MASK 0x0080

#define REG_CFG_CLK_CAM_SHIFT 2
#define REG_CFG_CLK_CAM_SIZE 1
#define REG_CFG_CLK_CAM_MASK 0x0004

#define REG_CFG_CLK_DSP_SHIFT 1
#define REG_CFG_CLK_DSP_SIZE 0
#define REG_CFG_CLK_DSP_MASK 0x0002

#define REG_CFG_CLK_ARM2X_SHIFT 0
#define REG_CFG_CLK_ARM2X_SIZE 1
#define REG_CFG_CLK_ARM2X_MASK 0x0001

#ifndef SDK_ASM
#define REG_CFG_CLK_FIELD(cam_cki, wram, cam, dsp, arm2x)                      \
  (u16)(((u32)(cam_cki) << REG_CFG_CLK_CAM_CKI_SHIFT) |                        \
        ((u32)(wram) << REG_CFG_CLK_WRAM_SHIFT) |                              \
        ((u32)(cam) << REG_CFG_CLK_CAM_SHIFT) |                                \
        ((u32)(dsp) << REG_CFG_CLK_DSP_SHIFT) |                                \
        ((u32)(arm2x) << REG_CFG_CLK_ARM2X_SHIFT))
#endif

#define REG_CFG_DSP_RST_OFF_SHIFT 0
#define REG_CFG_DSP_RST_OFF_SIZE 1
#define REG_CFG_DSP_RST_OFF_MASK 0x01

#ifndef SDK_ASM
#define REG_CFG_DSP_RST_FIELD(off)                                             \
  (u8)(((u32)(off) << REG_CFG_DSP_RST_OFF_SHIFT))
#endif

#define REG_CFG_DS_MDFY_MC_SHIFT 7
#define REG_CFG_DS_MDFY_MC_SIZE 1
#define REG_CFG_DS_MDFY_MC_MASK 0x80

#define REG_CFG_DS_MDFY_DIV_SHIFT 4
#define REG_CFG_DS_MDFY_DIV_SIZE 1
#define REG_CFG_DS_MDFY_DIV_MASK 0x10

#define REG_CFG_DS_MDFY_G2_SHIFT 3
#define REG_CFG_DS_MDFY_G2_SIZE 1
#define REG_CFG_DS_MDFY_G2_MASK 0x08

#define REG_CFG_DS_MDFY_REN_SHIFT 2
#define REG_CFG_DS_MDFY_REN_SIZE 1
#define REG_CFG_DS_MDFY_REN_MASK 0x04

#define REG_CFG_DS_MDFY_GEO_SHIFT 1
#define REG_CFG_DS_MDFY_GEO_SIZE 1
#define REG_CFG_DS_MDFY_GEO_MASK 0x02

#define REG_CFG_DS_MDFY_DMA_SHIFT 0
#define REG_CFG_DS_MDFY_DMA_SIZE 1
#define REG_CFG_DS_MDFY_DMA_MASK 0x01

#ifndef SDK_ASM
#define REG_CFG_DS_MDFY_FIELD(mc, div, g2, ren, geo, dma)                      \
  (u8)(((u32)(mc) << REG_CFG_DS_MDFY_MC_SHIFT) |                               \
       ((u32)(div) << REG_CFG_DS_MDFY_DIV_SHIFT) |                             \
       ((u32)(g2) << REG_CFG_DS_MDFY_G2_SHIFT) |                               \
       ((u32)(ren) << REG_CFG_DS_MDFY_REN_SHIFT) |                             \
       ((u32)(geo) << REG_CFG_DS_MDFY_GEO_SHIFT) |                             \
       ((u32)(dma) << REG_CFG_DS_MDFY_DMA_SHIFT))
#endif

#define REG_CFG_DS_EX_MAIM_MEM_SHIFT 6
#define REG_CFG_DS_EX_MAIM_MEM_SIZE 2
#define REG_CFG_DS_EX_MAIM_MEM_MASK 0xc0

#define REG_CFG_DS_EX_VRAM_SHIFT 5
#define REG_CFG_DS_EX_VRAM_SIZE 1
#define REG_CFG_DS_EX_VRAM_MASK 0x20

#define REG_CFG_DS_EX_LCDC_SHIFT 4
#define REG_CFG_DS_EX_LCDC_SIZE 1
#define REG_CFG_DS_EX_LCDC_MASK 0x10

#define REG_CFG_DS_EX_INTC_SHIFT 0
#define REG_CFG_DS_EX_INTC_SIZE 1
#define REG_CFG_DS_EX_INTC_MASK 0x01

#ifndef SDK_ASM
#define REG_CFG_DS_EX_FIELD(maim_mem, vram, lcdc, intc)                        \
  (u8)(((u32)(maim_mem) << REG_CFG_DS_EX_MAIM_MEM_SHIFT) |                     \
       ((u32)(vram) << REG_CFG_DS_EX_VRAM_SHIFT) |                             \
       ((u32)(lcdc) << REG_CFG_DS_EX_LCDC_SHIFT) |                             \
       ((u32)(intc) << REG_CFG_DS_EX_INTC_SHIFT))
#endif

#define REG_CFG_TWL_EX_CFG_E_SHIFT 15
#define REG_CFG_TWL_EX_CFG_E_SIZE 1
#define REG_CFG_TWL_EX_CFG_E_MASK 0x8000

#define REG_CFG_TWL_EX_WRAM_SHIFT 9
#define REG_CFG_TWL_EX_WRAM_SIZE 0
#define REG_CFG_TWL_EX_WRAM_MASK 0x0200

#define REG_CFG_TWL_EX_MC_B_SHIFT 8
#define REG_CFG_TWL_EX_MC_B_SIZE 1
#define REG_CFG_TWL_EX_MC_B_MASK 0x0100

#define REG_CFG_TWL_EX_DSP_SHIFT 2
#define REG_CFG_TWL_EX_DSP_SIZE 1
#define REG_CFG_TWL_EX_DSP_MASK 0x0004

#define REG_CFG_TWL_EX_CAM_SHIFT 1
#define REG_CFG_TWL_EX_CAM_SIZE 1
#define REG_CFG_TWL_EX_CAM_MASK 0x0002

#define REG_CFG_TWL_EX_DMA4_SHIFT 0
#define REG_CFG_TWL_EX_DMA4_SIZE 1
#define REG_CFG_TWL_EX_DMA4_MASK 0x0001

#ifndef SDK_ASM
#define REG_CFG_TWL_EX_FIELD(cfg_e, wram, mc_b, dsp, cam, dma4)                \
  (u16)(((u32)(cfg_e) << REG_CFG_TWL_EX_CFG_E_SHIFT) |                         \
        ((u32)(wram) << REG_CFG_TWL_EX_WRAM_SHIFT) |                           \
        ((u32)(mc_b) << REG_CFG_TWL_EX_MC_B_SHIFT) |                           \
        ((u32)(dsp) << REG_CFG_TWL_EX_DSP_SHIFT) |                             \
        ((u32)(cam) << REG_CFG_TWL_EX_CAM_SHIFT) |                             \
        ((u32)(dma4) << REG_CFG_TWL_EX_DMA4_SHIFT))
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

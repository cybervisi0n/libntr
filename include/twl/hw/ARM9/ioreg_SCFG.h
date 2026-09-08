#ifndef TWL_HW_ARM9_IOREG_SCFG_H_
#define TWL_HW_ARM9_IOREG_SCFG_H_

#ifndef SDK_ASM
#include <nitro/types.h>
#include <twl/hw/ARM9/mmap_global.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define REG_A9ROM_OFFSET 0x4000
#define REG_A9ROM_ADDR (HW_REG_BASE + REG_A9ROM_OFFSET)
#define reg_SCFG_A9ROM (*(const REGType8v *)REG_A9ROM_ADDR)

#define REG_CLK_OFFSET 0x4004
#define REG_CLK_ADDR (HW_REG_BASE + REG_CLK_OFFSET)
#define reg_SCFG_CLK (*(REGType16v *)REG_CLK_ADDR)

#define REG_RST_OFFSET 0x4006
#define REG_RST_ADDR (HW_REG_BASE + REG_RST_OFFSET)
#define reg_SCFG_RST (*(REGType16v *)REG_RST_ADDR)

#define REG_EXT_OFFSET 0x4008
#define REG_EXT_ADDR (HW_REG_BASE + REG_EXT_OFFSET)
#define reg_SCFG_EXT (*(REGType32v *)REG_EXT_ADDR)

#define REG_SCFG_A9ROM_RSEL_SHIFT 1
#define REG_SCFG_A9ROM_RSEL_SIZE 1
#define REG_SCFG_A9ROM_RSEL_MASK 0x02

#define REG_SCFG_A9ROM_SEC_SHIFT 0
#define REG_SCFG_A9ROM_SEC_SIZE 1
#define REG_SCFG_A9ROM_SEC_MASK 0x01

#ifndef SDK_ASM
#define REG_SCFG_A9ROM_FIELD(rsel, sec)                                        \
  (u8)(((u32)(rsel) << REG_SCFG_A9ROM_RSEL_SHIFT) |                            \
       ((u32)(sec) << REG_SCFG_A9ROM_SEC_SHIFT))
#endif

#define REG_SCFG_CLK_CAMCKI_SHIFT 8
#define REG_SCFG_CLK_CAMCKI_SIZE 1
#define REG_SCFG_CLK_CAMCKI_MASK 0x0100

#define REG_SCFG_CLK_WRAMHCLK_SHIFT 7
#define REG_SCFG_CLK_WRAMHCLK_SIZE 1
#define REG_SCFG_CLK_WRAMHCLK_MASK 0x0080

#define REG_SCFG_CLK_CAMHCLK_SHIFT 2
#define REG_SCFG_CLK_CAMHCLK_SIZE 1
#define REG_SCFG_CLK_CAMHCLK_MASK 0x0004

#define REG_SCFG_CLK_DSPHCLK_SHIFT 1
#define REG_SCFG_CLK_DSPHCLK_SIZE 1
#define REG_SCFG_CLK_DSPHCLK_MASK 0x0002

#define REG_SCFG_CLK_CPUSPD_SHIFT 0
#define REG_SCFG_CLK_CPUSPD_SIZE 1
#define REG_SCFG_CLK_CPUSPD_MASK 0x0001

#ifndef SDK_ASM
#define REG_SCFG_CLK_FIELD(camcki, wramhclk, camhclk, dsphclk, cpuspd)         \
  (u16)(((u32)(camcki) << REG_SCFG_CLK_CAMCKI_SHIFT) |                         \
        ((u32)(wramhclk) << REG_SCFG_CLK_WRAMHCLK_SHIFT) |                     \
        ((u32)(camhclk) << REG_SCFG_CLK_CAMHCLK_SHIFT) |                       \
        ((u32)(dsphclk) << REG_SCFG_CLK_DSPHCLK_SHIFT) |                       \
        ((u32)(cpuspd) << REG_SCFG_CLK_CPUSPD_SHIFT))
#endif

#define REG_SCFG_RST_DSPRSTB_SHIFT 0
#define REG_SCFG_RST_DSPRSTB_SIZE 1
#define REG_SCFG_RST_DSPRSTB_MASK 0x0001

#ifndef SDK_ASM
#define REG_SCFG_RST_FIELD(dsprstb)                                            \
  (u16)(((u32)(dsprstb) << REG_SCFG_RST_DSPRSTB_SHIFT))
#endif

#define REG_SCFG_EXT_CFG_SHIFT 31
#define REG_SCFG_EXT_CFG_SIZE 1
#define REG_SCFG_EXT_CFG_MASK 0x80000000

#define REG_SCFG_EXT_WRAM_SHIFT 25
#define REG_SCFG_EXT_WRAM_SIZE 1
#define REG_SCFG_EXT_WRAM_MASK 0x02000000

#define REG_SCFG_EXT_MC_B_SHIFT 24
#define REG_SCFG_EXT_MC_B_SIZE 1
#define REG_SCFG_EXT_MC_B_MASK 0x01000000

#define REG_SCFG_EXT_DSP_SHIFT 18
#define REG_SCFG_EXT_DSP_SIZE 1
#define REG_SCFG_EXT_DSP_MASK 0x00040000

#define REG_SCFG_EXT_CAM_SHIFT 17
#define REG_SCFG_EXT_CAM_SIZE 1
#define REG_SCFG_EXT_CAM_MASK 0x00020000

#define REG_SCFG_EXT_DMAC_SHIFT 16
#define REG_SCFG_EXT_DMAC_SIZE 1
#define REG_SCFG_EXT_DMAC_MASK 0x00010000

#define REG_SCFG_EXT_PSRAM_SHIFT 14
#define REG_SCFG_EXT_PSRAM_SIZE 2
#define REG_SCFG_EXT_PSRAM_MASK 0x0000c000

#define REG_SCFG_EXT_VRAM_SHIFT 13
#define REG_SCFG_EXT_VRAM_SIZE 1
#define REG_SCFG_EXT_VRAM_MASK 0x00002000

#define REG_SCFG_EXT_LCDC_SHIFT 12
#define REG_SCFG_EXT_LCDC_SIZE 1
#define REG_SCFG_EXT_LCDC_MASK 0x00001000

#define REG_SCFG_EXT_INTC_SHIFT 8
#define REG_SCFG_EXT_INTC_SIZE 1
#define REG_SCFG_EXT_INTC_MASK 0x00000100

#define REG_SCFG_EXT_MC_SHIFT 7
#define REG_SCFG_EXT_MC_SIZE 1
#define REG_SCFG_EXT_MC_MASK 0x00000080

#define REG_SCFG_EXT_DIV_SHIFT 4
#define REG_SCFG_EXT_DIV_SIZE 1
#define REG_SCFG_EXT_DIV_MASK 0x00000010

#define REG_SCFG_EXT_G2DE_SHIFT 3
#define REG_SCFG_EXT_G2DE_SIZE 1
#define REG_SCFG_EXT_G2DE_MASK 0x00000008

#define REG_SCFG_EXT_REN_SHIFT 2
#define REG_SCFG_EXT_REN_SIZE 1
#define REG_SCFG_EXT_REN_MASK 0x00000004

#define REG_SCFG_EXT_GEO_SHIFT 1
#define REG_SCFG_EXT_GEO_SIZE 1
#define REG_SCFG_EXT_GEO_MASK 0x00000002

#define REG_SCFG_EXT_DMA_SHIFT 0
#define REG_SCFG_EXT_DMA_SIZE 1
#define REG_SCFG_EXT_DMA_MASK 0x00000001

#ifndef SDK_ASM
#define REG_SCFG_EXT_FIELD(cfg, wram, mc_b, dsp, cam, dmac, psram, vram, lcdc, \
                           intc, mc, div, g2de, ren, geo, dma)                 \
  (u32)(((u32)(cfg) << REG_SCFG_EXT_CFG_SHIFT) |                               \
        ((u32)(wram) << REG_SCFG_EXT_WRAM_SHIFT) |                             \
        ((u32)(mc_b) << REG_SCFG_EXT_MC_B_SHIFT) |                             \
        ((u32)(dsp) << REG_SCFG_EXT_DSP_SHIFT) |                               \
        ((u32)(cam) << REG_SCFG_EXT_CAM_SHIFT) |                               \
        ((u32)(dmac) << REG_SCFG_EXT_DMAC_SHIFT) |                             \
        ((u32)(psram) << REG_SCFG_EXT_PSRAM_SHIFT) |                           \
        ((u32)(vram) << REG_SCFG_EXT_VRAM_SHIFT) |                             \
        ((u32)(lcdc) << REG_SCFG_EXT_LCDC_SHIFT) |                             \
        ((u32)(intc) << REG_SCFG_EXT_INTC_SHIFT) |                             \
        ((u32)(mc) << REG_SCFG_EXT_MC_SHIFT) |                                 \
        ((u32)(div) << REG_SCFG_EXT_DIV_SHIFT) |                               \
        ((u32)(g2de) << REG_SCFG_EXT_G2DE_SHIFT) |                             \
        ((u32)(ren) << REG_SCFG_EXT_REN_SHIFT) |                               \
        ((u32)(geo) << REG_SCFG_EXT_GEO_SHIFT) |                               \
        ((u32)(dma) << REG_SCFG_EXT_DMA_SHIFT))
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

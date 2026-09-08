#ifndef TWL_HW_ARM9_IOREG_MI_H_
#define TWL_HW_ARM9_IOREG_MI_H_

#ifndef SDK_ASM
#include <nitro/types.h>
#include <twl/hw/ARM9/mmap_global.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern REGType32v s_reg_MI_DMA0SAD;
extern REGType32v s_reg_MI_DMA0DAD;
extern REGType32v s_reg_MI_DMA0CNT;
extern REGType32v s_reg_MI_DMA1SAD;
extern REGType32v s_reg_MI_DMA1DAD;
extern REGType32v s_reg_MI_DMA1CNT;
extern REGType32v s_reg_MI_DMA2SAD;
extern REGType32v s_reg_MI_DMA2DAD;
extern REGType32v s_reg_MI_DMA2CNT;
extern REGType32v s_reg_MI_DMA3SAD;
extern REGType32v s_reg_MI_DMA3DAD;
extern REGType32v s_reg_MI_DMA3CNT;
extern REGType32v s_reg_MI_DMA0_CLR_DATA;
extern REGType32v s_reg_MI_DMA1_CLR_DATA;
extern REGType32v s_reg_MI_DMA2_CLR_DATA;
extern REGType32v s_reg_MI_DMA3_CLR_DATA;
extern REGType16v s_reg_MI_MCCNT0;
extern REGType16v s_reg_MI_MCD0;
extern REGType32v s_reg_MI_MCD1;
extern REGType32v s_reg_MI_MCCNT1;
extern REGType32v s_reg_MI_MCCMD0;
extern REGType32v s_reg_MI_MCCMD1;
extern REGType16v s_reg_MI_EXMEMCNT;

#define REG_DMA0SAD_OFFSET 0x0b0
#define REG_DMA0SAD_ADDR (&s_reg_MI_DMA0SAD)
#define reg_MI_DMA0SAD (*(REGType32v *)REG_DMA0SAD_ADDR)

#define REG_DMA0DAD_OFFSET 0x0b4
#define REG_DMA0DAD_ADDR (&s_reg_MI_DMA0DAD)
#define reg_MI_DMA0DAD (*(REGType32v *)REG_DMA0DAD_ADDR)

#define REG_DMA0CNT_OFFSET 0x0b8
#define REG_DMA0CNT_ADDR (&s_reg_MI_DMA0CNT)
#define reg_MI_DMA0CNT (*(REGType32v *)REG_DMA0CNT_ADDR)

#define REG_DMA1SAD_OFFSET 0x0bc
#define REG_DMA1SAD_ADDR (&s_reg_MI_DMA1SAD)
#define reg_MI_DMA1SAD (*(REGType32v *)REG_DMA1SAD_ADDR)

#define REG_DMA1DAD_OFFSET 0x0c0
#define REG_DMA1DAD_ADDR (&s_reg_MI_DMA1DAD)
#define reg_MI_DMA1DAD (*(REGType32v *)REG_DMA1DAD_ADDR)

#define REG_DMA1CNT_OFFSET 0x0c4
#define REG_DMA1CNT_ADDR (&s_reg_MI_DMA1CNT)
#define reg_MI_DMA1CNT (*(REGType32v *)REG_DMA1CNT_ADDR)

#define REG_DMA2SAD_OFFSET 0x0c8
#define REG_DMA2SAD_ADDR (&s_reg_MI_DMA2SAD)
#define reg_MI_DMA2SAD (*(REGType32v *)REG_DMA2SAD_ADDR)

#define REG_DMA2DAD_OFFSET 0x0cc
#define REG_DMA2DAD_ADDR (&s_reg_MI_DMA2DAD)
#define reg_MI_DMA2DAD (*(REGType32v *)REG_DMA2DAD_ADDR)

#define REG_DMA2CNT_OFFSET 0x0d0
#define REG_DMA2CNT_ADDR (&s_reg_MI_DMA2CNT)
#define reg_MI_DMA2CNT (*(REGType32v *)REG_DMA2CNT_ADDR)

#define REG_DMA3SAD_OFFSET 0x0d4
#define REG_DMA3SAD_ADDR (&s_reg_MI_DMA3SAD)
#define reg_MI_DMA3SAD (*(REGType32v *)REG_DMA3SAD_ADDR)

#define REG_DMA3DAD_OFFSET 0x0d8
#define REG_DMA3DAD_ADDR (&s_reg_MI_DMA3DAD)
#define reg_MI_DMA3DAD (*(REGType32v *)REG_DMA3DAD_ADDR)

#define REG_DMA3CNT_OFFSET 0x0dc
#define REG_DMA3CNT_ADDR (&s_reg_MI_DMA3CNT)
#define reg_MI_DMA3CNT (*(REGType32v *)REG_DMA3CNT_ADDR)

#define REG_DMA0_CLR_DATA_OFFSET 0x0e0
#define REG_DMA0_CLR_DATA_ADDR (&s_reg_MI_DMA0_CLR_DATA)
#define reg_MI_DMA0_CLR_DATA (*(REGType32v *)REG_DMA0_CLR_DATA_ADDR)

#define REG_DMA1_CLR_DATA_OFFSET 0x0e4
#define REG_DMA1_CLR_DATA_ADDR (&s_reg_MI_DMA1_CLR_DATA)
#define reg_MI_DMA1_CLR_DATA (*(REGType32v *)REG_DMA1_CLR_DATA_ADDR)

#define REG_DMA2_CLR_DATA_OFFSET 0x0e8
#define REG_DMA2_CLR_DATA_ADDR (&s_reg_MI_DMA2_CLR_DATA)
#define reg_MI_DMA2_CLR_DATA (*(REGType32v *)REG_DMA2_CLR_DATA_ADDR)

#define REG_DMA3_CLR_DATA_OFFSET 0x0ec
#define REG_DMA3_CLR_DATA_ADDR (&s_reg_MI_DMA3_CLR_DATA)
#define reg_MI_DMA3_CLR_DATA (*(REGType32v *)REG_DMA3_CLR_DATA_ADDR)

#define REG_NDMAGCNT_OFFSET 0x4100
#define REG_NDMAGCNT_ADDR (HW_REG_BASE + REG_NDMAGCNT_OFFSET)
#define reg_MI_NDMAGCNT (*(REGType32v *)REG_NDMAGCNT_ADDR)

#define REG_NDMA0SAD_OFFSET 0x4104
#define REG_NDMA0SAD_ADDR (HW_REG_BASE + REG_NDMA0SAD_OFFSET)
#define reg_MI_NDMA0SAD (*(REGType32v *)REG_NDMA0SAD_ADDR)

#define REG_NDAM0DAD_OFFSET 0x4108
#define REG_NDAM0DAD_ADDR (HW_REG_BASE + REG_NDAM0DAD_OFFSET)
#define reg_MI_NDAM0DAD (*(REGType32v *)REG_NDAM0DAD_ADDR)

#define REG_NDMA0TCNT_OFFSET 0x410c
#define REG_NDMA0TCNT_ADDR (HW_REG_BASE + REG_NDMA0TCNT_OFFSET)
#define reg_MI_NDMA0TCNT (*(REGType32v *)REG_NDMA0TCNT_ADDR)

#define REG_NDMA0WCNT_OFFSET 0x4110
#define REG_NDMA0WCNT_ADDR (HW_REG_BASE + REG_NDMA0WCNT_OFFSET)
#define reg_MI_NDMA0WCNT (*(REGType32v *)REG_NDMA0WCNT_ADDR)

#define REG_NDMA0BCNT_OFFSET 0x4114
#define REG_NDMA0BCNT_ADDR (HW_REG_BASE + REG_NDMA0BCNT_OFFSET)
#define reg_MI_NDMA0BCNT (*(REGType32v *)REG_NDMA0BCNT_ADDR)

#define REG_NDMA0FDATA_OFFSET 0x4118
#define REG_NDMA0FDATA_ADDR (HW_REG_BASE + REG_NDMA0FDATA_OFFSET)
#define reg_MI_NDMA0FDATA (*(REGType32v *)REG_NDMA0FDATA_ADDR)

#define REG_NDMA0CNT_OFFSET 0x411c
#define REG_NDMA0CNT_ADDR (HW_REG_BASE + REG_NDMA0CNT_OFFSET)
#define reg_MI_NDMA0CNT (*(REGType32v *)REG_NDMA0CNT_ADDR)

#define REG_NDMA1SAD_OFFSET 0x4120
#define REG_NDMA1SAD_ADDR (HW_REG_BASE + REG_NDMA1SAD_OFFSET)
#define reg_MI_NDMA1SAD (*(REGType32v *)REG_NDMA1SAD_ADDR)

#define REG_NDAM1DAD_OFFSET 0x4124
#define REG_NDAM1DAD_ADDR (HW_REG_BASE + REG_NDAM1DAD_OFFSET)
#define reg_MI_NDAM1DAD (*(REGType32v *)REG_NDAM1DAD_ADDR)

#define REG_NDMA1TCNT_OFFSET 0x4128
#define REG_NDMA1TCNT_ADDR (HW_REG_BASE + REG_NDMA1TCNT_OFFSET)
#define reg_MI_NDMA1TCNT (*(REGType32v *)REG_NDMA1TCNT_ADDR)

#define REG_NDMA1WCNT_OFFSET 0x412c
#define REG_NDMA1WCNT_ADDR (HW_REG_BASE + REG_NDMA1WCNT_OFFSET)
#define reg_MI_NDMA1WCNT (*(REGType32v *)REG_NDMA1WCNT_ADDR)

#define REG_NDMA1BCNT_OFFSET 0x4130
#define REG_NDMA1BCNT_ADDR (HW_REG_BASE + REG_NDMA1BCNT_OFFSET)
#define reg_MI_NDMA1BCNT (*(REGType32v *)REG_NDMA1BCNT_ADDR)

#define REG_NDMA1FDATA_OFFSET 0x4134
#define REG_NDMA1FDATA_ADDR (HW_REG_BASE + REG_NDMA1FDATA_OFFSET)
#define reg_MI_NDMA1FDATA (*(REGType32v *)REG_NDMA1FDATA_ADDR)

#define REG_NDMA1CNT_OFFSET 0x4138
#define REG_NDMA1CNT_ADDR (HW_REG_BASE + REG_NDMA1CNT_OFFSET)
#define reg_MI_NDMA1CNT (*(REGType32v *)REG_NDMA1CNT_ADDR)

#define REG_NDMA2SAD_OFFSET 0x413c
#define REG_NDMA2SAD_ADDR (HW_REG_BASE + REG_NDMA2SAD_OFFSET)
#define reg_MI_NDMA2SAD (*(REGType32v *)REG_NDMA2SAD_ADDR)

#define REG_NDAM2DAD_OFFSET 0x4140
#define REG_NDAM2DAD_ADDR (HW_REG_BASE + REG_NDAM2DAD_OFFSET)
#define reg_MI_NDAM2DAD (*(REGType32v *)REG_NDAM2DAD_ADDR)

#define REG_NDMA2TCNT_OFFSET 0x4144
#define REG_NDMA2TCNT_ADDR (HW_REG_BASE + REG_NDMA2TCNT_OFFSET)
#define reg_MI_NDMA2TCNT (*(REGType32v *)REG_NDMA2TCNT_ADDR)

#define REG_NDMA2WCNT_OFFSET 0x4148
#define REG_NDMA2WCNT_ADDR (HW_REG_BASE + REG_NDMA2WCNT_OFFSET)
#define reg_MI_NDMA2WCNT (*(REGType32v *)REG_NDMA2WCNT_ADDR)

#define REG_NDMA2BCNT_OFFSET 0x414c
#define REG_NDMA2BCNT_ADDR (HW_REG_BASE + REG_NDMA2BCNT_OFFSET)
#define reg_MI_NDMA2BCNT (*(REGType32v *)REG_NDMA2BCNT_ADDR)

#define REG_NDMA2FDATA_OFFSET 0x4150
#define REG_NDMA2FDATA_ADDR (HW_REG_BASE + REG_NDMA2FDATA_OFFSET)
#define reg_MI_NDMA2FDATA (*(REGType32v *)REG_NDMA2FDATA_ADDR)

#define REG_NDMA2CNT_OFFSET 0x4154
#define REG_NDMA2CNT_ADDR (HW_REG_BASE + REG_NDMA2CNT_OFFSET)
#define reg_MI_NDMA2CNT (*(REGType32v *)REG_NDMA2CNT_ADDR)

#define REG_NDMA3SAD_OFFSET 0x4158
#define REG_NDMA3SAD_ADDR (HW_REG_BASE + REG_NDMA3SAD_OFFSET)
#define reg_MI_NDMA3SAD (*(REGType32v *)REG_NDMA3SAD_ADDR)

#define REG_NDAM3DAD_OFFSET 0x415c
#define REG_NDAM3DAD_ADDR (HW_REG_BASE + REG_NDAM3DAD_OFFSET)
#define reg_MI_NDAM3DAD (*(REGType32v *)REG_NDAM3DAD_ADDR)

#define REG_NDMA3TCNT_OFFSET 0x4160
#define REG_NDMA3TCNT_ADDR (HW_REG_BASE + REG_NDMA3TCNT_OFFSET)
#define reg_MI_NDMA3TCNT (*(REGType32v *)REG_NDMA3TCNT_ADDR)

#define REG_NDMA3WCNT_OFFSET 0x4164
#define REG_NDMA3WCNT_ADDR (HW_REG_BASE + REG_NDMA3WCNT_OFFSET)
#define reg_MI_NDMA3WCNT (*(REGType32v *)REG_NDMA3WCNT_ADDR)

#define REG_NDMA3BCNT_OFFSET 0x4168
#define REG_NDMA3BCNT_ADDR (HW_REG_BASE + REG_NDMA3BCNT_OFFSET)
#define reg_MI_NDMA3BCNT (*(REGType32v *)REG_NDMA3BCNT_ADDR)

#define REG_NDMA3FDATA_OFFSET 0x416c
#define REG_NDMA3FDATA_ADDR (HW_REG_BASE + REG_NDMA3FDATA_OFFSET)
#define reg_MI_NDMA3FDATA (*(REGType32v *)REG_NDMA3FDATA_ADDR)

#define REG_NDMA3CNT_OFFSET 0x4170
#define REG_NDMA3CNT_ADDR (HW_REG_BASE + REG_NDMA3CNT_OFFSET)
#define reg_MI_NDMA3CNT (*(REGType32v *)REG_NDMA3CNT_ADDR)

#define REG_MCCNT0_OFFSET 0x1a0
#define REG_MCCNT0_ADDR (&s_reg_MI_MCCNT0)
#define reg_MI_MCCNT0 (*(REGType16v *)REG_MCCNT0_ADDR)

#define REG_MCD0_OFFSET 0x1a2
#define REG_MCD0_ADDR (&s_reg_MI_MCD0)
#define reg_MI_MCD0 (*(REGType16v *)REG_MCD0_ADDR)

#define REG_MCD1_OFFSET 0x100010
#define REG_MCD1_ADDR (&s_reg_MI_MCD1)
#define reg_MI_MCD1 (*(REGType32v *)REG_MCD1_ADDR)

#define REG_MCCNT1_OFFSET 0x1a4
#define REG_MCCNT1_ADDR (&s_reg_MI_MCCNT1)
#define reg_MI_MCCNT1 (*(REGType32v *)REG_MCCNT1_ADDR)

#define REG_MCCMD0_OFFSET 0x1a8
#define REG_MCCMD0_ADDR (&s_reg_MI_MCCMD0)
#define reg_MI_MCCMD0 (*(REGType32v *)REG_MCCMD0_ADDR)

#define REG_MCCMD1_OFFSET 0x1ac
#define REG_MCCMD1_ADDR (&s_reg_MI_MCCMD1)
#define reg_MI_MCCMD1 (*(REGType32v *)REG_MCCMD1_ADDR)

#define REG_MCCNT0_A_OFFSET 0x1a0
#define REG_MCCNT0_A_ADDR (HW_REG_BASE + REG_MCCNT0_A_OFFSET)
#define reg_MI_MCCNT0_A (*(REGType16v *)REG_MCCNT0_A_ADDR)

#define REG_MCD0_A_OFFSET 0x1a2
#define REG_MCD0_A_ADDR (HW_REG_BASE + REG_MCD0_A_OFFSET)
#define reg_MI_MCD0_A (*(REGType16v *)REG_MCD0_A_ADDR)

#define REG_MCD1_A_OFFSET 0x100010
#define REG_MCD1_A_ADDR (HW_REG_BASE + REG_MCD1_A_OFFSET)
#define reg_MI_MCD1_A (*(REGType32v *)REG_MCD1_A_ADDR)

#define REG_MCCNT1_A_OFFSET 0x1a4
#define REG_MCCNT1_A_ADDR (HW_REG_BASE + REG_MCCNT1_A_OFFSET)
#define reg_MI_MCCNT1_A (*(REGType32v *)REG_MCCNT1_A_ADDR)

#define REG_MCCMD0_A_OFFSET 0x1a8
#define REG_MCCMD0_A_ADDR (HW_REG_BASE + REG_MCCMD0_A_OFFSET)
#define reg_MI_MCCMD0_A (*(REGType32v *)REG_MCCMD0_A_ADDR)

#define REG_MCCMD1_A_OFFSET 0x1ac
#define REG_MCCMD1_A_ADDR (HW_REG_BASE + REG_MCCMD1_A_OFFSET)
#define reg_MI_MCCMD1_A (*(REGType32v *)REG_MCCMD1_A_ADDR)

#define REG_MCSRC0_A_OFFSET 0x1b0
#define REG_MCSRC0_A_ADDR (HW_REG_BASE + REG_MCSRC0_A_OFFSET)
#define reg_MI_MCSRC0_A (*(REGType32v *)REG_MCSRC0_A_ADDR)

#define REG_MCSRC1_A_OFFSET 0x1b4
#define REG_MCSRC1_A_ADDR (HW_REG_BASE + REG_MCSRC1_A_OFFSET)
#define reg_MI_MCSRC1_A (*(REGType32v *)REG_MCSRC1_A_ADDR)

#define REG_MCSRC2_A_OFFSET 0x1b8
#define REG_MCSRC2_A_ADDR (HW_REG_BASE + REG_MCSRC2_A_OFFSET)
#define reg_MI_MCSRC2_A (*(REGType32v *)REG_MCSRC2_A_ADDR)

#define REG_MCCNT0_B_OFFSET 0x21a0
#define REG_MCCNT0_B_ADDR (HW_REG_BASE + REG_MCCNT0_B_OFFSET)
#define reg_MI_MCCNT0_B (*(REGType16v *)REG_MCCNT0_B_ADDR)

#define REG_MCD0_B_OFFSET 0x21a2
#define REG_MCD0_B_ADDR (HW_REG_BASE + REG_MCD0_B_OFFSET)
#define reg_MI_MCD0_B (*(REGType16v *)REG_MCD0_B_ADDR)

#define REG_MCD1_B_OFFSET 0x102010
#define REG_MCD1_B_ADDR (HW_REG_BASE + REG_MCD1_B_OFFSET)
#define reg_MI_MCD1_B (*(REGType32v *)REG_MCD1_B_ADDR)

#define REG_MCCNT1_B_OFFSET 0x21a4
#define REG_MCCNT1_B_ADDR (HW_REG_BASE + REG_MCCNT1_B_OFFSET)
#define reg_MI_MCCNT1_B (*(REGType32v *)REG_MCCNT1_B_ADDR)

#define REG_MCCMD0_B_OFFSET 0x21a8
#define REG_MCCMD0_B_ADDR (HW_REG_BASE + REG_MCCMD0_B_OFFSET)
#define reg_MI_MCCMD0_B (*(REGType32v *)REG_MCCMD0_B_ADDR)

#define REG_MCCMD1_B_OFFSET 0x21ac
#define REG_MCCMD1_B_ADDR (HW_REG_BASE + REG_MCCMD1_B_OFFSET)
#define reg_MI_MCCMD1_B (*(REGType32v *)REG_MCCMD1_B_ADDR)

#define REG_MCSRC0_B_OFFSET 0x21b0
#define REG_MCSRC0_B_ADDR (HW_REG_BASE + REG_MCSRC0_B_OFFSET)
#define reg_MI_MCSRC0_B (*(REGType32v *)REG_MCSRC0_B_ADDR)

#define REG_MCSRC1_B_OFFSET 0x21b4
#define REG_MCSRC1_B_ADDR (HW_REG_BASE + REG_MCSRC1_B_OFFSET)
#define reg_MI_MCSRC1_B (*(REGType32v *)REG_MCSRC1_B_ADDR)

#define REG_MCSRC2_B_OFFSET 0x21b8
#define REG_MCSRC2_B_ADDR (HW_REG_BASE + REG_MCSRC2_B_OFFSET)
#define reg_MI_MCSRC2_B (*(REGType32v *)REG_MCSRC2_B_ADDR)

#define REG_EXMEMCNT_OFFSET 0x204
#define REG_EXMEMCNT_ADDR (&s_reg_MI_EXMEMCNT)
#define reg_MI_EXMEMCNT (*(REGType16v *)REG_EXMEMCNT_ADDR)

#define REG_MC_OFFSET 0x4010
#define REG_MC_ADDR (HW_REG_BASE + REG_MC_OFFSET)
#define reg_MI_MC (*(const REGType16v *)REG_MC_ADDR)

#define REG_MBK1_OFFSET 0x4040
#define REG_MBK1_ADDR (HW_REG_BASE + REG_MBK1_OFFSET)
#define reg_MI_MBK1 (*(REGType32v *)REG_MBK1_ADDR)

#define REG_MBK_A0_OFFSET 0x4040
#define REG_MBK_A0_ADDR (HW_REG_BASE + REG_MBK_A0_OFFSET)
#define reg_MI_MBK_A0 (*(REGType8v *)REG_MBK_A0_ADDR)

#define REG_MBK_A1_OFFSET 0x4041
#define REG_MBK_A1_ADDR (HW_REG_BASE + REG_MBK_A1_OFFSET)
#define reg_MI_MBK_A1 (*(REGType8v *)REG_MBK_A1_ADDR)

#define REG_MBK_A2_OFFSET 0x4042
#define REG_MBK_A2_ADDR (HW_REG_BASE + REG_MBK_A2_OFFSET)
#define reg_MI_MBK_A2 (*(REGType8v *)REG_MBK_A2_ADDR)

#define REG_MBK_A3_OFFSET 0x4043
#define REG_MBK_A3_ADDR (HW_REG_BASE + REG_MBK_A3_OFFSET)
#define reg_MI_MBK_A3 (*(REGType8v *)REG_MBK_A3_ADDR)

#define REG_MBK2_OFFSET 0x4044
#define REG_MBK2_ADDR (HW_REG_BASE + REG_MBK2_OFFSET)
#define reg_MI_MBK2 (*(REGType32v *)REG_MBK2_ADDR)

#define REG_MBK_B0_OFFSET 0x4044
#define REG_MBK_B0_ADDR (HW_REG_BASE + REG_MBK_B0_OFFSET)
#define reg_MI_MBK_B0 (*(REGType8v *)REG_MBK_B0_ADDR)

#define REG_MBK_B1_OFFSET 0x4045
#define REG_MBK_B1_ADDR (HW_REG_BASE + REG_MBK_B1_OFFSET)
#define reg_MI_MBK_B1 (*(REGType8v *)REG_MBK_B1_ADDR)

#define REG_MBK_B2_OFFSET 0x4046
#define REG_MBK_B2_ADDR (HW_REG_BASE + REG_MBK_B2_OFFSET)
#define reg_MI_MBK_B2 (*(REGType8v *)REG_MBK_B2_ADDR)

#define REG_MBK_B3_OFFSET 0x4047
#define REG_MBK_B3_ADDR (HW_REG_BASE + REG_MBK_B3_OFFSET)
#define reg_MI_MBK_B3 (*(REGType8v *)REG_MBK_B3_ADDR)

#define REG_MBK3_OFFSET 0x4048
#define REG_MBK3_ADDR (HW_REG_BASE + REG_MBK3_OFFSET)
#define reg_MI_MBK3 (*(REGType32v *)REG_MBK3_ADDR)

#define REG_MBK_B4_OFFSET 0x4048
#define REG_MBK_B4_ADDR (HW_REG_BASE + REG_MBK_B4_OFFSET)
#define reg_MI_MBK_B4 (*(REGType8v *)REG_MBK_B4_ADDR)

#define REG_MBK_B5_OFFSET 0x4049
#define REG_MBK_B5_ADDR (HW_REG_BASE + REG_MBK_B5_OFFSET)
#define reg_MI_MBK_B5 (*(REGType8v *)REG_MBK_B5_ADDR)

#define REG_MBK_B6_OFFSET 0x404a
#define REG_MBK_B6_ADDR (HW_REG_BASE + REG_MBK_B6_OFFSET)
#define reg_MI_MBK_B6 (*(REGType8v *)REG_MBK_B6_ADDR)

#define REG_MBK_B7_OFFSET 0x404b
#define REG_MBK_B7_ADDR (HW_REG_BASE + REG_MBK_B7_OFFSET)
#define reg_MI_MBK_B7 (*(REGType8v *)REG_MBK_B7_ADDR)

#define REG_MBK4_OFFSET 0x404c
#define REG_MBK4_ADDR (HW_REG_BASE + REG_MBK4_OFFSET)
#define reg_MI_MBK4 (*(REGType32v *)REG_MBK4_ADDR)

#define REG_MBK_C0_OFFSET 0x404c
#define REG_MBK_C0_ADDR (HW_REG_BASE + REG_MBK_C0_OFFSET)
#define reg_MI_MBK_C0 (*(REGType8v *)REG_MBK_C0_ADDR)

#define REG_MBK_C1_OFFSET 0x404d
#define REG_MBK_C1_ADDR (HW_REG_BASE + REG_MBK_C1_OFFSET)
#define reg_MI_MBK_C1 (*(REGType8v *)REG_MBK_C1_ADDR)

#define REG_MBK_C2_OFFSET 0x404e
#define REG_MBK_C2_ADDR (HW_REG_BASE + REG_MBK_C2_OFFSET)
#define reg_MI_MBK_C2 (*(REGType8v *)REG_MBK_C2_ADDR)

#define REG_MBK_C3_OFFSET 0x404f
#define REG_MBK_C3_ADDR (HW_REG_BASE + REG_MBK_C3_OFFSET)
#define reg_MI_MBK_C3 (*(REGType8v *)REG_MBK_C3_ADDR)

#define REG_MBK5_OFFSET 0x4050
#define REG_MBK5_ADDR (HW_REG_BASE + REG_MBK5_OFFSET)
#define reg_MI_MBK5 (*(REGType32v *)REG_MBK5_ADDR)

#define REG_MBK_C4_OFFSET 0x4050
#define REG_MBK_C4_ADDR (HW_REG_BASE + REG_MBK_C4_OFFSET)
#define reg_MI_MBK_C4 (*(REGType8v *)REG_MBK_C4_ADDR)

#define REG_MBK_C5_OFFSET 0x4051
#define REG_MBK_C5_ADDR (HW_REG_BASE + REG_MBK_C5_OFFSET)
#define reg_MI_MBK_C5 (*(REGType8v *)REG_MBK_C5_ADDR)

#define REG_MBK_C6_OFFSET 0x4052
#define REG_MBK_C6_ADDR (HW_REG_BASE + REG_MBK_C6_OFFSET)
#define reg_MI_MBK_C6 (*(REGType8v *)REG_MBK_C6_ADDR)

#define REG_MBK_C7_OFFSET 0x4053
#define REG_MBK_C7_ADDR (HW_REG_BASE + REG_MBK_C7_OFFSET)
#define reg_MI_MBK_C7 (*(REGType8v *)REG_MBK_C7_ADDR)

#define REG_MBK6_OFFSET 0x4054
#define REG_MBK6_ADDR (HW_REG_BASE + REG_MBK6_OFFSET)
#define reg_MI_MBK6 (*(REGType32v *)REG_MBK6_ADDR)

#define REG_MBK7_OFFSET 0x4058
#define REG_MBK7_ADDR (HW_REG_BASE + REG_MBK7_OFFSET)
#define reg_MI_MBK7 (*(REGType32v *)REG_MBK7_ADDR)

#define REG_MBK8_OFFSET 0x405c
#define REG_MBK8_ADDR (HW_REG_BASE + REG_MBK8_OFFSET)
#define reg_MI_MBK8 (*(REGType32v *)REG_MBK8_ADDR)

#define REG_MBK9_OFFSET 0x4060
#define REG_MBK9_ADDR (HW_REG_BASE + REG_MBK9_OFFSET)
#define reg_MI_MBK9 (*(REGType32v *)REG_MBK9_ADDR)

#define REG_MBK_A_LOCK_OFFSET 0x4060
#define REG_MBK_A_LOCK_ADDR (HW_REG_BASE + REG_MBK_A_LOCK_OFFSET)
#define reg_MI_MBK_A_LOCK (*(REGType8v *)REG_MBK_A_LOCK_ADDR)

#define REG_MBK_B_LOCK_OFFSET 0x4061
#define REG_MBK_B_LOCK_ADDR (HW_REG_BASE + REG_MBK_B_LOCK_OFFSET)
#define reg_MI_MBK_B_LOCK (*(REGType8v *)REG_MBK_B_LOCK_ADDR)

#define REG_MBK_C_LOCK_OFFSET 0x4062
#define REG_MBK_C_LOCK_ADDR (HW_REG_BASE + REG_MBK_C_LOCK_OFFSET)
#define reg_MI_MBK_C_LOCK (*(REGType8v *)REG_MBK_C_LOCK_ADDR)

#define REG_MI_DMA0SAD_DMASRC_SHIFT 0
#define REG_MI_DMA0SAD_DMASRC_SIZE 28
#define REG_MI_DMA0SAD_DMASRC_MASK 0x0fffffff

#ifndef SDK_ASM
#define REG_MI_DMA0SAD_FIELD(dmasrc)                                           \
  (u32)(((u32)(dmasrc) << REG_MI_DMA0SAD_DMASRC_SHIFT))
#endif

#define REG_MI_DMA0DAD_DMADEST_SHIFT 0
#define REG_MI_DMA0DAD_DMADEST_SIZE 28
#define REG_MI_DMA0DAD_DMADEST_MASK 0x0fffffff

#ifndef SDK_ASM
#define REG_MI_DMA0DAD_FIELD(dmadest)                                          \
  (u32)(((u32)(dmadest) << REG_MI_DMA0DAD_DMADEST_SHIFT))
#endif

#define REG_MI_DMA0CNT_E_SHIFT 31
#define REG_MI_DMA0CNT_E_SIZE 1
#define REG_MI_DMA0CNT_E_MASK 0x80000000

#define REG_MI_DMA0CNT_I_SHIFT 30
#define REG_MI_DMA0CNT_I_SIZE 1
#define REG_MI_DMA0CNT_I_MASK 0x40000000

#define REG_MI_DMA0CNT_MODE_SHIFT 27
#define REG_MI_DMA0CNT_MODE_SIZE 3
#define REG_MI_DMA0CNT_MODE_MASK 0x38000000

#define REG_MI_DMA0CNT_SB_SHIFT 26
#define REG_MI_DMA0CNT_SB_SIZE 1
#define REG_MI_DMA0CNT_SB_MASK 0x04000000

#define REG_MI_DMA0CNT_CM_SHIFT 25
#define REG_MI_DMA0CNT_CM_SIZE 1
#define REG_MI_DMA0CNT_CM_MASK 0x02000000

#define REG_MI_DMA0CNT_SAR_SHIFT 23
#define REG_MI_DMA0CNT_SAR_SIZE 2
#define REG_MI_DMA0CNT_SAR_MASK 0x01800000

#define REG_MI_DMA0CNT_DAR_SHIFT 21
#define REG_MI_DMA0CNT_DAR_SIZE 2
#define REG_MI_DMA0CNT_DAR_MASK 0x00600000

#define REG_MI_DMA0CNT_WORDCNT_SHIFT 0
#define REG_MI_DMA0CNT_WORDCNT_SIZE 21
#define REG_MI_DMA0CNT_WORDCNT_MASK 0x001fffff

#ifndef SDK_ASM
#define REG_MI_DMA0CNT_FIELD(e, i, mode, sb, cm, sar, dar, wordcnt)            \
  (u32)(((u32)(e) << REG_MI_DMA0CNT_E_SHIFT) |                                 \
        ((u32)(i) << REG_MI_DMA0CNT_I_SHIFT) |                                 \
        ((u32)(mode) << REG_MI_DMA0CNT_MODE_SHIFT) |                           \
        ((u32)(sb) << REG_MI_DMA0CNT_SB_SHIFT) |                               \
        ((u32)(cm) << REG_MI_DMA0CNT_CM_SHIFT) |                               \
        ((u32)(sar) << REG_MI_DMA0CNT_SAR_SHIFT) |                             \
        ((u32)(dar) << REG_MI_DMA0CNT_DAR_SHIFT) |                             \
        ((u32)(wordcnt) << REG_MI_DMA0CNT_WORDCNT_SHIFT))
#endif

#define REG_MI_DMA1SAD_DMASRC_SHIFT 0
#define REG_MI_DMA1SAD_DMASRC_SIZE 28
#define REG_MI_DMA1SAD_DMASRC_MASK 0x0fffffff

#ifndef SDK_ASM
#define REG_MI_DMA1SAD_FIELD(dmasrc)                                           \
  (u32)(((u32)(dmasrc) << REG_MI_DMA1SAD_DMASRC_SHIFT))
#endif

#define REG_MI_DMA1DAD_DMADEST_SHIFT 0
#define REG_MI_DMA1DAD_DMADEST_SIZE 28
#define REG_MI_DMA1DAD_DMADEST_MASK 0x0fffffff

#ifndef SDK_ASM
#define REG_MI_DMA1DAD_FIELD(dmadest)                                          \
  (u32)(((u32)(dmadest) << REG_MI_DMA1DAD_DMADEST_SHIFT))
#endif

#define REG_MI_DMA1CNT_E_SHIFT 31
#define REG_MI_DMA1CNT_E_SIZE 1
#define REG_MI_DMA1CNT_E_MASK 0x80000000

#define REG_MI_DMA1CNT_I_SHIFT 30
#define REG_MI_DMA1CNT_I_SIZE 1
#define REG_MI_DMA1CNT_I_MASK 0x40000000

#define REG_MI_DMA1CNT_MODE_SHIFT 27
#define REG_MI_DMA1CNT_MODE_SIZE 3
#define REG_MI_DMA1CNT_MODE_MASK 0x38000000

#define REG_MI_DMA1CNT_SB_SHIFT 26
#define REG_MI_DMA1CNT_SB_SIZE 1
#define REG_MI_DMA1CNT_SB_MASK 0x04000000

#define REG_MI_DMA1CNT_CM_SHIFT 25
#define REG_MI_DMA1CNT_CM_SIZE 1
#define REG_MI_DMA1CNT_CM_MASK 0x02000000

#define REG_MI_DMA1CNT_SAR_SHIFT 23
#define REG_MI_DMA1CNT_SAR_SIZE 2
#define REG_MI_DMA1CNT_SAR_MASK 0x01800000

#define REG_MI_DMA1CNT_DAR_SHIFT 21
#define REG_MI_DMA1CNT_DAR_SIZE 2
#define REG_MI_DMA1CNT_DAR_MASK 0x00600000

#define REG_MI_DMA1CNT_WORDCNT_SHIFT 0
#define REG_MI_DMA1CNT_WORDCNT_SIZE 21
#define REG_MI_DMA1CNT_WORDCNT_MASK 0x001fffff

#ifndef SDK_ASM
#define REG_MI_DMA1CNT_FIELD(e, i, mode, sb, cm, sar, dar, wordcnt)            \
  (u32)(((u32)(e) << REG_MI_DMA1CNT_E_SHIFT) |                                 \
        ((u32)(i) << REG_MI_DMA1CNT_I_SHIFT) |                                 \
        ((u32)(mode) << REG_MI_DMA1CNT_MODE_SHIFT) |                           \
        ((u32)(sb) << REG_MI_DMA1CNT_SB_SHIFT) |                               \
        ((u32)(cm) << REG_MI_DMA1CNT_CM_SHIFT) |                               \
        ((u32)(sar) << REG_MI_DMA1CNT_SAR_SHIFT) |                             \
        ((u32)(dar) << REG_MI_DMA1CNT_DAR_SHIFT) |                             \
        ((u32)(wordcnt) << REG_MI_DMA1CNT_WORDCNT_SHIFT))
#endif

#define REG_MI_DMA2SAD_DMASRC_SHIFT 0
#define REG_MI_DMA2SAD_DMASRC_SIZE 28
#define REG_MI_DMA2SAD_DMASRC_MASK 0x0fffffff

#ifndef SDK_ASM
#define REG_MI_DMA2SAD_FIELD(dmasrc)                                           \
  (u32)(((u32)(dmasrc) << REG_MI_DMA2SAD_DMASRC_SHIFT))
#endif

#define REG_MI_DMA2DAD_DMADEST_SHIFT 0
#define REG_MI_DMA2DAD_DMADEST_SIZE 28
#define REG_MI_DMA2DAD_DMADEST_MASK 0x0fffffff

#ifndef SDK_ASM
#define REG_MI_DMA2DAD_FIELD(dmadest)                                          \
  (u32)(((u32)(dmadest) << REG_MI_DMA2DAD_DMADEST_SHIFT))
#endif

#define REG_MI_DMA2CNT_E_SHIFT 31
#define REG_MI_DMA2CNT_E_SIZE 1
#define REG_MI_DMA2CNT_E_MASK 0x80000000

#define REG_MI_DMA2CNT_I_SHIFT 30
#define REG_MI_DMA2CNT_I_SIZE 1
#define REG_MI_DMA2CNT_I_MASK 0x40000000

#define REG_MI_DMA2CNT_MODE_SHIFT 27
#define REG_MI_DMA2CNT_MODE_SIZE 3
#define REG_MI_DMA2CNT_MODE_MASK 0x38000000

#define REG_MI_DMA2CNT_SB_SHIFT 26
#define REG_MI_DMA2CNT_SB_SIZE 1
#define REG_MI_DMA2CNT_SB_MASK 0x04000000

#define REG_MI_DMA2CNT_CM_SHIFT 25
#define REG_MI_DMA2CNT_CM_SIZE 1
#define REG_MI_DMA2CNT_CM_MASK 0x02000000

#define REG_MI_DMA2CNT_SAR_SHIFT 23
#define REG_MI_DMA2CNT_SAR_SIZE 2
#define REG_MI_DMA2CNT_SAR_MASK 0x01800000

#define REG_MI_DMA2CNT_DAR_SHIFT 21
#define REG_MI_DMA2CNT_DAR_SIZE 2
#define REG_MI_DMA2CNT_DAR_MASK 0x00600000

#define REG_MI_DMA2CNT_WORDCNT_SHIFT 0
#define REG_MI_DMA2CNT_WORDCNT_SIZE 21
#define REG_MI_DMA2CNT_WORDCNT_MASK 0x001fffff

#ifndef SDK_ASM
#define REG_MI_DMA2CNT_FIELD(e, i, mode, sb, cm, sar, dar, wordcnt)            \
  (u32)(((u32)(e) << REG_MI_DMA2CNT_E_SHIFT) |                                 \
        ((u32)(i) << REG_MI_DMA2CNT_I_SHIFT) |                                 \
        ((u32)(mode) << REG_MI_DMA2CNT_MODE_SHIFT) |                           \
        ((u32)(sb) << REG_MI_DMA2CNT_SB_SHIFT) |                               \
        ((u32)(cm) << REG_MI_DMA2CNT_CM_SHIFT) |                               \
        ((u32)(sar) << REG_MI_DMA2CNT_SAR_SHIFT) |                             \
        ((u32)(dar) << REG_MI_DMA2CNT_DAR_SHIFT) |                             \
        ((u32)(wordcnt) << REG_MI_DMA2CNT_WORDCNT_SHIFT))
#endif

#define REG_MI_DMA3SAD_DMASRC_SHIFT 0
#define REG_MI_DMA3SAD_DMASRC_SIZE 28
#define REG_MI_DMA3SAD_DMASRC_MASK 0x0fffffff

#ifndef SDK_ASM
#define REG_MI_DMA3SAD_FIELD(dmasrc)                                           \
  (u32)(((u32)(dmasrc) << REG_MI_DMA3SAD_DMASRC_SHIFT))
#endif

#define REG_MI_DMA3DAD_DMADEST_SHIFT 0
#define REG_MI_DMA3DAD_DMADEST_SIZE 28
#define REG_MI_DMA3DAD_DMADEST_MASK 0x0fffffff

#ifndef SDK_ASM
#define REG_MI_DMA3DAD_FIELD(dmadest)                                          \
  (u32)(((u32)(dmadest) << REG_MI_DMA3DAD_DMADEST_SHIFT))
#endif

#define REG_MI_DMA3CNT_E_SHIFT 31
#define REG_MI_DMA3CNT_E_SIZE 1
#define REG_MI_DMA3CNT_E_MASK 0x80000000

#define REG_MI_DMA3CNT_I_SHIFT 30
#define REG_MI_DMA3CNT_I_SIZE 1
#define REG_MI_DMA3CNT_I_MASK 0x40000000

#define REG_MI_DMA3CNT_MODE_SHIFT 27
#define REG_MI_DMA3CNT_MODE_SIZE 3
#define REG_MI_DMA3CNT_MODE_MASK 0x38000000

#define REG_MI_DMA3CNT_SB_SHIFT 26
#define REG_MI_DMA3CNT_SB_SIZE 1
#define REG_MI_DMA3CNT_SB_MASK 0x04000000

#define REG_MI_DMA3CNT_CM_SHIFT 25
#define REG_MI_DMA3CNT_CM_SIZE 1
#define REG_MI_DMA3CNT_CM_MASK 0x02000000

#define REG_MI_DMA3CNT_SAR_SHIFT 23
#define REG_MI_DMA3CNT_SAR_SIZE 2
#define REG_MI_DMA3CNT_SAR_MASK 0x01800000

#define REG_MI_DMA3CNT_DAR_SHIFT 21
#define REG_MI_DMA3CNT_DAR_SIZE 2
#define REG_MI_DMA3CNT_DAR_MASK 0x00600000

#define REG_MI_DMA3CNT_WORDCNT_SHIFT 0
#define REG_MI_DMA3CNT_WORDCNT_SIZE 21
#define REG_MI_DMA3CNT_WORDCNT_MASK 0x001fffff

#ifndef SDK_ASM
#define REG_MI_DMA3CNT_FIELD(e, i, mode, sb, cm, sar, dar, wordcnt)            \
  (u32)(((u32)(e) << REG_MI_DMA3CNT_E_SHIFT) |                                 \
        ((u32)(i) << REG_MI_DMA3CNT_I_SHIFT) |                                 \
        ((u32)(mode) << REG_MI_DMA3CNT_MODE_SHIFT) |                           \
        ((u32)(sb) << REG_MI_DMA3CNT_SB_SHIFT) |                               \
        ((u32)(cm) << REG_MI_DMA3CNT_CM_SHIFT) |                               \
        ((u32)(sar) << REG_MI_DMA3CNT_SAR_SHIFT) |                             \
        ((u32)(dar) << REG_MI_DMA3CNT_DAR_SHIFT) |                             \
        ((u32)(wordcnt) << REG_MI_DMA3CNT_WORDCNT_SHIFT))
#endif

#define REG_MI_NDMAGCNT_ARBITER_SHIFT 31
#define REG_MI_NDMAGCNT_ARBITER_SIZE 1
#define REG_MI_NDMAGCNT_ARBITER_MASK 0x80000000

#define REG_MI_NDMAGCNT_CPUCYCLE_SHIFT 16
#define REG_MI_NDMAGCNT_CPUCYCLE_SIZE 4
#define REG_MI_NDMAGCNT_CPUCYCLE_MASK 0x000f0000

#ifndef SDK_ASM
#define REG_MI_NDMAGCNT_FIELD(arbiter, cpucycle)                               \
  (u32)(((u32)(arbiter) << REG_MI_NDMAGCNT_ARBITER_SHIFT) |                    \
        ((u32)(cpucycle) << REG_MI_NDMAGCNT_CPUCYCLE_SHIFT))
#endif

#define REG_MI_NDMA0SAD_DMASRC_SHIFT 2
#define REG_MI_NDMA0SAD_DMASRC_SIZE 30
#define REG_MI_NDMA0SAD_DMASRC_MASK 0xfffffffc

#ifndef SDK_ASM
#define REG_MI_NDMA0SAD_FIELD(dmasrc)                                          \
  (u32)(((u32)(dmasrc) << REG_MI_NDMA0SAD_DMASRC_SHIFT))
#endif

#define REG_MI_NDAM0DAD_DMADEST_SHIFT 2
#define REG_MI_NDAM0DAD_DMADEST_SIZE 30
#define REG_MI_NDAM0DAD_DMADEST_MASK 0xfffffffc

#ifndef SDK_ASM
#define REG_MI_NDAM0DAD_FIELD(dmadest)                                         \
  (u32)(((u32)(dmadest) << REG_MI_NDAM0DAD_DMADEST_SHIFT))
#endif

#define REG_MI_NDMA0TCNT_TOTALCNT_SHIFT 0
#define REG_MI_NDMA0TCNT_TOTALCNT_SIZE 28
#define REG_MI_NDMA0TCNT_TOTALCNT_MASK 0x0fffffff

#ifndef SDK_ASM
#define REG_MI_NDMA0TCNT_FIELD(totalcnt)                                       \
  (u32)(((u32)(totalcnt) << REG_MI_NDMA0TCNT_TOTALCNT_SHIFT))
#endif

#define REG_MI_NDMA0WCNT_WORDCNT_SHIFT 0
#define REG_MI_NDMA0WCNT_WORDCNT_SIZE 24
#define REG_MI_NDMA0WCNT_WORDCNT_MASK 0x00ffffff

#ifndef SDK_ASM
#define REG_MI_NDMA0WCNT_FIELD(wordcnt)                                        \
  (u32)(((u32)(wordcnt) << REG_MI_NDMA0WCNT_WORDCNT_SHIFT))
#endif

#define REG_MI_NDMA0BCNT_PS_SHIFT 16
#define REG_MI_NDMA0BCNT_PS_SIZE 2
#define REG_MI_NDMA0BCNT_PS_MASK 0x00030000

#define REG_MI_NDMA0BCNT_ICNT_SHIFT 0
#define REG_MI_NDMA0BCNT_ICNT_SIZE 16
#define REG_MI_NDMA0BCNT_ICNT_MASK 0x0000ffff

#ifndef SDK_ASM
#define REG_MI_NDMA0BCNT_FIELD(ps, icnt)                                       \
  (u32)(((u32)(ps) << REG_MI_NDMA0BCNT_PS_SHIFT) |                             \
        ((u32)(icnt) << REG_MI_NDMA0BCNT_ICNT_SHIFT))
#endif

#define REG_MI_NDMA0FDATA_FDATA_SHIFT 0
#define REG_MI_NDMA0FDATA_FDATA_SIZE 32
#define REG_MI_NDMA0FDATA_FDATA_MASK 0xffffffff

#ifndef SDK_ASM
#define REG_MI_NDMA0FDATA_FIELD(fdata)                                         \
  (u32)(((u32)(fdata) << REG_MI_NDMA0FDATA_FDATA_SHIFT))
#endif

#define REG_MI_NDMA0CNT_E_SHIFT 31
#define REG_MI_NDMA0CNT_E_SIZE 1
#define REG_MI_NDMA0CNT_E_MASK 0x80000000

#define REG_MI_NDMA0CNT_I_SHIFT 30
#define REG_MI_NDMA0CNT_I_SIZE 1
#define REG_MI_NDMA0CNT_I_MASK 0x40000000

#define REG_MI_NDMA0CNT_CM_SHIFT 29
#define REG_MI_NDMA0CNT_CM_SIZE 1
#define REG_MI_NDMA0CNT_CM_MASK 0x20000000

#define REG_MI_NDMA0CNT_IM_SHIFT 28
#define REG_MI_NDMA0CNT_IM_SIZE 1
#define REG_MI_NDMA0CNT_IM_MASK 0x10000000

#define REG_MI_NDMA0CNT_MODE_SHIFT 24
#define REG_MI_NDMA0CNT_MODE_SIZE 4
#define REG_MI_NDMA0CNT_MODE_MASK 0x0f000000

#define REG_MI_NDMA0CNT_WORDCNT_SHIFT 16
#define REG_MI_NDMA0CNT_WORDCNT_SIZE 4
#define REG_MI_NDMA0CNT_WORDCNT_MASK 0x000f0000

#define REG_MI_NDMA0CNT_SRL_SHIFT 15
#define REG_MI_NDMA0CNT_SRL_SIZE 1
#define REG_MI_NDMA0CNT_SRL_MASK 0x00008000

#define REG_MI_NDMA0CNT_SAR_SHIFT 13
#define REG_MI_NDMA0CNT_SAR_SIZE 2
#define REG_MI_NDMA0CNT_SAR_MASK 0x00006000

#define REG_MI_NDMA0CNT_DRL_SHIFT 12
#define REG_MI_NDMA0CNT_DRL_SIZE 1
#define REG_MI_NDMA0CNT_DRL_MASK 0x00001000

#define REG_MI_NDMA0CNT_DAR_SHIFT 10
#define REG_MI_NDMA0CNT_DAR_SIZE 2
#define REG_MI_NDMA0CNT_DAR_MASK 0x00000c00

#ifndef SDK_ASM
#define REG_MI_NDMA0CNT_FIELD(e, i, cm, im, mode, wordcnt, srl, sar, drl, dar) \
  (u32)(((u32)(e) << REG_MI_NDMA0CNT_E_SHIFT) |                                \
        ((u32)(i) << REG_MI_NDMA0CNT_I_SHIFT) |                                \
        ((u32)(cm) << REG_MI_NDMA0CNT_CM_SHIFT) |                              \
        ((u32)(im) << REG_MI_NDMA0CNT_IM_SHIFT) |                              \
        ((u32)(mode) << REG_MI_NDMA0CNT_MODE_SHIFT) |                          \
        ((u32)(wordcnt) << REG_MI_NDMA0CNT_WORDCNT_SHIFT) |                    \
        ((u32)(srl) << REG_MI_NDMA0CNT_SRL_SHIFT) |                            \
        ((u32)(sar) << REG_MI_NDMA0CNT_SAR_SHIFT) |                            \
        ((u32)(drl) << REG_MI_NDMA0CNT_DRL_SHIFT) |                            \
        ((u32)(dar) << REG_MI_NDMA0CNT_DAR_SHIFT))
#endif

#define REG_MI_NDMA1SAD_DMASRC_SHIFT 2
#define REG_MI_NDMA1SAD_DMASRC_SIZE 30
#define REG_MI_NDMA1SAD_DMASRC_MASK 0xfffffffc

#ifndef SDK_ASM
#define REG_MI_NDMA1SAD_FIELD(dmasrc)                                          \
  (u32)(((u32)(dmasrc) << REG_MI_NDMA1SAD_DMASRC_SHIFT))
#endif

#define REG_MI_NDAM1DAD_DMADEST_SHIFT 2
#define REG_MI_NDAM1DAD_DMADEST_SIZE 30
#define REG_MI_NDAM1DAD_DMADEST_MASK 0xfffffffc

#ifndef SDK_ASM
#define REG_MI_NDAM1DAD_FIELD(dmadest)                                         \
  (u32)(((u32)(dmadest) << REG_MI_NDAM1DAD_DMADEST_SHIFT))
#endif

#define REG_MI_NDMA1TCNT_TOTALCNT_SHIFT 0
#define REG_MI_NDMA1TCNT_TOTALCNT_SIZE 28
#define REG_MI_NDMA1TCNT_TOTALCNT_MASK 0x0fffffff

#ifndef SDK_ASM
#define REG_MI_NDMA1TCNT_FIELD(totalcnt)                                       \
  (u32)(((u32)(totalcnt) << REG_MI_NDMA1TCNT_TOTALCNT_SHIFT))
#endif

#define REG_MI_NDMA1WCNT_WORDCNT_SHIFT 0
#define REG_MI_NDMA1WCNT_WORDCNT_SIZE 24
#define REG_MI_NDMA1WCNT_WORDCNT_MASK 0x00ffffff

#ifndef SDK_ASM
#define REG_MI_NDMA1WCNT_FIELD(wordcnt)                                        \
  (u32)(((u32)(wordcnt) << REG_MI_NDMA1WCNT_WORDCNT_SHIFT))
#endif

#define REG_MI_NDMA1BCNT_PS_SHIFT 16
#define REG_MI_NDMA1BCNT_PS_SIZE 2
#define REG_MI_NDMA1BCNT_PS_MASK 0x00030000

#define REG_MI_NDMA1BCNT_ICNT_SHIFT 0
#define REG_MI_NDMA1BCNT_ICNT_SIZE 16
#define REG_MI_NDMA1BCNT_ICNT_MASK 0x0000ffff

#ifndef SDK_ASM
#define REG_MI_NDMA1BCNT_FIELD(ps, icnt)                                       \
  (u32)(((u32)(ps) << REG_MI_NDMA1BCNT_PS_SHIFT) |                             \
        ((u32)(icnt) << REG_MI_NDMA1BCNT_ICNT_SHIFT))
#endif

#define REG_MI_NDMA1FDATA_FDATA_SHIFT 0
#define REG_MI_NDMA1FDATA_FDATA_SIZE 32
#define REG_MI_NDMA1FDATA_FDATA_MASK 0xffffffff

#ifndef SDK_ASM
#define REG_MI_NDMA1FDATA_FIELD(fdata)                                         \
  (u32)(((u32)(fdata) << REG_MI_NDMA1FDATA_FDATA_SHIFT))
#endif

#define REG_MI_NDMA1CNT_E_SHIFT 31
#define REG_MI_NDMA1CNT_E_SIZE 1
#define REG_MI_NDMA1CNT_E_MASK 0x80000000

#define REG_MI_NDMA1CNT_I_SHIFT 30
#define REG_MI_NDMA1CNT_I_SIZE 1
#define REG_MI_NDMA1CNT_I_MASK 0x40000000

#define REG_MI_NDMA1CNT_CM_SHIFT 29
#define REG_MI_NDMA1CNT_CM_SIZE 1
#define REG_MI_NDMA1CNT_CM_MASK 0x20000000

#define REG_MI_NDMA1CNT_IM_SHIFT 28
#define REG_MI_NDMA1CNT_IM_SIZE 1
#define REG_MI_NDMA1CNT_IM_MASK 0x10000000

#define REG_MI_NDMA1CNT_MODE_SHIFT 24
#define REG_MI_NDMA1CNT_MODE_SIZE 4
#define REG_MI_NDMA1CNT_MODE_MASK 0x0f000000

#define REG_MI_NDMA1CNT_WORDCNT_SHIFT 16
#define REG_MI_NDMA1CNT_WORDCNT_SIZE 4
#define REG_MI_NDMA1CNT_WORDCNT_MASK 0x000f0000

#define REG_MI_NDMA1CNT_SRL_SHIFT 15
#define REG_MI_NDMA1CNT_SRL_SIZE 1
#define REG_MI_NDMA1CNT_SRL_MASK 0x00008000

#define REG_MI_NDMA1CNT_SAR_SHIFT 13
#define REG_MI_NDMA1CNT_SAR_SIZE 2
#define REG_MI_NDMA1CNT_SAR_MASK 0x00006000

#define REG_MI_NDMA1CNT_DRL_SHIFT 12
#define REG_MI_NDMA1CNT_DRL_SIZE 1
#define REG_MI_NDMA1CNT_DRL_MASK 0x00001000

#define REG_MI_NDMA1CNT_DAR_SHIFT 10
#define REG_MI_NDMA1CNT_DAR_SIZE 2
#define REG_MI_NDMA1CNT_DAR_MASK 0x00000c00

#ifndef SDK_ASM
#define REG_MI_NDMA1CNT_FIELD(e, i, cm, im, mode, wordcnt, srl, sar, drl, dar) \
  (u32)(((u32)(e) << REG_MI_NDMA1CNT_E_SHIFT) |                                \
        ((u32)(i) << REG_MI_NDMA1CNT_I_SHIFT) |                                \
        ((u32)(cm) << REG_MI_NDMA1CNT_CM_SHIFT) |                              \
        ((u32)(im) << REG_MI_NDMA1CNT_IM_SHIFT) |                              \
        ((u32)(mode) << REG_MI_NDMA1CNT_MODE_SHIFT) |                          \
        ((u32)(wordcnt) << REG_MI_NDMA1CNT_WORDCNT_SHIFT) |                    \
        ((u32)(srl) << REG_MI_NDMA1CNT_SRL_SHIFT) |                            \
        ((u32)(sar) << REG_MI_NDMA1CNT_SAR_SHIFT) |                            \
        ((u32)(drl) << REG_MI_NDMA1CNT_DRL_SHIFT) |                            \
        ((u32)(dar) << REG_MI_NDMA1CNT_DAR_SHIFT))
#endif

#define REG_MI_NDMA2SAD_DMASRC_SHIFT 2
#define REG_MI_NDMA2SAD_DMASRC_SIZE 30
#define REG_MI_NDMA2SAD_DMASRC_MASK 0xfffffffc

#ifndef SDK_ASM
#define REG_MI_NDMA2SAD_FIELD(dmasrc)                                          \
  (u32)(((u32)(dmasrc) << REG_MI_NDMA2SAD_DMASRC_SHIFT))
#endif

#define REG_MI_NDAM2DAD_DMADEST_SHIFT 2
#define REG_MI_NDAM2DAD_DMADEST_SIZE 30
#define REG_MI_NDAM2DAD_DMADEST_MASK 0xfffffffc

#ifndef SDK_ASM
#define REG_MI_NDAM2DAD_FIELD(dmadest)                                         \
  (u32)(((u32)(dmadest) << REG_MI_NDAM2DAD_DMADEST_SHIFT))
#endif

#define REG_MI_NDMA2TCNT_TOTALCNT_SHIFT 0
#define REG_MI_NDMA2TCNT_TOTALCNT_SIZE 28
#define REG_MI_NDMA2TCNT_TOTALCNT_MASK 0x0fffffff

#ifndef SDK_ASM
#define REG_MI_NDMA2TCNT_FIELD(totalcnt)                                       \
  (u32)(((u32)(totalcnt) << REG_MI_NDMA2TCNT_TOTALCNT_SHIFT))
#endif

#define REG_MI_NDMA2WCNT_WORDCNT_SHIFT 0
#define REG_MI_NDMA2WCNT_WORDCNT_SIZE 24
#define REG_MI_NDMA2WCNT_WORDCNT_MASK 0x00ffffff

#ifndef SDK_ASM
#define REG_MI_NDMA2WCNT_FIELD(wordcnt)                                        \
  (u32)(((u32)(wordcnt) << REG_MI_NDMA2WCNT_WORDCNT_SHIFT))
#endif

#define REG_MI_NDMA2BCNT_PS_SHIFT 16
#define REG_MI_NDMA2BCNT_PS_SIZE 2
#define REG_MI_NDMA2BCNT_PS_MASK 0x00030000

#define REG_MI_NDMA2BCNT_ICNT_SHIFT 0
#define REG_MI_NDMA2BCNT_ICNT_SIZE 16
#define REG_MI_NDMA2BCNT_ICNT_MASK 0x0000ffff

#ifndef SDK_ASM
#define REG_MI_NDMA2BCNT_FIELD(ps, icnt)                                       \
  (u32)(((u32)(ps) << REG_MI_NDMA2BCNT_PS_SHIFT) |                             \
        ((u32)(icnt) << REG_MI_NDMA2BCNT_ICNT_SHIFT))
#endif

#define REG_MI_NDMA2FDATA_FDATA_SHIFT 0
#define REG_MI_NDMA2FDATA_FDATA_SIZE 32
#define REG_MI_NDMA2FDATA_FDATA_MASK 0xffffffff

#ifndef SDK_ASM
#define REG_MI_NDMA2FDATA_FIELD(fdata)                                         \
  (u32)(((u32)(fdata) << REG_MI_NDMA2FDATA_FDATA_SHIFT))
#endif

#define REG_MI_NDMA2CNT_E_SHIFT 31
#define REG_MI_NDMA2CNT_E_SIZE 1
#define REG_MI_NDMA2CNT_E_MASK 0x80000000

#define REG_MI_NDMA2CNT_I_SHIFT 30
#define REG_MI_NDMA2CNT_I_SIZE 1
#define REG_MI_NDMA2CNT_I_MASK 0x40000000

#define REG_MI_NDMA2CNT_CM_SHIFT 29
#define REG_MI_NDMA2CNT_CM_SIZE 1
#define REG_MI_NDMA2CNT_CM_MASK 0x20000000

#define REG_MI_NDMA2CNT_IM_SHIFT 28
#define REG_MI_NDMA2CNT_IM_SIZE 1
#define REG_MI_NDMA2CNT_IM_MASK 0x10000000

#define REG_MI_NDMA2CNT_MODE_SHIFT 24
#define REG_MI_NDMA2CNT_MODE_SIZE 4
#define REG_MI_NDMA2CNT_MODE_MASK 0x0f000000

#define REG_MI_NDMA2CNT_WORDCNT_SHIFT 16
#define REG_MI_NDMA2CNT_WORDCNT_SIZE 4
#define REG_MI_NDMA2CNT_WORDCNT_MASK 0x000f0000

#define REG_MI_NDMA2CNT_SRL_SHIFT 15
#define REG_MI_NDMA2CNT_SRL_SIZE 1
#define REG_MI_NDMA2CNT_SRL_MASK 0x00008000

#define REG_MI_NDMA2CNT_SAR_SHIFT 13
#define REG_MI_NDMA2CNT_SAR_SIZE 2
#define REG_MI_NDMA2CNT_SAR_MASK 0x00006000

#define REG_MI_NDMA2CNT_DRL_SHIFT 12
#define REG_MI_NDMA2CNT_DRL_SIZE 1
#define REG_MI_NDMA2CNT_DRL_MASK 0x00001000

#define REG_MI_NDMA2CNT_DAR_SHIFT 10
#define REG_MI_NDMA2CNT_DAR_SIZE 2
#define REG_MI_NDMA2CNT_DAR_MASK 0x00000c00

#ifndef SDK_ASM
#define REG_MI_NDMA2CNT_FIELD(e, i, cm, im, mode, wordcnt, srl, sar, drl, dar) \
  (u32)(((u32)(e) << REG_MI_NDMA2CNT_E_SHIFT) |                                \
        ((u32)(i) << REG_MI_NDMA2CNT_I_SHIFT) |                                \
        ((u32)(cm) << REG_MI_NDMA2CNT_CM_SHIFT) |                              \
        ((u32)(im) << REG_MI_NDMA2CNT_IM_SHIFT) |                              \
        ((u32)(mode) << REG_MI_NDMA2CNT_MODE_SHIFT) |                          \
        ((u32)(wordcnt) << REG_MI_NDMA2CNT_WORDCNT_SHIFT) |                    \
        ((u32)(srl) << REG_MI_NDMA2CNT_SRL_SHIFT) |                            \
        ((u32)(sar) << REG_MI_NDMA2CNT_SAR_SHIFT) |                            \
        ((u32)(drl) << REG_MI_NDMA2CNT_DRL_SHIFT) |                            \
        ((u32)(dar) << REG_MI_NDMA2CNT_DAR_SHIFT))
#endif

#define REG_MI_NDMA3SAD_DMASRC_SHIFT 2
#define REG_MI_NDMA3SAD_DMASRC_SIZE 30
#define REG_MI_NDMA3SAD_DMASRC_MASK 0xfffffffc

#ifndef SDK_ASM
#define REG_MI_NDMA3SAD_FIELD(dmasrc)                                          \
  (u32)(((u32)(dmasrc) << REG_MI_NDMA3SAD_DMASRC_SHIFT))
#endif

#define REG_MI_NDAM3DAD_DMADEST_SHIFT 2
#define REG_MI_NDAM3DAD_DMADEST_SIZE 30
#define REG_MI_NDAM3DAD_DMADEST_MASK 0xfffffffc

#ifndef SDK_ASM
#define REG_MI_NDAM3DAD_FIELD(dmadest)                                         \
  (u32)(((u32)(dmadest) << REG_MI_NDAM3DAD_DMADEST_SHIFT))
#endif

#define REG_MI_NDMA3TCNT_TOTALCNT_SHIFT 0
#define REG_MI_NDMA3TCNT_TOTALCNT_SIZE 28
#define REG_MI_NDMA3TCNT_TOTALCNT_MASK 0x0fffffff

#ifndef SDK_ASM
#define REG_MI_NDMA3TCNT_FIELD(totalcnt)                                       \
  (u32)(((u32)(totalcnt) << REG_MI_NDMA3TCNT_TOTALCNT_SHIFT))
#endif

#define REG_MI_NDMA3WCNT_WORDCNT_SHIFT 0
#define REG_MI_NDMA3WCNT_WORDCNT_SIZE 24
#define REG_MI_NDMA3WCNT_WORDCNT_MASK 0x00ffffff

#ifndef SDK_ASM
#define REG_MI_NDMA3WCNT_FIELD(wordcnt)                                        \
  (u32)(((u32)(wordcnt) << REG_MI_NDMA3WCNT_WORDCNT_SHIFT))
#endif

#define REG_MI_NDMA3BCNT_PS_SHIFT 16
#define REG_MI_NDMA3BCNT_PS_SIZE 2
#define REG_MI_NDMA3BCNT_PS_MASK 0x00030000

#define REG_MI_NDMA3BCNT_ICNT_SHIFT 0
#define REG_MI_NDMA3BCNT_ICNT_SIZE 16
#define REG_MI_NDMA3BCNT_ICNT_MASK 0x0000ffff

#ifndef SDK_ASM
#define REG_MI_NDMA3BCNT_FIELD(ps, icnt)                                       \
  (u32)(((u32)(ps) << REG_MI_NDMA3BCNT_PS_SHIFT) |                             \
        ((u32)(icnt) << REG_MI_NDMA3BCNT_ICNT_SHIFT))
#endif

#define REG_MI_NDMA3FDATA_FDATA_SHIFT 0
#define REG_MI_NDMA3FDATA_FDATA_SIZE 32
#define REG_MI_NDMA3FDATA_FDATA_MASK 0xffffffff

#ifndef SDK_ASM
#define REG_MI_NDMA3FDATA_FIELD(fdata)                                         \
  (u32)(((u32)(fdata) << REG_MI_NDMA3FDATA_FDATA_SHIFT))
#endif

#define REG_MI_NDMA3CNT_E_SHIFT 31
#define REG_MI_NDMA3CNT_E_SIZE 1
#define REG_MI_NDMA3CNT_E_MASK 0x80000000

#define REG_MI_NDMA3CNT_I_SHIFT 30
#define REG_MI_NDMA3CNT_I_SIZE 1
#define REG_MI_NDMA3CNT_I_MASK 0x40000000

#define REG_MI_NDMA3CNT_CM_SHIFT 29
#define REG_MI_NDMA3CNT_CM_SIZE 1
#define REG_MI_NDMA3CNT_CM_MASK 0x20000000

#define REG_MI_NDMA3CNT_IM_SHIFT 28
#define REG_MI_NDMA3CNT_IM_SIZE 1
#define REG_MI_NDMA3CNT_IM_MASK 0x10000000

#define REG_MI_NDMA3CNT_MODE_SHIFT 24
#define REG_MI_NDMA3CNT_MODE_SIZE 4
#define REG_MI_NDMA3CNT_MODE_MASK 0x0f000000

#define REG_MI_NDMA3CNT_WORDCNT_SHIFT 16
#define REG_MI_NDMA3CNT_WORDCNT_SIZE 4
#define REG_MI_NDMA3CNT_WORDCNT_MASK 0x000f0000

#define REG_MI_NDMA3CNT_SRL_SHIFT 15
#define REG_MI_NDMA3CNT_SRL_SIZE 1
#define REG_MI_NDMA3CNT_SRL_MASK 0x00008000

#define REG_MI_NDMA3CNT_SAR_SHIFT 13
#define REG_MI_NDMA3CNT_SAR_SIZE 2
#define REG_MI_NDMA3CNT_SAR_MASK 0x00006000

#define REG_MI_NDMA3CNT_DRL_SHIFT 12
#define REG_MI_NDMA3CNT_DRL_SIZE 1
#define REG_MI_NDMA3CNT_DRL_MASK 0x00001000

#define REG_MI_NDMA3CNT_DAR_SHIFT 10
#define REG_MI_NDMA3CNT_DAR_SIZE 2
#define REG_MI_NDMA3CNT_DAR_MASK 0x00000c00

#ifndef SDK_ASM
#define REG_MI_NDMA3CNT_FIELD(e, i, cm, im, mode, wordcnt, srl, sar, drl, dar) \
  (u32)(((u32)(e) << REG_MI_NDMA3CNT_E_SHIFT) |                                \
        ((u32)(i) << REG_MI_NDMA3CNT_I_SHIFT) |                                \
        ((u32)(cm) << REG_MI_NDMA3CNT_CM_SHIFT) |                              \
        ((u32)(im) << REG_MI_NDMA3CNT_IM_SHIFT) |                              \
        ((u32)(mode) << REG_MI_NDMA3CNT_MODE_SHIFT) |                          \
        ((u32)(wordcnt) << REG_MI_NDMA3CNT_WORDCNT_SHIFT) |                    \
        ((u32)(srl) << REG_MI_NDMA3CNT_SRL_SHIFT) |                            \
        ((u32)(sar) << REG_MI_NDMA3CNT_SAR_SHIFT) |                            \
        ((u32)(drl) << REG_MI_NDMA3CNT_DRL_SHIFT) |                            \
        ((u32)(dar) << REG_MI_NDMA3CNT_DAR_SHIFT))
#endif

#define REG_MI_MCCNT0_E_SHIFT 15
#define REG_MI_MCCNT0_E_SIZE 1
#define REG_MI_MCCNT0_E_MASK 0x8000

#define REG_MI_MCCNT0_I_SHIFT 14
#define REG_MI_MCCNT0_I_SIZE 1
#define REG_MI_MCCNT0_I_MASK 0x4000

#define REG_MI_MCCNT0_SEL_SHIFT 13
#define REG_MI_MCCNT0_SEL_SIZE 1
#define REG_MI_MCCNT0_SEL_MASK 0x2000

#define REG_MI_MCCNT0_BUSY_SHIFT 7
#define REG_MI_MCCNT0_BUSY_SIZE 1
#define REG_MI_MCCNT0_BUSY_MASK 0x0080

#define REG_MI_MCCNT0_MODE_SHIFT 6
#define REG_MI_MCCNT0_MODE_SIZE 1
#define REG_MI_MCCNT0_MODE_MASK 0x0040

#define REG_MI_MCCNT0_BAUDRATE_SHIFT 0
#define REG_MI_MCCNT0_BAUDRATE_SIZE 2
#define REG_MI_MCCNT0_BAUDRATE_MASK 0x0003

#ifndef SDK_ASM
#define REG_MI_MCCNT0_FIELD(e, i, sel, busy, mode, baudrate)                   \
  (u16)(((u32)(e) << REG_MI_MCCNT0_E_SHIFT) |                                  \
        ((u32)(i) << REG_MI_MCCNT0_I_SHIFT) |                                  \
        ((u32)(sel) << REG_MI_MCCNT0_SEL_SHIFT) |                              \
        ((u32)(busy) << REG_MI_MCCNT0_BUSY_SHIFT) |                            \
        ((u32)(mode) << REG_MI_MCCNT0_MODE_SHIFT) |                            \
        ((u32)(baudrate) << REG_MI_MCCNT0_BAUDRATE_SHIFT))
#endif

#define REG_MI_MCD0_DATA_SHIFT 0
#define REG_MI_MCD0_DATA_SIZE 8
#define REG_MI_MCD0_DATA_MASK 0x00ff

#ifndef SDK_ASM
#define REG_MI_MCD0_FIELD(data) (u16)(((u32)(data) << REG_MI_MCD0_DATA_SHIFT))
#endif

#define REG_MI_MCCNT1_START_SHIFT 31
#define REG_MI_MCCNT1_START_SIZE 1
#define REG_MI_MCCNT1_START_MASK 0x80000000

#define REG_MI_MCCNT1_WR_SHIFT 30
#define REG_MI_MCCNT1_WR_SIZE 1
#define REG_MI_MCCNT1_WR_MASK 0x40000000

#define REG_MI_MCCNT1_RESB_SHIFT 29
#define REG_MI_MCCNT1_RESB_SIZE 1
#define REG_MI_MCCNT1_RESB_MASK 0x20000000

#define REG_MI_MCCNT1_RTM_SHIFT 28
#define REG_MI_MCCNT1_RTM_SIZE 1
#define REG_MI_MCCNT1_RTM_MASK 0x10000000

#define REG_MI_MCCNT1_CT_SHIFT 27
#define REG_MI_MCCNT1_CT_SIZE 1
#define REG_MI_MCCNT1_CT_MASK 0x08000000

#define REG_MI_MCCNT1_PC_SHIFT 24
#define REG_MI_MCCNT1_PC_SIZE 3
#define REG_MI_MCCNT1_PC_MASK 0x07000000

#define REG_MI_MCCNT1_RDY_SHIFT 23
#define REG_MI_MCCNT1_RDY_SIZE 1
#define REG_MI_MCCNT1_RDY_MASK 0x00800000

#define REG_MI_MCCNT1_L2_SHIFT 16
#define REG_MI_MCCNT1_L2_SIZE 6
#define REG_MI_MCCNT1_L2_MASK 0x003f0000

#define REG_MI_MCCNT1_SCR_SHIFT 15
#define REG_MI_MCCNT1_SCR_SIZE 1
#define REG_MI_MCCNT1_SCR_MASK 0x00008000

#define REG_MI_MCCNT1_SE_SHIFT 14
#define REG_MI_MCCNT1_SE_SIZE 1
#define REG_MI_MCCNT1_SE_MASK 0x00004000

#define REG_MI_MCCNT1_DS_SHIFT 13
#define REG_MI_MCCNT1_DS_SIZE 1
#define REG_MI_MCCNT1_DS_MASK 0x00002000

#define REG_MI_MCCNT1_L1_SHIFT 0
#define REG_MI_MCCNT1_L1_SIZE 13
#define REG_MI_MCCNT1_L1_MASK 0x00001fff

#ifndef SDK_ASM
#define REG_MI_MCCNT1_FIELD(start, wr, resb, rtm, ct, pc, rdy, l2, scr, se,    \
                            ds, l1)                                            \
  (u32)(((u32)(start) << REG_MI_MCCNT1_START_SHIFT) |                          \
        ((u32)(wr) << REG_MI_MCCNT1_WR_SHIFT) |                                \
        ((u32)(resb) << REG_MI_MCCNT1_RESB_SHIFT) |                            \
        ((u32)(rtm) << REG_MI_MCCNT1_RTM_SHIFT) |                              \
        ((u32)(ct) << REG_MI_MCCNT1_CT_SHIFT) |                                \
        ((u32)(pc) << REG_MI_MCCNT1_PC_SHIFT) |                                \
        ((u32)(rdy) << REG_MI_MCCNT1_RDY_SHIFT) |                              \
        ((u32)(l2) << REG_MI_MCCNT1_L2_SHIFT) |                                \
        ((u32)(scr) << REG_MI_MCCNT1_SCR_SHIFT) |                              \
        ((u32)(se) << REG_MI_MCCNT1_SE_SHIFT) |                                \
        ((u32)(ds) << REG_MI_MCCNT1_DS_SHIFT) |                                \
        ((u32)(l1) << REG_MI_MCCNT1_L1_SHIFT))
#endif

#define REG_MI_MCCMD0_CMD3_SHIFT 24
#define REG_MI_MCCMD0_CMD3_SIZE 8
#define REG_MI_MCCMD0_CMD3_MASK 0xff000000

#define REG_MI_MCCMD0_CMD2_SHIFT 16
#define REG_MI_MCCMD0_CMD2_SIZE 8
#define REG_MI_MCCMD0_CMD2_MASK 0x00ff0000

#define REG_MI_MCCMD0_CMD1_SHIFT 8
#define REG_MI_MCCMD0_CMD1_SIZE 8
#define REG_MI_MCCMD0_CMD1_MASK 0x0000ff00

#define REG_MI_MCCMD0_CMD0_SHIFT 0
#define REG_MI_MCCMD0_CMD0_SIZE 8
#define REG_MI_MCCMD0_CMD0_MASK 0x000000ff

#ifndef SDK_ASM
#define REG_MI_MCCMD0_FIELD(cmd3, cmd2, cmd1, cmd0)                            \
  (u32)(((u32)(cmd3) << REG_MI_MCCMD0_CMD3_SHIFT) |                            \
        ((u32)(cmd2) << REG_MI_MCCMD0_CMD2_SHIFT) |                            \
        ((u32)(cmd1) << REG_MI_MCCMD0_CMD1_SHIFT) |                            \
        ((u32)(cmd0) << REG_MI_MCCMD0_CMD0_SHIFT))
#endif

#define REG_MI_MCCMD1_CMD7_SHIFT 24
#define REG_MI_MCCMD1_CMD7_SIZE 8
#define REG_MI_MCCMD1_CMD7_MASK 0xff000000

#define REG_MI_MCCMD1_CMD6_SHIFT 16
#define REG_MI_MCCMD1_CMD6_SIZE 8
#define REG_MI_MCCMD1_CMD6_MASK 0x00ff0000

#define REG_MI_MCCMD1_CMD5_SHIFT 8
#define REG_MI_MCCMD1_CMD5_SIZE 8
#define REG_MI_MCCMD1_CMD5_MASK 0x0000ff00

#define REG_MI_MCCMD1_CMD4_SHIFT 0
#define REG_MI_MCCMD1_CMD4_SIZE 8
#define REG_MI_MCCMD1_CMD4_MASK 0x000000ff

#ifndef SDK_ASM
#define REG_MI_MCCMD1_FIELD(cmd7, cmd6, cmd5, cmd4)                            \
  (u32)(((u32)(cmd7) << REG_MI_MCCMD1_CMD7_SHIFT) |                            \
        ((u32)(cmd6) << REG_MI_MCCMD1_CMD6_SHIFT) |                            \
        ((u32)(cmd5) << REG_MI_MCCMD1_CMD5_SHIFT) |                            \
        ((u32)(cmd4) << REG_MI_MCCMD1_CMD4_SHIFT))
#endif

#define REG_MI_MCCNT0_A_E_SHIFT 15
#define REG_MI_MCCNT0_A_E_SIZE 1
#define REG_MI_MCCNT0_A_E_MASK 0x8000

#define REG_MI_MCCNT0_A_I_SHIFT 14
#define REG_MI_MCCNT0_A_I_SIZE 1
#define REG_MI_MCCNT0_A_I_MASK 0x4000

#define REG_MI_MCCNT0_A_SEL_SHIFT 13
#define REG_MI_MCCNT0_A_SEL_SIZE 1
#define REG_MI_MCCNT0_A_SEL_MASK 0x2000

#define REG_MI_MCCNT0_A_BUSY_SHIFT 7
#define REG_MI_MCCNT0_A_BUSY_SIZE 1
#define REG_MI_MCCNT0_A_BUSY_MASK 0x0080

#define REG_MI_MCCNT0_A_MODE_SHIFT 6
#define REG_MI_MCCNT0_A_MODE_SIZE 1
#define REG_MI_MCCNT0_A_MODE_MASK 0x0040

#define REG_MI_MCCNT0_A_BAUDRATE_SHIFT 0
#define REG_MI_MCCNT0_A_BAUDRATE_SIZE 2
#define REG_MI_MCCNT0_A_BAUDRATE_MASK 0x0003

#ifndef SDK_ASM
#define REG_MI_MCCNT0_A_FIELD(e, i, sel, busy, mode, baudrate)                 \
  (u16)(((u32)(e) << REG_MI_MCCNT0_A_E_SHIFT) |                                \
        ((u32)(i) << REG_MI_MCCNT0_A_I_SHIFT) |                                \
        ((u32)(sel) << REG_MI_MCCNT0_A_SEL_SHIFT) |                            \
        ((u32)(busy) << REG_MI_MCCNT0_A_BUSY_SHIFT) |                          \
        ((u32)(mode) << REG_MI_MCCNT0_A_MODE_SHIFT) |                          \
        ((u32)(baudrate) << REG_MI_MCCNT0_A_BAUDRATE_SHIFT))
#endif

#define REG_MI_MCD0_A_DATA_SHIFT 0
#define REG_MI_MCD0_A_DATA_SIZE 8
#define REG_MI_MCD0_A_DATA_MASK 0x00ff

#ifndef SDK_ASM
#define REG_MI_MCD0_A_FIELD(data)                                              \
  (u16)(((u32)(data) << REG_MI_MCD0_A_DATA_SHIFT))
#endif

#define REG_MI_MCD1_A_DATA_SHIFT 0
#define REG_MI_MCD1_A_DATA_SIZE 32
#define REG_MI_MCD1_A_DATA_MASK 0xffffffff

#ifndef SDK_ASM
#define REG_MI_MCD1_A_FIELD(data)                                              \
  (u32)(((u32)(data) << REG_MI_MCD1_A_DATA_SHIFT))
#endif

#define REG_MI_MCCNT1_A_START_SHIFT 31
#define REG_MI_MCCNT1_A_START_SIZE 1
#define REG_MI_MCCNT1_A_START_MASK 0x80000000

#define REG_MI_MCCNT1_A_WR_SHIFT 30
#define REG_MI_MCCNT1_A_WR_SIZE 1
#define REG_MI_MCCNT1_A_WR_MASK 0x40000000

#define REG_MI_MCCNT1_A_RESB_SHIFT 29
#define REG_MI_MCCNT1_A_RESB_SIZE 1
#define REG_MI_MCCNT1_A_RESB_MASK 0x20000000

#define REG_MI_MCCNT1_A_TRM_SHIFT 28
#define REG_MI_MCCNT1_A_TRM_SIZE 1
#define REG_MI_MCCNT1_A_TRM_MASK 0x10000000

#define REG_MI_MCCNT1_A_CT_SHIFT 27
#define REG_MI_MCCNT1_A_CT_SIZE 1
#define REG_MI_MCCNT1_A_CT_MASK 0x08000000

#define REG_MI_MCCNT1_A_PC_SHIFT 24
#define REG_MI_MCCNT1_A_PC_SIZE 3
#define REG_MI_MCCNT1_A_PC_MASK 0x07000000

#define REG_MI_MCCNT1_A_RDY_SHIFT 23
#define REG_MI_MCCNT1_A_RDY_SIZE 1
#define REG_MI_MCCNT1_A_RDY_MASK 0x00800000

#define REG_MI_MCCNT1_A_CS_SHIFT 22
#define REG_MI_MCCNT1_A_CS_SIZE 1
#define REG_MI_MCCNT1_A_CS_MASK 0x00400000

#define REG_MI_MCCNT1_A_L2_SHIFT 16
#define REG_MI_MCCNT1_A_L2_SIZE 6
#define REG_MI_MCCNT1_A_L2_MASK 0x003f0000

#define REG_MI_MCCNT1_A_SCR_SHIFT 15
#define REG_MI_MCCNT1_A_SCR_SIZE 1
#define REG_MI_MCCNT1_A_SCR_MASK 0x00008000

#define REG_MI_MCCNT1_A_SE_SHIFT 14
#define REG_MI_MCCNT1_A_SE_SIZE 1
#define REG_MI_MCCNT1_A_SE_MASK 0x00004000

#define REG_MI_MCCNT1_A_DS_SHIFT 13
#define REG_MI_MCCNT1_A_DS_SIZE 1
#define REG_MI_MCCNT1_A_DS_MASK 0x00002000

#define REG_MI_MCCNT1_A_L1_SHIFT 0
#define REG_MI_MCCNT1_A_L1_SIZE 13
#define REG_MI_MCCNT1_A_L1_MASK 0x00001fff

#ifndef SDK_ASM
#define REG_MI_MCCNT1_A_FIELD(start, wr, resb, trm, ct, pc, rdy, cs, l2, scr,  \
                              se, ds, l1)                                      \
  (u32)(((u32)(start) << REG_MI_MCCNT1_A_START_SHIFT) |                        \
        ((u32)(wr) << REG_MI_MCCNT1_A_WR_SHIFT) |                              \
        ((u32)(resb) << REG_MI_MCCNT1_A_RESB_SHIFT) |                          \
        ((u32)(trm) << REG_MI_MCCNT1_A_TRM_SHIFT) |                            \
        ((u32)(ct) << REG_MI_MCCNT1_A_CT_SHIFT) |                              \
        ((u32)(pc) << REG_MI_MCCNT1_A_PC_SHIFT) |                              \
        ((u32)(rdy) << REG_MI_MCCNT1_A_RDY_SHIFT) |                            \
        ((u32)(cs) << REG_MI_MCCNT1_A_CS_SHIFT) |                              \
        ((u32)(l2) << REG_MI_MCCNT1_A_L2_SHIFT) |                              \
        ((u32)(scr) << REG_MI_MCCNT1_A_SCR_SHIFT) |                            \
        ((u32)(se) << REG_MI_MCCNT1_A_SE_SHIFT) |                              \
        ((u32)(ds) << REG_MI_MCCNT1_A_DS_SHIFT) |                              \
        ((u32)(l1) << REG_MI_MCCNT1_A_L1_SHIFT))
#endif

#define REG_MI_MCCMD0_A_CMD3_SHIFT 24
#define REG_MI_MCCMD0_A_CMD3_SIZE 8
#define REG_MI_MCCMD0_A_CMD3_MASK 0xff000000

#define REG_MI_MCCMD0_A_CMD2_SHIFT 16
#define REG_MI_MCCMD0_A_CMD2_SIZE 8
#define REG_MI_MCCMD0_A_CMD2_MASK 0x00ff0000

#define REG_MI_MCCMD0_A_CMD1_SHIFT 8
#define REG_MI_MCCMD0_A_CMD1_SIZE 8
#define REG_MI_MCCMD0_A_CMD1_MASK 0x0000ff00

#define REG_MI_MCCMD0_A_CMD0_SHIFT 0
#define REG_MI_MCCMD0_A_CMD0_SIZE 8
#define REG_MI_MCCMD0_A_CMD0_MASK 0x000000ff

#ifndef SDK_ASM
#define REG_MI_MCCMD0_A_FIELD(cmd3, cmd2, cmd1, cmd0)                          \
  (u32)(((u32)(cmd3) << REG_MI_MCCMD0_A_CMD3_SHIFT) |                          \
        ((u32)(cmd2) << REG_MI_MCCMD0_A_CMD2_SHIFT) |                          \
        ((u32)(cmd1) << REG_MI_MCCMD0_A_CMD1_SHIFT) |                          \
        ((u32)(cmd0) << REG_MI_MCCMD0_A_CMD0_SHIFT))
#endif

#define REG_MI_MCCMD1_A_CMD7_SHIFT 24
#define REG_MI_MCCMD1_A_CMD7_SIZE 8
#define REG_MI_MCCMD1_A_CMD7_MASK 0xff000000

#define REG_MI_MCCMD1_A_CMD6_SHIFT 16
#define REG_MI_MCCMD1_A_CMD6_SIZE 8
#define REG_MI_MCCMD1_A_CMD6_MASK 0x00ff0000

#define REG_MI_MCCMD1_A_CMD5_SHIFT 8
#define REG_MI_MCCMD1_A_CMD5_SIZE 8
#define REG_MI_MCCMD1_A_CMD5_MASK 0x0000ff00

#define REG_MI_MCCMD1_A_CMD4_SHIFT 0
#define REG_MI_MCCMD1_A_CMD4_SIZE 8
#define REG_MI_MCCMD1_A_CMD4_MASK 0x000000ff

#ifndef SDK_ASM
#define REG_MI_MCCMD1_A_FIELD(cmd7, cmd6, cmd5, cmd4)                          \
  (u32)(((u32)(cmd7) << REG_MI_MCCMD1_A_CMD7_SHIFT) |                          \
        ((u32)(cmd6) << REG_MI_MCCMD1_A_CMD6_SHIFT) |                          \
        ((u32)(cmd5) << REG_MI_MCCMD1_A_CMD5_SHIFT) |                          \
        ((u32)(cmd4) << REG_MI_MCCMD1_A_CMD4_SHIFT))
#endif

#define REG_MI_MCSRC0_A_SCRA_SHIFT 0
#define REG_MI_MCSRC0_A_SCRA_SIZE 32
#define REG_MI_MCSRC0_A_SCRA_MASK 0xffffffff

#ifndef SDK_ASM
#define REG_MI_MCSRC0_A_FIELD(scra)                                            \
  (u32)(((u32)(scra) << REG_MI_MCSRC0_A_SCRA_SHIFT))
#endif

#define REG_MI_MCSRC1_A_SCRB_SHIFT 0
#define REG_MI_MCSRC1_A_SCRB_SIZE 32
#define REG_MI_MCSRC1_A_SCRB_MASK 0xffffffff

#ifndef SDK_ASM
#define REG_MI_MCSRC1_A_FIELD(scrb)                                            \
  (u32)(((u32)(scrb) << REG_MI_MCSRC1_A_SCRB_SHIFT))
#endif

#define REG_MI_MCSRC2_A_SCRB_SHIFT 26
#define REG_MI_MCSRC2_A_SCRB_SIZE 7
#define REG_MI_MCSRC2_A_SCRB_MASK 0x1fc000000

#define REG_MI_MCSRC2_A_SCRA_SHIFT 0
#define REG_MI_MCSRC2_A_SCRA_SIZE 7
#define REG_MI_MCSRC2_A_SCRA_MASK 0x0000007f

#ifndef SDK_ASM
#define REG_MI_MCSRC2_A_FIELD(scrb, scra)                                      \
  (u32)(((u32)(scrb) << REG_MI_MCSRC2_A_SCRB_SHIFT) |                          \
        ((u32)(scra) << REG_MI_MCSRC2_A_SCRA_SHIFT))
#endif

#define REG_MI_MCCNT0_B_E_SHIFT 15
#define REG_MI_MCCNT0_B_E_SIZE 1
#define REG_MI_MCCNT0_B_E_MASK 0x8000

#define REG_MI_MCCNT0_B_I_SHIFT 14
#define REG_MI_MCCNT0_B_I_SIZE 1
#define REG_MI_MCCNT0_B_I_MASK 0x4000

#define REG_MI_MCCNT0_B_SEL_SHIFT 13
#define REG_MI_MCCNT0_B_SEL_SIZE 1
#define REG_MI_MCCNT0_B_SEL_MASK 0x2000

#define REG_MI_MCCNT0_B_BUSY_SHIFT 7
#define REG_MI_MCCNT0_B_BUSY_SIZE 1
#define REG_MI_MCCNT0_B_BUSY_MASK 0x0080

#define REG_MI_MCCNT0_B_MODE_SHIFT 6
#define REG_MI_MCCNT0_B_MODE_SIZE 1
#define REG_MI_MCCNT0_B_MODE_MASK 0x0040

#define REG_MI_MCCNT0_B_BAUDRATE_SHIFT 0
#define REG_MI_MCCNT0_B_BAUDRATE_SIZE 2
#define REG_MI_MCCNT0_B_BAUDRATE_MASK 0x0003

#ifndef SDK_ASM
#define REG_MI_MCCNT0_B_FIELD(e, i, sel, busy, mode, baudrate)                 \
  (u16)(((u32)(e) << REG_MI_MCCNT0_B_E_SHIFT) |                                \
        ((u32)(i) << REG_MI_MCCNT0_B_I_SHIFT) |                                \
        ((u32)(sel) << REG_MI_MCCNT0_B_SEL_SHIFT) |                            \
        ((u32)(busy) << REG_MI_MCCNT0_B_BUSY_SHIFT) |                          \
        ((u32)(mode) << REG_MI_MCCNT0_B_MODE_SHIFT) |                          \
        ((u32)(baudrate) << REG_MI_MCCNT0_B_BAUDRATE_SHIFT))
#endif

#define REG_MI_MCD0_B_DATA_SHIFT 0
#define REG_MI_MCD0_B_DATA_SIZE 8
#define REG_MI_MCD0_B_DATA_MASK 0x00ff

#ifndef SDK_ASM
#define REG_MI_MCD0_B_FIELD(data)                                              \
  (u16)(((u32)(data) << REG_MI_MCD0_B_DATA_SHIFT))
#endif

#define REG_MI_MCD1_B_DATA_SHIFT 0
#define REG_MI_MCD1_B_DATA_SIZE 32
#define REG_MI_MCD1_B_DATA_MASK 0xffffffff

#ifndef SDK_ASM
#define REG_MI_MCD1_B_FIELD(data)                                              \
  (u32)(((u32)(data) << REG_MI_MCD1_B_DATA_SHIFT))
#endif

#define REG_MI_MCCNT1_B_START_SHIFT 31
#define REG_MI_MCCNT1_B_START_SIZE 1
#define REG_MI_MCCNT1_B_START_MASK 0x80000000

#define REG_MI_MCCNT1_B_WR_SHIFT 30
#define REG_MI_MCCNT1_B_WR_SIZE 1
#define REG_MI_MCCNT1_B_WR_MASK 0x40000000

#define REG_MI_MCCNT1_B_RESB_SHIFT 29
#define REG_MI_MCCNT1_B_RESB_SIZE 1
#define REG_MI_MCCNT1_B_RESB_MASK 0x20000000

#define REG_MI_MCCNT1_B_TRM_SHIFT 28
#define REG_MI_MCCNT1_B_TRM_SIZE 1
#define REG_MI_MCCNT1_B_TRM_MASK 0x10000000

#define REG_MI_MCCNT1_B_CT_SHIFT 27
#define REG_MI_MCCNT1_B_CT_SIZE 1
#define REG_MI_MCCNT1_B_CT_MASK 0x08000000

#define REG_MI_MCCNT1_B_PC_SHIFT 24
#define REG_MI_MCCNT1_B_PC_SIZE 3
#define REG_MI_MCCNT1_B_PC_MASK 0x07000000

#define REG_MI_MCCNT1_B_RDY_SHIFT 23
#define REG_MI_MCCNT1_B_RDY_SIZE 1
#define REG_MI_MCCNT1_B_RDY_MASK 0x00800000

#define REG_MI_MCCNT1_B_CS_SHIFT 22
#define REG_MI_MCCNT1_B_CS_SIZE 1
#define REG_MI_MCCNT1_B_CS_MASK 0x00400000

#define REG_MI_MCCNT1_B_L2_SHIFT 16
#define REG_MI_MCCNT1_B_L2_SIZE 6
#define REG_MI_MCCNT1_B_L2_MASK 0x003f0000

#define REG_MI_MCCNT1_B_SCR_SHIFT 15
#define REG_MI_MCCNT1_B_SCR_SIZE 1
#define REG_MI_MCCNT1_B_SCR_MASK 0x00008000

#define REG_MI_MCCNT1_B_SE_SHIFT 14
#define REG_MI_MCCNT1_B_SE_SIZE 1
#define REG_MI_MCCNT1_B_SE_MASK 0x00004000

#define REG_MI_MCCNT1_B_DS_SHIFT 13
#define REG_MI_MCCNT1_B_DS_SIZE 1
#define REG_MI_MCCNT1_B_DS_MASK 0x00002000

#define REG_MI_MCCNT1_B_L1_SHIFT 0
#define REG_MI_MCCNT1_B_L1_SIZE 13
#define REG_MI_MCCNT1_B_L1_MASK 0x00001fff

#ifndef SDK_ASM
#define REG_MI_MCCNT1_B_FIELD(start, wr, resb, trm, ct, pc, rdy, cs, l2, scr,  \
                              se, ds, l1)                                      \
  (u32)(((u32)(start) << REG_MI_MCCNT1_B_START_SHIFT) |                        \
        ((u32)(wr) << REG_MI_MCCNT1_B_WR_SHIFT) |                              \
        ((u32)(resb) << REG_MI_MCCNT1_B_RESB_SHIFT) |                          \
        ((u32)(trm) << REG_MI_MCCNT1_B_TRM_SHIFT) |                            \
        ((u32)(ct) << REG_MI_MCCNT1_B_CT_SHIFT) |                              \
        ((u32)(pc) << REG_MI_MCCNT1_B_PC_SHIFT) |                              \
        ((u32)(rdy) << REG_MI_MCCNT1_B_RDY_SHIFT) |                            \
        ((u32)(cs) << REG_MI_MCCNT1_B_CS_SHIFT) |                              \
        ((u32)(l2) << REG_MI_MCCNT1_B_L2_SHIFT) |                              \
        ((u32)(scr) << REG_MI_MCCNT1_B_SCR_SHIFT) |                            \
        ((u32)(se) << REG_MI_MCCNT1_B_SE_SHIFT) |                              \
        ((u32)(ds) << REG_MI_MCCNT1_B_DS_SHIFT) |                              \
        ((u32)(l1) << REG_MI_MCCNT1_B_L1_SHIFT))
#endif

#define REG_MI_MCCMD0_B_CMD3_SHIFT 24
#define REG_MI_MCCMD0_B_CMD3_SIZE 8
#define REG_MI_MCCMD0_B_CMD3_MASK 0xff000000

#define REG_MI_MCCMD0_B_CMD2_SHIFT 16
#define REG_MI_MCCMD0_B_CMD2_SIZE 8
#define REG_MI_MCCMD0_B_CMD2_MASK 0x00ff0000

#define REG_MI_MCCMD0_B_CMD1_SHIFT 8
#define REG_MI_MCCMD0_B_CMD1_SIZE 8
#define REG_MI_MCCMD0_B_CMD1_MASK 0x0000ff00

#define REG_MI_MCCMD0_B_CMD0_SHIFT 0
#define REG_MI_MCCMD0_B_CMD0_SIZE 8
#define REG_MI_MCCMD0_B_CMD0_MASK 0x000000ff

#ifndef SDK_ASM
#define REG_MI_MCCMD0_B_FIELD(cmd3, cmd2, cmd1, cmd0)                          \
  (u32)(((u32)(cmd3) << REG_MI_MCCMD0_B_CMD3_SHIFT) |                          \
        ((u32)(cmd2) << REG_MI_MCCMD0_B_CMD2_SHIFT) |                          \
        ((u32)(cmd1) << REG_MI_MCCMD0_B_CMD1_SHIFT) |                          \
        ((u32)(cmd0) << REG_MI_MCCMD0_B_CMD0_SHIFT))
#endif

#define REG_MI_MCCMD1_B_CMD7_SHIFT 24
#define REG_MI_MCCMD1_B_CMD7_SIZE 8
#define REG_MI_MCCMD1_B_CMD7_MASK 0xff000000

#define REG_MI_MCCMD1_B_CMD6_SHIFT 16
#define REG_MI_MCCMD1_B_CMD6_SIZE 8
#define REG_MI_MCCMD1_B_CMD6_MASK 0x00ff0000

#define REG_MI_MCCMD1_B_CMD5_SHIFT 8
#define REG_MI_MCCMD1_B_CMD5_SIZE 8
#define REG_MI_MCCMD1_B_CMD5_MASK 0x0000ff00

#define REG_MI_MCCMD1_B_CMD4_SHIFT 0
#define REG_MI_MCCMD1_B_CMD4_SIZE 8
#define REG_MI_MCCMD1_B_CMD4_MASK 0x000000ff

#ifndef SDK_ASM
#define REG_MI_MCCMD1_B_FIELD(cmd7, cmd6, cmd5, cmd4)                          \
  (u32)(((u32)(cmd7) << REG_MI_MCCMD1_B_CMD7_SHIFT) |                          \
        ((u32)(cmd6) << REG_MI_MCCMD1_B_CMD6_SHIFT) |                          \
        ((u32)(cmd5) << REG_MI_MCCMD1_B_CMD5_SHIFT) |                          \
        ((u32)(cmd4) << REG_MI_MCCMD1_B_CMD4_SHIFT))
#endif

#define REG_MI_MCSRC0_B_SCRA_SHIFT 0
#define REG_MI_MCSRC0_B_SCRA_SIZE 32
#define REG_MI_MCSRC0_B_SCRA_MASK 0xffffffff

#ifndef SDK_ASM
#define REG_MI_MCSRC0_B_FIELD(scra)                                            \
  (u32)(((u32)(scra) << REG_MI_MCSRC0_B_SCRA_SHIFT))
#endif

#define REG_MI_MCSRC1_B_SCRB_SHIFT 0
#define REG_MI_MCSRC1_B_SCRB_SIZE 32
#define REG_MI_MCSRC1_B_SCRB_MASK 0xffffffff

#ifndef SDK_ASM
#define REG_MI_MCSRC1_B_FIELD(scrb)                                            \
  (u32)(((u32)(scrb) << REG_MI_MCSRC1_B_SCRB_SHIFT))
#endif

#define REG_MI_MCSRC2_B_SCRB_SHIFT 26
#define REG_MI_MCSRC2_B_SCRB_SIZE 7
#define REG_MI_MCSRC2_B_SCRB_MASK 0x1fc000000

#define REG_MI_MCSRC2_B_SCRA_SHIFT 0
#define REG_MI_MCSRC2_B_SCRA_SIZE 7
#define REG_MI_MCSRC2_B_SCRA_MASK 0x0000007f

#ifndef SDK_ASM
#define REG_MI_MCSRC2_B_FIELD(scrb, scra)                                      \
  (u32)(((u32)(scrb) << REG_MI_MCSRC2_B_SCRB_SHIFT) |                          \
        ((u32)(scra) << REG_MI_MCSRC2_B_SCRA_SHIFT))
#endif

#define REG_MI_EXMEMCNT_EP_SHIFT 15
#define REG_MI_EXMEMCNT_EP_SIZE 1
#define REG_MI_EXMEMCNT_EP_MASK 0x8000

#define REG_MI_EXMEMCNT_IFM_SHIFT 14
#define REG_MI_EXMEMCNT_IFM_SIZE 1
#define REG_MI_EXMEMCNT_IFM_MASK 0x4000

#define REG_MI_EXMEMCNT_CE2_SHIFT 13
#define REG_MI_EXMEMCNT_CE2_SIZE 1
#define REG_MI_EXMEMCNT_CE2_MASK 0x2000

#define REG_MI_EXMEMCNT_MPA_SHIFT 11
#define REG_MI_EXMEMCNT_MPA_SIZE 1
#define REG_MI_EXMEMCNT_MPA_MASK 0x0800

#define REG_MI_EXMEMCNT_MP_SHIFT 11
#define REG_MI_EXMEMCNT_MP_SIZE 1
#define REG_MI_EXMEMCNT_MP_MASK 0x0800

#define REG_MI_EXMEMCNT_MPB_SHIFT 10
#define REG_MI_EXMEMCNT_MPB_SIZE 1
#define REG_MI_EXMEMCNT_MPB_MASK 0x0400

#define REG_MI_EXMEMCNT_CP_SHIFT 7
#define REG_MI_EXMEMCNT_CP_SIZE 1
#define REG_MI_EXMEMCNT_CP_MASK 0x0080

#define REG_MI_EXMEMCNT_PHI_SHIFT 5
#define REG_MI_EXMEMCNT_PHI_SIZE 2
#define REG_MI_EXMEMCNT_PHI_MASK 0x0060

#define REG_MI_EXMEMCNT_ROM2nd_SHIFT 4
#define REG_MI_EXMEMCNT_ROM2nd_SIZE 1
#define REG_MI_EXMEMCNT_ROM2nd_MASK 0x0010

#define REG_MI_EXMEMCNT_ROM1st_SHIFT 2
#define REG_MI_EXMEMCNT_ROM1st_SIZE 2
#define REG_MI_EXMEMCNT_ROM1st_MASK 0x000c

#define REG_MI_EXMEMCNT_RAM_SHIFT 0
#define REG_MI_EXMEMCNT_RAM_SIZE 2
#define REG_MI_EXMEMCNT_RAM_MASK 0x0003

#ifndef SDK_ASM
#define REG_MI_EXMEMCNT_FIELD(ep, ifm, ce2, mpa, mp, mpb, cp, phi, rom2nd,     \
                              rom1st, ram)                                     \
  (u16)(((u32)(ep) << REG_MI_EXMEMCNT_EP_SHIFT) |                              \
        ((u32)(ifm) << REG_MI_EXMEMCNT_IFM_SHIFT) |                            \
        ((u32)(ce2) << REG_MI_EXMEMCNT_CE2_SHIFT) |                            \
        ((u32)(mpa) << REG_MI_EXMEMCNT_MPA_SHIFT) |                            \
        ((u32)(mp) << REG_MI_EXMEMCNT_MP_SHIFT) |                              \
        ((u32)(mpb) << REG_MI_EXMEMCNT_MPB_SHIFT) |                            \
        ((u32)(cp) << REG_MI_EXMEMCNT_CP_SHIFT) |                              \
        ((u32)(phi) << REG_MI_EXMEMCNT_PHI_SHIFT) |                            \
        ((u32)(rom2nd) << REG_MI_EXMEMCNT_ROM2nd_SHIFT) |                      \
        ((u32)(rom1st) << REG_MI_EXMEMCNT_ROM1st_SHIFT) |                      \
        ((u32)(ram) << REG_MI_EXMEMCNT_RAM_SHIFT))
#endif

#define REG_MI_MC_SWP_SHIFT 15
#define REG_MI_MC_SWP_SIZE 1
#define REG_MI_MC_SWP_MASK 0x8000

#define REG_MI_MC_SL2_MODE_SHIFT 6
#define REG_MI_MC_SL2_MODE_SIZE 2
#define REG_MI_MC_SL2_MODE_MASK 0x00c0

#define REG_MI_MC_SL2_CDET_SHIFT 4
#define REG_MI_MC_SL2_CDET_SIZE 1
#define REG_MI_MC_SL2_CDET_MASK 0x0010

#define REG_MI_MC_SL1_MODE_SHIFT 2
#define REG_MI_MC_SL1_MODE_SIZE 2
#define REG_MI_MC_SL1_MODE_MASK 0x000c

#define REG_MI_MC_SL1_CDET_SHIFT 0
#define REG_MI_MC_SL1_CDET_SIZE 1
#define REG_MI_MC_SL1_CDET_MASK 0x0001

#ifndef SDK_ASM
#define REG_MI_MC_FIELD(swp, sl2_mode, sl2_cdet, sl1_mode, sl1_cdet)           \
  (u16)(((u32)(swp) << REG_MI_MC_SWP_SHIFT) |                                  \
        ((u32)(sl2_mode) << REG_MI_MC_SL2_MODE_SHIFT) |                        \
        ((u32)(sl2_cdet) << REG_MI_MC_SL2_CDET_SHIFT) |                        \
        ((u32)(sl1_mode) << REG_MI_MC_SL1_MODE_SHIFT) |                        \
        ((u32)(sl1_cdet) << REG_MI_MC_SL1_CDET_SHIFT))
#endif

#define REG_MI_MBK1_WA3_E_SHIFT 31
#define REG_MI_MBK1_WA3_E_SIZE 1
#define REG_MI_MBK1_WA3_E_MASK 0x80000000

#define REG_MI_MBK1_WA3_OF_SHIFT 26
#define REG_MI_MBK1_WA3_OF_SIZE 2
#define REG_MI_MBK1_WA3_OF_MASK 0x0c000000

#define REG_MI_MBK1_WA3_M_SHIFT 24
#define REG_MI_MBK1_WA3_M_SIZE 1
#define REG_MI_MBK1_WA3_M_MASK 0x01000000

#define REG_MI_MBK1_WA2_E_SHIFT 23
#define REG_MI_MBK1_WA2_E_SIZE 1
#define REG_MI_MBK1_WA2_E_MASK 0x00800000

#define REG_MI_MBK1_WA2_OF_SHIFT 18
#define REG_MI_MBK1_WA2_OF_SIZE 2
#define REG_MI_MBK1_WA2_OF_MASK 0x000c0000

#define REG_MI_MBK1_WA2_M_SHIFT 16
#define REG_MI_MBK1_WA2_M_SIZE 1
#define REG_MI_MBK1_WA2_M_MASK 0x00010000

#define REG_MI_MBK1_WA1_E_SHIFT 15
#define REG_MI_MBK1_WA1_E_SIZE 1
#define REG_MI_MBK1_WA1_E_MASK 0x00008000

#define REG_MI_MBK1_WA1_OF_SHIFT 10
#define REG_MI_MBK1_WA1_OF_SIZE 2
#define REG_MI_MBK1_WA1_OF_MASK 0x00000c00

#define REG_MI_MBK1_WA1_M_SHIFT 8
#define REG_MI_MBK1_WA1_M_SIZE 1
#define REG_MI_MBK1_WA1_M_MASK 0x00000100

#define REG_MI_MBK1_WA0_E_SHIFT 7
#define REG_MI_MBK1_WA0_E_SIZE 1
#define REG_MI_MBK1_WA0_E_MASK 0x00000080

#define REG_MI_MBK1_WA0_OF_SHIFT 2
#define REG_MI_MBK1_WA0_OF_SIZE 2
#define REG_MI_MBK1_WA0_OF_MASK 0x0000000c

#define REG_MI_MBK1_WA0_M_SHIFT 0
#define REG_MI_MBK1_WA0_M_SIZE 1
#define REG_MI_MBK1_WA0_M_MASK 0x00000001

#ifndef SDK_ASM
#define REG_MI_MBK1_FIELD(wa3_e, wa3_of, wa3_m, wa2_e, wa2_of, wa2_m, wa1_e,   \
                          wa1_of, wa1_m, wa0_e, wa0_of, wa0_m)                 \
  (u32)(((u32)(wa3_e) << REG_MI_MBK1_WA3_E_SHIFT) |                            \
        ((u32)(wa3_of) << REG_MI_MBK1_WA3_OF_SHIFT) |                          \
        ((u32)(wa3_m) << REG_MI_MBK1_WA3_M_SHIFT) |                            \
        ((u32)(wa2_e) << REG_MI_MBK1_WA2_E_SHIFT) |                            \
        ((u32)(wa2_of) << REG_MI_MBK1_WA2_OF_SHIFT) |                          \
        ((u32)(wa2_m) << REG_MI_MBK1_WA2_M_SHIFT) |                            \
        ((u32)(wa1_e) << REG_MI_MBK1_WA1_E_SHIFT) |                            \
        ((u32)(wa1_of) << REG_MI_MBK1_WA1_OF_SHIFT) |                          \
        ((u32)(wa1_m) << REG_MI_MBK1_WA1_M_SHIFT) |                            \
        ((u32)(wa0_e) << REG_MI_MBK1_WA0_E_SHIFT) |                            \
        ((u32)(wa0_of) << REG_MI_MBK1_WA0_OF_SHIFT) |                          \
        ((u32)(wa0_m) << REG_MI_MBK1_WA0_M_SHIFT))
#endif

#define REG_MI_MBK_A0_E_SHIFT 7
#define REG_MI_MBK_A0_E_SIZE 1
#define REG_MI_MBK_A0_E_MASK 0x80

#define REG_MI_MBK_A0_OF_SHIFT 2
#define REG_MI_MBK_A0_OF_SIZE 2
#define REG_MI_MBK_A0_OF_MASK 0x0c

#define REG_MI_MBK_A0_M_SHIFT 0
#define REG_MI_MBK_A0_M_SIZE 1
#define REG_MI_MBK_A0_M_MASK 0x01

#ifndef SDK_ASM
#define REG_MI_MBK_A0_FIELD(e, of, m)                                          \
  (u8)(((u32)(e) << REG_MI_MBK_A0_E_SHIFT) |                                   \
       ((u32)(of) << REG_MI_MBK_A0_OF_SHIFT) |                                 \
       ((u32)(m) << REG_MI_MBK_A0_M_SHIFT))
#endif

#define REG_MI_MBK_A1_E_SHIFT 7
#define REG_MI_MBK_A1_E_SIZE 1
#define REG_MI_MBK_A1_E_MASK 0x80

#define REG_MI_MBK_A1_OF_SHIFT 2
#define REG_MI_MBK_A1_OF_SIZE 2
#define REG_MI_MBK_A1_OF_MASK 0x0c

#define REG_MI_MBK_A1_M_SHIFT 0
#define REG_MI_MBK_A1_M_SIZE 1
#define REG_MI_MBK_A1_M_MASK 0x01

#ifndef SDK_ASM
#define REG_MI_MBK_A1_FIELD(e, of, m)                                          \
  (u8)(((u32)(e) << REG_MI_MBK_A1_E_SHIFT) |                                   \
       ((u32)(of) << REG_MI_MBK_A1_OF_SHIFT) |                                 \
       ((u32)(m) << REG_MI_MBK_A1_M_SHIFT))
#endif

#define REG_MI_MBK_A2_E_SHIFT 7
#define REG_MI_MBK_A2_E_SIZE 1
#define REG_MI_MBK_A2_E_MASK 0x80

#define REG_MI_MBK_A2_OF_SHIFT 2
#define REG_MI_MBK_A2_OF_SIZE 2
#define REG_MI_MBK_A2_OF_MASK 0x0c

#define REG_MI_MBK_A2_M_SHIFT 0
#define REG_MI_MBK_A2_M_SIZE 1
#define REG_MI_MBK_A2_M_MASK 0x01

#ifndef SDK_ASM
#define REG_MI_MBK_A2_FIELD(e, of, m)                                          \
  (u8)(((u32)(e) << REG_MI_MBK_A2_E_SHIFT) |                                   \
       ((u32)(of) << REG_MI_MBK_A2_OF_SHIFT) |                                 \
       ((u32)(m) << REG_MI_MBK_A2_M_SHIFT))
#endif

#define REG_MI_MBK_A3_E_SHIFT 7
#define REG_MI_MBK_A3_E_SIZE 1
#define REG_MI_MBK_A3_E_MASK 0x80

#define REG_MI_MBK_A3_OF_SHIFT 2
#define REG_MI_MBK_A3_OF_SIZE 2
#define REG_MI_MBK_A3_OF_MASK 0x0c

#define REG_MI_MBK_A3_M_SHIFT 0
#define REG_MI_MBK_A3_M_SIZE 1
#define REG_MI_MBK_A3_M_MASK 0x01

#ifndef SDK_ASM
#define REG_MI_MBK_A3_FIELD(e, of, m)                                          \
  (u8)(((u32)(e) << REG_MI_MBK_A3_E_SHIFT) |                                   \
       ((u32)(of) << REG_MI_MBK_A3_OF_SHIFT) |                                 \
       ((u32)(m) << REG_MI_MBK_A3_M_SHIFT))
#endif

#define REG_MI_MBK2_WB3_E_SHIFT 31
#define REG_MI_MBK2_WB3_E_SIZE 1
#define REG_MI_MBK2_WB3_E_MASK 0x80000000

#define REG_MI_MBK2_WB3_OF_SHIFT 28
#define REG_MI_MBK2_WB3_OF_SIZE 3
#define REG_MI_MBK2_WB3_OF_MASK 0x70000000

#define REG_MI_MBK2_WB3_M_SHIFT 24
#define REG_MI_MBK2_WB3_M_SIZE 2
#define REG_MI_MBK2_WB3_M_MASK 0x03000000

#define REG_MI_MBK2_WB2_E_SHIFT 23
#define REG_MI_MBK2_WB2_E_SIZE 1
#define REG_MI_MBK2_WB2_E_MASK 0x00800000

#define REG_MI_MBK2_WB2_OF_SHIFT 18
#define REG_MI_MBK2_WB2_OF_SIZE 3
#define REG_MI_MBK2_WB2_OF_MASK 0x001c0000

#define REG_MI_MBK2_WB2_M_SHIFT 16
#define REG_MI_MBK2_WB2_M_SIZE 2
#define REG_MI_MBK2_WB2_M_MASK 0x00030000

#define REG_MI_MBK2_WB1_E_SHIFT 15
#define REG_MI_MBK2_WB1_E_SIZE 1
#define REG_MI_MBK2_WB1_E_MASK 0x00008000

#define REG_MI_MBK2_WB1_OF_SHIFT 10
#define REG_MI_MBK2_WB1_OF_SIZE 3
#define REG_MI_MBK2_WB1_OF_MASK 0x00001c00

#define REG_MI_MBK2_WB1_M_SHIFT 8
#define REG_MI_MBK2_WB1_M_SIZE 2
#define REG_MI_MBK2_WB1_M_MASK 0x00000300

#define REG_MI_MBK2_WB0_E_SHIFT 7
#define REG_MI_MBK2_WB0_E_SIZE 1
#define REG_MI_MBK2_WB0_E_MASK 0x00000080

#define REG_MI_MBK2_WB0_OF_SHIFT 2
#define REG_MI_MBK2_WB0_OF_SIZE 3
#define REG_MI_MBK2_WB0_OF_MASK 0x0000001c

#define REG_MI_MBK2_WB0_M_SHIFT 0
#define REG_MI_MBK2_WB0_M_SIZE 2
#define REG_MI_MBK2_WB0_M_MASK 0x00000003

#ifndef SDK_ASM
#define REG_MI_MBK2_FIELD(wb3_e, wb3_of, wb3_m, wb2_e, wb2_of, wb2_m, wb1_e,   \
                          wb1_of, wb1_m, wb0_e, wb0_of, wb0_m)                 \
  (u32)(((u32)(wb3_e) << REG_MI_MBK2_WB3_E_SHIFT) |                            \
        ((u32)(wb3_of) << REG_MI_MBK2_WB3_OF_SHIFT) |                          \
        ((u32)(wb3_m) << REG_MI_MBK2_WB3_M_SHIFT) |                            \
        ((u32)(wb2_e) << REG_MI_MBK2_WB2_E_SHIFT) |                            \
        ((u32)(wb2_of) << REG_MI_MBK2_WB2_OF_SHIFT) |                          \
        ((u32)(wb2_m) << REG_MI_MBK2_WB2_M_SHIFT) |                            \
        ((u32)(wb1_e) << REG_MI_MBK2_WB1_E_SHIFT) |                            \
        ((u32)(wb1_of) << REG_MI_MBK2_WB1_OF_SHIFT) |                          \
        ((u32)(wb1_m) << REG_MI_MBK2_WB1_M_SHIFT) |                            \
        ((u32)(wb0_e) << REG_MI_MBK2_WB0_E_SHIFT) |                            \
        ((u32)(wb0_of) << REG_MI_MBK2_WB0_OF_SHIFT) |                          \
        ((u32)(wb0_m) << REG_MI_MBK2_WB0_M_SHIFT))
#endif

#define REG_MI_MBK_B0_E_SHIFT 7
#define REG_MI_MBK_B0_E_SIZE 1
#define REG_MI_MBK_B0_E_MASK 0x80

#define REG_MI_MBK_B0_OF_SHIFT 2
#define REG_MI_MBK_B0_OF_SIZE 3
#define REG_MI_MBK_B0_OF_MASK 0x1c

#define REG_MI_MBK_B0_M_SHIFT 0
#define REG_MI_MBK_B0_M_SIZE 2
#define REG_MI_MBK_B0_M_MASK 0x03

#ifndef SDK_ASM
#define REG_MI_MBK_B0_FIELD(e, of, m)                                          \
  (u8)(((u32)(e) << REG_MI_MBK_B0_E_SHIFT) |                                   \
       ((u32)(of) << REG_MI_MBK_B0_OF_SHIFT) |                                 \
       ((u32)(m) << REG_MI_MBK_B0_M_SHIFT))
#endif

#define REG_MI_MBK_B1_E_SHIFT 7
#define REG_MI_MBK_B1_E_SIZE 1
#define REG_MI_MBK_B1_E_MASK 0x80

#define REG_MI_MBK_B1_OF_SHIFT 2
#define REG_MI_MBK_B1_OF_SIZE 3
#define REG_MI_MBK_B1_OF_MASK 0x1c

#define REG_MI_MBK_B1_M_SHIFT 0
#define REG_MI_MBK_B1_M_SIZE 2
#define REG_MI_MBK_B1_M_MASK 0x03

#ifndef SDK_ASM
#define REG_MI_MBK_B1_FIELD(e, of, m)                                          \
  (u8)(((u32)(e) << REG_MI_MBK_B1_E_SHIFT) |                                   \
       ((u32)(of) << REG_MI_MBK_B1_OF_SHIFT) |                                 \
       ((u32)(m) << REG_MI_MBK_B1_M_SHIFT))
#endif

#define REG_MI_MBK_B2_E_SHIFT 7
#define REG_MI_MBK_B2_E_SIZE 1
#define REG_MI_MBK_B2_E_MASK 0x80

#define REG_MI_MBK_B2_OF_SHIFT 2
#define REG_MI_MBK_B2_OF_SIZE 3
#define REG_MI_MBK_B2_OF_MASK 0x1c

#define REG_MI_MBK_B2_M_SHIFT 0
#define REG_MI_MBK_B2_M_SIZE 2
#define REG_MI_MBK_B2_M_MASK 0x03

#ifndef SDK_ASM
#define REG_MI_MBK_B2_FIELD(e, of, m)                                          \
  (u8)(((u32)(e) << REG_MI_MBK_B2_E_SHIFT) |                                   \
       ((u32)(of) << REG_MI_MBK_B2_OF_SHIFT) |                                 \
       ((u32)(m) << REG_MI_MBK_B2_M_SHIFT))
#endif

#define REG_MI_MBK_B3_E_SHIFT 7
#define REG_MI_MBK_B3_E_SIZE 1
#define REG_MI_MBK_B3_E_MASK 0x80

#define REG_MI_MBK_B3_OF_SHIFT 2
#define REG_MI_MBK_B3_OF_SIZE 3
#define REG_MI_MBK_B3_OF_MASK 0x1c

#define REG_MI_MBK_B3_M_SHIFT 0
#define REG_MI_MBK_B3_M_SIZE 2
#define REG_MI_MBK_B3_M_MASK 0x03

#ifndef SDK_ASM
#define REG_MI_MBK_B3_FIELD(e, of, m)                                          \
  (u8)(((u32)(e) << REG_MI_MBK_B3_E_SHIFT) |                                   \
       ((u32)(of) << REG_MI_MBK_B3_OF_SHIFT) |                                 \
       ((u32)(m) << REG_MI_MBK_B3_M_SHIFT))
#endif

#define REG_MI_MBK3_WB7_E_SHIFT 31
#define REG_MI_MBK3_WB7_E_SIZE 1
#define REG_MI_MBK3_WB7_E_MASK 0x80000000

#define REG_MI_MBK3_WB7_OF_SHIFT 28
#define REG_MI_MBK3_WB7_OF_SIZE 3
#define REG_MI_MBK3_WB7_OF_MASK 0x70000000

#define REG_MI_MBK3_WB7_M_SHIFT 24
#define REG_MI_MBK3_WB7_M_SIZE 2
#define REG_MI_MBK3_WB7_M_MASK 0x03000000

#define REG_MI_MBK3_WB6_E_SHIFT 23
#define REG_MI_MBK3_WB6_E_SIZE 1
#define REG_MI_MBK3_WB6_E_MASK 0x00800000

#define REG_MI_MBK3_WB6_OF_SHIFT 18
#define REG_MI_MBK3_WB6_OF_SIZE 3
#define REG_MI_MBK3_WB6_OF_MASK 0x001c0000

#define REG_MI_MBK3_WB6_M_SHIFT 16
#define REG_MI_MBK3_WB6_M_SIZE 2
#define REG_MI_MBK3_WB6_M_MASK 0x00030000

#define REG_MI_MBK3_WB5_E_SHIFT 15
#define REG_MI_MBK3_WB5_E_SIZE 1
#define REG_MI_MBK3_WB5_E_MASK 0x00008000

#define REG_MI_MBK3_WB5_OF_SHIFT 10
#define REG_MI_MBK3_WB5_OF_SIZE 3
#define REG_MI_MBK3_WB5_OF_MASK 0x00001c00

#define REG_MI_MBK3_WB5_M_SHIFT 8
#define REG_MI_MBK3_WB5_M_SIZE 2
#define REG_MI_MBK3_WB5_M_MASK 0x00000300

#define REG_MI_MBK3_WB4_E_SHIFT 7
#define REG_MI_MBK3_WB4_E_SIZE 1
#define REG_MI_MBK3_WB4_E_MASK 0x00000080

#define REG_MI_MBK3_WB4_OF_SHIFT 2
#define REG_MI_MBK3_WB4_OF_SIZE 3
#define REG_MI_MBK3_WB4_OF_MASK 0x0000001c

#define REG_MI_MBK3_WB4_M_SHIFT 0
#define REG_MI_MBK3_WB4_M_SIZE 2
#define REG_MI_MBK3_WB4_M_MASK 0x00000003

#ifndef SDK_ASM
#define REG_MI_MBK3_FIELD(wb7_e, wb7_of, wb7_m, wb6_e, wb6_of, wb6_m, wb5_e,   \
                          wb5_of, wb5_m, wb4_e, wb4_of, wb4_m)                 \
  (u32)(((u32)(wb7_e) << REG_MI_MBK3_WB7_E_SHIFT) |                            \
        ((u32)(wb7_of) << REG_MI_MBK3_WB7_OF_SHIFT) |                          \
        ((u32)(wb7_m) << REG_MI_MBK3_WB7_M_SHIFT) |                            \
        ((u32)(wb6_e) << REG_MI_MBK3_WB6_E_SHIFT) |                            \
        ((u32)(wb6_of) << REG_MI_MBK3_WB6_OF_SHIFT) |                          \
        ((u32)(wb6_m) << REG_MI_MBK3_WB6_M_SHIFT) |                            \
        ((u32)(wb5_e) << REG_MI_MBK3_WB5_E_SHIFT) |                            \
        ((u32)(wb5_of) << REG_MI_MBK3_WB5_OF_SHIFT) |                          \
        ((u32)(wb5_m) << REG_MI_MBK3_WB5_M_SHIFT) |                            \
        ((u32)(wb4_e) << REG_MI_MBK3_WB4_E_SHIFT) |                            \
        ((u32)(wb4_of) << REG_MI_MBK3_WB4_OF_SHIFT) |                          \
        ((u32)(wb4_m) << REG_MI_MBK3_WB4_M_SHIFT))
#endif

#define REG_MI_MBK_B4_E_SHIFT 7
#define REG_MI_MBK_B4_E_SIZE 1
#define REG_MI_MBK_B4_E_MASK 0x80

#define REG_MI_MBK_B4_OF_SHIFT 2
#define REG_MI_MBK_B4_OF_SIZE 3
#define REG_MI_MBK_B4_OF_MASK 0x1c

#define REG_MI_MBK_B4_M_SHIFT 0
#define REG_MI_MBK_B4_M_SIZE 2
#define REG_MI_MBK_B4_M_MASK 0x03

#ifndef SDK_ASM
#define REG_MI_MBK_B4_FIELD(e, of, m)                                          \
  (u8)(((u32)(e) << REG_MI_MBK_B4_E_SHIFT) |                                   \
       ((u32)(of) << REG_MI_MBK_B4_OF_SHIFT) |                                 \
       ((u32)(m) << REG_MI_MBK_B4_M_SHIFT))
#endif

#define REG_MI_MBK_B5_E_SHIFT 7
#define REG_MI_MBK_B5_E_SIZE 1
#define REG_MI_MBK_B5_E_MASK 0x80

#define REG_MI_MBK_B5_OF_SHIFT 2
#define REG_MI_MBK_B5_OF_SIZE 3
#define REG_MI_MBK_B5_OF_MASK 0x1c

#define REG_MI_MBK_B5_M_SHIFT 0
#define REG_MI_MBK_B5_M_SIZE 2
#define REG_MI_MBK_B5_M_MASK 0x03

#ifndef SDK_ASM
#define REG_MI_MBK_B5_FIELD(e, of, m)                                          \
  (u8)(((u32)(e) << REG_MI_MBK_B5_E_SHIFT) |                                   \
       ((u32)(of) << REG_MI_MBK_B5_OF_SHIFT) |                                 \
       ((u32)(m) << REG_MI_MBK_B5_M_SHIFT))
#endif

#define REG_MI_MBK_B6_E_SHIFT 7
#define REG_MI_MBK_B6_E_SIZE 1
#define REG_MI_MBK_B6_E_MASK 0x80

#define REG_MI_MBK_B6_OF_SHIFT 2
#define REG_MI_MBK_B6_OF_SIZE 3
#define REG_MI_MBK_B6_OF_MASK 0x1c

#define REG_MI_MBK_B6_M_SHIFT 0
#define REG_MI_MBK_B6_M_SIZE 2
#define REG_MI_MBK_B6_M_MASK 0x03

#ifndef SDK_ASM
#define REG_MI_MBK_B6_FIELD(e, of, m)                                          \
  (u8)(((u32)(e) << REG_MI_MBK_B6_E_SHIFT) |                                   \
       ((u32)(of) << REG_MI_MBK_B6_OF_SHIFT) |                                 \
       ((u32)(m) << REG_MI_MBK_B6_M_SHIFT))
#endif

#define REG_MI_MBK_B7_E_SHIFT 7
#define REG_MI_MBK_B7_E_SIZE 1
#define REG_MI_MBK_B7_E_MASK 0x80

#define REG_MI_MBK_B7_OF_SHIFT 2
#define REG_MI_MBK_B7_OF_SIZE 3
#define REG_MI_MBK_B7_OF_MASK 0x1c

#define REG_MI_MBK_B7_M_SHIFT 0
#define REG_MI_MBK_B7_M_SIZE 2
#define REG_MI_MBK_B7_M_MASK 0x03

#ifndef SDK_ASM
#define REG_MI_MBK_B7_FIELD(e, of, m)                                          \
  (u8)(((u32)(e) << REG_MI_MBK_B7_E_SHIFT) |                                   \
       ((u32)(of) << REG_MI_MBK_B7_OF_SHIFT) |                                 \
       ((u32)(m) << REG_MI_MBK_B7_M_SHIFT))
#endif

#define REG_MI_MBK4_WC3_E_SHIFT 31
#define REG_MI_MBK4_WC3_E_SIZE 1
#define REG_MI_MBK4_WC3_E_MASK 0x80000000

#define REG_MI_MBK4_WC3_OF_SHIFT 28
#define REG_MI_MBK4_WC3_OF_SIZE 3
#define REG_MI_MBK4_WC3_OF_MASK 0x70000000

#define REG_MI_MBK4_WC3_M_SHIFT 24
#define REG_MI_MBK4_WC3_M_SIZE 2
#define REG_MI_MBK4_WC3_M_MASK 0x03000000

#define REG_MI_MBK4_WC2_E_SHIFT 23
#define REG_MI_MBK4_WC2_E_SIZE 1
#define REG_MI_MBK4_WC2_E_MASK 0x00800000

#define REG_MI_MBK4_WC2_OF_SHIFT 18
#define REG_MI_MBK4_WC2_OF_SIZE 3
#define REG_MI_MBK4_WC2_OF_MASK 0x001c0000

#define REG_MI_MBK4_WC2_M_SHIFT 16
#define REG_MI_MBK4_WC2_M_SIZE 2
#define REG_MI_MBK4_WC2_M_MASK 0x00030000

#define REG_MI_MBK4_WC1_E_SHIFT 15
#define REG_MI_MBK4_WC1_E_SIZE 1
#define REG_MI_MBK4_WC1_E_MASK 0x00008000

#define REG_MI_MBK4_WC1_OF_SHIFT 10
#define REG_MI_MBK4_WC1_OF_SIZE 3
#define REG_MI_MBK4_WC1_OF_MASK 0x00001c00

#define REG_MI_MBK4_WC1_M_SHIFT 8
#define REG_MI_MBK4_WC1_M_SIZE 2
#define REG_MI_MBK4_WC1_M_MASK 0x00000300

#define REG_MI_MBK4_WC0_E_SHIFT 7
#define REG_MI_MBK4_WC0_E_SIZE 1
#define REG_MI_MBK4_WC0_E_MASK 0x00000080

#define REG_MI_MBK4_WC0_OF_SHIFT 2
#define REG_MI_MBK4_WC0_OF_SIZE 3
#define REG_MI_MBK4_WC0_OF_MASK 0x0000001c

#define REG_MI_MBK4_WC0_M_SHIFT 0
#define REG_MI_MBK4_WC0_M_SIZE 2
#define REG_MI_MBK4_WC0_M_MASK 0x00000003

#ifndef SDK_ASM
#define REG_MI_MBK4_FIELD(wc3_e, wc3_of, wc3_m, wc2_e, wc2_of, wc2_m, wc1_e,   \
                          wc1_of, wc1_m, wc0_e, wc0_of, wc0_m)                 \
  (u32)(((u32)(wc3_e) << REG_MI_MBK4_WC3_E_SHIFT) |                            \
        ((u32)(wc3_of) << REG_MI_MBK4_WC3_OF_SHIFT) |                          \
        ((u32)(wc3_m) << REG_MI_MBK4_WC3_M_SHIFT) |                            \
        ((u32)(wc2_e) << REG_MI_MBK4_WC2_E_SHIFT) |                            \
        ((u32)(wc2_of) << REG_MI_MBK4_WC2_OF_SHIFT) |                          \
        ((u32)(wc2_m) << REG_MI_MBK4_WC2_M_SHIFT) |                            \
        ((u32)(wc1_e) << REG_MI_MBK4_WC1_E_SHIFT) |                            \
        ((u32)(wc1_of) << REG_MI_MBK4_WC1_OF_SHIFT) |                          \
        ((u32)(wc1_m) << REG_MI_MBK4_WC1_M_SHIFT) |                            \
        ((u32)(wc0_e) << REG_MI_MBK4_WC0_E_SHIFT) |                            \
        ((u32)(wc0_of) << REG_MI_MBK4_WC0_OF_SHIFT) |                          \
        ((u32)(wc0_m) << REG_MI_MBK4_WC0_M_SHIFT))
#endif

#define REG_MI_MBK_C0_E_SHIFT 7
#define REG_MI_MBK_C0_E_SIZE 1
#define REG_MI_MBK_C0_E_MASK 0x80

#define REG_MI_MBK_C0_OF_SHIFT 2
#define REG_MI_MBK_C0_OF_SIZE 3
#define REG_MI_MBK_C0_OF_MASK 0x1c

#define REG_MI_MBK_C0_M_SHIFT 0
#define REG_MI_MBK_C0_M_SIZE 2
#define REG_MI_MBK_C0_M_MASK 0x03

#ifndef SDK_ASM
#define REG_MI_MBK_C0_FIELD(e, of, m)                                          \
  (u8)(((u32)(e) << REG_MI_MBK_C0_E_SHIFT) |                                   \
       ((u32)(of) << REG_MI_MBK_C0_OF_SHIFT) |                                 \
       ((u32)(m) << REG_MI_MBK_C0_M_SHIFT))
#endif

#define REG_MI_MBK_C1_E_SHIFT 7
#define REG_MI_MBK_C1_E_SIZE 1
#define REG_MI_MBK_C1_E_MASK 0x80

#define REG_MI_MBK_C1_OF_SHIFT 2
#define REG_MI_MBK_C1_OF_SIZE 3
#define REG_MI_MBK_C1_OF_MASK 0x1c

#define REG_MI_MBK_C1_M_SHIFT 0
#define REG_MI_MBK_C1_M_SIZE 2
#define REG_MI_MBK_C1_M_MASK 0x03

#ifndef SDK_ASM
#define REG_MI_MBK_C1_FIELD(e, of, m)                                          \
  (u8)(((u32)(e) << REG_MI_MBK_C1_E_SHIFT) |                                   \
       ((u32)(of) << REG_MI_MBK_C1_OF_SHIFT) |                                 \
       ((u32)(m) << REG_MI_MBK_C1_M_SHIFT))
#endif

#define REG_MI_MBK_C2_E_SHIFT 7
#define REG_MI_MBK_C2_E_SIZE 1
#define REG_MI_MBK_C2_E_MASK 0x80

#define REG_MI_MBK_C2_OF_SHIFT 2
#define REG_MI_MBK_C2_OF_SIZE 3
#define REG_MI_MBK_C2_OF_MASK 0x1c

#define REG_MI_MBK_C2_M_SHIFT 0
#define REG_MI_MBK_C2_M_SIZE 2
#define REG_MI_MBK_C2_M_MASK 0x03

#ifndef SDK_ASM
#define REG_MI_MBK_C2_FIELD(e, of, m)                                          \
  (u8)(((u32)(e) << REG_MI_MBK_C2_E_SHIFT) |                                   \
       ((u32)(of) << REG_MI_MBK_C2_OF_SHIFT) |                                 \
       ((u32)(m) << REG_MI_MBK_C2_M_SHIFT))
#endif

#define REG_MI_MBK_C3_E_SHIFT 7
#define REG_MI_MBK_C3_E_SIZE 1
#define REG_MI_MBK_C3_E_MASK 0x80

#define REG_MI_MBK_C3_OF_SHIFT 2
#define REG_MI_MBK_C3_OF_SIZE 3
#define REG_MI_MBK_C3_OF_MASK 0x1c

#define REG_MI_MBK_C3_M_SHIFT 0
#define REG_MI_MBK_C3_M_SIZE 2
#define REG_MI_MBK_C3_M_MASK 0x03

#ifndef SDK_ASM
#define REG_MI_MBK_C3_FIELD(e, of, m)                                          \
  (u8)(((u32)(e) << REG_MI_MBK_C3_E_SHIFT) |                                   \
       ((u32)(of) << REG_MI_MBK_C3_OF_SHIFT) |                                 \
       ((u32)(m) << REG_MI_MBK_C3_M_SHIFT))
#endif

#define REG_MI_MBK5_WC7_E_SHIFT 31
#define REG_MI_MBK5_WC7_E_SIZE 1
#define REG_MI_MBK5_WC7_E_MASK 0x80000000

#define REG_MI_MBK5_WC7_OF_SHIFT 28
#define REG_MI_MBK5_WC7_OF_SIZE 3
#define REG_MI_MBK5_WC7_OF_MASK 0x70000000

#define REG_MI_MBK5_WC7_M_SHIFT 24
#define REG_MI_MBK5_WC7_M_SIZE 2
#define REG_MI_MBK5_WC7_M_MASK 0x03000000

#define REG_MI_MBK5_WC6_E_SHIFT 23
#define REG_MI_MBK5_WC6_E_SIZE 1
#define REG_MI_MBK5_WC6_E_MASK 0x00800000

#define REG_MI_MBK5_WC6_OF_SHIFT 18
#define REG_MI_MBK5_WC6_OF_SIZE 3
#define REG_MI_MBK5_WC6_OF_MASK 0x001c0000

#define REG_MI_MBK5_WC6_M_SHIFT 16
#define REG_MI_MBK5_WC6_M_SIZE 2
#define REG_MI_MBK5_WC6_M_MASK 0x00030000

#define REG_MI_MBK5_WC5_E_SHIFT 15
#define REG_MI_MBK5_WC5_E_SIZE 1
#define REG_MI_MBK5_WC5_E_MASK 0x00008000

#define REG_MI_MBK5_WC5_OF_SHIFT 10
#define REG_MI_MBK5_WC5_OF_SIZE 3
#define REG_MI_MBK5_WC5_OF_MASK 0x00001c00

#define REG_MI_MBK5_WC5_M_SHIFT 8
#define REG_MI_MBK5_WC5_M_SIZE 2
#define REG_MI_MBK5_WC5_M_MASK 0x00000300

#define REG_MI_MBK5_WC4_E_SHIFT 7
#define REG_MI_MBK5_WC4_E_SIZE 1
#define REG_MI_MBK5_WC4_E_MASK 0x00000080

#define REG_MI_MBK5_WC4_OF_SHIFT 2
#define REG_MI_MBK5_WC4_OF_SIZE 3
#define REG_MI_MBK5_WC4_OF_MASK 0x0000001c

#define REG_MI_MBK5_WC4_M_SHIFT 0
#define REG_MI_MBK5_WC4_M_SIZE 2
#define REG_MI_MBK5_WC4_M_MASK 0x00000003

#ifndef SDK_ASM
#define REG_MI_MBK5_FIELD(wc7_e, wc7_of, wc7_m, wc6_e, wc6_of, wc6_m, wc5_e,   \
                          wc5_of, wc5_m, wc4_e, wc4_of, wc4_m)                 \
  (u32)(((u32)(wc7_e) << REG_MI_MBK5_WC7_E_SHIFT) |                            \
        ((u32)(wc7_of) << REG_MI_MBK5_WC7_OF_SHIFT) |                          \
        ((u32)(wc7_m) << REG_MI_MBK5_WC7_M_SHIFT) |                            \
        ((u32)(wc6_e) << REG_MI_MBK5_WC6_E_SHIFT) |                            \
        ((u32)(wc6_of) << REG_MI_MBK5_WC6_OF_SHIFT) |                          \
        ((u32)(wc6_m) << REG_MI_MBK5_WC6_M_SHIFT) |                            \
        ((u32)(wc5_e) << REG_MI_MBK5_WC5_E_SHIFT) |                            \
        ((u32)(wc5_of) << REG_MI_MBK5_WC5_OF_SHIFT) |                          \
        ((u32)(wc5_m) << REG_MI_MBK5_WC5_M_SHIFT) |                            \
        ((u32)(wc4_e) << REG_MI_MBK5_WC4_E_SHIFT) |                            \
        ((u32)(wc4_of) << REG_MI_MBK5_WC4_OF_SHIFT) |                          \
        ((u32)(wc4_m) << REG_MI_MBK5_WC4_M_SHIFT))
#endif

#define REG_MI_MBK_C4_E_SHIFT 7
#define REG_MI_MBK_C4_E_SIZE 1
#define REG_MI_MBK_C4_E_MASK 0x80

#define REG_MI_MBK_C4_OF_SHIFT 2
#define REG_MI_MBK_C4_OF_SIZE 3
#define REG_MI_MBK_C4_OF_MASK 0x1c

#define REG_MI_MBK_C4_M_SHIFT 0
#define REG_MI_MBK_C4_M_SIZE 2
#define REG_MI_MBK_C4_M_MASK 0x03

#ifndef SDK_ASM
#define REG_MI_MBK_C4_FIELD(e, of, m)                                          \
  (u8)(((u32)(e) << REG_MI_MBK_C4_E_SHIFT) |                                   \
       ((u32)(of) << REG_MI_MBK_C4_OF_SHIFT) |                                 \
       ((u32)(m) << REG_MI_MBK_C4_M_SHIFT))
#endif

#define REG_MI_MBK_C5_E_SHIFT 7
#define REG_MI_MBK_C5_E_SIZE 1
#define REG_MI_MBK_C5_E_MASK 0x80

#define REG_MI_MBK_C5_OF_SHIFT 2
#define REG_MI_MBK_C5_OF_SIZE 3
#define REG_MI_MBK_C5_OF_MASK 0x1c

#define REG_MI_MBK_C5_M_SHIFT 0
#define REG_MI_MBK_C5_M_SIZE 2
#define REG_MI_MBK_C5_M_MASK 0x03

#ifndef SDK_ASM
#define REG_MI_MBK_C5_FIELD(e, of, m)                                          \
  (u8)(((u32)(e) << REG_MI_MBK_C5_E_SHIFT) |                                   \
       ((u32)(of) << REG_MI_MBK_C5_OF_SHIFT) |                                 \
       ((u32)(m) << REG_MI_MBK_C5_M_SHIFT))
#endif

#define REG_MI_MBK_C6_E_SHIFT 7
#define REG_MI_MBK_C6_E_SIZE 1
#define REG_MI_MBK_C6_E_MASK 0x80

#define REG_MI_MBK_C6_OF_SHIFT 2
#define REG_MI_MBK_C6_OF_SIZE 3
#define REG_MI_MBK_C6_OF_MASK 0x1c

#define REG_MI_MBK_C6_M_SHIFT 0
#define REG_MI_MBK_C6_M_SIZE 2
#define REG_MI_MBK_C6_M_MASK 0x03

#ifndef SDK_ASM
#define REG_MI_MBK_C6_FIELD(e, of, m)                                          \
  (u8)(((u32)(e) << REG_MI_MBK_C6_E_SHIFT) |                                   \
       ((u32)(of) << REG_MI_MBK_C6_OF_SHIFT) |                                 \
       ((u32)(m) << REG_MI_MBK_C6_M_SHIFT))
#endif

#define REG_MI_MBK_C7_E_SHIFT 7
#define REG_MI_MBK_C7_E_SIZE 1
#define REG_MI_MBK_C7_E_MASK 0x80

#define REG_MI_MBK_C7_OF_SHIFT 2
#define REG_MI_MBK_C7_OF_SIZE 3
#define REG_MI_MBK_C7_OF_MASK 0x1c

#define REG_MI_MBK_C7_M_SHIFT 0
#define REG_MI_MBK_C7_M_SIZE 2
#define REG_MI_MBK_C7_M_MASK 0x03

#ifndef SDK_ASM
#define REG_MI_MBK_C7_FIELD(e, of, m)                                          \
  (u8)(((u32)(e) << REG_MI_MBK_C7_E_SHIFT) |                                   \
       ((u32)(of) << REG_MI_MBK_C7_OF_SHIFT) |                                 \
       ((u32)(m) << REG_MI_MBK_C7_M_SHIFT))
#endif

#define REG_MI_MBK6_WA_EADDR_SHIFT 20
#define REG_MI_MBK6_WA_EADDR_SIZE 9
#define REG_MI_MBK6_WA_EADDR_MASK 0x1ff00000

#define REG_MI_MBK6_WA_ISIZE_SHIFT 12
#define REG_MI_MBK6_WA_ISIZE_SIZE 2
#define REG_MI_MBK6_WA_ISIZE_MASK 0x00003000

#define REG_MI_MBK6_WA_SADDR_SHIFT 4
#define REG_MI_MBK6_WA_SADDR_SIZE 8
#define REG_MI_MBK6_WA_SADDR_MASK 0x00000ff0

#ifndef SDK_ASM
#define REG_MI_MBK6_FIELD(wa_eaddr, wa_isize, wa_saddr)                        \
  (u32)(((u32)(wa_eaddr) << REG_MI_MBK6_WA_EADDR_SHIFT) |                      \
        ((u32)(wa_isize) << REG_MI_MBK6_WA_ISIZE_SHIFT) |                      \
        ((u32)(wa_saddr) << REG_MI_MBK6_WA_SADDR_SHIFT))
#endif

#define REG_MI_MBK7_WB_EADDR_SHIFT 19
#define REG_MI_MBK7_WB_EADDR_SIZE 10
#define REG_MI_MBK7_WB_EADDR_MASK 0x1ff80000

#define REG_MI_MBK7_WB_ISIZE_SHIFT 12
#define REG_MI_MBK7_WB_ISIZE_SIZE 2
#define REG_MI_MBK7_WB_ISIZE_MASK 0x00003000

#define REG_MI_MBK7_WB_SADDR_SHIFT 3
#define REG_MI_MBK7_WB_SADDR_SIZE 9
#define REG_MI_MBK7_WB_SADDR_MASK 0x00000ff8

#ifndef SDK_ASM
#define REG_MI_MBK7_FIELD(wb_eaddr, wb_isize, wb_saddr)                        \
  (u32)(((u32)(wb_eaddr) << REG_MI_MBK7_WB_EADDR_SHIFT) |                      \
        ((u32)(wb_isize) << REG_MI_MBK7_WB_ISIZE_SHIFT) |                      \
        ((u32)(wb_saddr) << REG_MI_MBK7_WB_SADDR_SHIFT))
#endif

#define REG_MI_MBK8_WC_EADDR_SHIFT 19
#define REG_MI_MBK8_WC_EADDR_SIZE 10
#define REG_MI_MBK8_WC_EADDR_MASK 0x1ff80000

#define REG_MI_MBK8_WC_ISIZE_SHIFT 12
#define REG_MI_MBK8_WC_ISIZE_SIZE 2
#define REG_MI_MBK8_WC_ISIZE_MASK 0x00003000

#define REG_MI_MBK8_WC_SADDR_SHIFT 3
#define REG_MI_MBK8_WC_SADDR_SIZE 9
#define REG_MI_MBK8_WC_SADDR_MASK 0x00000ff8

#ifndef SDK_ASM
#define REG_MI_MBK8_FIELD(wc_eaddr, wc_isize, wc_saddr)                        \
  (u32)(((u32)(wc_eaddr) << REG_MI_MBK8_WC_EADDR_SHIFT) |                      \
        ((u32)(wc_isize) << REG_MI_MBK8_WC_ISIZE_SHIFT) |                      \
        ((u32)(wc_saddr) << REG_MI_MBK8_WC_SADDR_SHIFT))
#endif

#define REG_MI_MBK9_WC_LOCKST_SHIFT 16
#define REG_MI_MBK9_WC_LOCKST_SIZE 8
#define REG_MI_MBK9_WC_LOCKST_MASK 0x00ff0000

#define REG_MI_MBK9_WB_LOCLST_SHIFT 8
#define REG_MI_MBK9_WB_LOCLST_SIZE 8
#define REG_MI_MBK9_WB_LOCLST_MASK 0x0000ff00

#define REG_MI_MBK9_WA_LOCKST_SHIFT 0
#define REG_MI_MBK9_WA_LOCKST_SIZE 4
#define REG_MI_MBK9_WA_LOCKST_MASK 0x0000000f

#ifndef SDK_ASM
#define REG_MI_MBK9_FIELD(wc_lockst, wb_loclst, wa_lockst)                     \
  (u32)(((u32)(wc_lockst) << REG_MI_MBK9_WC_LOCKST_SHIFT) |                    \
        ((u32)(wb_loclst) << REG_MI_MBK9_WB_LOCLST_SHIFT) |                    \
        ((u32)(wa_lockst) << REG_MI_MBK9_WA_LOCKST_SHIFT))
#endif

#define REG_MI_MBK_A_LOCK_A3_SHIFT 3
#define REG_MI_MBK_A_LOCK_A3_SIZE 1
#define REG_MI_MBK_A_LOCK_A3_MASK 0x08

#define REG_MI_MBK_A_LOCK_A2_SHIFT 2
#define REG_MI_MBK_A_LOCK_A2_SIZE 1
#define REG_MI_MBK_A_LOCK_A2_MASK 0x04

#define REG_MI_MBK_A_LOCK_A1_SHIFT 1
#define REG_MI_MBK_A_LOCK_A1_SIZE 1
#define REG_MI_MBK_A_LOCK_A1_MASK 0x02

#define REG_MI_MBK_A_LOCK_A0_SHIFT 0
#define REG_MI_MBK_A_LOCK_A0_SIZE 1
#define REG_MI_MBK_A_LOCK_A0_MASK 0x01

#ifndef SDK_ASM
#define REG_MI_MBK_A_LOCK_FIELD(a3, a2, a1, a0)                                \
  (u8)(((u32)(a3) << REG_MI_MBK_A_LOCK_A3_SHIFT) |                             \
       ((u32)(a2) << REG_MI_MBK_A_LOCK_A2_SHIFT) |                             \
       ((u32)(a1) << REG_MI_MBK_A_LOCK_A1_SHIFT) |                             \
       ((u32)(a0) << REG_MI_MBK_A_LOCK_A0_SHIFT))
#endif

#define REG_MI_MBK_B_LOCK_B7_SHIFT 7
#define REG_MI_MBK_B_LOCK_B7_SIZE 1
#define REG_MI_MBK_B_LOCK_B7_MASK 0x80

#define REG_MI_MBK_B_LOCK_B6_SHIFT 6
#define REG_MI_MBK_B_LOCK_B6_SIZE 1
#define REG_MI_MBK_B_LOCK_B6_MASK 0x40

#define REG_MI_MBK_B_LOCK_B5_SHIFT 5
#define REG_MI_MBK_B_LOCK_B5_SIZE 1
#define REG_MI_MBK_B_LOCK_B5_MASK 0x20

#define REG_MI_MBK_B_LOCK_B4_SHIFT 4
#define REG_MI_MBK_B_LOCK_B4_SIZE 1
#define REG_MI_MBK_B_LOCK_B4_MASK 0x10

#define REG_MI_MBK_B_LOCK_B3_SHIFT 3
#define REG_MI_MBK_B_LOCK_B3_SIZE 1
#define REG_MI_MBK_B_LOCK_B3_MASK 0x08

#define REG_MI_MBK_B_LOCK_B2_SHIFT 2
#define REG_MI_MBK_B_LOCK_B2_SIZE 1
#define REG_MI_MBK_B_LOCK_B2_MASK 0x04

#define REG_MI_MBK_B_LOCK_B1_SHIFT 1
#define REG_MI_MBK_B_LOCK_B1_SIZE 1
#define REG_MI_MBK_B_LOCK_B1_MASK 0x02

#define REG_MI_MBK_B_LOCK_B0_SHIFT 0
#define REG_MI_MBK_B_LOCK_B0_SIZE 1
#define REG_MI_MBK_B_LOCK_B0_MASK 0x01

#ifndef SDK_ASM
#define REG_MI_MBK_B_LOCK_FIELD(b7, b6, b5, b4, b3, b2, b1, b0)                \
  (u8)(((u32)(b7) << REG_MI_MBK_B_LOCK_B7_SHIFT) |                             \
       ((u32)(b6) << REG_MI_MBK_B_LOCK_B6_SHIFT) |                             \
       ((u32)(b5) << REG_MI_MBK_B_LOCK_B5_SHIFT) |                             \
       ((u32)(b4) << REG_MI_MBK_B_LOCK_B4_SHIFT) |                             \
       ((u32)(b3) << REG_MI_MBK_B_LOCK_B3_SHIFT) |                             \
       ((u32)(b2) << REG_MI_MBK_B_LOCK_B2_SHIFT) |                             \
       ((u32)(b1) << REG_MI_MBK_B_LOCK_B1_SHIFT) |                             \
       ((u32)(b0) << REG_MI_MBK_B_LOCK_B0_SHIFT))
#endif

#define REG_MI_MBK_C_LOCK_C7_SHIFT 7
#define REG_MI_MBK_C_LOCK_C7_SIZE 1
#define REG_MI_MBK_C_LOCK_C7_MASK 0x80

#define REG_MI_MBK_C_LOCK_C6_SHIFT 6
#define REG_MI_MBK_C_LOCK_C6_SIZE 1
#define REG_MI_MBK_C_LOCK_C6_MASK 0x40

#define REG_MI_MBK_C_LOCK_C5_SHIFT 5
#define REG_MI_MBK_C_LOCK_C5_SIZE 1
#define REG_MI_MBK_C_LOCK_C5_MASK 0x20

#define REG_MI_MBK_C_LOCK_C4_SHIFT 4
#define REG_MI_MBK_C_LOCK_C4_SIZE 1
#define REG_MI_MBK_C_LOCK_C4_MASK 0x10

#define REG_MI_MBK_C_LOCK_C3_SHIFT 3
#define REG_MI_MBK_C_LOCK_C3_SIZE 1
#define REG_MI_MBK_C_LOCK_C3_MASK 0x08

#define REG_MI_MBK_C_LOCK_C2_SHIFT 2
#define REG_MI_MBK_C_LOCK_C2_SIZE 1
#define REG_MI_MBK_C_LOCK_C2_MASK 0x04

#define REG_MI_MBK_C_LOCK_C1_SHIFT 1
#define REG_MI_MBK_C_LOCK_C1_SIZE 1
#define REG_MI_MBK_C_LOCK_C1_MASK 0x02

#define REG_MI_MBK_C_LOCK_C0_SHIFT 0
#define REG_MI_MBK_C_LOCK_C0_SIZE 1
#define REG_MI_MBK_C_LOCK_C0_MASK 0x01

#ifndef SDK_ASM
#define REG_MI_MBK_C_LOCK_FIELD(c7, c6, c5, c4, c3, c2, c1, c0)                \
  (u8)(((u32)(c7) << REG_MI_MBK_C_LOCK_C7_SHIFT) |                             \
       ((u32)(c6) << REG_MI_MBK_C_LOCK_C6_SHIFT) |                             \
       ((u32)(c5) << REG_MI_MBK_C_LOCK_C5_SHIFT) |                             \
       ((u32)(c4) << REG_MI_MBK_C_LOCK_C4_SHIFT) |                             \
       ((u32)(c3) << REG_MI_MBK_C_LOCK_C3_SHIFT) |                             \
       ((u32)(c2) << REG_MI_MBK_C_LOCK_C2_SHIFT) |                             \
       ((u32)(c1) << REG_MI_MBK_C_LOCK_C1_SHIFT) |                             \
       ((u32)(c0) << REG_MI_MBK_C_LOCK_C0_SHIFT))
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

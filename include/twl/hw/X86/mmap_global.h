#ifndef TWL_HW_ARM9_MMAP_GLOBAL_H_
#define TWL_HW_ARM9_MMAP_GLOBAL_H_
#ifdef __cplusplus
extern "C" {
#endif

#define HW_ITCM_IMAGE 0x01000000
#define HW_ITCM 0x01ff8000
#define HW_ITCM_SIZE 0x8000
#define HW_ITCM_END (HW_ITCM + HW_ITCM_SIZE)

#ifndef SDK_ASM
#include <nitro/types.h>
extern u32 SDK_AUTOLOAD_DTCM_START[];
#ifdef SDK_PORT
#define HW_DTCM ((u64)SDK_AUTOLOAD_DTCM_START)
#else
#define HW_DTCM ((u32)SDK_AUTOLOAD_DTCM_START)
#endif
#else
.extern SDK_AUTOLOAD_DTCM_START
#define HW_DTCM SDK_AUTOLOAD_DTCM_START
#endif
#define HW_DTCM_SIZE 0x4000
#define HW_DTCM_END (HW_DTCM + HW_DTCM_SIZE)
//PC Port: Doubled the size of HW_MAIN_MEM from 0x400000 to 0x800000
extern u8 s_HW_MAIN_MEM[0x800000];

#define HW_MAIN_MEM (u64)(s_HW_MAIN_MEM)
#define HW_MAIN_MEM_SIZE        0x00800000
extern u8 s_HW_MAIN_MEM_EX[0x800000];
#define HW_MAIN_MEM_EX_SIZE     0x00800000
#define HW_MAIN_MEM_END (HW_MAIN_MEM + HW_MAIN_MEM_SIZE)
#define HW_MAIN_MEM_EX_END (HW_MAIN_MEM + HW_MAIN_MEM_EX_SIZE)

#define HW_TWL_MAIN_MEM_SIZE 0x01000000
#define HW_TWL_MAIN_MEM_EX_SIZE 0x01000000

extern u8 s_HW_TWL_MAIN_MEM[0x01000000];
#define HW_TWL_MAIN_MEM (u64)(s_HW_TWL_MAIN_MEM)
#define HW_TWL_MAIN_MEM_END (HW_TWL_MAIN_MEM + HW_TWL_MAIN_MEM_SIZE)
#define HW_TWL_MAIN_MEM_IMAGE (u64)(s_HW_TWL_MAIN_MEM)
#define HW_TWL_MAIN_MEM_IMAGE_END (HW_TWL_MAIN_MEM_IMAGE + HW_TWL_MAIN_MEM_SIZE)
extern u8 s_HW_TWL_MAIN_MEM_EX[0x01000000];
#define HW_TWL_MAIN_MEM_EX (u64)(s_HW_TWL_MAIN_MEM_EX)
#define HW_TWL_MAIN_MEM_EX_END (HW_TWL_MAIN_MEM_EX + HW_TWL_MAIN_MEM_EX_SIZE)

#define HW_WRAM_AREA 0x03000000
#define HW_WRAM_AREA_HALF (HW_WRAM_AREA + (HW_WRAM_AREA_SIZE / 2))
#define HW_WRAM_AREA_END 0x04000000
#define HW_WRAM_AREA_SIZE (HW_WRAM_AREA_END - HW_WRAM_AREA)

#define HW_WRAM_BASE 0x03000000
#define HW_WRAM_0_SIZE 0x4000
#define HW_WRAM_1_SIZE 0x4000
#define HW_WRAM_SIZE (HW_WRAM_0_SIZE + HW_WRAM_1_SIZE)
#ifdef SDK_BB
#define HW_WRAM_A_SIZE 0x20000
#define HW_WRAM_B_SIZE 0x20000
#define HW_WRAM_C_SIZE 0x20000
#else
#define HW_WRAM_A_SIZE 0x40000
#define HW_WRAM_B_SIZE 0x40000
#define HW_WRAM_C_SIZE 0x40000
#endif

#define HW_WRAM_0 (HW_WRAM_BASE + HW_WRAM_A_SIZE)
#define HW_WRAM_0_END (HW_WRAM_0 + HW_WRAM_0_SIZE)
#define HW_WRAM_1 HW_WRAM_0_END
#define HW_WRAM_1_END (HW_WRAM_1 + HW_WRAM_1_SIZE)
#define HW_WRAM HW_WRAM_0
#define HW_WRAM_END HW_WRAM_1_END

#define HW_WRAM_B_OFFSET 0x0740000
#define HW_WRAM_C_OFFSET 0x0700000
#define HW_WRAM_B (HW_WRAM_BASE + HW_WRAM_B_OFFSET)
#ifdef SDK_BB
#define HW_WRAM_B_END (HW_WRAM_B + (HW_WRAM_B_SIZE * 2))
#else
#define HW_WRAM_B_END (HW_WRAM_B + HW_WRAM_B_SIZE)
#endif
#define HW_WRAM_C (HW_WRAM_BASE + HW_WRAM_C_OFFSET)
#ifdef SDK_BB
#define HW_WRAM_C_END (HW_WRAM_C + (HW_WRAM_C_SIZE * 2))
#else
#define HW_WRAM_C_END (HW_WRAM_C + HW_WRAM_C_SIZE)
#endif
#define HW_WRAM_B_OR_C_MIRROR HW_WRAM_B_END
#ifdef SDK_BB
#define HW_WRAM_B_OR_C_MIRROR_END                                              \
  (HW_WRAM_B_OR_C_MIRROR +                                                     \
   (HW_WRAM_B_SIZE * 2)) // HW_WRAM_B_SIZE==HW_WRAM_C_SIZE
#else
#define HW_WRAM_B_OR_C_MIRROR_END                                              \
  (HW_WRAM_B_OR_C_MIRROR + HW_WRAM_B_SIZE) // HW_WRAM_B_SIZE==HW_WRAM_C_SIZE
#endif

#define HW_IOREG 0x04000000
#define HW_IOREG_END 0x05000000
#define HW_REG_BASE HW_IOREG // alias

extern u8 s_HW_BG_PLTT[0x200];
#define HW_BG_PLTT              ((u64)s_HW_BG_PLTT)
#define HW_BG_PLTT_END          ((u64)s_HW_BG_PLTT + 0x200)
#define HW_BG_PLTT_SIZE (HW_BG_PLTT_END - HW_BG_PLTT)
extern u8 s_HW_OBJ_PLTT[0x200];
#define HW_OBJ_PLTT             ((u64)s_HW_OBJ_PLTT)
#define HW_OBJ_PLTT_END         ((u64)s_HW_OBJ_PLTT + 0x200)
#define HW_OBJ_PLTT_SIZE        (HW_OBJ_PLTT_END-HW_OBJ_PLTT)
#define HW_PLTT HW_BG_PLTT
#define HW_PLTT_END HW_OBJ_PLTT_END
#define HW_PLTT_SIZE (HW_PLTT_END - HW_PLTT)
extern u8 s_HW_DB_BG_PLTT[0x200];
#define HW_DB_BG_PLTT             ((u64)s_HW_DB_BG_PLTT)
#define HW_DB_BG_PLTT_END         ((u64)s_HW_DB_BG_PLTT + 0x200)
#define HW_DB_BG_PLTT_SIZE      (HW_DB_BG_PLTT_END-HW_DB_BG_PLTT)
extern u8 s_HW_DB_OBJ_PLTT[0x200];
#define HW_DB_OBJ_PLTT            ((u64)s_HW_DB_OBJ_PLTT)
#define HW_DB_OBJ_PLTT_END        ((u64)s_HW_DB_OBJ_PLTT + 0x200)
#define HW_DB_OBJ_PLTT_SIZE (HW_DB_OBJ_PLTT_END - HW_DB_OBJ_PLTT)
#define HW_DB_PLTT HW_DB_BG_PLTT
#define HW_DB_PLTT_END HW_DB_OBJ_PLTT_END
#define HW_DB_PLTT_SIZE (HW_DB_PLTT_END - HW_DB_PLTT)
extern u8 s_HW_BG_VRAM[0x80000];
#define HW_BG_VRAM              ((u64)s_HW_BG_VRAM)
#define HW_BG_VRAM_END          ((u64)s_HW_BG_VRAM + 0x80000)
#define HW_BG_VRAM_SIZE (HW_BG_VRAM_END - HW_BG_VRAM)
extern u8 s_HW_DB_BG_VRAM[0x20000];
#define HW_DB_BG_VRAM              ((u64)s_HW_DB_BG_VRAM)
#define HW_DB_BG_VRAM_END          ((u64)s_HW_DB_BG_VRAM + 0x20000)
#define HW_DB_BG_VRAM_SIZE (HW_DB_BG_VRAM_END - HW_DB_BG_VRAM)
extern u8 s_HW_OBJ_VRAM[0x40000];
#define HW_OBJ_VRAM             ((u64)s_HW_OBJ_VRAM)
#define HW_OBJ_VRAM_END         ((u64)s_HW_OBJ_VRAM + 0x40000)
#define HW_OBJ_VRAM_SIZE (HW_OBJ_VRAM_END - HW_OBJ_VRAM)
extern u8 s_HW_DB_OBJ_VRAM[0x20000];
#define HW_DB_OBJ_VRAM             ((u64)s_HW_DB_OBJ_VRAM)
#define HW_DB_OBJ_VRAM_END         ((u64)s_HW_DB_OBJ_VRAM + 0x40000)
#define HW_DB_OBJ_VRAM_SIZE (HW_DB_OBJ_VRAM_END - HW_DB_OBJ_VRAM)
extern u8 s_HW_LCDC_VRAM[0xA4000];
#define HW_LCDC_VRAM             ((u64)s_HW_LCDC_VRAM)
#define HW_LCDC_VRAM_END         ((u64)s_HW_LCDC_VRAM + 0xA4000)
#define HW_LCDC_VRAM_SIZE (HW_LCDC_VRAM_END - HW_LCDC_VRAM)
extern u8 s_HW_OAM[0x400];
#define HW_OAM              ((u64)s_HW_OAM)
#define HW_OAM_END          ((u64)s_HW_OAM + 0x400)
#define HW_OAM_SIZE (HW_OAM_END - HW_OAM)
extern u8 s_HW_DB_OAM[0x400];
#define HW_DB_OAM              ((u64)s_HW_DB_OAM)
#define HW_DB_OAM_END          ((u64)s_HW_DB_OAM + 0x400)
#define HW_DB_OAM_SIZE (HW_DB_OAM_END - HW_DB_OAM)

extern u8 s_HW_CTRDG_ROM[0x20000];
#define HW_CTRDG_ROM            ((u64)s_HW_CTRDG_ROM)
#define HW_CTRDG_ROM_END        ((u64)s_HW_CTRDG_ROM + 0x20000)
#define HW_CTRDG_RAM 0x0a000000
#define HW_CTRDG_RAM_END 0x0a010000

#define HW_BIOS_SIZE 0x00002000
#define HW_BIOS 0xffff0000
#define HW_BIOS_END (HW_BIOS + HW_BIOS_SIZE)
#define HW_TWL_BIOS_SIZE 0x00010000
#define HW_TWL_BIOS HW_BIOS
#define HW_TWL_BIOS_END (HW_TWL_BIOS + HW_TWL_BIOS_SIZE)

#define HW_RESET_VECTOR HW_BIOS

#define HW_EXT_WRAM_ARM7 0x06000000
#define HW_EXT_WRAM_ARM7_END 0x06040000
#define HW_EXT_WRAM_ARM7_SIZE (HW_EXT_WRAM_ARM7_END - HW_EXT_WRAM_ARM7)

extern u8 s_HW_EXT_WRAM[0x40000];
#define HW_EXT_WRAM ((u64)s_HW_EXT_WRAM)
#define HW_EXT_WRAM_END ((u64)s_HW_EXT_WRAM + 0x40000)
#define HW_EXT_WRAM_SIZE 0x40000

//ARM7 WRAM
extern u8 s_HW_PRV_WRAM[0x10000];
#define HW_PRV_WRAM    ((u64)s_HW_PRV_WRAM)
#define HW_PRV_WRAM_END ((u64)s_HW_PRV_WRAM + 0x10000)
#define HW_PRV_WRAM_SIZE 0x10000

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* TWL_HW_ARM9_MMAP_GLOBAL_H_ */

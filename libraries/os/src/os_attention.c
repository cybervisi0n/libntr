#include <os_attention.h>
#include <nitro/os/common/interrupt.h>

#ifdef SDK_TWLLTD

typedef enum {
  SPEC_DEST_NONE,
  SPEC_DEST_KOREA,
  SPEC_DEST_CHINA,

  SPEC_DEST_NUM
} SPEC_DEST;

typedef enum {
  IMAGE_OBJ_01_CHR, // Character data for the upper screen
  IMAGE_OBJ_01_SCR, // Screen data for the upper screen
  IMAGE_OBJ_02_CHR, // Character data for the lower screen
  IMAGE_OBJ_02_SCR, // Screen data for the lower screen
  IMAGE_OBJ_PAL,    // Palette data

  IMAGE_OBJ_NUM
} IMAGE_OBJ_INDEX;

static u8 *OSi_LoadImage(SPEC_DEST dest, IMAGE_OBJ_INDEX index, u32 *p_size);
void OSi_WaitVBlank(void);
void OSi_VBlankIntr(void);
void OSi_PrepareAttention(void);
void OSi_ShowAttention(void);

static u8 *OSi_LoadImage(SPEC_DEST dest, IMAGE_OBJ_INDEX index, u32 *p_size) {
  extern u8 _binary_attention_limited_01nbfc_bin[];
  extern u8 _binary_attention_limited_01nbfc_bin_end[];
  extern u8 _binary_attention_limited_01nbfs_bin[];
  extern u8 _binary_attention_limited_01nbfs_bin_end[];
  extern u8 _binary_attention_limited_02nbfc_bin[];
  extern u8 _binary_attention_limited_02nbfc_bin_end[];
  extern u8 _binary_attention_limited_02nbfs_bin[];
  extern u8 _binary_attention_limited_02nbfs_bin_end[];
  extern u8 _binary_attention_limited_nbfp_bin[];
  extern u8 _binary_attention_limited_nbfp_bin_end[];

  extern u8 _binary_attention_limited_korea_01nbfc_bin[];
  extern u8 _binary_attention_limited_korea_01nbfc_bin_end[];
  extern u8 _binary_attention_limited_korea_01nbfs_bin[];
  extern u8 _binary_attention_limited_korea_01nbfs_bin_end[];
  extern u8 _binary_attention_limited_korea_02nbfc_bin[];
  extern u8 _binary_attention_limited_korea_02nbfc_bin_end[];
  extern u8 _binary_attention_limited_korea_02nbfs_bin[];
  extern u8 _binary_attention_limited_korea_02nbfs_bin_end[];
  extern u8 _binary_attention_limited_korea_nbfp_bin[];
  extern u8 _binary_attention_limited_korea_nbfp_bin_end[];

  extern u8 _binary_attention_limited_china_01nbfc_bin[];
  extern u8 _binary_attention_limited_china_01nbfc_bin_end[];
  extern u8 _binary_attention_limited_china_01nbfs_bin[];
  extern u8 _binary_attention_limited_china_01nbfs_bin_end[];
  extern u8 _binary_attention_limited_china_02nbfc_bin[];
  extern u8 _binary_attention_limited_china_02nbfc_bin_end[];
  extern u8 _binary_attention_limited_china_02nbfs_bin[];
  extern u8 _binary_attention_limited_china_02nbfs_bin_end[];
  extern u8 _binary_attention_limited_china_nbfp_bin[];
  extern u8 _binary_attention_limited_china_nbfp_bin_end[];

  static u8 *ptr_table[SPEC_DEST_NUM][IMAGE_OBJ_NUM] = {
      {_binary_attention_limited_01nbfc_bin,
       _binary_attention_limited_01nbfs_bin,
       _binary_attention_limited_02nbfc_bin,
       _binary_attention_limited_02nbfs_bin,
       _binary_attention_limited_nbfp_bin},
      {_binary_attention_limited_korea_01nbfc_bin,
       _binary_attention_limited_korea_01nbfs_bin,
       _binary_attention_limited_korea_02nbfc_bin,
       _binary_attention_limited_korea_02nbfs_bin,
       _binary_attention_limited_korea_nbfp_bin},
      {_binary_attention_limited_china_01nbfc_bin,
       _binary_attention_limited_china_01nbfs_bin,
       _binary_attention_limited_china_02nbfc_bin,
       _binary_attention_limited_china_02nbfs_bin,
       _binary_attention_limited_china_nbfp_bin}};

  static u8 *ptr_end_table[SPEC_DEST_NUM][IMAGE_OBJ_NUM] = {
      {_binary_attention_limited_01nbfc_bin_end,
       _binary_attention_limited_01nbfs_bin_end,
       _binary_attention_limited_02nbfc_bin_end,
       _binary_attention_limited_02nbfs_bin_end,
       _binary_attention_limited_nbfp_bin_end},
      {_binary_attention_limited_korea_01nbfc_bin_end,
       _binary_attention_limited_korea_01nbfs_bin_end,
       _binary_attention_limited_korea_02nbfc_bin_end,
       _binary_attention_limited_korea_02nbfs_bin_end,
       _binary_attention_limited_korea_nbfp_bin_end},
      {_binary_attention_limited_china_01nbfc_bin_end,
       _binary_attention_limited_china_01nbfs_bin_end,
       _binary_attention_limited_china_02nbfc_bin_end,
       _binary_attention_limited_china_02nbfs_bin_end,
       _binary_attention_limited_china_nbfp_bin_end}};

  if (p_size) {
    *p_size = (u32)(ptr_end_table[dest][index] - ptr_table[dest][index]);
  }

  return (u8 *)ptr_table[dest][index];
}

void OSi_WaitVBlank(void) {
#if 0
    OS_WaitIrq(TRUE, OS_IE_V_BLANK);
#else

  while (1) {
    if (OS_GetIrqCheckFlag() & OS_IE_V_BLANK)
      break;
  }
  OS_ClearIrqCheckFlag(OS_IE_V_BLANK);
#endif
}

void OSi_VBlankIntr(void) { OS_SetIrqCheckFlag(OS_IE_V_BLANK); }

SDK_WEAK_SYMBOL void OS_ShowAttentionOfLimitedRom(void) {

  OSi_PrepareAttention();

  {

    u8 flag = *(u8 *)(HW_ROM_HEADER_BUF + 0x1d);
    SPEC_DEST dest;
    u32 plt_size;
    void *data_plt;

    if (flag & 0x40) {
      dest = SPEC_DEST_KOREA;
    } else if (flag & 0x80) {
      dest = SPEC_DEST_CHINA;
    } else {
      dest = SPEC_DEST_NONE;
    }

    MI_UncompressLZ16(OSi_LoadImage(dest, IMAGE_OBJ_01_CHR, NULL),
                      (u32 *)HW_BG_VRAM);
    MI_UncompressLZ16(OSi_LoadImage(dest, IMAGE_OBJ_01_SCR, NULL),
                      (u32 *)(HW_BG_VRAM + 0xf000));

    MI_UncompressLZ16(OSi_LoadImage(dest, IMAGE_OBJ_02_CHR, NULL),
                      (u32 *)HW_DB_BG_VRAM);
    MI_UncompressLZ16(OSi_LoadImage(dest, IMAGE_OBJ_02_SCR, NULL),
                      (u32 *)(HW_DB_BG_VRAM + 0xf000));

    data_plt = OSi_LoadImage(dest, IMAGE_OBJ_PAL, &plt_size);

    SVC_CpuCopyFast(data_plt, (u32 *)(HW_BG_PLTT), plt_size);
    SVC_CpuCopyFast(data_plt, (u32 *)(HW_DB_BG_PLTT), plt_size);
  }

  OSi_ShowAttention();
}

void OSi_PrepareAttention(void) {
  u16 gx_powcnt = reg_GX_POWCNT;

  reg_GX_DISPCNT = 0;
  reg_GXS_DB_DISPCNT = 0;

  if (!(gx_powcnt & REG_GX_POWCNT_LCD_MASK)) {

    SVC_WaitByLoop(HW_CPU_CLOCK_ARM9 / 40);
  }

  reg_GX_POWCNT = (u16)(REG_GX_POWCNT_DSEL_MASK | REG_GX_POWCNT_E2DG_MASK |
                        REG_GX_POWCNT_E2DGB_MASK | REG_GX_POWCNT_LCD_MASK);

  reg_GX_MASTER_BRIGHT = (u16)(1 << REG_GX_MASTER_BRIGHT_E_MOD_SHIFT);
  reg_GXS_DB_MASTER_BRIGHT = reg_GX_MASTER_BRIGHT;

  reg_GX_VRAMCNT_A =
      (u8)((1 << REG_GX_VRAMCNT_A_MST_SHIFT) | (1 << REG_GX_VRAMCNT_A_E_SHIFT));
  reg_G2_BG0CNT =
      (u16)((GX_BG_SCRSIZE_TEXT_256x256 << REG_G2_BG0CNT_SCREENSIZE_SHIFT) |
            (GX_BG_COLORMODE_16 << REG_G2_BG0CNT_COLORMODE_SHIFT) |
            (GX_BG_SCRBASE_0xf000 << REG_G2_BG0CNT_SCREENBASE_SHIFT) |
            (GX_BG_CHARBASE_0x00000 << REG_G2_BG0CNT_CHARBASE_SHIFT) |
            (0 << REG_G2_BG0CNT_PRIORITY_SHIFT));
  reg_G2_BG0OFS = 0;
  reg_GX_DISPCNT |= ((GX_BGMODE_0 << REG_GX_DISPCNT_BGMODE_SHIFT) |
                     (GX_PLANEMASK_BG0 << REG_GX_DISPCNT_DISPLAY_SHIFT));

  reg_GX_VRAMCNT_C =
      (u8)((4 << REG_GX_VRAMCNT_C_MST_SHIFT) | (1 << REG_GX_VRAMCNT_C_E_SHIFT));
  reg_G2S_DB_BG0CNT =
      (u16)((GX_BG_SCRSIZE_TEXT_256x256 << REG_G2S_DB_BG0CNT_SCREENSIZE_SHIFT) |
            (GX_BG_COLORMODE_16 << REG_G2S_DB_BG0CNT_COLORMODE_SHIFT) |
            (GX_BG_SCRBASE_0xf000 << REG_G2S_DB_BG0CNT_SCREENBASE_SHIFT) |
            (GX_BG_CHARBASE_0x00000 << REG_G2S_DB_BG0CNT_CHARBASE_SHIFT) |
            (0 << REG_G2S_DB_BG0CNT_PRIORITY_SHIFT));
  reg_G2S_DB_BG0OFS = 0;
  reg_GXS_DB_DISPCNT |= ((GX_BGMODE_0 << REG_GXS_DB_DISPCNT_BGMODE_SHIFT) |
                         ((GX_PLANEMASK_BG0 | GX_PLANEMASK_OBJ)
                          << REG_GXS_DB_DISPCNT_DISPLAY_SHIFT));
}

void OSi_ShowAttention(void) {

  reg_GX_DISPCNT |= (u32)(GX_DISPMODE_GRAPHICS << REG_GX_DISPCNT_MODE_SHIFT);
  reg_GXS_DB_DISPCNT |= (u32)(REG_GXS_DB_DISPCNT_MODE_MASK);

  reg_GX_DISPSTAT |= REG_GX_DISPSTAT_VBI_MASK;
  OS_SetIrqFunction(OS_IE_V_BLANK, OSi_VBlankIntr);

  while (1) {
    OSi_WaitVBlank();
  }
}

#endif // #ifdef SDK_TWLLTD

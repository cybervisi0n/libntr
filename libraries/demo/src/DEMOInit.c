#include <nitro.h>
#include <nitro/demo/DEMOInit.h>
#include <nitro/demo/DEMOKey.h>

void DEMOInitCommon(void) {
  OS_Init();
  FX_Init();
  GX_Init();
  GX_DispOff();
  GXS_DispOff();
  OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
  OS_EnableIrqMask(OS_IE_V_BLANK);
  OS_EnableIrq();
  DEMOReadKey();
  GX_VBlankIntr(TRUE);
}

void DEMOInitVRAM(void) {
  GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
  MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
  GX_DisableBankForLCDC();
  MI_CpuFillFast((void *)HW_OAM, 192, HW_OAM_SIZE);
  MI_CpuClearFast((void *)HW_PLTT, HW_PLTT_SIZE);
  MI_CpuFillFast((void *)HW_DB_OAM, 192, HW_DB_OAM_SIZE);
  MI_CpuClearFast((void *)HW_DB_PLTT, HW_DB_PLTT_SIZE);
}

void DEMOInitDisplayBG0Only(void) {
  GX_SetBankForBG(GX_VRAM_BG_128_A);
  GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_2D);
  GX_SetVisiblePlane(GX_PLANEMASK_BG0);
  G2_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_256,
                   GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000,
                   GX_BG_EXTPLTT_01);
  G2_SetBG0Priority(0);
  G2_BG0Mosaic(FALSE);
}

void DEMOInitDisplaySubBG0Only(void) {
  GX_SetBankForSubBG(GX_VRAM_SUB_BG_128_C);
  GXS_SetGraphicsMode(GX_BGMODE_0);
  GXS_SetVisiblePlane(GX_PLANEMASK_BG0);
  G2S_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_256,
                    GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000,
                    GX_BG_EXTPLTT_01);
  G2S_SetBG0Priority(0);
  G2S_BG0Mosaic(FALSE);
}

void DEMOInitDisplayBG2Only(void) {
  GX_SetBankForBG(GX_VRAM_BG_128_A);
  GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_2, GX_BG0_AS_2D);
  GX_SetVisiblePlane(GX_PLANEMASK_BG2);
  G2_SetBG2ControlAffine(GX_BG_SCRSIZE_AFFINE_128x128, GX_BG_AREAOVER_XLU,
                         GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000);
  G2_SetBG2Priority(0);
  G2_BG2Mosaic(FALSE);
}

void DEMOInitDisplaySubBG2Only(void) {
  GX_SetBankForSubBG(GX_VRAM_SUB_BG_128_C);
  GXS_SetGraphicsMode(GX_BGMODE_2);
  GXS_SetVisiblePlane(GX_PLANEMASK_BG2);
  G2S_SetBG2ControlAffine(GX_BG_SCRSIZE_AFFINE_128x128, GX_BG_AREAOVER_XLU,
                          GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000);
  G2S_SetBG2Priority(0);
  G2S_BG2Mosaic(FALSE);
}

void DEMOInitDisplayOBJOnly(void) {
  GX_SetBankForOBJ(GX_VRAM_OBJ_128_A);
  GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_2D);
  GX_SetVisiblePlane(GX_PLANEMASK_OBJ);
  GX_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);
}

void DEMOInitDisplaySubOBJOnly(void) {
  GX_SetBankForSubOBJ(GX_VRAM_SUB_OBJ_128_D);
  GXS_SetGraphicsMode(GX_BGMODE_0);
  GXS_SetVisiblePlane(GX_PLANEMASK_OBJ);
  GXS_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);
}

void DEMOInitDisplay3D(void) {
  G3X_Init();
  G3X_InitMtxStack();
  GX_SetBankForTex(GX_VRAM_TEX_0_A);
  GX_SetBankForBG(GX_VRAM_BG_128_D);
  GX_SetBankForTexPltt(GX_VRAM_TEXPLTT_0123_E);
  GX_SetBankForOBJ(GX_VRAM_OBJ_32_FG);
  GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_4, GX_BG0_AS_3D);
  GX_SetVisiblePlane(GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_OBJ);
  G2_SetBG0Priority(1);
  G2_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16,
                   GX_BG_SCRBASE_0x3800, GX_BG_CHARBASE_0x00000,
                   GX_BG_EXTPLTT_01);
  G2_SetBG1Priority(0);
  G2_BG1Mosaic(FALSE);
  G3X_SetShading(GX_SHADING_TOON);
  G3X_AntiAlias(TRUE);
  G2_BlendNone();
  G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);
  G3X_SetClearColor(GX_RGB(0, 0, 0), 31, 0x7fff, 63, FALSE);
  G3_ViewPort(0, 0, 255, 191);
  {
    fx32 right = FX32_ONE;
    fx32 top = FX32_ONE * 3 / 4;
    fx32 near = FX32_ONE;
    fx32 far = FX32_ONE * 400;

    G3_Perspective(FX32_SIN30, FX32_COS30, FX32_ONE * 4 / 3, near, far, NULL);
    G3_StoreMtx(0);
  }
}

void DEMOInit(void) {
  DEMOInitCommon();
  DEMOInitVRAM();
  DEMOInitDisplay3D();
}

void DEMOStartDisplay(void) {
  OS_WaitVBlankIntr();
  GX_DispOn();
  GXS_DispOn();
}

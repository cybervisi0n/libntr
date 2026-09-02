#include <nitro.h>
#include <nitro/demo/DEMO.h>

s16 gCubeGeometry[3 * 8] = {
    FX16_ONE,  FX16_ONE,  FX16_ONE, FX16_ONE,  FX16_ONE,  -FX16_ONE,
    FX16_ONE,  -FX16_ONE, FX16_ONE, FX16_ONE,  -FX16_ONE, -FX16_ONE,
    -FX16_ONE, FX16_ONE,  FX16_ONE, -FX16_ONE, FX16_ONE,  -FX16_ONE,
    -FX16_ONE, -FX16_ONE, FX16_ONE, -FX16_ONE, -FX16_ONE, -FX16_ONE};

static void vtx(int idx) {
  G3_Vtx(gCubeGeometry[idx * 3], gCubeGeometry[idx * 3 + 1],
         gCubeGeometry[idx * 3 + 2]);
}

static void quad(int idx0, int idx1, int idx2, int idx3) {
  vtx(idx0);
  vtx(idx1);
  vtx(idx2);
  vtx(idx3);
}

void NitroMain(void) {
  u16 Rotate = 0;
  DEMOInitCommon();
  DEMOInitVRAM();
  DEMOInitDisplay3D();
  DEMOStartDisplay();
  while (1) {
    G3X_Reset();
    Rotate += 256;
    {
      VecFx32 Eye = {0, 0, FX32_ONE};
      VecFx32 at = {0, 0, 0};
      VecFx32 vUp = {0, FX32_ONE, 0};
      G3_LookAt(&Eye, &vUp, &at, NULL);
    }
    G3_PushMtx();
    {
      fx16 s = FX_SinIdx(Rotate);
      fx16 c = FX_CosIdx(Rotate);

      G3_Translate(0, 0, -5 * FX32_ONE);

      G3_RotX(s, c);
      G3_RotY(s, c);
      G3_RotZ(s, c);
    }
    G3_MaterialColorDiffAmb(GX_RGB(31, 31, 31), GX_RGB(16, 16, 16), TRUE);
    G3_MaterialColorSpecEmi(GX_RGB(16, 16, 16), GX_RGB(0, 0, 0), FALSE);
    G3_PolygonAttr(GX_LIGHTMASK_NONE, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 0,
                   31, 0);
    G3_Begin(GX_BEGIN_QUADS);
    {
      quad(2, 0, 4, 6);
      quad(7, 5, 1, 3);
      quad(6, 4, 5, 7);
      quad(3, 1, 0, 2);
      quad(5, 4, 0, 1);
      quad(6, 7, 3, 2);
    }
    G3_End();
    G3_PopMtx(1);
    G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);
    OS_WaitVBlankIntr();
  }
}

void VBlankIntr(void) { OS_SetIrqCheckFlag(OS_IE_V_BLANK); }

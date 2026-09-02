#include <nitro/demo/DEMOUtility.h>

#define DEMO_INTENSITY_DF 23
#define DEMO_INTENSITY_AM 8
#define DEMO_INTENSITY_SP 31

static const GXRgb DEMO_DIFFUSE_COL =
    GX_RGB(DEMO_INTENSITY_DF, DEMO_INTENSITY_DF, DEMO_INTENSITY_DF);
static const GXRgb DEMO_AMBIENT_COL =
    GX_RGB(DEMO_INTENSITY_AM, DEMO_INTENSITY_AM, DEMO_INTENSITY_AM);
static const GXRgb DEMO_SPECULAR_COL =
    GX_RGB(DEMO_INTENSITY_SP, DEMO_INTENSITY_SP, DEMO_INTENSITY_SP);
static const GXRgb DEMO_EMISSION_COL = GX_RGB(0, 0, 0);
static const u32 LIGHTING_L_DOT_S_SHIFT = 8;

void DEMO_Set3DDefaultMaterial(BOOL bUsediffuseAsVtxCol,
                               BOOL bUseShininessTbl) {
  G3_MaterialColorDiffAmb(DEMO_DIFFUSE_COL, DEMO_AMBIENT_COL,
                          bUsediffuseAsVtxCol);
  G3_MaterialColorSpecEmi(DEMO_SPECULAR_COL, DEMO_EMISSION_COL,
                          bUseShininessTbl);
}

#include <nitro/code32.h>
void DEMO_Set3DDefaultShininessTable() {
  u8 i;
  u32 shininess_table[32];
  u8 *pShininess = (u8 *)&shininess_table[0];
  const u8 tableLength = 32 * sizeof(u32);
  for (i = 0; i < tableLength - 1; i++) {
    pShininess[i] =
        (u8)(((s64)(i * 2 + 1) * (i * 2 + 1) * (i * 2 + 1) * (i * 2 + 1)) >>
             (LIGHTING_L_DOT_S_SHIFT * (4 - 1)));
  }
  pShininess[tableLength - 1] = 0xFF;
  G3_Shininess(&shininess_table[0]);
}
#include <nitro/codereset.h>

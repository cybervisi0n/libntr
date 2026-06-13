#include <nitro/gx/g3.h>
#include <nitro/hw/X86/ioreg.h>
#include <nitro/hw/X86/mmap_shared.h>
#include <nitro/fx/fx.h>
#include <nitro/fx/fx_const.h>
#include <simulator/glad/glad.h>

#ifndef SIM_G3_HANDLER_H
#define SIM_G3_HANDLER_H
//Type definitions
typedef struct G3SIM_Matrix_t { fx32 nums[4][4] } G3SIM_Matrix_t;

typedef struct {
    G3SIM_Matrix_t mtxs[32];
    u8 stackPtr;
} G3SIM_MatrixStack_t;

typedef struct {
    u8 ops[4];
} G3SIM_CommandBlock_t;

typedef struct {
    u32 textureOffset;
    u8 repeatS;
    u8 repeatT;
    u8 flipS;
    u8 flipT;
    u32 textureSSize;
    u32 textureTSize;
    u8 textureFormat;
    u8 color0;
    u8 texCordTransform;
} G3SIM_TexImageParam_t;

typedef struct {
    u8 lightFlag;
    u8 polygonMode;
    u8 cullMode;
    u8 translucentDepth;
    u8 renderFar;
    u8 singleDotDisp;
    u8 depthTest;
    u8 fogEnable;
    float alpha;
    u8 alphaInt;
    u8 polygonId;
} G3SIM_PolygonAttr_t;

typedef struct {
    u8 diffuseRed;
    u8 diffuseGreen;
    u8 diffuseBlue;
    u8 setVertexColor;
    u8 ambRed;
    u8 ambGreen;
    u8 ambBlue;
} G3SIM_DiffAmb_t;

typedef struct {
    union {
        struct {
            fx32 x;
            fx32 y;
            fx32 z;
            fx32 w;
        };
        fx32 nums[4];
    };
    // TODO: eventually keep these in fixed point format
    GLfloat s;
    GLfloat t;
    GLfloat r;
    GLfloat g;
    GLfloat b;
    GLfloat a;
} G3SIM_FxVtx_t;

extern const char * G3SIM_VertexShader;
extern const char * G3SIM_FragmentShader;
extern const char * G2SIM_VertexShader;
extern const char * G2SIM_FragmentShader;


//Global Variables
extern GXMtxMode s_curMtxMode;
extern GXBegin s_primType;
extern G3SIM_FxVtx_t s_g3PolygonVerts[4];
extern u8 s_G3numInPoly;
extern u8 s_G3drawPoly;
extern G3SIM_TexImageParam_t s_texImageParam;
extern u32 s_texPlttBase;
extern GLfloat s_g3NextTexCoordS;
extern GLfloat s_g3NextTexCoordT;
extern u32 s_G3triStripTriNum;
extern u32 s_G3quadStripTriNum;
extern u8 s_g3CurColor[3];
extern G3SIM_PolygonAttr_t s_curPolygonAttr;

//Procedures

//GPU Commands
void G3SIM_Begin(GXBegin type);
void G3SIM_BoxTest(s16 x, s16 y, s16 z, s16 width, s16 height, s16 depth);
void G3SIM_Color(u16 color);
void G3SIM_DecodeTex4(u8* vramTex, u16* colorAddr, u8* out, u32 s, u32 t);
void G3SIM_DecodeTex16(u8* vramTex, u16* colorAddr, u8* out, u32 s, u32 t);
void G3SIM_DecodeTex256(u8* vramTex, u16* colorAddr, u8* out, u32 s, u32 t);
void G3SIM_DecodeTexA3I5(u8* vramTex, u16* colorAddr, u8* out, u32 s, u32 t);
void G3SIM_DecodeTexA5I3(u8* vramTex, u16* colorAddr, u8* out, u32 s, u32 t);
void G3SIM_DecodeTexDirect(u8* vramTex, u8* out, u32 s, u32 t);
void G3SIM_DiffAmb(u32 data);
void G3SIM_Identity();
void G3SIM_LightColor(u32 data);
void G3SIM_LightVector(u32 data);
void G3SIM_MtxLoad43(MtxFx43 * mtx);
void G3SIM_MtxLoad44(MtxFx44 * mtx);
void G3SIM_MtxMode(GXMtxMode mode);
void G3SIM_MtxMult33(MtxFx33 * mtx);
void G3SIM_MtxMult43(MtxFx43 * mtx);
void G3SIM_MtxMult44(MtxFx44 * mtx);
void G3SIM_MtxPop(u8 num);
void G3SIM_MtxPush();
void G3SIM_MtxRestore(u8 idx);
void G3SIM_MtxScale(fx32 * scale);
void G3SIM_MtxStore(u8 idx);
void G3SIM_MtxTranslate(fx32* trans);
void G3SIM_Normal(u32 data);
void G3SIM_PolygonAttr(u32 data);
void G3SIM_SpecEmi(u32 data);
void G3SIM_SwapBuffers(u8 depthBufferMode);
void G3SIM_TexCoord(u32 data);
void G3SIM_TexImageParam(u32 data);
void G3SIM_TexPlttBase(u32 data);
void G3SIM_Vtx(s16 x, s16 y, s16 z);

// Not GPU Commands
int G3SIM_ClipPolygon(fx32 verts[][4], int nVerts, int clipStart, BOOL attribs);
u8 G3SIM_SwapBuffersCalledThisFrame();
void G3SIM_ResetSwapBuffersCall();
void G3SIM_SubmitPolygon(G3SIM_FxVtx_t * fxVerts, int numVerts);
#endif
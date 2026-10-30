#include "nitro/fx/fx.h"
#include <pthread.h>
#include <SDL2/SDL.h>

#ifdef SDK_BUILD_NX
#include <glad/glad.h>
#else
#include <simulator/glad/glad.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include <nitro/gx/g3.h>
#include <nitro/hw/X86/ioreg.h>
#include <nitro/hw/X86/mmap_shared.h>

#include <simulator/drawmsg.h>
#include <simulator/queue.h>
#include <simulator/sim_g3_CommandHandler.hpp>
#include <simulator/sim_g3_GlobalState.hpp>
#include <simulator/sim_g3_Utilities.hpp>
#include <simulator/sim.h>

#ifdef SDK_TRACY_ENABLE
#include "tracy/TracyC.h"
#endif

static void CommonDebugCommandHandling(draw_command_type_t aCommand);


void ClipSegment(fx32* outbuf, fx32 * vin, fx32 * vout, int comp, s32 plane, BOOL attribs)
{
    s64 factor_num = (s32)vin[3] - (plane * (s32)vin[comp]);
    s32 factor_den = factor_num - ((s32)vout[3] - (plane*(s32)vout[comp]));

#define INTERPOLATE(var)  { outbuf[var] = (vin[var] + ((vout[var] - vin[var]) * factor_num) / factor_den); }

    if (comp != 0) INTERPOLATE(0);
    if (comp != 1) INTERPOLATE(1);
    if (comp != 2) INTERPOLATE(2);
    INTERPOLATE(3);
    outbuf[comp] = plane*outbuf[3];

    //if (attribs)
    //{
    //    INTERPOLATE(Color[0]);
    //    INTERPOLATE(Color[1]);
    //    INTERPOLATE(Color[2]);
//
    //    INTERPOLATE(TexCoords[0]);
    //    INTERPOLATE(TexCoords[1]);
    //}

    //outbuf->Clipped = true;

#undef INTERPOLATE
}



static int ClipAgainstPlane(fx32 verts[][4], int nVerts, int clipStart, int comp, BOOL attribs)
{
    fx32 temp[10][4];
    int prev, next;
    int c = clipStart;

    if (clipStart == 2)
    {
        temp[0][0] = verts[0][0]; temp[0][1] = verts[0][1]; temp[0][2] = verts[0][2]; temp[0][3] = verts[0][3];
        temp[1][0] = verts[1][0]; temp[1][1] = verts[1][1]; temp[1][2] = verts[1][2]; temp[1][3] = verts[1][3];
    }

    for(int i = clipStart; i < nVerts; i++) {
        prev = i-1; if (prev < 0) prev = nVerts-1;
        next = i+1; if (next >= nVerts) next = 0;

        fx32 vtx[4];
        vtx[0] = verts[i][0]; vtx[1] = verts[i][1]; vtx[2] = verts[i][2]; vtx[3] = verts[i][3];
        if(vtx[comp] > vtx[3]) {
            //if ((comp == 2) && (!(gpu.CurPolygonAttr & (1<<12)))) return 0;  //I think this is the W buffering thing

            fx32 * vprev = verts[prev];
            if(vprev[comp] <= vprev[3]) {
                //ClipSegment((fx32*)&temp[c], &vtx[0], vprev, comp, 1, attribs);
                c++;
            }

            fx32 * vnext = verts[next];
            if(vnext[comp] <= vnext[3]) {
                //ClipSegment((fx32*)&temp[c], &vtx[0], vnext, comp, 1, attribs);
                c++;
            }
        } else {
            memcpy(&temp[c++], vtx, sizeof(fx32)*4);
        }
    }

    nVerts = c;
    c = clipStart;
    for(int i=clipStart; i < nVerts; i++) {
        prev = i-1; if (prev < 0) prev = nVerts-1;
        next = i+1; if (next >= nVerts) next = 0;

        fx32 vtx[4];
        vtx[0] = temp[i][0]; vtx[1] = temp[i][1]; vtx[2] = temp[i][2]; vtx[3] = temp[i][3];
        if(vtx[comp] < -vtx[3]) {
            fx32 * vprev = temp[prev];
            if(vprev[comp] >= -vprev[3]) {
                //ClipSegment((fx32*)&verts[c], &vtx[0], vprev, comp, -1, attribs);
                c++;
            }

            fx32 * vnext = temp[next];
            if(vnext[comp] >= -vnext[3]) {
                //ClipSegment((fx32*)&verts[c], &vtx[0], vnext, comp, -1, attribs);
                c++;
            }
        } else {
            memcpy(&verts[c++], vtx, sizeof(fx32)*4);
        }
    }

    return c;
}

int G3SIM_ClipPolygon(fx32 verts[][4], int nVerts, int clipStart, BOOL attribs)
{
    // Z clipping
    nVerts = ClipAgainstPlane(verts, nVerts, clipStart, 2, attribs);

    // Y clipping
    nVerts = ClipAgainstPlane(verts, nVerts, clipStart, 1, attribs);

    // X clipping
    nVerts = ClipAgainstPlane(verts, nVerts, clipStart, 0, attribs);
    return nVerts;
}

static void calculateVtxFx(fx32 * x, fx32 * y, fx32 * z, fx32 * w)
{
    fx32 myW = FX32_ONE;
    fx32 oldVec[4];
    fx32 newVec[4];
    fx32 curSum;

    oldVec[0] = *x;
    oldVec[1] = *y;
    oldVec[2] = *z;
    oldVec[3] = FX32_ONE;

    auto& g3State = SIM::G3::GetGlobalState();
    MtxFx44 clipMatrix = g3State.GetClipMatrix();

    for( int i=0; i < 4; i++ )
    {
        curSum = 0;
        for(int j=0; j < 4; j++ )
        {
            curSum = curSum + FX_Mul(oldVec[j], clipMatrix.m[j][i]);
        }
        newVec[i] = curSum;
    }
    *x = newVec[0];
    *y = newVec[1];
    *z = newVec[2];
    *w = newVec[3];
}

//Begin vertices
namespace SIM::G3 {
void Begin(GXBegin type)
{
    auto& g3State = GetGlobalState();
    char debugMessage[50] = {0};
    char * typeString = "";
    switch(type) {
        case GX_BEGIN_TRIANGLES:
            typeString = "SepTri";
            break;
        case GX_BEGIN_QUADS:
            typeString = "SepQuad";
            break;
        case GX_BEGIN_TRIANGLE_STRIP:
            typeString = "TriStrip";
            break;
        case GX_BEGIN_QUAD_STRIP:
            typeString = "QuadStrip";
            break;
    }
    snprintf(debugMessage, 50, "GX_Begin %s", typeString);

    glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_OTHER, 1337, GL_DEBUG_SEVERITY_NOTIFICATION, 50, debugMessage);

    g3State.SetPrimitiveType(type);
    g3State.SetCurrentIndexInPolygon(0);
    g3State.SetPolygonReady(false);
    g3State.SetTriStripTriangleIndex(0);
    g3State.SetQuadStripQuadIndex(0);
}

//Box Test
void BoxTest(s16 x, s16 y, s16 z, s16 width, s16 height, s16 depth)
{
    fx32 cube[8][4];
    fx32 face[10][4];

    s16 x0 = x;
    s16 y0 = y;
    s16 z0 = z;
    s16 x1 = width;
    s16 y1 = height;
    s16 z1 = depth;

    int res = 0;

    x1 += x0;
    y1 += y0;
    z1 += z0;

    cube[0][0] = x0; cube[0][1] = y0; cube[0][2] = z0;
    cube[1][0] = x1; cube[1][1] = y0; cube[1][2] = z0;
    cube[2][0] = x1; cube[2][1] = y1; cube[2][2] = z0;
    cube[3][0] = x0; cube[3][1] = y1; cube[3][2] = z0;
    cube[4][0] = x0; cube[4][1] = y1; cube[4][2] = z1;
    cube[5][0] = x0; cube[5][1] = y0; cube[5][2] = z1;
    cube[6][0] = x1; cube[6][1] = y0; cube[6][2] = z1;
    cube[7][0] = x1; cube[7][1] = y1; cube[7][2] = z1;

    for(int i=0; i < 8; i++) {
        calculateVtxFx(&cube[i][0], &cube[i][1], &cube[i][2], &cube[i][3]);
    }

    #define SetupFace(_face0,_face1,_face2,_face3) \
        face[0][0] = cube[_face0][0]; face[0][1] = cube[_face0][1]; face[0][2] = cube[_face0][2]; face[0][3] = cube[_face0][3]; \
        face[1][0] = cube[_face1][0]; face[1][1] = cube[_face1][1]; face[1][2] = cube[_face1][2]; face[1][3] = cube[_face1][3]; \
        face[2][0] = cube[_face2][0]; face[2][1] = cube[_face2][1]; face[2][2] = cube[_face2][2]; face[2][3] = cube[_face2][3]; \
        face[3][0] = cube[_face3][0]; face[3][1] = cube[_face3][1]; face[3][2] = cube[_face3][2]; face[3][3] = cube[_face3][3];

    // front face (-Z)
    SetupFace(0, 1, 2, 3);
    res += G3SIM_ClipPolygon(face, 4, 0, FALSE);

    // back face (+Z)
    SetupFace(4, 5, 6, 7);
    res += G3SIM_ClipPolygon(face, 4, 0, FALSE);

    // left face (-X)
    SetupFace(0, 3, 4, 5);
    res += G3SIM_ClipPolygon(face, 4, 0, FALSE);

    // right face (+X)
    SetupFace(1, 2, 7, 6);
    res += G3SIM_ClipPolygon(face, 4, 0, FALSE);

    // bottom face (-Y)
    SetupFace(0, 1, 6, 5);
    res += G3SIM_ClipPolygon(face, 4, 0, FALSE);

    // top face (+Y)
    SetupFace(2, 3, 4, 7);
    res += G3SIM_ClipPolygon(face, 4, 0, FALSE);

    if(res > 0){
        s_reg_G3X_GXSTAT |= (1<<1);
        return;
    }
    
    s_reg_G3X_GXSTAT &= 0b11111111111111111111111111111101;
    return;
}

//Set the color
void Color(u16 color)
{
    RGBBytes rgbColor;

    SIM_u16ToRGB(color, &rgbColor.r, &rgbColor.g, &rgbColor.b);
    GetGlobalState().SetVertexColor(rgbColor);
}

void DiffAmb(u32 data)
{
    auto& g3State = GetGlobalState();

    RGBBytes diffuse;
    RGBBytes ambient;

    diffuse.r = (data & 0b11111);
    diffuse.g = (data & 0b1111100000) >> 5;
    diffuse.b = (data & 0b111110000000000) >> 10;

    diffuse.r <<=3;
    diffuse.g <<=3;
    diffuse.b <<=3;

    bool setVtxColor = ((data & 0b1000000000000000) >> 15) != 0;

    ambient.r = (data & 0b111110000000000000000) >> 16;
    ambient.g = (data & 0b11111000000000000000000000) >> 21;
    ambient.b = (data & 0b1111100000000000000000000000000) >> 26;

    ambient.r <<=3;
    ambient.g <<=3;
    ambient.b <<=3;

    if(setVtxColor) {
        g3State.SetVertexColor(diffuse);
    }

    g3State.SetDiffuseColor(diffuse);
    g3State.SetAmbientColor(ambient);
}

//Copy the identity matrix to the current matrix
void Identity()
{
    GetGlobalState().IdentityMatrix();
}

void LightColor(u32 data)
{
    RGBBytes color;
    color.r = (data & 0b11111);
    color.g = (data & 0b1111100000) >> 5;
    color.b = (data & 0b111110000000000) >> 10;
    color.r <<= 3;
    color.g <<= 3;
    color.b <<= 3;
    u8 lightNum = (data & 0b11000000000000000000000000000000) >> 30;

    GetGlobalState().SetLightColor(lightNum, color);
}

void LightVector(u32 data)
{
    u8 lightVecX = (data & 0b1111111111);
    u8 lightVecXSign = (lightVecX & 0b1000000000) >> 9;
    lightVecX = lightVecX & 0b111111111;
    u8 lightVecY = (data & 0b11111111110000000000) >> 10;
    u8 lightVecYSign = (lightVecY & 0b1000000000) >> 9;
    lightVecY = lightVecY & 0b111111111;
    u8 lightVecZ = (data & 0b111111111100000000000000000000) >> 20;
    u8 lightVecZSign = (lightVecZ & 0b1000000000) >> 9;
    lightVecZ = lightVecZ & 0b111111111;
    u8 lightNum = (data & 0b11000000000000000000000000000000) >> 30;

    FloatVec3 vector = {};

    // Should get us a number from -1 to 1
    vector.x = (float)lightVecX / 511.0f;
    if(lightVecXSign) {
        vector.x = vector.x * -1.0f;
    }
    vector.y = (float)lightVecY / 511.0f;
    if(lightVecYSign) {
        vector.y = vector.y * -1.0f;
    }
    vector.z = (float)lightVecZ / 511.0f;
    if(lightVecZSign) {
        vector.z = vector.z * -1.0f;
    }

    GetGlobalState().SetLightVector(lightNum, vector);
}

//Load Matrix
void MtxLoad43(MtxFx43 * mtx)
{
    // Convert to a 4x4 matrix and put FX32_ONE in the last row/column
    MtxFx44 matrix = {};
    MtxFx43 temp43 = *mtx;
    for(int i=0; i < 4; i++) {
        for(int j=0; j < 3; j++) {
            matrix.m[i][j] = temp43.m[i][j];
        }

        matrix.m[i][3] = 0;
    }

    matrix.m[3][3] = FX32_ONE;

    MtxLoad44(&matrix);
}

void MtxLoad44(MtxFx44 * mtx)
{
    auto& g3State = GetGlobalState();
    MtxFx44 matrix = *mtx;

    g3State.SetCurrentMatrix(matrix);
    if(g3State.GetMatrixMode() == GX_MTXMODE_POSITION_VECTOR) {
        g3State.SetPositionMatrix(matrix);
    }
}

//Change matrix mode
void MtxMode(GXMtxMode mode)
{
    GetGlobalState().SetMatrixMode(mode);
}

//CurrentMatrix = CurrentMatrix * mtx
void MtxMult33(MtxFx33 * mtx)
{
    auto& g3State = GetGlobalState();
    MtxFx33 a;
    MtxFx44 a4x4 = g3State.GetCurrentMatrix();
    MTX_Copy44To33(&a4x4, &a);
    MtxFx33 b = *mtx;

    MtxFx33 result3x3 = Util::MultiplyMtxFx33(a, b);
    MtxFx44 result4x4 = a4x4;
    MTX_Copy33To44(&result3x3, &result4x4);
    g3State.SetCurrentMatrix(result4x4);

    if(g3State.GetMatrixMode() == GX_MTXMODE_POSITION_VECTOR) {
        a4x4 = g3State.GetPositionMatrix();
        MTX_Copy44To33(&a4x4, &a);
        result3x3 = Util::MultiplyMtxFx33(a, b);
        result4x4 = a4x4;
        MTX_Copy33To44(&result3x3, &result4x4);
        g3State.SetPositionMatrix(result4x4);
    }
}

//CurrentMatrix = CurrentMatrix * mtxs
void MtxMult43(MtxFx43 * mtx)
{
    auto& g3State = GetGlobalState();
    MtxFx44 matrix = {};
    MTX_Copy43To44(mtx, &matrix);
    matrix.m[3][3] = FX32_ONE;

    MtxFx44 result = Util::MultiplyMtxFx44(g3State.GetCurrentMatrix(), matrix);
    g3State.SetCurrentMatrix(result);

    if(g3State.GetMatrixMode() == GX_MTXMODE_POSITION_VECTOR) {
        result = Util::MultiplyMtxFx44(g3State.GetPositionMatrix(), matrix);
        g3State.SetPositionMatrix(result);
    }
}

void MtxMult44(MtxFx44 * mtx)
{
    auto& g3State = GetGlobalState();

    MtxFx44 matrix = *mtx;

    MtxFx44 result = Util::MultiplyMtxFx44(g3State.GetCurrentMatrix(), matrix);
    g3State.SetCurrentMatrix(result);

    if(g3State.GetMatrixMode() == GX_MTXMODE_POSITION_VECTOR) {
        result = Util::MultiplyMtxFx44(g3State.GetPositionMatrix(), matrix);
        g3State.SetPositionMatrix(result);
    }
}

//TODO: Keep G3X_GXSTAT up to date
void MtxPop(u8 num)
{
    auto& g3State = GetGlobalState();
    g3State.PopMatrix(num);

    return;
}

void MtxPush()
{
    auto& g3State = GetGlobalState();
    g3State.PushMatrix();

    return;
}

void MtxRestore(u8 idx)
{
    auto& g3State = GetGlobalState();
    g3State.RestoreMatrix(idx);

    return;
}

void MtxScale(fx32 * scale)
{
    fx32 s[3];

    s[0] = scale[0];
    s[1] = scale[1];
    s[2] = scale[2];
    
    auto& g3State = GetGlobalState();
    auto currentMatrix = g3State.GetCurrentMatrix();
    if(g3State.GetMatrixMode() == GX_MTXMODE_POSITION_VECTOR) {
        currentMatrix = g3State.GetPositionMatrix();
    }

    for(int i=0; i < 3; i ++)
    {
        for( int j=0; j < 3; j ++)
        {
            currentMatrix.m[i][j] = FX_Mul(s[i], currentMatrix.m[i][j]);
        }   
    }

    if(g3State.GetMatrixMode() == GX_MTXMODE_POSITION_VECTOR) {
        g3State.SetPositionMatrix(currentMatrix);
    } else {
        g3State.SetCurrentMatrix(currentMatrix);
    }
}

void MtxStore(u8 idx)
{
    auto& g3State = GetGlobalState();
    g3State.StoreMatrix(idx);

    return;
}



void MtxTranslate(fx32* trans)
{
    auto& g3State = GetGlobalState();

    fx32 s[3];
    s[0] = trans[0];
    s[1] = trans[1];
    s[2] = trans[2];

    MtxFx44 currentMatrix = g3State.GetCurrentMatrix();

    currentMatrix.m[3][0] += (FX_Mul(s[0],currentMatrix.m[0][0]) + FX_Mul(s[1],currentMatrix.m[1][0]) + FX_Mul(s[2],currentMatrix.m[2][0]));
    currentMatrix.m[3][1] += (FX_Mul(s[0],currentMatrix.m[0][1]) + FX_Mul(s[1],currentMatrix.m[1][1]) + FX_Mul(s[2],currentMatrix.m[2][1]));
    currentMatrix.m[3][2] += (FX_Mul(s[0],currentMatrix.m[0][2]) + FX_Mul(s[1],currentMatrix.m[1][2]) + FX_Mul(s[2],currentMatrix.m[2][2]));
    currentMatrix.m[3][3] += (FX_Mul(s[0],currentMatrix.m[0][3]) + FX_Mul(s[1],currentMatrix.m[1][3]) + FX_Mul(s[2],currentMatrix.m[2][3]));

    g3State.SetCurrentMatrix(currentMatrix);

    if(g3State.GetMatrixMode() == GX_MTXMODE_POSITION_VECTOR) {
        currentMatrix = g3State.GetPositionMatrix();

        currentMatrix.m[3][0] += (FX_Mul(s[0],currentMatrix.m[0][0]) + FX_Mul(s[1],currentMatrix.m[1][0]) + FX_Mul(s[2],currentMatrix.m[2][0]));
        currentMatrix.m[3][1] += (FX_Mul(s[0],currentMatrix.m[0][1]) + FX_Mul(s[1],currentMatrix.m[1][1]) + FX_Mul(s[2],currentMatrix.m[2][1]));
        currentMatrix.m[3][2] += (FX_Mul(s[0],currentMatrix.m[0][2]) + FX_Mul(s[1],currentMatrix.m[1][2]) + FX_Mul(s[2],currentMatrix.m[2][2]));
        currentMatrix.m[3][3] += (FX_Mul(s[0],currentMatrix.m[0][3]) + FX_Mul(s[1],currentMatrix.m[1][3]) + FX_Mul(s[2],currentMatrix.m[2][3]));

        g3State.SetPositionMatrix(currentMatrix);
    }
}

void Normal(u32 data)
{
    auto& g3State = GetGlobalState();
    u8 vecX = (data & 0b1111111111);
    u8 vecXSign = (vecX & 0b1000000000) >> 9;
    vecX = vecX & 0b111111111;
    u8 vecY = (data & 0b11111111110000000000) >> 10;
    u8 vecYSign = (vecY & 0b1000000000) >> 9;
    vecY = vecY & 0b111111111;
    u8 vecZ = (data & 0b111111111100000000000000000000) >> 20;
    u8 vecZSign = (vecZ & 0b1000000000) >> 9;
    vecZ = vecZ & 0b111111111;

    float normalVectorIn[3];
    // Should get us a number from -1 to 1
    normalVectorIn[0] = (float)vecX / 511.0f;
    if(vecXSign) {
        normalVectorIn[0] = normalVectorIn[0] * -1.0f;
    }
    normalVectorIn[1] = (float)vecY / 511.0f;
    if(vecYSign) {
        normalVectorIn[1] = normalVectorIn[1] * -1.0f;
    }
    normalVectorIn[2] = (float)vecZ / 511.0f;
    if(vecZSign) {
        normalVectorIn[2] = normalVectorIn[2] * -1.0f;
    }

    //Multiply by the vector matrix
    float normalVector[3];
    MtxFx44 vectorMatrix = g3State.GetVectorMatrix();
    normalVector[0] = (normalVectorIn[0] * FX_FX32_TO_F32(vectorMatrix.m[0][0]))
                    + (normalVectorIn[1] * FX_FX32_TO_F32(vectorMatrix.m[1][0]))
                    + (normalVectorIn[2] * FX_FX32_TO_F32(vectorMatrix.m[2][0]));
    
    normalVector[1] = (normalVectorIn[0] * FX_FX32_TO_F32(vectorMatrix.m[0][1]))
                    + (normalVectorIn[1] * FX_FX32_TO_F32(vectorMatrix.m[1][1]))
                    + (normalVectorIn[2] * FX_FX32_TO_F32(vectorMatrix.m[2][1]));

    normalVector[2] = (normalVectorIn[0] * FX_FX32_TO_F32(vectorMatrix.m[0][2]))
                    + (normalVectorIn[1] * FX_FX32_TO_F32(vectorMatrix.m[1][2]))
                    + (normalVectorIn[2] * FX_FX32_TO_F32(vectorMatrix.m[2][2]));

    // Calculate the color

    float color[3] = {};
    auto emissionColor = g3State.GetEmissionColor();
    auto specularColor = g3State.GetSpecularColor();
    auto diffuseColor = g3State.GetDiffuseColor();
    auto ambientColor = g3State.GetAmbientColor();

    color[0] = (float)emissionColor.r / 255.0f;
    color[1] = (float)emissionColor.g / 255.0f;
    color[2] = (float)emissionColor.b / 255.0f;

    for(int lightNum=0; lightNum<4; lightNum++) {
        auto lightColor = g3State.GetLightColor(lightNum);
        if((g3State.GetCurrentDrawCall()->GetPolygonAttr().lightFlag >> lightNum) & 1)
        {
            // Light is enabled

            // Multiply light vector by vector matrix
            auto currentLightVector = g3State.GetLightVector(lightNum);
            float lightVector[3];
            lightVector[0] = (currentLightVector.x * FX_FX32_TO_F32(vectorMatrix.m[0][0]))
                           + (currentLightVector.y * FX_FX32_TO_F32(vectorMatrix.m[1][0]))
                           + (currentLightVector.z * FX_FX32_TO_F32(vectorMatrix.m[2][0]));

            lightVector[1] = (currentLightVector.x * FX_FX32_TO_F32(vectorMatrix.m[0][1]))
                           + (currentLightVector.y * FX_FX32_TO_F32(vectorMatrix.m[1][1]))
                           + (currentLightVector.z * FX_FX32_TO_F32(vectorMatrix.m[2][1]));

            lightVector[2] = (currentLightVector.x * FX_FX32_TO_F32(vectorMatrix.m[0][2]))
                           + (currentLightVector.y * FX_FX32_TO_F32(vectorMatrix.m[1][2]))
                           + (currentLightVector.z * FX_FX32_TO_F32(vectorMatrix.m[2][2]));

            //Calculate Diffuse level
            float diffuseLevel = (lightVector[0] * normalVector[0])
                               + (lightVector[1] * normalVector[1])
                               + (lightVector[2] * normalVector[2]);

            diffuseLevel = diffuseLevel * -1.0f;
            if(diffuseLevel < 0.0f) {
                diffuseLevel = 0.0f;
            }

            //Specular color
            color[0] = color[0] + (((float)specularColor.r / 255.0f) * ((float)lightColor.r / 255.0f)); // * ShininessLevel TODO
            color[1] = color[1] + (((float)specularColor.g / 255.0f) * ((float)lightColor.g / 255.0f)); // * ShininessLevel TODO
            color[2] = color[2] + (((float)specularColor.b / 255.0f) * ((float)lightColor.b / 255.0f)); // * ShininessLevel TODO

            //Diffuse color
            color[0] = color[0] + (((float)diffuseColor.r / 255.0f) * ((float)lightColor.r / 255.0f) * diffuseLevel); // * DiffuseLevel TODO
            color[1] = color[1] + (((float)diffuseColor.g / 255.0f) * ((float)lightColor.g / 255.0f) * diffuseLevel); // * DiffuseLevel TODO
            color[2] = color[2] + (((float)diffuseColor.b / 255.0f) * ((float)lightColor.b / 255.0f) * diffuseLevel); // * DiffuseLevel TODO

            //Ambient color
            color[0] = color[0] + (((float)ambientColor.r / 255.0f) * ((float)lightColor.r / 255.0f));
            color[1] = color[1] + (((float)ambientColor.g / 255.0f) * ((float)lightColor.g / 255.0f));
            color[2] = color[2] + (((float)ambientColor.b / 255.0f) * ((float)lightColor.b / 255.0f));
        }
    }

    if(color[0] > 1.0f) {
        color[0] = 1.0f;
    }
    if(color[1] > 1.0f) {
        color[1] = 1.0f;
    }
    if(color[2] > 1.0f) {
        color[2] = 1.0f;
    }

    RGBBytes vertexColor;


    vertexColor.r = (u8)(color[0] * 255.0f);
    vertexColor.g = (u8)(color[1] * 255.0f);
    vertexColor.b = (u8)(color[2] * 255.0f);

    g3State.SetVertexColor(vertexColor);
}

//PolygonAttr
void PolygonAttrCommand(u32 data)
{
    auto& g3State = GetGlobalState();
    std::shared_ptr<DrawCall> drawCall = g3State.GetCurrentDrawCall();
    PolygonAttr attr = drawCall->GetPolygonAttr();
    u8 lightFlag = data & 0b1111;
    u8 alpha = (data & 0b111110000000000000000) >> 16;
    u8 polygonMode = (data & 0b110000) >> 4;
    u8 cullMode = (data & 0b11000000) >> 6;
    u8 depthTest = (data & 0b100000000000000) >> 14;
    u8 useFog = (data & 0b1000000000000000) >> 15;

    if(cullMode != attr.cullMode 
    || polygonMode != attr.polygonMode 
    || alpha != attr.alphaInt 
    || depthTest != attr.depthTest 
    || useFog != attr.fogEnable)
    {
        // Flush the vertex buffer
        g3State.FlushDrawCall();
    }

    if(depthTest != g3State.GetCurrentDrawCall()->GetPolygonAttr().depthTest)
    {
        if(depthTest){
            glDepthFunc(GL_LEQUAL);
        } else {
            glDepthFunc(GL_LESS);
        }
    }

    attr.lightFlag = lightFlag;
    attr.cullMode = cullMode;
    attr.polygonMode = polygonMode;
    attr.alphaInt = alpha;
    attr.depthTest = depthTest;
    attr.fogEnable = useFog;

    if(alpha == 31){
        attr.alpha = 1.0f;
    } else {
        attr.alpha = (float)alpha / 31.0f;
    }

    switch(cullMode) {
        case GX_CULL_ALL:
            glEnable( GL_CULL_FACE );
            glCullFace(GL_FRONT_AND_BACK);
            break;
        case GX_CULL_FRONT:
            glEnable( GL_CULL_FACE );
            glCullFace(GL_BACK);
            break;
        case GX_CULL_BACK:
            glEnable( GL_CULL_FACE );
            glCullFace(GL_FRONT);
            break;
        case GX_CULL_NONE:
            glDisable( GL_CULL_FACE );
            break;
    }

    drawCall->SetPolygonAttr(attr);
    return;
}

void SpecEmi(u32 data)
{
    RGBBytes specularColor;
    RGBBytes emissionColor;
    auto& g3State = GetGlobalState();

    specularColor.r = (data & 0b11111);
    specularColor.g = (data & 0b1111100000) >> 5;
    specularColor.b = (data & 0b111110000000000) >> 10;
    specularColor.r <<= 3;
    specularColor.g <<= 3;
    specularColor.b <<= 3;
    bool setVtxColor = ((data & 0b1000000000000000) >> 15) != 0;
    emissionColor.r = (data & 0b111110000000000000000) >> 16;
    emissionColor.g = (data & 0b11111000000000000000000000) >> 21;
    emissionColor.b = (data & 0b1111100000000000000000000000000) >> 26;
    emissionColor.r <<= 3;
    emissionColor.g <<= 3;
    emissionColor.b <<= 3;

    if(setVtxColor) {
        g3State.SetVertexColor(specularColor);
    }

    g3State.SetSpecularColor(specularColor);
    g3State.SetEmissionColor(emissionColor);
}

//SwapBuffers
void SwapBuffers(u8 depthBufferMode)
{
    auto& g3State = GetGlobalState();

    g3State.SetUseWBuffering(depthBufferMode);
    g3State.SetSwapBuffersCalledThisFrame(true);
}

//TexCoord
void TexCoord(u32 data)
{
    auto& g3State = GetGlobalState();
    s16 s = data & 0xFFFF;
    s16 t = (data & 0xFFFF0000) >> 16;

    //Multiply by the texture matrix
    //See GBATek section on Texture Matrix
    if(g3State.GetCurrentDrawCall()->GetTexImageParam().texCordTransform == 1) {
        float texCoordVector[4];
        texCoordVector[0] = (float)s / 16.0f;
        texCoordVector[1] = (float)t / 16.0f;
        texCoordVector[2] = 1.0f / 16.0f;
        texCoordVector[3] = 1.0f / 16.0f;

        MtxFx44 textureMtx = g3State.GetTextureMatrix();


        g3State.SetNextTexCoordS((texCoordVector[0] * FX_FX32_TO_F32(textureMtx.m[0][0]))
                          + (texCoordVector[1] * FX_FX32_TO_F32(textureMtx.m[1][0]))
                          + (texCoordVector[2] * FX_FX32_TO_F32(textureMtx.m[2][0]))
                          + (texCoordVector[3] * FX_FX32_TO_F32(textureMtx.m[3][0])));

        g3State.SetNextTexCoordT((texCoordVector[0] * FX_FX32_TO_F32(textureMtx.m[0][1]))
                          + (texCoordVector[1] * FX_FX32_TO_F32(textureMtx.m[1][1]))
                          + (texCoordVector[2] * FX_FX32_TO_F32(textureMtx.m[2][1]))
                          + (texCoordVector[3] * FX_FX32_TO_F32(textureMtx.m[3][1])));
    } else {
        g3State.SetNextTexCoordS(((float)s)/16.0f);
        g3State.SetNextTexCoordT(((float)t)/16.0f);
    }
}

//TexImageParam
void TexImageParamCommand(u32 data)
{
    SIM::G3::TexImageParam param;
    param.textureOffset = ((u16)data) << 3;
    param.repeatS = (data & 0b10000000000000000) >> 16;
    param.repeatT = (data & 0b100000000000000000) >> 17;
    param.flipS = (data & 0b1000000000000000000) >> 18;
    param.flipT = (data & 0b10000000000000000000) >> 19;
    param.textureSSize = 8 << ((data & 0b11100000000000000000000) >> 20);
    param.textureTSize = 8 << ((data & 0b11100000000000000000000000) >> 23);
    param.textureFormat = (data & 0b11100000000000000000000000000) >> 26;
    param.color0 = (data & 0b100000000000000000000000000000) >> 29;
    param.texCordTransform = (data & 0b11000000000000000000000000000000) >> 30;
    auto& g3State = SIM::G3::GetGlobalState();
    g3State.GetCurrentDrawCall()->SetTexImageParam(param);
    return;
}

//TexPlttBase
void TexPlttBase(u32 data)
{
    auto& g3State = GetGlobalState();
    const auto texParam = g3State.GetCurrentDrawCall()->GetTexImageParam();

    if( texParam.textureFormat == 2 )
    {
        g3State.GetCurrentDrawCall()->SetTexPaletteBase(data << 3);
    }
    else
    {
        g3State.GetCurrentDrawCall()->SetTexPaletteBase(data << 4);
    }
}

//Add a vertex
void Vtx(s16 x, s16 y, s16 z)
{
    fx32 fxX;
    fx32 fxY;
    fx32 fxZ;
    fx32 fxW;

    fxX = x;
    fxY = y;
    fxZ = z;
    fxW = FX32_ONE;

    calculateVtxFx(&fxX, &fxY, &fxZ, &fxW);

    FixedPointVertex currentVert;

    

    currentVert.x = fxX;
    currentVert.y = fxY;
    currentVert.z = fxZ;
    currentVert.w = fxW;

    auto& g3State = GetGlobalState();
    int polygonIndex = g3State.GetCurrentIndexInPolygon();
    auto vertexColor = g3State.GetVertexColor();
    currentVert.r = (float)vertexColor.r / 255.0f;
    currentVert.g = (float)vertexColor.g / 255.0f;
    currentVert.b = (float)vertexColor.b / 255.0f;
    currentVert.a = g3State.GetCurrentDrawCall()->GetPolygonAttr().alpha;

    const auto texParam = g3State.GetCurrentDrawCall()->GetTexImageParam();
    currentVert.s = g3State.GetNextTexCoordS() / (float)texParam.textureSSize;
    currentVert.t = g3State.GetNextTexCoordT() / (float)texParam.textureTSize;
    g3State.SetPolygonVertex(polygonIndex, currentVert);

    polygonIndex++;
    g3State.SetCurrentIndexInPolygon(polygonIndex);

    switch(g3State.GetPrimitiveType()) {
        case GX_BEGIN_QUADS:
            if(polygonIndex >= 4) {
                polygonIndex = 0;
                g3State.SetCurrentIndexInPolygon(polygonIndex);
            }
            break;
        case GX_BEGIN_TRIANGLES:
            if(polygonIndex >= 3) {
                polygonIndex = 0;
                g3State.SetCurrentIndexInPolygon(polygonIndex);
            }
            break;
        case GX_BEGIN_QUAD_STRIP:
            if(polygonIndex >= 4) {
                polygonIndex = 2;
                g3State.SetCurrentIndexInPolygon(polygonIndex);
                g3State.SetPolygonReady(true);
                u32 quadStripNum = g3State.GetQuadStripQuadIndex();
                quadStripNum++;
                g3State.SetQuadStripQuadIndex(quadStripNum);
            }
            break;
        case GX_BEGIN_TRIANGLE_STRIP:
            if(polygonIndex >= 3) {
                polygonIndex = 2;
                g3State.SetCurrentIndexInPolygon(polygonIndex);
                g3State.SetPolygonReady(true);
                u32 triStripNum = g3State.GetTriStripTriangleIndex();
                triStripNum++;
                g3State.SetTriStripTriangleIndex(triStripNum);
            }
            break;
        default:
            break;
    }
}
} // namespace SIM::G3


u8 G3SIM_SwapBuffersCalledThisFrame()
{
    return SIM::G3::GetGlobalState().GetSwapBuffersCalledThisFrame();
}

void G3SIM_ResetSwapBuffersCall()
{
    SIM::G3::GetGlobalState().SetSwapBuffersCalledThisFrame(false);
}
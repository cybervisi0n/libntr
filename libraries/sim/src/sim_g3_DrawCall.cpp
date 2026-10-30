#include "simulator/sim_g3_DrawCall.hpp"

#include <cstring>

#include <nitro/gx/g3.h>

namespace SIM::G3 {

static constexpr auto InitialRenderVertsCapacity = 512;

DrawCall::DrawCall() {
    mRenderVerts = new RenderVertex[InitialRenderVertsCapacity];
    mRenderVertsCapacity = InitialRenderVertsCapacity;
    mRenderVertsCount = 0;
}

DrawCall::DrawCall(const DrawCall& source) {
    mRenderVerts = new RenderVertex[source.mRenderVertsCount];
    mRenderVertsCount = source.mRenderVertsCount;
    memcpy(mRenderVerts, source.mRenderVerts, sizeof(RenderVertex) * mRenderVertsCount);
    mRenderVertsCapacity = mRenderVertsCount;
    mTexImageParam = source.mTexImageParam;
    mTexture = source.mTexture;
    mPolygonAttr = source.mPolygonAttr;
    mTexPaletteBase = source.mTexPaletteBase;
}

DrawCall::~DrawCall() {
    delete[] mRenderVerts;
}

void DrawCall::AddVertex(const RenderVertex& vert) {
    if(mRenderVertsCount >= mRenderVertsCapacity) {
        // Reallocate mRenderVerts
        RenderVertex * newVerts = new RenderVertex[mRenderVertsCapacity * 2];
        memcpy(newVerts, mRenderVerts, sizeof(RenderVertex) * mRenderVertsCapacity);
        mRenderVertsCapacity *= 2;
        delete[] mRenderVerts;
        mRenderVerts = newVerts;
    }

    mRenderVerts[mRenderVertsCount] = vert;
    mRenderVertsCount++;
}

void DrawCall::ClearVerts() {
    // Set count to 0 without freeing/reallocating
    mRenderVertsCount = 0;
}

bool DrawCall::IsTransparent() {
    if(mTexImageParam.textureFormat == GX_TEXFMT_A3I5
    || mTexImageParam.textureFormat == GX_TEXFMT_A5I3
    || mPolygonAttr.alpha < 0.99f
    ) {
        return true;
    }

    return false;
}

void DrawCall::SetFogDepthTable(float * depthTable) {
    memcpy(mFogDepthTable, depthTable, 32 * sizeof(float));
}

void DrawCall::SetFogDensityTable(float * densityTable) {
    memcpy(mFogDensityTable, densityTable, 32 * sizeof(float));
}



}
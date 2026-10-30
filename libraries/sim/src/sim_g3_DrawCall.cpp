#include "simulator/sim_g3_DrawCall.hpp"

#include <cstring>

namespace SIM::G3 {

static constexpr auto InitialRenderVertsCapacity = 512;

DrawCall::DrawCall() {
    mRenderVerts = new RenderVertex[InitialRenderVertsCapacity];
    mRenderVertsCapacity = InitialRenderVertsCapacity;
    mRenderVertsCount = 0;
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



}
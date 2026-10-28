#include "simulator/sim_g3_GlobalState.hpp"

#include <nitro/fx/fx.h>
#include <nitro/fx/fx_const.h>

static SIM::G3::GlobalState sG3GlobalState;

namespace SIM::G3 {

static constexpr MtxFx44 Identity44 = {
{
    FX32_ONE, 0, 0, 0,
    0, FX32_ONE, 0, 0,
    0, 0, FX32_ONE, 0,
    0, 0, 0, FX32_ONE
}
};


GlobalState::GlobalState() {
    
}

GlobalState::~GlobalState() {
    
}

void GlobalState::Init() {
    mCurrentDrawCall = std::make_shared<DrawCall>();

    // Initialize Matrix Stacks
    for(auto& matrix : mPositionStack) {
        matrix = Identity44;
    }

    for(auto& matrix : mVectorStack) {
        matrix = Identity44;
    }
    mPositionVectorMatrixStackPointer = 0;

    mProjectionStack[0] = Identity44;
    mProjectionMatrixStackPointer = 0;
    mTextureStack[0] = Identity44;
    mTextureMatrixStackPointer = 0;

    // Initialize matrices
    mCurrentProjectionMatrix = Identity44;
    mCurrentPositionMatrix = Identity44;
    mCurrentVectorMatrix = Identity44;
    mCurrentTextureMatrix = Identity44;
    mCurrentClipMatrix = Identity44;
}


void GlobalState::PushMatrix() {
    if(mCurrentMatrixMode == GX_MTXMODE_POSITION 
    || mCurrentMatrixMode == GX_MTXMODE_POSITION_VECTOR) {
        mPositionStack[mPositionVectorMatrixStackPointer] = mCurrentPositionMatrix;
        mVectorStack[mPositionVectorMatrixStackPointer] = mCurrentVectorMatrix;
        mPositionVectorMatrixStackPointer++;
    } else if(mCurrentMatrixMode == GX_MTXMODE_PROJECTION) {
        mProjectionMatrixStackPointer++;
    } else if(mCurrentMatrixMode == GX_MTXMODE_TEXTURE) {
        mTextureMatrixStackPointer++;
    }
}

void GlobalState::PopMatrix(u8 popNum) {
    if(mCurrentMatrixMode == GX_MTXMODE_POSITION 
    || mCurrentMatrixMode == GX_MTXMODE_POSITION_VECTOR) {
        mPositionVectorMatrixStackPointer-= popNum;
        if(mPositionVectorMatrixStackPointer < 0) {
            // matrix stack underflow
            mCurrentPositionMatrix = Identity44;
            mCurrentVectorMatrix = Identity44;
            mPositionVectorMatrixStackPointer = 0;
        } else {
            mCurrentPositionMatrix = mPositionStack[mPositionVectorMatrixStackPointer];
            mCurrentVectorMatrix = mVectorStack[mPositionVectorMatrixStackPointer];
        }
    } else if(mCurrentMatrixMode == GX_MTXMODE_PROJECTION) {
        mProjectionMatrixStackPointer -= popNum;
        if(mProjectionMatrixStackPointer < 0) {
            // matrix stack underflow
            mCurrentProjectionMatrix = Identity44;
            mProjectionMatrixStackPointer = 0;
        } else {
            mCurrentProjectionMatrix = mProjectionStack[mProjectionMatrixStackPointer];
        }
    } else if(mCurrentMatrixMode == GX_MTXMODE_TEXTURE) {
        mTextureMatrixStackPointer -= popNum;
        if(mTextureMatrixStackPointer < 0) {
            // matrix stack underflow
            mCurrentTextureMatrix = Identity44;
            mTextureMatrixStackPointer = 0;
        } else {
            mCurrentTextureMatrix = mTextureStack[mTextureMatrixStackPointer];
        }
    }
}



GlobalState& GetGlobalState() {
    return sG3GlobalState;
}

}
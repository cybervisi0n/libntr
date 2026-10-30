#include "simulator/sim_g3_GlobalState.hpp"

#include <cstring>

#include "simulator/sim_g3_Utilities.hpp"

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
        UpdateClipMatrix();
    } else if(mCurrentMatrixMode == GX_MTXMODE_PROJECTION) {
        mProjectionMatrixStackPointer -= popNum;
        if(mProjectionMatrixStackPointer < 0) {
            // matrix stack underflow
            mCurrentProjectionMatrix = Identity44;
            mProjectionMatrixStackPointer = 0;
        } else {
            mCurrentProjectionMatrix = mProjectionStack[mProjectionMatrixStackPointer];
        }
        UpdateClipMatrix();
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

void GlobalState::StoreMatrix(u8 idx) {
    if(mCurrentMatrixMode == GX_MTXMODE_POSITION
    || mCurrentMatrixMode == GX_MTXMODE_POSITION_VECTOR) {
        if(idx > mPositionStack.size()) {
            return;
        }

        mPositionStack[idx] = mCurrentPositionMatrix;
        mVectorStack[idx] = mCurrentVectorMatrix;
    } else if (mCurrentMatrixMode == GX_MTXMODE_PROJECTION) {
        if(idx > mProjectionStack.size()) {
            return;
        }

        mProjectionStack[idx] = mCurrentProjectionMatrix;
    } else if (mCurrentMatrixMode == GX_MTXMODE_TEXTURE) {
        if(idx > mTextureStack.size()) {
            return;
        }

        mTextureStack[idx] = mCurrentTextureMatrix;
    }
}

void GlobalState::RestoreMatrix(u8 idx) {
    if(mCurrentMatrixMode == GX_MTXMODE_POSITION
    || mCurrentMatrixMode == GX_MTXMODE_POSITION_VECTOR) {
        if(idx > mPositionStack.size()) {
            return;
        }

        mCurrentPositionMatrix = mPositionStack[idx];
        mCurrentVectorMatrix = mVectorStack[idx];
        UpdateClipMatrix();
    } else if (mCurrentMatrixMode == GX_MTXMODE_PROJECTION) {
        if(idx > mProjectionStack.size()) {
            return;
        }

        mCurrentProjectionMatrix = mProjectionStack[idx];
        UpdateClipMatrix();
    } else if (mCurrentMatrixMode == GX_MTXMODE_TEXTURE) {
        if(idx > mTextureStack.size()) {
            return;
        }

        mCurrentTextureMatrix = mTextureStack[idx];
    }
}

void GlobalState::IdentityMatrix() {
    switch(mCurrentMatrixMode) {
        case GX_MTXMODE_PROJECTION:
            mCurrentProjectionMatrix = Identity44;
            UpdateClipMatrix();
            break;
        case GX_MTXMODE_POSITION:
            mCurrentPositionMatrix = Identity44;
            UpdateClipMatrix();
            break;
        case GX_MTXMODE_POSITION_VECTOR:
            mCurrentPositionMatrix = Identity44;
            mCurrentVectorMatrix = Identity44;
            UpdateClipMatrix();
            break;
        case GX_MTXMODE_TEXTURE:
            mCurrentTextureMatrix = Identity44;
        default:
            break;
    }
}

MtxFx44 GlobalState::GetCurrentMatrix() {
    switch(mCurrentMatrixMode) {
        case GX_MTXMODE_PROJECTION:
            return mCurrentProjectionMatrix;
        case GX_MTXMODE_POSITION:
            return mCurrentPositionMatrix;
        case GX_MTXMODE_POSITION_VECTOR:
            return mCurrentVectorMatrix;
        case GX_MTXMODE_TEXTURE:
            return mCurrentTextureMatrix;
        default:
            return Identity44;
    }
}

void GlobalState::SetCurrentMatrix(const MtxFx44& matrix) {
    switch(mCurrentMatrixMode) {
        case GX_MTXMODE_PROJECTION:
            mCurrentProjectionMatrix = matrix;
            break;
        case GX_MTXMODE_POSITION:
            mCurrentPositionMatrix = matrix;
            break;
        case GX_MTXMODE_POSITION_VECTOR:
            mCurrentVectorMatrix = matrix;
            break;
        case GX_MTXMODE_TEXTURE:
            mCurrentTextureMatrix = matrix;
            break;
        default:
            break;
    }

    if(mCurrentMatrixMode == GX_MTXMODE_PROJECTION
    || mCurrentMatrixMode == GX_MTXMODE_POSITION) {
        UpdateClipMatrix();
    }
}

void GlobalState::SetProjectionMatrix(const MtxFx44& matrix) {
    mCurrentProjectionMatrix = matrix;
    UpdateClipMatrix();
}

void GlobalState::SetPositionMatrix(const MtxFx44& matrix) {
    mCurrentPositionMatrix = matrix;
    UpdateClipMatrix();
}


void GlobalState::UpdateClipMatrix() {
    mCurrentClipMatrix = Util::MultiplyMtxFx44(mCurrentProjectionMatrix, mCurrentPositionMatrix);

    memcpy( (void*)&s_reg_G3X_CLIPMTX_RESULT_0, &mCurrentClipMatrix, sizeof( MtxFx44 ) );
}



GlobalState& GetGlobalState() {
    return sG3GlobalState;
}

}
#include "simulator/sim_g3_GlobalState.hpp"

#include <cstring>

#include "simulator/sim_g3_Utilities.hpp"
#include "simulator/sim_crc32.h"

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
    mGlRenderer = std::make_shared<GlRenderer>();

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

void GlobalState::InitGlRenderer() {
    mGlRenderer->Init();
}

void GlobalState::FlushDrawCall() {
    // Handle texture info from current drawCall
    u8 * texBuf = nullptr;
	std::shared_ptr<Texture> texture = nullptr;
    auto texImageParam = mCurrentDrawCall->GetTexImageParam();

    if(texImageParam.textureFormat != GX_TEXFMT_NONE) {
		GLuint g3TextureId;
		u8 * vramTex = (u8*)((u64)GetTexturePointer() + texImageParam.textureOffset);
		u8 * vramPltt = (u8*)((u64)GetTexturePalettePointer() + mCurrentDrawCall->GetTexPaletteBase());
		u16 * colorAddr = (u16*)vramPltt;

		//Calculate the texture + palette CRC
		u32 paletteCRC = SIM_crc32buf(vramPltt, 512);
		u32 vramTexBufSize = 0;

		if(texImageParam.textureFormat == GX_TEXFMT_PLTT4) {
			vramTexBufSize = (texImageParam.textureSSize * texImageParam.textureTSize) >> 2;
		} else if(texImageParam.textureFormat == GX_TEXFMT_PLTT16) {
			vramTexBufSize = (texImageParam.textureSSize * texImageParam.textureTSize) >> 1;
		} else {
			vramTexBufSize = (texImageParam.textureSSize * texImageParam.textureTSize);
		}

		u32 textureCRC = SIM_crc32buf(vramTex, vramTexBufSize);
		texture = mTextureCache.GetTexture(textureCRC, paletteCRC);
		if(texture == nullptr) {
			// decode texture and create it in the cache
			//Convert the DS texture data into a format opengl can understand
			u8 * outTexBuf = new u8[4*texImageParam.textureSSize * texImageParam.textureTSize];
			memset((void*)outTexBuf, 0, 4*texImageParam.textureSSize * texImageParam.textureTSize);


			switch( texImageParam.textureFormat ){
				case GX_TEXFMT_A3I5:
					Util::DecodeTexA3I5(vramTex, colorAddr, outTexBuf, texImageParam.textureSSize, texImageParam.textureTSize);
					break;
				case GX_TEXFMT_PLTT4:
					Util::DecodeTex4(vramTex, colorAddr, outTexBuf, texImageParam.textureSSize, texImageParam.textureTSize, texImageParam.color0);
					break;
				case GX_TEXFMT_PLTT16:
					Util::DecodeTex16(vramTex, colorAddr, outTexBuf, texImageParam.textureSSize, texImageParam.textureTSize, texImageParam.color0);
					break;
				case GX_TEXFMT_PLTT256:
					Util::DecodeTex256(vramTex, colorAddr, outTexBuf, texImageParam.textureSSize, texImageParam.textureTSize, texImageParam.color0);
					break;
				case GX_TEXFMT_COMP4x4:
					//SIM_assert_always();
					//TODO
					break;
				case GX_TEXFMT_A5I3:
					Util::DecodeTexA5I3(vramTex, colorAddr, outTexBuf, texImageParam.textureSSize, texImageParam.textureTSize);
					break;
				case GX_TEXFMT_DIRECT:
					Util::DecodeTexDirect(vramTex, outTexBuf, texImageParam.textureSSize, texImageParam.textureTSize);
					break;
			}

			// This creates a GL texture and uploads it to the GPU with glTexImage2D
			texture = std::make_shared<SIM::G3::Texture>(texImageParam.textureTSize, texImageParam.textureSSize, outTexBuf);

			mTextureCache.AddTexture(textureCRC, paletteCRC, texture);

			// Texture buffer can be freed now, it has been sent to the GPU
			delete[] outTexBuf;
		}

        mCurrentDrawCall->SetTexture(texture);
    } else {
        mCurrentDrawCall->SetTexture(nullptr);
    }


    if(mCurrentDrawCall->IsTransparent()) {
        // TODO Copy current draw call to array of transparent drawcalls

        return;
    }

    mGlRenderer->Draw(mCurrentDrawCall);
    mCurrentDrawCall->ClearVerts();
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

void * GlobalState::GetTexturePointer() {
	void * vramBank = NULL;
	switch( mVramTex ){
		case GX_VRAM_TEX_0_A:
		case GX_VRAM_TEX_01_AB:
		case GX_VRAM_TEX_012_ABC:
		case GX_VRAM_TEX_0123_ABCD:
		case GX_VRAM_TEX_01_AC:
		case GX_VRAM_TEX_01_AD:
		case GX_VRAM_TEX_012_ABD:
		case GX_VRAM_TEX_012_ACD:
			vramBank = (void *)HW_LCDC_VRAM_A;
			break;
		case GX_VRAM_TEX_0_B:
		case GX_VRAM_TEX_01_BC:
		case GX_VRAM_TEX_012_BCD:
		case GX_VRAM_TEX_01_BD:
			vramBank = (void *)HW_LCDC_VRAM_B;
			break;
		case GX_VRAM_TEX_0_C:
		case GX_VRAM_TEX_01_CD:
			vramBank = (void *)HW_LCDC_VRAM_C;
			break;
		case GX_VRAM_TEX_0_D:
			vramBank = (void *)HW_LCDC_VRAM_A;
			break;
		default:
			//?
			break;
	}
	
	return vramBank;
}

void * GlobalState::GetTexturePalettePointer() {
	void * vramBank = NULL;
	switch( mVramTexPalette ) {
		case GX_VRAM_TEXPLTT_NONE:
		case GX_VRAM_TEXPLTT_0123_E:
		case GX_VRAM_TEXPLTT_01234_EF:
		case GX_VRAM_TEXPLTT_012345_EFG:
			vramBank = (void *)HW_LCDC_VRAM_E;
			break;
		case GX_VRAM_TEXPLTT_0_F:
		case GX_VRAM_TEXPLTT_01_FG:
			vramBank = (void *)HW_LCDC_VRAM_F;
			break;
		case GX_VRAM_TEXPLTT_0_G:
			vramBank = (void *)HW_LCDC_VRAM_G;
			break;
	}
	
	return vramBank;
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
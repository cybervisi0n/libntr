#include "simulator/sim_g3_GlobalState.hpp"

#include <cstring>

#include "simulator/sim_g3_Utilities.hpp"
#include "simulator/sim_crc32.h"

#include <nitro/fx/fx.h>
#include <nitro/fx/fx_const.h>

#define getbit(x,n) ( ( (x) >> (n) ) & 1 )

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

void GlobalState::SubmitPolygon(int numVerts) {
    RenderVertex glVerts[6];
    u8 addPolygon = TRUE;
    int numFinalVerts = numVerts;

    if(numVerts < 3 || numVerts > 4) {
        //SIM_assert_always();
        return;
    }

    for(int i=0; i < numVerts; i++) {
        FixedPointVertex * curVtx = &mPolygonVerts[i];
    
        // Truncate W to 24 bits
        curVtx->w &= 0x00FFFFFF;
    
        float floatX, floatY, floatW;
        float screenX, screenY;
    
        floatX = FX_FX32_TO_F32(curVtx->x);
        floatY = FX_FX32_TO_F32(curVtx->y);
        floatW = FX_FX32_TO_F32(curVtx->w);
    
        screenX = (((floatX+floatW)*256.0f) / (2*floatW));
        screenY = (((floatY+floatW)*192.0f) / (2*floatW));
        					
        screenX = (screenX / 128.0f) - 1.0f;
        screenY = (screenY / 96.0f) - 1.0f;
    
        glVerts[i].x = screenX;
        glVerts[i].y = screenY;
        glVerts[i].s = curVtx->s;
        glVerts[i].t = curVtx->t;
        glVerts[i].r = curVtx->r;
        glVerts[i].g = curVtx->g;
        glVerts[i].b = curVtx->b;
        glVerts[i].a = curVtx->a;
    }

    // Get the W size and remove the polygon if W is 0
    u32 wsize = 0;
    for(int i=0; i < 4; i++) {
        FixedPointVertex * curVtx = &mPolygonVerts[i];
        u32 w = curVtx->w;
        if(w == 0){
            addPolygon = FALSE;
        }
        while ((w >> wsize) && (wsize < 32)) {
            wsize += 4;
        }
    }

    // Normalize W
    for(int i=0; i < 4; i++) {
        FixedPointVertex * curVtx = &mPolygonVerts[i];
        
        s32 w, wshifted;
        
        if(wsize < 16) {
        	w = curVtx->w << (16-wsize);
        	wshifted = w >> (16-wsize);
        } else {
        	w = curVtx->w >> (wsize - 16);
        	wshifted = w << (wsize - 16);
        }
        
        s32 z;
        
        if(curVtx->w) {
        	z = ((((s64)curVtx->z * 0x4000) / curVtx->w) + 0x3FFF) * 0x200;
        } else {
        	z = 0x7FFE00;
        }
        
        if(mUseWBuffering) {
        	z = wshifted;
        }
        
        if(z < 0) {
        	z = 0;
        } else if(z > 0xFFFFFF) {
        	z = 0xFFFFFF;
        }
        
        glVerts[i].z = (((float)z / 16777215.0f) * 2.0f) - 1.0f;
        glVerts[i].w = (((float)w / 16777215.0f) * 2.0f) - 1.0f;
    }

    // If we have a quad, convert it into 2 triangles
    if(numVerts == 4) {
        //make a copy of the quad
        RenderVertex quad[4];
        memcpy(quad, glVerts, sizeof(RenderVertex) * 4);

        //Convert to 2 triangles
        #define CopyVertex(srcIdx, destIdx) memcpy(&glVerts[destIdx], &quad[srcIdx], sizeof(RenderVertex))
        CopyVertex(0, 0);
        CopyVertex(1, 1);
        CopyVertex(2, 2);
        CopyVertex(2, 3);
        CopyVertex(3, 4);
        CopyVertex(0, 5);
        #undef CopyVertex

        numFinalVerts = 6;
    }

    // Check if any of the verts are horribly out of bounds
    // TODO: This is a HACK and we should eventually remove it
    for(int i=0; i < numFinalVerts; i++) {
        if(glVerts[i].x > 50.0f || glVerts[i].x < -50.0f) {
            addPolygon = FALSE;
        }
        if(glVerts[i].y > 50.0f || glVerts[i].y < -50.0f) {
            addPolygon = FALSE;
        }
    }

    if(addPolygon) {
        for(int i=0; i < numFinalVerts; i++) {
            mCurrentDrawCall->AddVertex(glVerts[i]);
        }
    }
}

void GlobalState::FlushDrawCall() {
    // Handle texture info from current drawCall
    u8 * texBuf = nullptr;
	std::shared_ptr<Texture> texture = nullptr;
    auto texImageParam = mCurrentDrawCall->GetTexImageParam();

	u8 * vramTex = (u8*)((u64)GetTexturePointer() + texImageParam.textureOffset);
	u8 * vramPltt = (u8*)((u64)GetTexturePalettePointer() + mCurrentDrawCall->GetTexPaletteBase());

    if(texImageParam.textureFormat != GX_TEXFMT_NONE && vramTex != nullptr) {
		GLuint g3TextureId;
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

    // Process fog
    bool useFog = getbit(s_reg_G3X_DISP3DCNT, 7) != 0;
    mCurrentDrawCall->SetFogEnabled(useFog);
    if(useFog) {
		// Fog color
		u32 fogRed = s_reg_G3X_FOG_COLOR & 0b11111;
		u32 fogGreen = (s_reg_G3X_FOG_COLOR >> 5) & 0b11111;
		u32 fogBlue = (s_reg_G3X_FOG_COLOR >> 10) & 0b11111;
		u32 fogAlpha = (s_reg_G3X_FOG_COLOR >> 16) & 0b11111;
		float fogRedF = (float)fogRed / 32.0f;
		float fogGreenF = (float)fogGreen / 32.0f;
		float fogBlueF = (float)fogBlue / 32.0f;
		float fogAlphaF = (float)fogAlpha / 32.0f;
        mCurrentDrawCall->SetFogColor(fogRedF, fogGreenF, fogBlueF, fogAlphaF);

		u32 fogShift = (s_reg_G3X_DISP3DCNT >> 8) & 0b1111;
		u32 fogStep = 0x400 >> fogShift;

		//Fog depth boundaries
		u32 fogOffset = s_reg_G3X_FOG_OFFSET;

		float fogDepthBoundary[32];
		float fogDensity[32];
		u8 * fogTablePtr = (u8*)&s_reg_G3X_FOG_TABLE_0;
		for(int i=0; i < 32; i++) {
			fogDepthBoundary[i] = ((float)(fogOffset + fogStep*(i+1)) / 32767.0f) * 2.0f - 1.0f;
			fogDensity[i] = (float)fogTablePtr[i] / 127.0f;
		}

        mCurrentDrawCall->SetFogDepthTable(fogDepthBoundary);
        mCurrentDrawCall->SetFogDensityTable(fogDensity);
    }


    if(mCurrentDrawCall->IsTransparent()) {
        // Copy current draw call to mTransparentDrawCalls, these will render at the end of the frame
        mTransparentDrawCalls.push_back(std::make_shared<DrawCall>(*mCurrentDrawCall));
        mCurrentDrawCall->ClearVerts();
        return;
    }

    mGlRenderer->Draw(mCurrentDrawCall);
    mCurrentDrawCall->ClearVerts();
}

void GlobalState::DrawTransparentItems() {
    for(auto drawCall : mTransparentDrawCalls) {
        mGlRenderer->Draw(drawCall);
    }

    mTransparentDrawCalls.clear();
}


void GlobalState::PushMatrix() {
    if(mCurrentMatrixMode == GX_MTXMODE_POSITION 
    || mCurrentMatrixMode == GX_MTXMODE_POSITION_VECTOR) {
        mPositionStack[mPositionVectorMatrixStackPointer] = mCurrentPositionMatrix;
        mVectorStack[mPositionVectorMatrixStackPointer] = mCurrentVectorMatrix;
        mPositionVectorMatrixStackPointer++;
    } else if(mCurrentMatrixMode == GX_MTXMODE_PROJECTION) {
        mProjectionStack[mProjectionMatrixStackPointer] = mCurrentProjectionMatrix;
        mProjectionMatrixStackPointer++;
    } else if(mCurrentMatrixMode == GX_MTXMODE_TEXTURE) {
        mTextureStack[mTextureMatrixStackPointer] = mCurrentTextureMatrix;
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
    mCurrentClipMatrix = Util::MultiplyMtxFx44(mCurrentPositionMatrix, mCurrentProjectionMatrix);

    memcpy( (void*)&s_reg_G3X_CLIPMTX_RESULT_0, &mCurrentClipMatrix, sizeof( MtxFx44 ) );
}



GlobalState& GetGlobalState() {
    return sG3GlobalState;
}

}
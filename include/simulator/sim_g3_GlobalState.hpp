#ifndef LIBNTR_SIM_G3_GLOBAL_STATE_HPP
#define LIBNTR_SIM_G3_GLOBAL_STATE_HPP

#include <nitro/types.h>
#include <nitro/fx/fx.h>
#include <nitro/gx/g3.h>
#include "simulator/sim_g3_DrawCall.hpp"

#include <array>
#include <vector>

namespace SIM::G3 {

union RGBBytes {
    u8 bytes[3];
    struct {
        u8 r;
        u8 g;
        u8 b;
    };
};

struct FixedPointVertex {
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
    float s;
    float t;
    float r;
    float g;
    float b;
    float a;
};

class GlobalState {
 public:
  GlobalState();
  ~GlobalState();

 private:
  GXMtxMode mCurrentMatrixMode = GX_MTXMODE_PROJECTION;
  GXBegin mCurrentPrimitiveType = GX_BEGIN_TRIANGLES;

  std::array<RGBBytes, 4> mLightColor;
  float mLightVector[4][3];

  int mCurrentIndexInPolygon = 0;
  bool mDrawPolygon = false;
  u32 mTriStripTriangleIndex;
  u32 mQuadStripQuadIndex;

  FixedPointVertex mCurrentPolygonVerts[4];

  RGBBytes mVertexColor = {0};
  RGBBytes mDiffuseColor = {0};
  RGBBytes mAmbientColor = {0};
  RGBBytes mSpecularColor = {0};
  RGBBytes mEmissionColor = {0};

  MtxFx44 mCurrentProjectionMatrix;
  MtxFx44 mCurrentPositionMatrix;
  MtxFx44 mCurrentVectorMatrix;
  MtxFx44 mCurrentTextureMatrix;
  MtxFx44 mCurrentClipMatrix;

  int mProjectionMatrixStackPointer;
  int mPositionVectorMatrixStackPointer;
  int mTextureMatrixStackPointer;

  std::array<MtxFx44, 1> mProjectionStack;
  std::array<MtxFx44, 31> mPositionStack;
  std::array<MtxFx44, 31> mVectorStack;
  std::array<MtxFx44, 1> mTextureStack;
  
  std::shared_ptr<DrawCall> mCurrentDrawCall = nullptr;
  std::vector<std::shared_ptr<DrawCall>> mTransparentDrawCalls = {};
  

  float mNextTexCoordS;
  float mNextTexCoordT;

  bool mSwapBuffersCalledThisFrame = false;

  
};

}



#endif
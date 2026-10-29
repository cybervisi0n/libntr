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

  void Init();

  // Matrix stack operations
  void PushMatrix();
  void PopMatrix(u8 popNum);
  void StoreMatrix(u8 idx);
  void RestoreMatrix(u8 idx);

  // Matrix operations with logic handled by GlobalState
  void IdentityMatrix();

  inline std::shared_ptr<DrawCall> GetCurrentDrawCall() { return mCurrentDrawCall; }
  inline GXMtxMode GetMatrixMode() { return mCurrentMatrixMode; };
  MtxFx44 GetCurrentMatrix();
  inline MtxFx44 GetProjectionMatrix() { return mCurrentProjectionMatrix; };
  inline MtxFx44 GetPositionMatrix() { return mCurrentPositionMatrix; };
  inline MtxFx44 GetVectorMatrix() { return mCurrentVectorMatrix; };
  inline MtxFx44 GetTextureMatrix() { return mCurrentTextureMatrix; };
  inline MtxFx44 GetClipMatrix() { return mCurrentClipMatrix; }
  inline RGBBytes GetVertexColor() { return mVertexColor; }
  inline RGBBytes GetDiffuseColor() { return mDiffuseColor; }
  inline RGBBytes GetAmbientColor() { return mAmbientColor; }
  inline RGBBytes GetSpecularColor() { return mSpecularColor; }
  inline RGBBytes GetEmissionColor() { return mEmissionColor; }
  inline RGBBytes GetLightColor(int lightId) { mLightColor[lightId]; }

  inline void SetMatrixMode(GXMtxMode mode) { mCurrentMatrixMode = mode; }
  void SetCurrentMatrix(const MtxFx44& matrix);
  void SetProjectionMatrix(const MtxFx44& matrix);
  void SetPositionMatrix(const MtxFx44& matrix);
  inline void SetVectorMatrix(const MtxFx44& matrix) { mCurrentVectorMatrix = matrix; }
  inline void SetTextureMatrix(const MtxFx44& matrix) { mCurrentTextureMatrix = matrix; }
  inline void SetVertexColor(const RGBBytes& color) { mVertexColor = color; }
  inline void SetDiffuseColor(const RGBBytes& color) { mDiffuseColor = color; }
  inline void SetAmbientColor(const RGBBytes& color) { mAmbientColor = color; }
  inline void SetSpecularColor(const RGBBytes& color) { mSpecularColor = color; }
  inline void SetEmissionColor(const RGBBytes& color) { mEmissionColor = color; }
  inline void SetLightColor(int lightId, const RGBBytes& color) { mLightColor[lightId] = color; }


 private:
  void UpdateClipMatrix();
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

  std::array<MtxFx44, 1> mProjectionStack = {};
  std::array<MtxFx44, 31> mPositionStack = {};
  std::array<MtxFx44, 31> mVectorStack = {};
  std::array<MtxFx44, 1> mTextureStack = {};
  
  std::shared_ptr<DrawCall> mCurrentDrawCall = nullptr;
  std::vector<std::shared_ptr<DrawCall>> mTransparentDrawCalls = {};
  

  float mNextTexCoordS;
  float mNextTexCoordT;

  bool mSwapBuffersCalledThisFrame = false;

  
};

GlobalState& GetGlobalState();

}



#endif
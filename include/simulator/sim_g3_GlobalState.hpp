#ifndef LIBNTR_SIM_G3_GLOBAL_STATE_HPP
#define LIBNTR_SIM_G3_GLOBAL_STATE_HPP

#include <nitro/types.h>
#include <nitro/fx/fx.h>
#include <nitro/gx/g3.h>
#include <nitro/gx/gx_vramcnt.h>
#include "simulator/sim_g3_DrawCall.hpp"
#include "simulator/sim_g3_GlRenderer.hpp"
#include "simulator/sim_g3_TextureCache.hpp"

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

union FloatVec3 {
    float nums[3];
    struct {
        float x;
        float y;
        float z;
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
  void InitGlRenderer();

  void FlushDrawCall();
  void DrawTransparentItems();

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
  inline FloatVec3 GetLightVector(int lightId) { return mLightVector[lightId]; }
  inline int GetCurrentIndexInPolygon() { return mCurrentIndexInPolygon; }
  inline bool GetPolygonReady() { return mPolygonReady; }
  inline u32 GetTriStripTriangleIndex() { return mTriStripTriangleIndex; }
  inline u32 GetQuadStripQuadIndex() { return mQuadStripQuadIndex; }
  inline GXBegin GetPrimitiveType() { return mCurrentPrimitiveType; }
  inline bool GetUseWBuffering() { return mUseWBuffering; }
  inline bool GetSwapBuffersCalledThisFrame() { return mSwapBuffersCalledThisFrame; }
  inline float GetNextTexCoordS() { return mNextTexCoordS; }
  inline float GetNextTexCoordT() { return mNextTexCoordT; }
  inline FixedPointVertex GetPolygonVertex(int index) { return mPolygonVerts[index]; }
  inline GXVRamTex GetVramTex() { return mVramTex; }
  inline GXVRamTexPltt GetVramTexPalette() { return mVramTexPalette; }
  void * GetTexturePointer();
  void * GetTexturePalettePointer();
  inline std::shared_ptr<GlRenderer> GetGlRenderer() { return mGlRenderer; }

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
  inline void SetLightVector(int lightId, const FloatVec3& vector) { mLightVector[lightId] = vector; }
  inline void SetCurrentIndexInPolygon(int index) { mCurrentIndexInPolygon = index; }
  inline void SetPolygonReady(bool ready) { mPolygonReady = ready; }
  inline void SetTriStripTriangleIndex(u32 index) { mTriStripTriangleIndex = index; }
  inline void SetQuadStripQuadIndex(u32 index) { mQuadStripQuadIndex = index; }
  inline void SetPrimitiveType(GXBegin type) { mCurrentPrimitiveType = type; }
  inline void SetUseWBuffering(bool useWBuffering) { mUseWBuffering = useWBuffering; }
  inline void SetSwapBuffersCalledThisFrame(bool swapBuffers) { mSwapBuffersCalledThisFrame = swapBuffers; }
  inline void SetNextTexCoordS(float texCoord) { mNextTexCoordS = texCoord; }
  inline void SetNextTexCoordT(float texCoord) { mNextTexCoordT = texCoord; }
  inline void SetPolygonVertex(int index, const FixedPointVertex& vertex) { mPolygonVerts[index] = vertex; }
  inline void SetVramTex(GXVRamTex tex) { mVramTex = tex; } // TODO: Add C Bridge
  inline void SetVramTexPalette(GXVRamTexPltt texPalette) { mVramTexPalette = texPalette; } // TODO: add C bridge

 private:
  void UpdateClipMatrix();
  GXMtxMode mCurrentMatrixMode = GX_MTXMODE_PROJECTION;
  GXBegin mCurrentPrimitiveType = GX_BEGIN_TRIANGLES;

  std::array<RGBBytes, 4> mLightColor = {};
  std::array<FloatVec3, 4> mLightVector = {};

  int mCurrentIndexInPolygon = 0;
  bool mPolygonReady = false;
  u32 mTriStripTriangleIndex = 0;
  u32 mQuadStripQuadIndex = 0;

  bool mUseWBuffering = false;
  bool mSwapBuffersCalledThisFrame = false;

  FixedPointVertex mPolygonVerts[4];

  RGBBytes mVertexColor = {0};
  RGBBytes mDiffuseColor = {0};
  RGBBytes mAmbientColor = {0};
  RGBBytes mSpecularColor = {0};
  RGBBytes mEmissionColor = {0};

  MtxFx44 mCurrentProjectionMatrix = {};
  MtxFx44 mCurrentPositionMatrix = {};
  MtxFx44 mCurrentVectorMatrix = {};
  MtxFx44 mCurrentTextureMatrix = {};
  MtxFx44 mCurrentClipMatrix = {};

  int mProjectionMatrixStackPointer;
  int mPositionVectorMatrixStackPointer;
  int mTextureMatrixStackPointer;

  std::array<MtxFx44, 1> mProjectionStack = {};
  std::array<MtxFx44, 31> mPositionStack = {};
  std::array<MtxFx44, 31> mVectorStack = {};
  std::array<MtxFx44, 1> mTextureStack = {};
  
  std::shared_ptr<DrawCall> mCurrentDrawCall = nullptr;
  std::vector<std::shared_ptr<DrawCall>> mTransparentDrawCalls = {};
  std::shared_ptr<GlRenderer> mGlRenderer = nullptr;

  float mNextTexCoordS = 0.0f;
  float mNextTexCoordT = 0.0f;

  TextureCache mTextureCache;

  GXVRamTex mVramTex;
  GXVRamTexPltt mVramTexPalette;
};

GlobalState& GetGlobalState();

}



#endif
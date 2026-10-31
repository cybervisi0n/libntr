#ifndef LIBNTR_SIM_G3_DRAWCALL_HPP
#define LIBNTR_SIM_G3_DRAWCALL_HPP

#include <nitro/types.h>
#include "simulator/sim_g3_Texture.hpp"

#include <memory>

namespace SIM::G3 {

struct RenderVertex {
	//Position
	float x;
	float y;
	float z;
	float w;
	//TexCoord
	float s;
	float t;
	//Color
	float r;
	float g;
	float b;
	float a;
};

struct TexImageParam {
    u32 textureOffset;
    u8 repeatS;
    u8 repeatT;
    u8 flipS;
    u8 flipT;
    u32 textureSSize;
    u32 textureTSize;
    u8 textureFormat;
    u8 color0;
    u8 texCordTransform;
};

struct PolygonAttr {
    u8 lightFlag;
    u8 polygonMode;
    u8 cullMode;
    u8 translucentDepth;
    u8 renderFar;
    u8 singleDotDisp;
    u8 depthTest;
    u8 fogEnable;
    float alpha;
    u8 alphaInt;
    u8 polygonId;
};

// Stores all data necessary for a complete drawcall
class DrawCall {
 public:
  DrawCall();
  DrawCall(const DrawCall& source);
  ~DrawCall();

  void AddVertex(const RenderVertex& vert);
  void ClearVerts();

  bool IsTransparent();

  const RenderVertex * GetRenderVerts() { return mRenderVerts; }
  inline size_t GetRenderVertsCount() { return mRenderVertsCount; }
  inline PolygonAttr GetPolygonAttr() { return mPolygonAttr; }
  inline TexImageParam GetTexImageParam() { return mTexImageParam; }
  inline u32 GetTexPaletteBase() { return mTexPaletteBase; }
  inline std::shared_ptr<Texture> GetTexture() { return mTexture; }
  inline float * GetFogDepthTable() { return mFogDepthTable; }
  inline float * GetFogDensityTable() { return mFogDensityTable; }
  inline bool GetFogEnabled() { return mFogEnabled; }
  inline float * GetFogColor() { return mFogColor; }

  inline void SetTexImageParam(const TexImageParam& param) { mTexImageParam = param; }
  inline void SetTexPaletteBase(const u32 texPaletteBase) { mTexPaletteBase = texPaletteBase; }
  inline void SetPolygonAttr(const PolygonAttr& attr) { mPolygonAttr = attr; }
  inline void SetTexture(std::shared_ptr<Texture> texture) { mTexture = texture; }
  inline void SetFogColor(float r, float g, float b, float a) {
    mFogColor[0] = r;
    mFogColor[1] = g;
    mFogColor[2] = b;
    mFogColor[3] = a;
  }
  void SetFogDepthTable(float * depthTable);
  void SetFogDensityTable(float * densityTable);
  inline void SetFogEnabled(bool enable) { mFogEnabled = enable; }

 private:
  RenderVertex * mRenderVerts;
  TexImageParam mTexImageParam = {};
  u32 mTexPaletteBase = 0;
  PolygonAttr mPolygonAttr = {};
  std::shared_ptr<Texture> mTexture = nullptr;
  size_t mRenderVertsCount;
  size_t mRenderVertsCapacity;
  bool mFogEnabled = false;
  float mFogColor[4] = {};
  float mFogDepthTable[32] = {};
  float mFogDensityTable[32] = {};
};

}



#endif
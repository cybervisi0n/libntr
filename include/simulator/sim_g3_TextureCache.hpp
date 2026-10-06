#ifndef LIBNTR_SIM_G3_TEXTURE_ACHEHPP
#define LIBNTR_SIM_G3_TEXTURE_ACHEHPP

#include <nitro/types.h>
#include <unordered_map>
#include <memory>

#include "simulator/sim_g3_Texture.hpp"

namespace SIM::G3 {

class TextureCache {
 public:
  TextureCache();
  ~TextureCache();

  void AddTexture(u32 textureCRC, u32 paletteCRC, std::shared_ptr<Texture> texture);
  std::shared_ptr<Texture> GetTexture(u32 textureCRC, u32 paletteCRC);
  
 private:
  u64 GetCombinedCRC(u32 textureCRC, u32 paletteCRC);
  std::unordered_map<u64, std::shared_ptr<Texture>> mTextureMap = {};
};

}


#endif
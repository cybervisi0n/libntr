#include "simulator/sim_g3_TextureCache.hpp"

namespace SIM::G3 {

TextureCache::TextureCache() {

}

TextureCache::~TextureCache() {
    
}

void TextureCache::AddTexture(u32 textureCRC, u32 paletteCRC, std::shared_ptr<Texture> texture) {
    u64 combinedCRC = GetCombinedCRC(textureCRC, paletteCRC);
    mTextureMap[combinedCRC] = texture;
}

std::shared_ptr<Texture> TextureCache::GetTexture(u32 textureCRC, u32 paletteCRC) {
    u64 combinedCRC = GetCombinedCRC(textureCRC, paletteCRC);
    if(mTextureMap.count(combinedCRC)) {
        return mTextureMap[combinedCRC];
    }

    return nullptr;
}

u64 TextureCache::GetCombinedCRC(u32 textureCRC, u32 paletteCRC) {
    return (((u64)(textureCRC)) | (((u64)paletteCRC)) << 32);
}

}
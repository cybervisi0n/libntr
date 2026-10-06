#ifndef LIBNTR_SIM_G3_TEXTURE_HPP
#define LIBNTR_SIM_G3_TEXTURE_HPP

#include <nitro/types.h>

namespace SIM::G3 {

class Texture {
 public:
  Texture(u16 height, u16 width, u8 * data);
  ~Texture();

  void Activate();
  inline unsigned int GetTextureID() {return mGlTextureId;};

 private:
  unsigned int mGlTextureId;
};

}


#endif
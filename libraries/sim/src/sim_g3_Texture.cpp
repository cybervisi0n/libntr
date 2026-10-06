#include "simulator/sim_g3_Texture.hpp"

#include "simulator/glad/glad.h"

namespace SIM::G3 {

Texture::Texture(u16 height, u16 width, u8 * data) {
  glGenTextures(1, &mGlTextureId);                                                      
  glActiveTexture(GL_TEXTURE0);                                                
  glBindTexture(GL_TEXTURE_2D, mGlTextureId);                                           
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);           
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);           
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);                
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);                
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA,        
               GL_UNSIGNED_BYTE, data);
}

Texture::~Texture() {
    glDeleteTextures(1, &mGlTextureId);
}

void Texture::Activate() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mGlTextureId);
}

}
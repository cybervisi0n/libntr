#ifdef SDK_BUILD_NX
#include <glad/glad.h>
#else
#include <simulator/glad/glad.h>
#endif
#include <nitro/types.h>
#include <simulator/sim_g3_DrawCall.hpp>

#ifndef LIBNTR_SIM_G3_GL_RENDERER_HPP
#define LIBNTR_SIM_G3_GL_RENDERER_HPP

namespace SIM::G3 {

class GlRenderer {
 public:
  GlRenderer();
  ~GlRenderer();

  void Init();

  inline void SetShaderProgramId(unsigned int id) { mShaderProgramId = id; }

  void Draw(std::shared_ptr<DrawCall> drawCall);

 private:
  unsigned int mVertexArray;
  unsigned int mVertexBuffer;
  unsigned int mShaderProgramId;
  int mVertexBufferCount = 0;
};

}



#endif
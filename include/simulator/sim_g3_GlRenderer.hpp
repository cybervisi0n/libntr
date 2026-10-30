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

  void Draw(std::shared_ptr<DrawCall> drawCall);

 private:
  unsigned int mVertexArray;
  unsigned int mVertexBuffer;
  int mVertexBufferCount = 0;
};

//void G3SIM_AddVtx(SIM::G3::RenderVertex * vtx); -> Moves to DrawCall
//void G3SIM_DrawArray(); // Becomes GlRenderer::Draw()
//void G3SIM_DrawCleanUp(); // resets the render vertex count -> move to DrawCall
//void G3SIM_DrawInit();  // move to GlRenderer::Init
//void G3SIM_FlushArray(); // Move to globalState
//void G3SIM_DrawItems(); // Move to globalstate (it keeps an array of DrawCalls for transparent draws)

}



#endif
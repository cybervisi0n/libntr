#ifndef LIBNTR_SIM_RENDER_CONTEXT_MANAGER_HPP
#define LIBNTR_SIM_RENDER_CONTEXT_MANAGER_HPP

#include <nitro/types.h>
#include <SDL2/SDL.h>

namespace SIM {

void InitRenderContextManager(SDL_Window * window, SDL_GLContext context);

void SetContextPointer(SDL_GLContext * context);

bool AcquireRenderContext(void);
bool ReleaseRenderContext(void);


}

#endif

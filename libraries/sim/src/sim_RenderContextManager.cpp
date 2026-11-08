#include "simulator/sim_RenderContextManager.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>

static SDL_threadID s_renderContextThread = 0;
static SDL_mutex *s_renderContextMutex = nullptr;

static SDL_GLContext s_context;
static SDL_Window *s_window;

namespace SIM {

void InitRenderContextManager(SDL_Window *window, SDL_GLContext context)
{
    s_context = context;
    s_window = window;

    s_renderContextThread = SDL_ThreadID();
    s_renderContextMutex = SDL_CreateMutex();
}

bool AcquireRenderContext(void)
{
    const SDL_threadID currentThread = SDL_ThreadID();

    if (s_renderContextMutex == NULL) {
        return false;
    }
    SDL_LockMutex(s_renderContextMutex);

    if (s_window == NULL || s_context == NULL) {
        SDL_UnlockMutex(s_renderContextMutex);
        return false;
    }
    if (s_renderContextThread != 0 && s_renderContextThread != currentThread) {
        SDL_UnlockMutex(s_renderContextMutex);
        return false;
    }
    if (SDL_GL_GetCurrentContext() != s_context && SDL_GL_MakeCurrent(s_window, s_context) != 0) {
        fprintf(stderr, "libPorpoise SIM: OpenGL context acquisition failed: %s\n", SDL_GetError());
        SDL_UnlockMutex(s_renderContextMutex);
        return false;
    }
    s_renderContextThread = currentThread;
    SDL_UnlockMutex(s_renderContextMutex);
    return true;
}

bool ReleaseRenderContext(void)
{
    const SDL_threadID currentThread = SDL_ThreadID();

    if (s_renderContextMutex == NULL) {
        return false;
    }
    SDL_LockMutex(s_renderContextMutex);

    if (s_context == NULL || s_renderContextThread == 0) {
        SDL_UnlockMutex(s_renderContextMutex);
        return true;
    }
    if (s_renderContextThread != currentThread) {
        SDL_UnlockMutex(s_renderContextMutex);
        return false;
    }
    if (SDL_GL_GetCurrentContext() == s_context && SDL_GL_MakeCurrent(NULL, NULL) != 0) {
        SDL_UnlockMutex(s_renderContextMutex);
        return false;
    }
    s_renderContextThread = 0;
    SDL_UnlockMutex(s_renderContextMutex);
    return true;
}

}

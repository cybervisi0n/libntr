#include <SDL2/SDL_video.h>
#include "simulator/sim.h"
#include "simulator/sim_gui.hpp"

#include "gui_utils.hpp"



namespace SIM::GUI {

u32 CenteredXPx(u32 width) {
    auto sdlWindow = SIM_GetSDLWindow();

    int windowWidth;
    SDL_GetWindowSizeInPixels(sdlWindow, &windowWidth, nullptr);
    if(width > windowWidth) {
        return 0;
    }

    return (windowWidth - width) / 2;
}

u32 CenteredYPx(u32 height) {
    auto sdlWindow = SIM_GetSDLWindow();

    int windowHeight;
    SDL_GetWindowSizeInPixels(sdlWindow, nullptr, &windowHeight);
    if(height > windowHeight) {
        return 0;
    }

    return (windowHeight - height) / 2;
}

u32 PercentXPx(float percent) {
    auto sdlWindow = SIM_GetSDLWindow();

    int windowWidth;
    SDL_GetWindowSizeInPixels(sdlWindow, &windowWidth, nullptr);

    return (u32)((float)windowWidth * percent);
}

u32 PercentYPx(float percent) {
    auto sdlWindow = SIM_GetSDLWindow();

    int windowHeight;
    SDL_GetWindowSizeInPixels(sdlWindow, nullptr, &windowHeight);

    return (u32)((float)windowHeight * percent);
}

}
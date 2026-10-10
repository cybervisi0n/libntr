#include "simulator/sim.h"
#include "simulator/sim_gui.hpp"

#include "simulator/imgui/imgui.hpp"
#include "simulator/config/sim_config.h"

#include "gui_internal.hpp"

#include <array>
#include <SDL2/SDL.h>

namespace SIM::GUI {

static constexpr ImVec2 s_btnSize = {50, 20};
static constexpr u8 MinMasterVolume = 0;
static constexpr u8 MaxMasterVolume = 127;
static constexpr u8 MinInternalRes = 1;
static constexpr u8 MaxInternalRes = 20;

static SIM_config_type * sConfig;

static constexpr const char * ScreenLayoutStrings[] = {
    "Vertical",
    "Horizontal",
    "Large Screen"
};

static constexpr const char * VblankStrings[] = {
    "Off",
    "Every V-Blank",
    "Every second V-Blank"
};

void AppConfigInit() {
    sConfig = SIM_GetConfigPtr();
}

void AppConfigMain(bool *openState) {
    bool configChanged = false;
    int screenLayout = sConfig->screenLayout;
    bool swapScreens = sConfig->swapScreens;
    int vSyncInterval = sConfig->vsyncInterval;
    bool frameLimit = sConfig->capFrameRate;
    bool fullScreen = sConfig->fullScreen;
    
    ImGui::Begin("Config", openState);

    ImGui::Text("Internal Resolution");
    if(ImGui::SliderScalar(" ", ImGuiDataType_U8, &sConfig->internalResolutionScale, &MinInternalRes, &MaxInternalRes, "%dx NDS")) {
        configChanged = true;
        SIM_SetInternalResolutionAfterRender(sConfig->internalResolutionScale);
    }

    if(ImGui::Checkbox("Fullscreen", &fullScreen)) {
        configChanged = true;
    }

    if(ImGui::Combo("Layout", &screenLayout, ScreenLayoutStrings, 3, 3)) {
        configChanged = true;
    }
    if(ImGui::Checkbox("Swap Screens", &swapScreens)) {
        configChanged = true;
    }
    if(ImGui::Combo("VSync", &vSyncInterval, VblankStrings, 3, 3)) {
        configChanged = true;
    }
    if(ImGui::Checkbox("Cap framerate", &frameLimit)) {
        configChanged = true;
    }

    ImGui::Text("Master Volume");
    if(ImGui::SliderScalar("  ", ImGuiDataType_U8, &sConfig->masterVolume, &MinMasterVolume, &MaxMasterVolume)) {
        configChanged = true;
    }

    ImGui::End();

    if(configChanged) {
        sConfig->screenLayout = static_cast<SIM_config_screen_layout_type>(screenLayout);
        sConfig->swapScreens = swapScreens;
        sConfig->fullScreen = fullScreen;
        if(sConfig->fullScreen) {
            SDL_SetWindowFullscreen(SIM_GetSDLWindow(), SDL_WINDOW_FULLSCREEN_DESKTOP);
        } else {
            SDL_SetWindowFullscreen(SIM_GetSDLWindow(), 0);
        }
        sConfig->vsyncInterval = vSyncInterval;
        if(vSyncInterval != SDL_GL_GetSwapInterval()) {
            SDL_GL_SetSwapInterval(vSyncInterval);
        }
        sConfig->capFrameRate = frameLimit;

        SIM_Config_SaveConfigFile(sConfig);
    }
}

}
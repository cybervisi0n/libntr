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
    
    ImGui::Begin("Config", openState);

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

    if(ImGui::SliderScalar("Master Volume", ImGuiDataType_U8, &sConfig->masterVolume, &MinMasterVolume, &MaxMasterVolume));

    ImGui::End();

    if(configChanged) {
        sConfig->screenLayout = static_cast<SIM_config_screen_layout_type>(screenLayout);
        sConfig->swapScreens = swapScreens;
        sConfig->vsyncInterval = vSyncInterval;
        if(vSyncInterval != SDL_GL_GetSwapInterval()) {
            SDL_GL_SetSwapInterval(vSyncInterval);
        }
        sConfig->capFrameRate = frameLimit;

        SIM_Config_SaveConfigFile(sConfig);
    }
}

}
#include <nitro.h>
#include <simulator/sim.h>
#include <simulator/glad/glad.h>

#include "gui_internal.h"

#include "simulator/gui.h"
#include "simulator/config/sim_config.h"

#include <nitro/os.h>

// External variables
extern SIM_config_type s_SIM_config;


static ImVec2 s_btnSize = {50, 20};
static bool sConfigChanged = false;
static int sScreenLayout;
static bool sSwapScreens;
static bool sFrameLimit = false;
static int sVSyncInterval;

static const char * ScreenLayoutStrings[] = {
    "Vertical",
    "Horizontal",
    "Large Screen"
};

static const char * VblankStrings[] = {
    "Off",
    "Every V-Blank",
    "Every second V-Blank"
};

// Static functions



void GUI_AppConfigMain(bool * p_open)
{
    sConfigChanged = false;
    sScreenLayout = s_SIM_config.screenLayout;
    sSwapScreens = s_SIM_config.swapScreens;
    sVSyncInterval = s_SIM_config.vsyncInterval;
    sFrameLimit = s_SIM_config.capFrameRate;

    igBegin("Config", p_open, 0);
    if(igCombo_Str_arr("Layout", &sScreenLayout, ScreenLayoutStrings, 3, 3)) {
        sConfigChanged = true;
    }
    if(igCheckbox("Swap Screens", &sSwapScreens)){
        sConfigChanged = true;
    }
    if(igCombo_Str_arr("VSync", &sVSyncInterval, VblankStrings, 3, 3)) {
        sConfigChanged = true;
    }
    if(igCheckbox("Cap framerate", &sFrameLimit)){
        sConfigChanged = true;
    }

    
    igEnd();

    if(sConfigChanged) {
        s_SIM_config.screenLayout = sScreenLayout;
        s_SIM_config.swapScreens = sSwapScreens;
        s_SIM_config.vsyncInterval = sVSyncInterval;
        if(sVSyncInterval != SDL_GL_GetSwapInterval()) {
            SDL_GL_SetSwapInterval(sVSyncInterval);
        }
        s_SIM_config.capFrameRate = sFrameLimit;

        SIM_Config_SaveConfigFile(&s_SIM_config);
    }
    return;
}

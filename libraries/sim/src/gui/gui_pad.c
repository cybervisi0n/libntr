#include <nitro.h>
#include <simulator/sim.h>
#include <simulator/glad/glad.h>

#include "gui_internal.h"

#include "simulator/gui.h"
#include "simulator/config/sim_config.h"

#include <nitro/os.h>

#include <SDL2/SDL.h>

// External variables
extern SIM_config_type s_SIM_config;


static ImVec2 s_btnSize = {50, 20};
static bool sConfigChanged = false;
static char joystickButtonNameBuf[10] = {0};

// Static functions
static void SetKeyConfig(int * aConfig) {
    SDL_Event Event;
    while(TRUE) {
        while( SDL_PollEvent(&Event)) {
            if(Event.type == SDL_KEYDOWN)
            {
                *aConfig = Event.key.keysym.sym;
                return;
            }
        }
    }
}

static void SetJoystickKeyConfig(int * aConfig) {
    SDL_Event Event;
    while(TRUE) {
        while( SDL_PollEvent(&Event)) {
            if(Event.type == SDL_JOYBUTTONDOWN)
            {
                *aConfig = Event.jbutton.button;
                return;
            }
            if(Event.type == SDL_JOYHATMOTION && Event.jhat.value != SDL_HAT_CENTERED)
            {
                *aConfig = Event.jhat.value | SIM_CONFIG_JOY_HAT_MASK;
                return;
            }
            if(Event.type == SDL_JOYAXISMOTION)
            {
                if(Event.jaxis.value > s_SIM_config.padSettings.joyAxisDeadzone
                || Event.jaxis.value < (s_SIM_config.padSettings.joyAxisDeadzone*-1)) {
                    if(Event.jaxis.value > 0) {
                        *aConfig = Event.jaxis.axis | SIM_CONFIG_JOY_AXIS_PLUS_MASK;
                    } else {
                        *aConfig = Event.jaxis.axis | SIM_CONFIG_JOY_AXIS_MINUS_MASK;
                    }
                    return;
                }
            }
        }
    }
}

static void GetJoyButtonName(int aButton, char * aBuf) {
    if(aButton == SIM_CONFIG_JOY_INVALID_KEY) {
        sprintf(aBuf, " ");
    } else if(aButton & SIM_CONFIG_JOY_HAT_MASK) {
        sprintf(aBuf, "Hat %d", aButton & ~(SIM_CONFIG_JOY_HAT_MASK));
    } else if(aButton & SIM_CONFIG_JOY_AXIS_MINUS_MASK) {
        sprintf(aBuf, "Axis %d-", aButton & ~(SIM_CONFIG_JOY_AXIS_MINUS_MASK));
    } else if(aButton & SIM_CONFIG_JOY_AXIS_PLUS_MASK){
        sprintf(aBuf, "Axis %d+", aButton & ~(SIM_CONFIG_JOY_AXIS_PLUS_MASK));
    } else {
        sprintf(aBuf, "%d", aButton);
    }
}


#define KeySettingButton(_keyString,_btnId,_key) \
igText(_keyString); \
igSameLine(0.0f, 1.0f); \
igPushID_Int(_btnId); \
if(igButton(SDL_GetKeyName(_key), s_btnSize)) { \
    SetKeyConfig(&_key); \
    sConfigChanged = true; \
} \
igPopID();


#define JoystickKeySettingButton(_keyString,_btnId,_key) \
igText(_keyString); \
igSameLine(0.0f, 1.0f); \
igPushID_Int(_btnId); \
GetJoyButtonName(_key,joystickButtonNameBuf); \
if(igButton(joystickButtonNameBuf, s_btnSize)) { \
    SetJoystickKeyConfig(&_key); \
    sConfigChanged = true; \
} \
igPopID();


void GUI_AppPadMain(bool * p_open)
{
    sConfigChanged = false;

    igBegin("Input", p_open, 0);

    igText("Keyboard");

    igColumns(2, "pad", true);

    KeySettingButton("A   ", 1, s_SIM_config.padSettings.aKey);
    KeySettingButton("B   ", 2, s_SIM_config.padSettings.bKey);
    KeySettingButton("X   ", 3, s_SIM_config.padSettings.xKey);
    KeySettingButton("Y   ", 4, s_SIM_config.padSettings.yKey);
    KeySettingButton("L   ", 5, s_SIM_config.padSettings.lKey);
    KeySettingButton("R   ", 6, s_SIM_config.padSettings.rKey);
    KeySettingButton("GUI ", 6, s_SIM_config.padSettings.guiKey);

    igNextColumn();

    KeySettingButton("Up     ", 7, s_SIM_config.padSettings.upKey);
    KeySettingButton("Down   ", 8, s_SIM_config.padSettings.downKey);
    KeySettingButton("Left   ", 9, s_SIM_config.padSettings.leftKey);
    KeySettingButton("Right  ", 10, s_SIM_config.padSettings.rightKey);
    KeySettingButton("Start  ", 11, s_SIM_config.padSettings.startKey);
    KeySettingButton("Select ", 12, s_SIM_config.padSettings.selectKey);

    igEndColumns();

    igSeparator();
    igText("Gamepad");

    igColumns(2, "joystick", true);

    JoystickKeySettingButton("A ", 13, s_SIM_config.padSettings.aJoyKey);
    JoystickKeySettingButton("B ", 14, s_SIM_config.padSettings.bJoyKey);
    JoystickKeySettingButton("X ", 15, s_SIM_config.padSettings.xJoyKey);
    JoystickKeySettingButton("Y ", 16, s_SIM_config.padSettings.yJoyKey);
    JoystickKeySettingButton("L ", 17, s_SIM_config.padSettings.lJoyKey);
    JoystickKeySettingButton("R ", 18, s_SIM_config.padSettings.rJoyKey);

    igNextColumn();

    JoystickKeySettingButton("Up     ", 19, s_SIM_config.padSettings.upJoyKey);
    JoystickKeySettingButton("Down   ", 20, s_SIM_config.padSettings.downJoyKey);
    JoystickKeySettingButton("Left   ", 21, s_SIM_config.padSettings.leftJoyKey);
    JoystickKeySettingButton("Right  ", 22, s_SIM_config.padSettings.rightJoyKey);
    JoystickKeySettingButton("Start  ", 23, s_SIM_config.padSettings.startJoyKey);
    JoystickKeySettingButton("Select ", 24, s_SIM_config.padSettings.selectJoyKey);

    igEndColumns();
    
    igEnd();

    if(sConfigChanged) {
        SIM_Config_SaveConfigFile(&s_SIM_config);
    }
    return;
}

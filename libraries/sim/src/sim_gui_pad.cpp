#include "simulator/sim.h"
#include "simulator/sim_gui.hpp"

#include "simulator/imgui/imgui.hpp"
#include "simulator/config/sim_config.h"

#include "gui_internal.hpp"

#include <format>
#include <string>
#include <SDL2/SDL.h>

namespace SIM::GUI {

static constexpr ImVec2 BtnSize = {50, 20};

static SIM_config_type * sConfig;
static bool sConfigChanged = false;

static void SetKeyConfig(int& config) {
    SDL_Event Event;
    while(true) {
        while( SDL_PollEvent(&Event)) {
            if(Event.type == SDL_KEYDOWN)
            {
                config = Event.key.keysym.sym;
                return;
            }
        }
    }
}

static void SetJoystickKeyConfig(int& config) {
    SDL_Event Event;
    while(true) {
        while( SDL_PollEvent(&Event)) {
            if(Event.type == SDL_JOYBUTTONDOWN)
            {
                config = Event.jbutton.button;
                return;
            }
            if(Event.type == SDL_JOYHATMOTION && Event.jhat.value != SDL_HAT_CENTERED)
            {
                config = Event.jhat.value | SIM_CONFIG_JOY_HAT_MASK;
                return;
            }
            if(Event.type == SDL_JOYAXISMOTION)
            {
                if(Event.jaxis.value > sConfig->padSettings.joyAxisDeadzone
                || Event.jaxis.value < (sConfig->padSettings.joyAxisDeadzone*-1)) {
                    if(Event.jaxis.value > 0) {
                        config = Event.jaxis.axis | SIM_CONFIG_JOY_AXIS_PLUS_MASK;
                    } else {
                        config = Event.jaxis.axis | SIM_CONFIG_JOY_AXIS_MINUS_MASK;
                    }
                    return;
                }
            }
        }
    }
}

static std::string GetJoystickKeyName(int key) {
    std::string ret = "";
    if(key & SIM_CONFIG_JOY_HAT_MASK) {
        ret = std::format("Hat {}", key & ~(SIM_CONFIG_JOY_HAT_MASK));
    } else if(key & SIM_CONFIG_JOY_AXIS_MINUS_MASK) {
        ret = std::format("Axis {}-", key & ~(SIM_CONFIG_JOY_AXIS_MINUS_MASK));
    } else if(key & SIM_CONFIG_JOY_AXIS_PLUS_MASK){
        ret = std::format("Axis {}+", key & ~(SIM_CONFIG_JOY_AXIS_PLUS_MASK));
    } else {
        ret = std::format("{}", key);
    }

    return ret;
}

static void KeySettingButton(const char * keyName, int btnId, int& key) {
    ImGui::Text(keyName);
    ImGui::SameLine(0.0f, 1.0f);
    ImGui::PushID(btnId);
    if(ImGui::Button(SDL_GetKeyName(key), BtnSize)) {
        SetKeyConfig(key);
        sConfigChanged = true;
    }
    ImGui::PopID();
}

static void JoystickKeySettingButton(const char * keyName, int btnId, int& key) {
    ImGui::Text(keyName);
    ImGui::SameLine(0.0f, 1.0f);
    ImGui::PushID(btnId);
    auto buttonName = GetJoystickKeyName(key);
    if(ImGui::Button(buttonName.c_str(), BtnSize)) {
        SetJoystickKeyConfig(key);
        sConfigChanged = true;
    }
    ImGui::PopID();
}

void AppPadInit() {
    sConfig = SIM_GetConfigPtr();
}

void AppPadMain(bool *openState) {
    ImGui::Begin("Input", openState);

    ImGui::Text("Keyboard");
    ImGui::Columns(2, "padKeyboard", true);

    KeySettingButton("A   ", 1, sConfig->padSettings.aKey);
    KeySettingButton("B   ", 2, sConfig->padSettings.bKey);
    KeySettingButton("X   ", 3, sConfig->padSettings.xKey);
    KeySettingButton("Y   ", 4, sConfig->padSettings.yKey);
    KeySettingButton("L   ", 5, sConfig->padSettings.lKey);
    KeySettingButton("R   ", 6, sConfig->padSettings.rKey);
    KeySettingButton("GUI ", 7, sConfig->padSettings.guiKey);
    KeySettingButton("Frame Cap Toggle ", 8, sConfig->padSettings.frameCapToggleKey);

    ImGui::NextColumn();

    KeySettingButton("Up     ", 9,  sConfig->padSettings.upKey);
    KeySettingButton("Down   ", 10,  sConfig->padSettings.downKey);
    KeySettingButton("Left   ", 11,  sConfig->padSettings.leftKey);
    KeySettingButton("Right  ", 12, sConfig->padSettings.rightKey);
    KeySettingButton("Start  ", 13, sConfig->padSettings.startKey);
    KeySettingButton("Select ", 14, sConfig->padSettings.selectKey);

    ImGui::Columns(1, "pad2", false);
    ImGui::Separator();
    ImGui::Text("Gamepad");
    ImGui::Columns(2, "padJoystick", true);

    JoystickKeySettingButton("A ", 15, sConfig->padSettings.aJoyKey);
    JoystickKeySettingButton("B ", 16, sConfig->padSettings.bJoyKey);
    JoystickKeySettingButton("X ", 17, sConfig->padSettings.xJoyKey);
    JoystickKeySettingButton("Y ", 18, sConfig->padSettings.yJoyKey);
    JoystickKeySettingButton("L ", 19, sConfig->padSettings.lJoyKey);
    JoystickKeySettingButton("R ", 20, sConfig->padSettings.rJoyKey);

    ImGui::NextColumn();

    JoystickKeySettingButton("Up     ", 21, sConfig->padSettings.upJoyKey);
    JoystickKeySettingButton("Down   ", 22, sConfig->padSettings.downJoyKey);
    JoystickKeySettingButton("Left   ", 23, sConfig->padSettings.leftJoyKey);
    JoystickKeySettingButton("Right  ", 24, sConfig->padSettings.rightJoyKey);
    JoystickKeySettingButton("Start  ", 25, sConfig->padSettings.startJoyKey);
    JoystickKeySettingButton("Select ", 26, sConfig->padSettings.selectJoyKey);

    ImGui::End();

    if(sConfigChanged) {
        SIM_Config_SaveConfigFile(sConfig);
    }
}

}
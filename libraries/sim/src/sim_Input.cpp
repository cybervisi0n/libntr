#include "simulator/sim_Input.hpp"

#include <nitro/types.h>
#include <nitro/ioreg.h>
#include <nitro/hw/X86/mmap_shared.h>

#include <SDL2/SDL.h>

#include "simulator/sim.h"
#include "simulator/config/sim_config.h"
#include "simulator/sim_gui.hpp"

// TODO: remove
extern TPData s_tpData;

namespace SIM::Input {

static SDL_Joystick *sSDLJoystick = NULL;

static u8 isMouseDown = 0;

static u64 map(u64 x, u64 in_min, u64 in_max, u64 out_min, u64 out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

static void HandleJoystickKeyUp(int aKey) {
  auto* simConfig = SIM_GetConfigPtr();
  if (aKey == simConfig->padSettings.aJoyKey) {
    // A Button
    s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT | 0b0000000000000001;
  } else if (aKey == simConfig->padSettings.bJoyKey) {
    // B Button
    s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT | 0b0000000000000010;
  } else if (aKey == simConfig->padSettings.xJoyKey) {
    // X Button
    *(vu16 *)HW_BUTTON_XY_BUF = *(vu16 *)HW_BUTTON_XY_BUF | 0b0000010000000000;
  } else if (aKey == simConfig->padSettings.yJoyKey) {
    // y Button
    *(vu16 *)HW_BUTTON_XY_BUF = *(vu16 *)HW_BUTTON_XY_BUF | 0b0000100000000000;
  } else if (aKey == simConfig->padSettings.upJoyKey) {
    // Dpad Up
    s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT | 0b0000000001000000;
  } else if (aKey == simConfig->padSettings.downJoyKey) {
    // Dpad Down
    s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT | 0b0000000010000000;
  } else if (aKey == simConfig->padSettings.leftJoyKey) {
    // Dpad Left
    s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT | 0b0000000000100000;
  } else if (aKey == simConfig->padSettings.rightJoyKey) {
    // Dpad Right
    s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT | 0b0000000000010000;
  } else if (aKey == simConfig->padSettings.lJoyKey) {
    // L
    s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT | 0b0000001000000000;
  } else if (aKey == simConfig->padSettings.rJoyKey) {
    // R
    s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT | 0b0000000100000000;
  } else if (aKey == simConfig->padSettings.startJoyKey) {
    // Start button
    s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT | 0b0000000000001000;
  } else if (aKey == simConfig->padSettings.selectJoyKey) {
    // Select button
    s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT | 0b0000000000000100;
  }
}

static void HandleJoystickKeyDown(int aKey) {
  auto * simConfig = SIM_GetConfigPtr();
  if (aKey == simConfig->padSettings.aJoyKey) {
    // A Button
    s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT & 0b1111111111111110;
  } else if (aKey == simConfig->padSettings.bJoyKey) {
    // B Button
    s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT & 0b1111111111111101;
  } else if (aKey == simConfig->padSettings.xJoyKey) {
    // X Button
    *(vu16 *)HW_BUTTON_XY_BUF = *(vu16 *)HW_BUTTON_XY_BUF & 0b1111100000000000;
  } else if (aKey == simConfig->padSettings.yJoyKey) {
    // y Button
    *(vu16 *)HW_BUTTON_XY_BUF = *(vu16 *)HW_BUTTON_XY_BUF & 0b1111010000000000;
  } else if (aKey == simConfig->padSettings.upJoyKey) {
    // Dpad Up
    s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT & 0b1111111110111111;
  } else if (aKey == simConfig->padSettings.downJoyKey) {
    // Dpad Down
    s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT & 0b1111111101111111;
  } else if (aKey == simConfig->padSettings.leftJoyKey) {
    // Dpad Left
    s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT & 0b1111111111011111;
  } else if (aKey == simConfig->padSettings.rightJoyKey) {
    // Dpad Right
    s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT & 0b1111111111101111;
  } else if (aKey == simConfig->padSettings.lJoyKey) {
    // L
    s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT & 0b1111110111111111;
  } else if (aKey == simConfig->padSettings.rJoyKey) {
    // R
    s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT & 0b1111111011111111;
  } else if (aKey == simConfig->padSettings.startJoyKey) {
    // Start button
    s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT & 0b1111111111110111;
  } else if (aKey == simConfig->padSettings.selectJoyKey) {
    // Select button
    s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT & 0b1111111111111011;
  }
}


void Process() {
  auto * simConfig = SIM_GetConfigPtr();

  // Check Joysticks
  if (sSDLJoystick == NULL && SDL_NumJoysticks() >= 1) {
    sSDLJoystick = SDL_JoystickOpen(0);
  }

  SDL_Event Event;
  while (SDL_PollEvent(&Event)) {
    SIM::GUI::ProcessEvent(&Event);

    if (Event.type == SDL_WINDOWEVENT) {
      switch (Event.window.event) {
      case SDL_WINDOWEVENT_CLOSE:
        exit(0);
        break;
      }
    }
    if (Event.type == SDL_KEYDOWN) {
      int keyRead = Event.key.keysym.sym;
      if (keyRead == simConfig->padSettings.aKey) {
        // A Button
        s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT & 0b1111111111111110;
      } else if (keyRead == simConfig->padSettings.bKey) {
        // B Button
        s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT & 0b1111111111111101;
      } else if (keyRead == simConfig->padSettings.xKey) {
        // X Button
        *(vu16 *)HW_BUTTON_XY_BUF =
            *(vu16 *)HW_BUTTON_XY_BUF & 0b1111100000000000;
      } else if (keyRead == simConfig->padSettings.yKey) {
        // y Button
        *(vu16 *)HW_BUTTON_XY_BUF =
            *(vu16 *)HW_BUTTON_XY_BUF & 0b1111010000000000;
      } else if (keyRead == simConfig->padSettings.upKey) {
        // Dpad Up
        s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT & 0b1111111110111111;
      } else if (keyRead == simConfig->padSettings.downKey) {
        // Dpad Down
        s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT & 0b1111111101111111;
      } else if (keyRead == simConfig->padSettings.leftKey) {
        // Dpad Left
        s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT & 0b1111111111011111;
      } else if (keyRead == simConfig->padSettings.rightKey) {
        // Dpad Right
        s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT & 0b1111111111101111;
      } else if (keyRead == simConfig->padSettings.lKey) {
        // L
        s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT & 0b1111110111111111;
      } else if (keyRead == simConfig->padSettings.rKey) {
        // R
        s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT & 0b1111111011111111;
      } else if (keyRead == simConfig->padSettings.startKey) {
        // Start button
        s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT & 0b1111111111110111;
      } else if (keyRead == simConfig->padSettings.selectKey) {
        // Select button
        s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT & 0b1111111111111011;
      } else if (keyRead == simConfig->padSettings.guiKey) {
        // Toggle Debug GUI
        SIM::GUI::Toggle();
      } else if (keyRead == simConfig->padSettings.frameCapToggleKey) {
        // Toggle Frame Cap
        if (simConfig->capFrameRate) {
          simConfig->capFrameRate = false;
          simConfig->vsyncInterval = 0; // VSync Off
        } else {
          simConfig->capFrameRate = true;
          simConfig->vsyncInterval = 1; // Every V-Blank
        }

        SDL_GL_SetSwapInterval(simConfig->vsyncInterval);
        
      } else if(keyRead == simConfig->padSettings.frameCapKey) {
        SIM_SetFastForward(true);
        SDL_GL_SetSwapInterval(0);
      }
    }
    if (Event.type == SDL_KEYUP) {
      int keyRead = Event.key.keysym.sym;
      if (keyRead == simConfig->padSettings.aKey) {
        // A Button
        s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT | 0b0000000000000001;
      } else if (keyRead == simConfig->padSettings.bKey) {
        // B Button
        s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT | 0b0000000000000010;
      } else if (keyRead == simConfig->padSettings.xKey) {
        // X Button
        *(vu16 *)HW_BUTTON_XY_BUF =
            *(vu16 *)HW_BUTTON_XY_BUF | 0b0000010000000000;
      } else if (keyRead == simConfig->padSettings.yKey) {
        // y Button
        *(vu16 *)HW_BUTTON_XY_BUF =
            *(vu16 *)HW_BUTTON_XY_BUF | 0b0000100000000000;
      } else if (keyRead == simConfig->padSettings.upKey) {
        // Dpad Up
        s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT | 0b0000000001000000;
      } else if (keyRead == simConfig->padSettings.downKey) {
        // Dpad Down
        s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT | 0b0000000010000000;
      } else if (keyRead == simConfig->padSettings.leftKey) {
        // Dpad Left
        s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT | 0b0000000000100000;
      } else if (keyRead == simConfig->padSettings.rightKey) {
        // Dpad Right
        s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT | 0b0000000000010000;
      } else if (keyRead == simConfig->padSettings.lKey) {
        // L
        s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT | 0b0000001000000000;
      } else if (keyRead == simConfig->padSettings.rKey) {
        // R
        s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT | 0b0000000100000000;
      } else if (keyRead == simConfig->padSettings.startKey) {
        // Start button
        s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT | 0b0000000000001000;
      } else if (keyRead == simConfig->padSettings.selectKey) {
        // Select button
        s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT | 0b0000000000000100;
      } else if(keyRead == simConfig->padSettings.frameCapKey) {
        SIM_SetFastForward(false);
        SDL_GL_SetSwapInterval(simConfig->vsyncInterval);
      }
    }
    if (Event.type == SDL_JOYBUTTONDOWN) {
      HandleJoystickKeyDown(Event.jbutton.button);
    }
    if (Event.type == SDL_JOYBUTTONUP) {
      HandleJoystickKeyUp(Event.jbutton.button);
    }
    if (Event.type == SDL_JOYHATMOTION) {
      if (Event.jhat.value == SDL_HAT_CENTERED) {
        // send keyUp event for all hats
        HandleJoystickKeyUp(SIM_CONFIG_JOY_HAT_MASK | SDL_HAT_UP);
        HandleJoystickKeyUp(SIM_CONFIG_JOY_HAT_MASK | SDL_HAT_DOWN);
        HandleJoystickKeyUp(SIM_CONFIG_JOY_HAT_MASK | SDL_HAT_LEFT);
        HandleJoystickKeyUp(SIM_CONFIG_JOY_HAT_MASK | SDL_HAT_RIGHT);
      } else {
        if (Event.jhat.value & SDL_HAT_UP) {
          HandleJoystickKeyDown(SIM_CONFIG_JOY_HAT_MASK | SDL_HAT_UP);
        } else {
          HandleJoystickKeyUp(SIM_CONFIG_JOY_HAT_MASK | SDL_HAT_UP);
        }
        if (Event.jhat.value & SDL_HAT_DOWN) {
          HandleJoystickKeyDown(SIM_CONFIG_JOY_HAT_MASK | SDL_HAT_DOWN);
        } else {
          HandleJoystickKeyUp(SIM_CONFIG_JOY_HAT_MASK | SDL_HAT_DOWN);
        }
        if (Event.jhat.value & SDL_HAT_LEFT) {
          HandleJoystickKeyDown(SIM_CONFIG_JOY_HAT_MASK | SDL_HAT_LEFT);
        } else {
          HandleJoystickKeyUp(SIM_CONFIG_JOY_HAT_MASK | SDL_HAT_LEFT);
        }
        if (Event.jhat.value & SDL_HAT_RIGHT) {
          HandleJoystickKeyDown(SIM_CONFIG_JOY_HAT_MASK | SDL_HAT_RIGHT);
        } else {
          HandleJoystickKeyUp(SIM_CONFIG_JOY_HAT_MASK | SDL_HAT_RIGHT);
        }
      }
    }
    if (Event.type == SDL_JOYAXISMOTION) {
      if (Event.jaxis.value > simConfig->padSettings.joyAxisDeadzone ||
          Event.jaxis.value <
              (simConfig->padSettings.joyAxisDeadzone * -1)) {
        if (Event.jaxis.value > 0) {
          HandleJoystickKeyDown(Event.jaxis.axis |
                                SIM_CONFIG_JOY_AXIS_PLUS_MASK);
          HandleJoystickKeyUp(Event.jaxis.axis |
                              SIM_CONFIG_JOY_AXIS_MINUS_MASK);
        } else {
          HandleJoystickKeyDown(Event.jaxis.axis |
                                SIM_CONFIG_JOY_AXIS_MINUS_MASK);
          HandleJoystickKeyUp(Event.jaxis.axis |
                              SIM_CONFIG_JOY_AXIS_PLUS_MASK);
        }
      } else {
        HandleJoystickKeyUp(Event.jaxis.axis | SIM_CONFIG_JOY_AXIS_PLUS_MASK);
        HandleJoystickKeyUp(Event.jaxis.axis |
                            SIM_CONFIG_JOY_AXIS_MINUS_MASK);
      }
    }
    if (Event.type == SDL_MOUSEBUTTONDOWN) {
      isMouseDown = 1;
      s_tpData.validity = 0;
      s_tpData.touch = 1;
    }
    if (Event.type == SDL_MOUSEBUTTONUP) {
      isMouseDown = 0;
      s_tpData.validity = 0;
      s_tpData.touch = 0;
    }
  }

  int x;
  int y;

  SDL_GetMouseState(&x, &y);

  const u32 * touchPanelCoords = SIM_GetTouchPanelCoords();

  // Convert touch coordinate to DS screen coordinates
  if (x >= touchPanelCoords[0] && x <= touchPanelCoords[2] &&
      y >= touchPanelCoords[1] && y <= touchPanelCoords[3]) {
    s_tpData.x = (u16)map(x, touchPanelCoords[0], touchPanelCoords[2], 0,
                          SIM_NDS_SCREEN_WIDTH);
    s_tpData.y = (u16)map(y, touchPanelCoords[1], touchPanelCoords[3], 0,
                          SIM_NDS_SCREEN_HEIGHT);
  } else {
    s_tpData.validity = 0;
    s_tpData.touch = 0;
  }

}

}

#ifndef SIMULATOR_GUI_H
#define SIMULATOR_GUI_H

#include <SDL2/SDL.h>
#include <stdbool.h>

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#define CIMGUI_USE_SDL2
#define CIMGUI_USE_OPENGL3
#include <simulator/gui/cimgui.h>
#include <simulator/gui/cimgui_impl.h>

void SIM_GUI_Init(SDL_Window * aWindow, SDL_GLContext aContext);
void SIM_GUI_Main(void);
void SIM_GUI_NewFrame(void);
void SIM_GUI_ProcessEvent(SDL_Event * aEvent);
void SIM_GUI_Render(void);
void SIM_GUI_Toggle(void);
bool SIM_GUI_IsGameLogicPaused(void);

void SIM_GUI_AppButton(const char * aLabel, bool * aState, void (*aAppFunc)(bool *));

#endif
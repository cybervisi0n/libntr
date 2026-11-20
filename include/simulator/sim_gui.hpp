#ifndef LIBNTR_SIM_GUI_HPP
#define LIBNTR_SIM_GUI_HPP

#include <nitro/types.h>
#include <SDL2/SDL.h>
#include <functional>
#include <string>

namespace SIM::GUI {

void Init(SDL_Window * window, SDL_GLContext context);
void Main();
void NewFrame();
bool ProcessEvent(SDL_Event * event);
void Render();
void Toggle();
void Enable();
void OpenFileDialog(std::function<void(std::string)> callback);
bool IsGameLogicPaused();
void SetGameLogicPaused(bool paused);
void AppButton(const char * label, bool * state, void (*aInitFunc)(void), void (*aAppFunc)(bool *));



}


#endif

#include <nitro.h>
#include <simulator/sim.h>

#include "gui_internal.h"

#include "simulator/gui.h"

extern u64 s_SIM_frameTime;

static SDL_Window * SIM_GUI_SDLwindow;
static SDL_GLContext SIM_GUI_GLcontext;

static ImGuiContext * SIM_GUI_ig_context;

static bool SIM_GUI_State = 0;

static bool s_showWindowConfig = 0;
static bool s_showWindowG2 = 0;
static bool s_showWindowNet = 0;
static bool s_showWindowPad = 0;
static bool s_showWindowPrjSpecific = 0;
static bool s_pauseGameLogic = 0;
static ImVec2 s_btnSize = {75, 20};

extern void SIM_GUI_Prj_main(bool *) __attribute__((weak));

void SIM_GUI_Init(SDL_Window * aWindow, SDL_GLContext aContext)
{
    SIM_GUI_SDLwindow = aWindow;
    SIM_GUI_GLcontext = aContext;
	SIM_GUI_ig_context = igCreateContext(NULL);

	ImGuiIO* ioptr = igGetIO_ContextPtr(SIM_GUI_ig_context);
	ioptr->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	ioptr->DisplaySize.x = (float)(SIM_NDS_SCREEN_WIDTH*SIM_NDS_WINDOW_SCALE_FACTOR);
	ioptr->DisplaySize.y = (float)(SIM_NDS_SCREEN_HEIGHT*2*SIM_NDS_WINDOW_SCALE_FACTOR);

	ImGui_ImplSDL2_InitForOpenGL(SIM_GUI_SDLwindow, SIM_GUI_GLcontext);
	const char * glsl_version = "#version 420";
  	ImGui_ImplOpenGL3_Init(glsl_version);

	igStyleColorsDark(NULL);
	ImFontAtlas_AddFontDefault(ioptr->Fonts, NULL);
}

void SIM_GUI_Main(void)
{
    if(SIM_GUI_State){
        igBegin("NitroSDK", &SIM_GUI_State, 0);
        float frameTimeMs = 0.0f;
        frameTimeMs = (float)s_SIM_frameTime / 1000000.0f;
        igText("FrameTime %.3fms", frameTimeMs);
        igText("%.0ffps", 1000.0f / frameTimeMs);
        if(igButton("Config", s_btnSize)) {
            if(s_showWindowConfig){
                s_showWindowConfig = 0;
            } else {
                s_showWindowConfig = 1;
            }
        }

        if(igButton("Input", s_btnSize)) {
            if(s_showWindowPad){
                s_showWindowPad = 0;
            } else {
                s_showWindowPad = 1;
            }
        }

        if(igButton("G2", s_btnSize)) {
            if(s_showWindowG2){
                s_showWindowG2 = 0;
            } else {
                s_showWindowG2 = 1;
            }
        }

        if(igButton("Net", s_btnSize)) {
            if(s_showWindowNet){
                s_showWindowNet = 0;
            } else {
                GUI_AppNetInit();
                s_showWindowNet = 1;
            }
        }

        if(igButton("Pause Game Logic", s_btnSize)) {
            if(s_pauseGameLogic){
                s_pauseGameLogic = 0;
            } else {
                s_pauseGameLogic = 1;
            }
        }

        if(igButton("PrjSpecific", s_btnSize)) {
            if(s_showWindowPrjSpecific){
                s_showWindowPrjSpecific = 0;
            } else {
                s_showWindowPrjSpecific = 1;
            }
        }

        if(s_showWindowConfig){
            GUI_AppConfigMain(&s_showWindowConfig);
        }

        if(s_showWindowPad){
            GUI_AppPadMain(&s_showWindowPad);
        }

        if(s_showWindowG2){
            GUI_AppG2Main(&s_showWindowG2);
        }

        if(s_showWindowNet){
            GUI_AppNetMain(&s_showWindowNet);
        }

        if(s_showWindowPrjSpecific){
            SIM_GUI_Prj_main(&s_showWindowPrjSpecific);
        }
        igEnd();
    }
    return;
}

void SIM_GUI_NewFrame(void)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    igNewFrame();
}

void SIM_GUI_ProcessEvent(SDL_Event * aEvent)
{   if(SIM_GUI_State){
        ImGui_ImplSDL2_ProcessEvent(aEvent);
    }
}

void SIM_GUI_Render(void)
{
	igRender();
	ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
}

void SIM_GUI_Toggle(void)
{
    if(SIM_GUI_State) {
        SIM_GUI_State = 0;
    } else {
        SIM_GUI_State = 1;
    }
}

bool SIM_GUI_IsGameLogicPaused(void)
{
    return s_pauseGameLogic;
}

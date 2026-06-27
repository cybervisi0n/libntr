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
static bool s_showWindowImguiDemo = 0;
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

        SIM_GUI_AppButton("Config", &s_showWindowConfig, NULL, GUI_AppConfigMain);
        SIM_GUI_AppButton("Input", &s_showWindowPad, NULL, GUI_AppPadMain);
        SIM_GUI_AppButton("G2", &s_showWindowG2, NULL, GUI_AppG2Main);
        SIM_GUI_AppButton("Net", &s_showWindowNet, GUI_AppNetInit, GUI_AppNetMain);
        SIM_GUI_AppButton("Imgui Demo", &s_showWindowImguiDemo, NULL, igShowDemoWindow);
        SIM_GUI_AppButton("PrjSpecific", &s_showWindowPrjSpecific, NULL, SIM_GUI_Prj_main);
        igCheckbox("Pause Game Logic", &s_pauseGameLogic);
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

void SIM_GUI_AppButton(const char * aLabel, bool * aState, void (*aInitFunc)(void), void (*aAppFunc)(bool *))
{
    if(igButton(aLabel, s_btnSize)) {
        if(*aState) {
            *aState = false;
        } else {
            if(aInitFunc){
                aInitFunc();
            }
            *aState = true;
        }
    }

    if(*aState) {
        aAppFunc(aState);
    }
}

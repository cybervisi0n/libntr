#include "simulator/sim.h"
#include "simulator/sim_gui.hpp"
#include <SDL2/SDL_events.h>

#include "simulator/imgui/imgui.hpp"

#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"
#include "gui_internal.hpp"
#include "gui_utils.hpp"

#if SIM_IMGUI_FILE_DIALOG_SUPPORT
#include "ImGuiFileDialog.h"
#endif

namespace SIM::GUI {
static SDL_Window * sWindow;
static SDL_GLContext sContext;
static ImGuiContext * sImguiContext;
static bool sEnabled = false;
static bool sPauseGameLogic = false;
static constexpr ImVec2 sButtonSize = {100, 20};
static constexpr auto MaxWindowWidth = 400;
static constexpr auto MaxWindowHeight = 600;

static bool sShowAppConfig = false;
static bool sShowAppPad = false;
static bool sShowAppNet = false;
static bool sShowAppDebug = false;
static bool sShowAppInfo = false;
static bool sShowImGuiDemo = false;
static bool sShowWindowPrjSpecific = false;

static std::function<void(std::string)> sFileDialogCallback = nullptr;

void __attribute__((weak)) PrjMain() {
    ImGui::Text("No application-specific menu defined.");
}
void PrjMain() __attribute__((weak));

void Init(SDL_Window * window, SDL_GLContext context) {
    sWindow = window;
    sContext = context;

    sImguiContext = ImGui::CreateContext(NULL);

    ImGuiIO& igIO = ImGui::GetIO();
	igIO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    igIO.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;       // Enable Gamepad Controls
	igIO.DisplaySize.x = (float)(SIM_NDS_SCREEN_WIDTH*SIM_NDS_WINDOW_SCALE_FACTOR);
	igIO.DisplaySize.y = (float)(SIM_NDS_SCREEN_HEIGHT*2*SIM_NDS_WINDOW_SCALE_FACTOR);



	ImGui_ImplSDL2_InitForOpenGL(sWindow, sContext);
    ImGui_ImplSDL2_SetGamepadMode(ImGui_ImplSDL2_GamepadMode_AutoFirst);
	const char * glsl_version = "#version 420";
  	ImGui_ImplOpenGL3_Init(glsl_version);

    ImGui::StyleColorsDark(NULL);
}

static void MainMenuTab(const char * title, void (*appFunc)(void)) {
    if(ImGui::BeginTabItem(title)) {
        if(appFunc) {
            appFunc();
        }
        ImGui::EndTabItem();
    }
}

void Main() {
    if(sEnabled) {
        u32 windowWidth = std::min<u32>(PercentXPx(0.5f), MaxWindowWidth);
        u32 windowHeight = std::min<u32>(PercentYPx(0.75f), MaxWindowHeight);
        
        u32 windowX = CenteredXPx(windowWidth);
        u32 windowY = CenteredYPx(windowHeight);
        
        
        ImGui::SetNextWindowPos(ImVec2(static_cast<float>(windowX), static_cast<float>(windowY)));
        ImGui::SetNextWindowSize(ImVec2(static_cast<float>(windowWidth), static_cast<float>(windowHeight)));
        ImGui::Begin("libntr menu", &sEnabled,
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoTitleBar);

        if(ImGui::BeginTabBar("mainTabBar", ImGuiTabBarFlags_FittingPolicyScroll)) {
            MainMenuTab("Settings", AppConfigMain);
            MainMenuTab("Controls", AppPadMain);
            MainMenuTab("Multiplayer", AppNetMain);
            MainMenuTab("Debug", AppDebugMain);
            MainMenuTab("App", PrjMain);
            MainMenuTab("Info", AppInfoMain);
            if(ImGui::TabItemButton("Close Menu")) {
                sEnabled = false;
            }
            
            ImGui::EndTabBar();
        }



        ImGui::End();

        #if SIM_IMGUI_FILE_DIALOG_SUPPORT
        if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey", ImGuiWindowFlags_NoCollapse, ImVec2(200, 200))) {
          if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

            // FilePathName will be the full path to the file selected
            if(sFileDialogCallback) {
                sFileDialogCallback(filePathName);
            }
          }

          // close
          ImGuiFileDialog::Instance()->Close();
        }
        #endif
    }
}

void OpenFileDialog(std::function<void(std::string)> callback) {
    sFileDialogCallback = callback;

    #if SIM_IMGUI_FILE_DIALOG_SUPPORT
    IGFD::FileDialogConfig config;
    	config.path = ".";
    ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Select NDS ROM", ".nds,.srl", config);
    #else 
    SIM_assert_always();
    #endif
}

void NewFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

bool ProcessEvent(SDL_Event * event) {
    if(sEnabled) {
        if(event->type == SDL_JOYHATMOTION || event->type == SDL_JOYBUTTONDOWN || event->type == SDL_JOYBUTTONUP || event->type == SDL_JOYAXISMOTION) {
            // Prevent further processing of joystick events when the GUI is open
            return true;
        }
        return ImGui_ImplSDL2_ProcessEvent(event);
    }

    return false;
}

void Render() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Toggle() {
    if(sEnabled) {
        sEnabled = false;
    } else {
        sEnabled = true;
    }
}

void Enable() {
    sEnabled = true;
}

bool IsGameLogicPaused() {
    return sPauseGameLogic;
}

void SetGameLogicPaused(bool paused) {
    sPauseGameLogic = paused;
}

// Generic button to open another GUI applet
void AppButton(const char * label, bool * state, void (*initFunc)(void), void (*appFunc)(bool *)) {
    if(ImGui::Button(label, sButtonSize)) {
        if(*state) {
            *state = false;
        } else {
            if(initFunc) {
                initFunc();
            }
            *state = true;
        }
    }

    if(*state) {
        appFunc(state);
    }
}

}
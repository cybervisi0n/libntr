#include "simulator/sim.h"
#include "simulator/sim_gui.hpp"

#include "simulator/imgui/imgui.hpp"

#include "gui_internal.hpp"

namespace SIM::GUI {

void AppInfoMain() {
    //ImGui::Begin("libntr");

    ImGui::Text("https://github.com/cybervisi0n/libntr");

    ImGui::Text("Git rev: %s", SIM_GetLibntrGitHash());

    float renderTimeMs = static_cast<float>(SIM_GetRenderFrameTime()) / 1000000.0f;
    ImGui::Text("Render time: %.3fms", renderTimeMs);

    float frameTimeMs = static_cast<float>(SIM_GetFullFrameTime()) / 1000000.0f;
    ImGui::Text("%.0ffps", 1000.0f / frameTimeMs);

    //ImGui::End();
}

}
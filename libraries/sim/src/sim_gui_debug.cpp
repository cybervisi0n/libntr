#include "simulator/sim.h"
#include "simulator/sim_gui.hpp"

#include "simulator/imgui/imgui.hpp"

#include "gui_internal.hpp"

namespace SIM::GUI {

void AppDebugMain(bool *openState) {
    ImGui::Begin("Debug", openState);

    ImGui::End();
}

}
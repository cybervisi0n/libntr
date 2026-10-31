#include "simulator/sim_g3_c_bridge.h"

#include "simulator/sim_g3_GlobalState.hpp"

void SIM_G3_SetVramTex(GXVRamTex vramTex) {
    SIM::G3::GetGlobalState().SetVramTex(vramTex);
}

void SIM_G3_SetVramTexPltt(GXVRamTexPltt vramTexPltt) {
    SIM::G3::GetGlobalState().SetVramTexPalette(vramTexPltt);
}
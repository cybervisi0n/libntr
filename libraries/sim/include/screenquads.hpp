#ifndef SCREENQUADS_HPP
#define SCREENQUADS_HPP
#include <nitro.h>
#include <simulator/config/sim_config.h>
#include "simulator/sim_g3_DrawCall.hpp"

#ifdef __cplusplus
extern "C" {
#endif

namespace SIM {

// TODO: might make sense to move RenderVertex out of G3
G3::RenderVertex * GetScreenQuadArray(SIM_config_screen_layout_type layout, bool isSwapped);
int GetScreenQuadVertexCount(SIM_config_screen_layout_type layout, bool isSwapped);

}

#ifdef __cplusplus
}
#endif

#endif
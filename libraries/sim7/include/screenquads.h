#ifndef SCREENQUADS_H
#define SCREENQUADS_H
#include <nitro.h>
#include <simulator/g3_draw.h>
#include <simulator/config/sim_config.h>

G3SIM_Vertex_t * sim_GetScreenQuadArray(SIM_config_screen_layout_type layout);
int sim_GetScreenQuadVertexCount(SIM_config_screen_layout_type layout);
#endif
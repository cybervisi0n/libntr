#ifndef LIBNTR_SIM_G3_C_BRIDGE_H
#define LIBNTR_SIM_G3_C_BRIDGE_H

#include <nitro/gx/gx_vramcnt.h>

#ifdef __cplusplus
extern "C" {
#endif

void SIM_G3_SetVramTex(GXVRamTex vramTex);
void SIM_G3_SetVramTexPltt(GXVRamTexPltt vramTexPltt);

#ifdef __cplusplus
}
#endif

#endif
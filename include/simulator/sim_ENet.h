#ifndef SIM_ENET_H
#define SIM_ENET_H

//PROSPERO_TODO: Compile ENet for Prospero
#ifndef SDK_BUILD_PROSPERO
#ifdef SDK_BUILD_NX
#include <sys/select.h>
#endif
#include <enet/enet.h>
#endif

#endif

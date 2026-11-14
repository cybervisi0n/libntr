#ifndef SIM_ENET_H
#define SIM_ENET_H

//PROSPERO_TODO: Compile ENet for Prospero
#if !defined(SDK_BUILD_PROSPERO) && !defined(SDK_BUILD_OUNCE)
#ifdef SDK_BUILD_NX
#include <sys/select.h>
#endif
#include <enet/enet.h>
#endif

#endif

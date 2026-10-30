#ifndef LIBNTR_SIM_G3_UTILITIES_HPP
#define LIBNTR_SIM_G3_UTILITIES_HPP

#include <nitro/types.h>
#include <nitro/fx/fx.h>

namespace SIM::G3::Util {

MtxFx33 MultiplyMtxFx33(MtxFx33 a, MtxFx33 b);
MtxFx44 MultiplyMtxFx44(MtxFx44 a, MtxFx44 b);

}


#endif
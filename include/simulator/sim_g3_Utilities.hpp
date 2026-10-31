#ifndef LIBNTR_SIM_G3_UTILITIES_HPP
#define LIBNTR_SIM_G3_UTILITIES_HPP

#include <nitro/types.h>
#include <nitro/fx/fx.h>

namespace SIM::G3::Util {

MtxFx33 MultiplyMtxFx33(MtxFx33 a, MtxFx33 b);
MtxFx44 MultiplyMtxFx44(MtxFx44 a, MtxFx44 b);

void DecodeTex4(u8* vramTex, u16* colorAddr, u8* out, u32 s, u32 t, u8 color0);
void DecodeTex16(u8* vramTex, u16* colorAddr, u8* out, u32 s, u32 t, u8 color0);
void DecodeTex256(u8* vramTex, u16* colorAddr, u8* out, u32 s, u32 t, u8 color0);
void DecodeTexA3I5(u8* vramTex, u16* colorAddr, u8* out, u32 s, u32 t);
void DecodeTexA5I3(u8* vramTex, u16* colorAddr, u8* out, u32 s, u32 t);
void DecodeTexDirect(u8* vramTex, u8* out, u32 s, u32 t);

}


#endif
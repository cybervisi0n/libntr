#ifndef TWL_HYBRID_H_
#define TWL_HYBRID_H_

#if SDK_VERSION_MAJOR != 5
#error This file is only supported on SDK version 5
#endif

#ifdef SDK_X86
#define ATTRIBUTE_ALIGN(x) __attribute__((aligned(x)))
#endif

#ifdef SDK_ASM

#ifndef SDK_TWL

#include <nitro/spec.h>
#include <nitro/types.h>
#include <nitro/memorymap.h>
#include <nitro/hw/common/armArch.h>
#else

#include <twl/spec.h>
#include <twl/types.h>
#include <twl/memorymap.h>
#include <twl/hw/common/armArch.h>
#endif // SDK_NITRO

#else // SDK_ASM

#ifdef __MWERKS__
#ifdef __cplusplus
#ifndef __embedded_cplusplus
#define __embedded_cplusplus 0
#endif
#ifndef __VEC__
#define __VEC__ 0
#endif
#endif
#if !defined(_MSL_USE_INLINE) && __option(dont_inline)
#define _MSL_USE_INLINE 0
#endif
#endif

#ifndef SDK_TWL

#include <nitro/spec.h>
#include <nitro/misc.h>
#include <nitro/init/crt0.h>

#include <nitro/types.h>
#include <nitro/memorymap.h>
#include <nitro/hw/common/armArch.h>
#include <nitro/hw/common/lcd.h>

#include <nitro/os.h>
#include <nitro/mi.h>
#else

#include <twl/spec.h>
#include <twl/misc.h>
#include <twl/init/crt0.h>

#include <twl/types.h>
#include <twl/memorymap.h>
#include <twl/hw/common/armArch.h>
#include <nitro/hw/common/lcd.h>

#include <twl/os.h>
#include <twl/mi.h>
#endif // SDK_NITRO

#include <nitro/pxi.h>
#include <nitro/pad.h>

#ifndef SDK_TWL

#include <nitro/spi.h>
#include <nitro/rtc.h>
#include <nitro/snd.h>
#else

#include <twl/spi.h>
#include <twl/rtc.h>
#include <twl/snd.h>
#endif

#include <nitro/card.h>
#include <nitro/fs.h>
#include <nitro/gx.h>
#include <nitro/wm.h>
#include <nitro/wvr.h>
#include <nitro/ctrdg.h>
#include <nitro/math.h>

#include <nitro/std.h>

#ifdef SDK_TWL

#include <twl/nwm.h>
#include <twl/scfg.h>
#include <twl/camera.h>
#include <twl/dsp.h>
#endif

#if defined(SDK_ARM9) || defined(SDK_PORT)

#include <nitro/fx/fx.h>
#include <nitro/fx/fx_const.h>
#include <nitro/fx/fx_trig.h>
#include <nitro/fx/fx_cp.h>
#include <nitro/fx/fx_vec.h>
#include <nitro/fx/fx_mtx.h>
#include <nitro/cp.h>
#include <nitro/ext.h>
#include <nitro/mb.h>
#include <nitro/wbt.h>
#include <nitro/wfs.h>
#include <nitro/env.h>
#include <nitro/vib.h>

#else // SDK_ARM7

#include <nitro/exi.h>

#endif // SDK_ARM7

#endif // SDK_ASM

#endif

#ifndef NITRO_MI_WRAM_H_
#define NITRO_MI_WRAM_H_

#ifdef __cplusplus
extern "C" {
#endif

#if SDK_VERSION_MAJOR == 4
#include <nitro/ioreg.h>
#elif SDK_VERSION_MAJOR == 5
#ifndef SDK_TWL
#include <nitro/ioreg.h>
#else
#include <twl/ioreg.h>
#endif
#endif

#if (defined(SDK_ARM9) || defined(SDK_PORT))
    typedef enum {
        MI_WRAM_ARM9_ALL    = 0 << REG_GX_VRAMCNT_WRAM_BANK_SHIFT,
        MI_WRAM_1616_1      = 1 << REG_GX_VRAMCNT_WRAM_BANK_SHIFT,
        MI_WRAM_1616_2      = 2 << REG_GX_VRAMCNT_WRAM_BANK_SHIFT,
        MI_WRAM_ARM7_ALL    = 3 << REG_GX_VRAMCNT_WRAM_BANK_SHIFT
    } MIWram;
#else
    typedef enum {
        MI_WRAM_ARM9_ALL    = (REG_MI_WVRAMSTAT_WRAM_1_MASK | REG_MI_WVRAMSTAT_WRAM_0_MASK),
        MI_WRAM_1616_1      = (REG_MI_WVRAMSTAT_WRAM_0_MASK),
        MI_WRAM_1616_2      = (REG_MI_WVRAMSTAT_WRAM_1_MASK),
        MI_WRAM_ARM7_ALL    = 0
    } MIWram;
#endif

#ifdef SDK_ARM9
    void MI_SetWramBank(MIWram cnt);
#endif

MIWram MI_GetWramBank(void);

#ifdef __cplusplus
}
#endif

#endif

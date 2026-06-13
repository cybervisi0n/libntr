#include <nitro/types.h>
#include <nitro/mi/swap.h>

#if defined( SDK_PORT )
u32 MI_SwapWord( u32 setData, vu32* destp )
{
    u32 temp = *destp;
    *destp = setData;
    return temp;
}

u8 MI_SwapByte( u32 setData, vu8* destp )
{
    u8 temp = *destp;
    *destp = setData;
    return temp;
}
#else
#include <nitro/code32.h>

asm u32 MI_SwapWord (register u32 setData, register vu32 * destp)
{
    swp r0, r0, [r1]
    bx lr
}

asm u8  MI_SwapByte (register u32 setData, register vu8 * destp)
{
    swpb r0, r0, [r1]
    bx lr
}
#endif

#include <nitro/codereset.h>

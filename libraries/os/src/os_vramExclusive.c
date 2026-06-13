#include <nitro/os/ARM9/vramExclusive.h>
#include <nitro/os/common/interrupt.h>

static u32 OSi_vramExclusive;
static u16 OSi_vramLockId[OS_VRAM_BANK_KINDS];

#ifdef SDK_PORT
static u32 OsCountZeroBits( u32 bitmap )
{   

    u32 res = 0;
    if( bitmap == 0 )
    {
        return 32;
    }
    while (!(bitmap & (1 << (32 - 1)))) {
        bitmap = (bitmap << 1);
        res++;
    }
    return res;
}
#else
#include <nitro/code32.h>

static asm u32 OsCountZeroBits (u32 bitmap)
{
    clz r0, r0
    bx lr
}

#include <nitro/codereset.h>
#endif

void OSi_InitVramExclusive (void)
{
    s32 i;

    OSi_vramExclusive = 0x0000;

    for(i = 0 ; i < OS_VRAM_BANK_KINDS ; i++) {
        OSi_vramLockId[i] = 0;
    }
}

BOOL OSi_TryLockVram (u16 bank, u16 lockId)
{
    u32 workMap;
    s32 zeroBits;
    OSIntrMode enabled = OS_DisableInterrupts();

    workMap = (u32)(bank & OSi_vramExclusive);
    while (TRUE) {
        zeroBits = (s32)(31 - OsCountZeroBits(workMap));
        if (zeroBits < 0) {
            break;
        }
        workMap &= ~(0x00000001 << zeroBits);
        if (OSi_vramLockId[zeroBits] != lockId) {
            (void)OS_RestoreInterrupts(enabled);
            return FALSE;
        }
    }

    workMap = (u32)(bank & OS_VRAM_BANK_ID_ALL);
    while (TRUE) {
        zeroBits = (s32)(31 - OsCountZeroBits(workMap));
        if (zeroBits < 0) {
            break;
        }
        workMap &= ~(0x00000001 << zeroBits);
        OSi_vramLockId[zeroBits] = lockId;
        OSi_vramExclusive |= (0x00000001 << zeroBits);
    }

    (void)OS_RestoreInterrupts(enabled);
    return TRUE;
}

void OSi_UnlockVram (u16 bank, u16 lockId)
{
    u32 workMap;
    s32 zeroBits;
    OSIntrMode enabled = OS_DisableInterrupts();

    workMap = (u32)(bank & OSi_vramExclusive & OS_VRAM_BANK_ID_ALL);
    while (TRUE) {
        zeroBits = (s32)(31 - OsCountZeroBits((u32)workMap));
        if (zeroBits < 0) {
            break;
        }
        workMap &= ~(0x00000001 << zeroBits);
        if (OSi_vramLockId[zeroBits] == lockId) {
            OSi_vramLockId[zeroBits] = 0;
            OSi_vramExclusive &= ~(0x00000001 << zeroBits);
        }
    }

    (void)OS_RestoreInterrupts(enabled);
}

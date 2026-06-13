#include <nitro/os.h>
#include <nitro/code32.h>

#ifdef SDK_PORT
void OS_EnableICacheForProtectionRegion( u32 flags )
{
    return;
}

void OS_DisableICacheForProtectionRegion( u32 flags )
{
    return;
}

u32 OS_GetICacheEnableFlagsForProtectionRegion( void )
{
    return 0;
}

void OS_SetIPermissionsForProtectionRegion( u32 setMask, u32 flags )
{
    return;
}

u32 OS_GetIPermissionsForProtectionRegion( void )
{
    return 0;
}

void OS_EnableDCacheForProtectionRegion( u32 flags )
{
    return;
}

void OS_DisableDCacheForProtectionRegion( u32 flags )
{
    return;
}

u32 OS_GetDCacheEnableFlagsForProtectionRegion( u32 flags )
{
    return 0;
}

void OS_SetDPermissionsForProtectionRegion( u32 setMask, u32 flags )
{
    return;
}

u32 OS_GetDPermissionsForProtectionRegion( void )
{
    return 0;
}

void OS_EnableWriteBufferForProtectionRegion( u32 flags )
{
    return;
}

void OS_DisableWriteBufferForProtectionRegion( u32 flags )
{
    return;
}

u32 OS_GetWriteBufferEnableFlagsForProtectionRegion( u32 flags )
{
    return 0;
}

void OS_SetProtectionRegion0( u32 param )
{
    return;
}

void OS_SetProtectionRegion1( u32 param )
{
    return;
}

void OS_SetProtectionRegion2( u32 param )
{
    return;
}

void OS_SetProtectionRegion3( u32 param )
{
    return;
}

void OS_SetProtectionRegion4( u32 param )
{
    return;
}

void OS_SetProtectionRegion5( u32 param )
{
    return;
}

void OS_SetProtectionRegion6( u32 param )
{
    return;
}

void OS_SetProtectionRegion7( u32 param )
{
    return;
}

u32 OS_GetProtectionRegion0( void )
{
    return 0;
}

u32 OS_GetProtectionRegion1( void )
{
    return 0;
}

u32 OS_GetProtectionRegion2( void )
{
    return 0;
}

u32 OS_GetProtectionRegion3( void )
{
    return 0;
}

u32 OS_GetProtectionRegion4( void )
{
    return  0;
}

u32 OS_GetProtectionRegion5( void )
{
    return 0;
}

u32 OS_GetProtectionRegion6( void )
{
    return 0;
}

u32 OS_GetProtectionRegion7( void )
{
    return 0;
}
#else
asm void OS_EnableICacheForProtectionRegion (register u32 flags)
{
    mrc p15, 0, r1, c2, c0, 1
    orr r1, r1, r0
    mcr p15, 0, r1, c2, c0, 1
    bx lr
}

asm void OS_DisableICacheForProtectionRegion (register u32 flags)
{
    mrc p15, 0, r1, c2, c0, 1
    bic r1, r1, r0
    mcr p15, 0, r1, c2, c0, 1
    bx lr
}

asm u32 OS_GetICacheEnableFlagsForProtectionRegion (void)
{
    mrc p15, 0, r0, c2, c0, 1
    bx lr
}

asm void OS_SetIPermissionsForProtectionRegion (register u32 setMask, register u32 flags)
{
    mrc p15, 0, r2, c5, c0, 3
    bic r2, r2, r0
    orr r2, r2, r1
    mcr p15, 0, r2, c5, c0, 3
    bx lr
}

asm u32 OS_GetIPermissionsForProtectionRegion (void)
{
    mrc p15, 0, r0, c5, c0, 3
    bx lr
}

asm void OS_EnableDCacheForProtectionRegion (register u32 flags)
{
    mrc p15, 0, r1, c2, c0, 0
    orr r1, r1, r0
    mcr p15, 0, r1, c2, c0, 0
    bx lr
}

asm void OS_DisableDCacheForProtectionRegion (register u32 flags)
{
    mrc p15, 0, r1, c2, c0, 0
    bic r1, r1, r0
    mcr p15, 0, r1, c2, c0, 0
    bx lr
}

asm u32 OS_GetDCacheEnableFlagsForProtectionRegion (register u32 flags)
{
    mrc p15, 0, r0, c2, c0, 0
    bx lr
}

asm void OS_SetDPermissionsForProtectionRegion (register u32 setMask, register u32 flags)
{
    mrc p15, 0, r2, c5, c0, 2
    bic r2, r2, r0
    orr r2, r2, r1
    mcr p15, 0, r2, c5, c0, 2
    bx lr
}

asm u32 OS_GetDPermissionsForProtectionRegion (void)
{
    mrc p15, 0, r0, c5, c0, 2
    bx lr
}

asm void OS_EnableWriteBufferForProtectionRegion (register u32 flags)
{
    mrc p15, 0, r1, c3, c0, 0
    orr r1, r1, r0
    mcr p15, 0, r1, c3, c0, 0
    bx lr
}

asm void OS_DisableWriteBufferForProtectionRegion (register u32 flags)
{
    mrc p15, 0, r1, c3, c0, 0
    bic r1, r1, r0
    mcr p15, 0, r1, c3, c0, 0
    bx lr
}

asm u32 OS_GetWriteBufferEnableFlagsForProtectionRegion (register u32 flags)
{
    mrc p15, 0, r0, c3, c0, 0
    bx lr
}

asm void OS_SetProtectionRegion0 (u32 param)
{
    mcr p15, 0, r0, c6, c0, 0
    bx lr
}

asm void OS_SetProtectionRegion1 (u32 param)
{
    mcr p15, 0, r0, c6, c1, 0
    bx lr
}

asm void OS_SetProtectionRegion2 (u32 param)
{
    mcr p15, 0, r0, c6, c2, 0
    bx lr
}

asm void OS_SetProtectionRegion3 (u32 param)
{
    mcr p15, 0, r0, c6, c3, 0
    bx lr
}

asm void OS_SetProtectionRegion4 (u32 param)
{
    mcr p15, 0, r0, c6, c4, 0
    bx lr
}

asm void OS_SetProtectionRegion5 (u32 param)
{
    mcr p15, 0, r0, c6, c5, 0
    bx lr
}

asm void OS_SetProtectionRegion6 (u32 param)
{
    mcr p15, 0, r0, c6, c6, 0
    bx lr
}

asm void OS_SetProtectionRegion7 (u32 param)
{
    mcr p15, 0, r0, c6, c7, 0
    bx lr
}

asm u32 OS_GetProtectionRegion0 (void)
{
    mrc p15, 0, r0, c6, c0, 0
    bx lr
}

asm u32 OS_GetProtectionRegion1 (void)
{
    mrc p15, 0, r0, c6, c1, 0
    bx lr
}

asm u32 OS_GetProtectionRegion2 (void)
{
    mrc p15, 0, r0, c6, c2, 0
    bx lr
}

asm u32 OS_GetProtectionRegion3 (void)
{
    mrc p15, 0, r0, c6, c3, 0
    bx lr
}

asm u32 OS_GetProtectionRegion4 (void)
{
    mrc p15, 0, r0, c6, c4, 0
    bx lr
}

asm u32 OS_GetProtectionRegion5 (void)
{
    mrc p15, 0, r0, c6, c5, 0
    bx lr
}

asm u32 OS_GetProtectionRegion6 (void)
{
    mrc p15, 0, r0, c6, c6, 0
    bx lr
}

asm u32 OS_GetProtectionRegion7 (void)
{
    mrc p15, 0, r0, c6, c7, 0
    bx lr
}
#endif

#include <nitro/codereset.h>

#include <nitro/os.h>

#ifndef OS_PROFILE_AVAILABLE
#ifdef SDK_PORT
    SDK_WEAK_SYMBOL void __PROFILE_ENTRY (void)
    {
    }

    SDK_WEAK_SYMBOL void __PROFILE_EXIT (void)
    {
    }
#else
    SDK_WEAK_SYMBOL asm void __PROFILE_ENTRY (void)
    {
        bx lr
    }

    SDK_WEAK_SYMBOL asm void __PROFILE_EXIT (void)
    {
        bx lr
    }
#endif
#endif

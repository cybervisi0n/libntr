#ifdef SDK_PORT
    #include <nitro/hw/X86/ioreg.h>
#else
#ifdef SDK_ARM9
    #include <nitro/hw/ARM9/ioreg.h>
#endif
#ifdef  SDK_ARM7
    #include <nitro/hw/ARM7/ioreg.h>
#endif
#endif
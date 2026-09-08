#ifdef SDK_PORT
#include <twl/hw/X86/ioreg.h> //TODO: Rename me
#else
#ifdef SDK_ARM9
#include <twl/hw/ARM9/ioreg.h>
#else // SDK_ARM7
#include <twl/hw/ARM7/ioreg.h>
#endif
#endif

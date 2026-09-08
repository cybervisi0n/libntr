#ifdef SDK_TWL

#ifdef SDK_TWLHYB

#include <twl/ltdwram_end.h>

#else // SDK_TWLHYB

#if defined(SDK_CW) || defined(SDK_RX) || defined(__MWERKS__)
#pragma section RSVWRAM end
#elif defined(SDK_ADS)
TO BE DEFINED
#elif defined(SDK_GCC)
TO BE DEFINED
#endif

#endif // SDK_TWLHYB

#endif // SDK_TWL

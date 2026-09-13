#include <nitro.h>

#if (SDK_VERSION_MAJOR == 5) && defined(SDK_TWL)
#include <twl/mi/common/dma.h>
#endif

void MI_Init (void)
{
#ifdef SDK_ARM9
    MI_SetWramBank(MI_WRAM_ARM7_ALL);
#endif

#if SDK_VERSION_MAJOR == 5
  if (OS_IsRunOnTwl()) {
#ifdef SDK_TWL
    MI_InitNDma();
#endif
  }
#endif

    MI_StopDma(0);
}

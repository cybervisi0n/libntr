#include <nitro/types.h>
#include <nitro/misc.h>
#include <nitro/mi.h>
#include <nitro/os.h>
#include <nitro/pxi.h>
#include <nitro/card/common.h>

#include <nitro/fs.h>

#include "../include/util.h"
#include "../include/command.h"
#include "../include/rom.h"

static BOOL is_init = FALSE;

void FS_Init(u32 default_dma_no) {
#if defined(FS_IMPLEMENT)
  SDK_TASSERTMSG(default_dma_no != 0,
                 "cannot use DMA channel 0 for ROM access");
  if (!is_init) {
    is_init = TRUE;
#if defined(SDK_TWLLTD)

    FS_InitFatDriver();
#endif
    FSi_InitRomArchive(default_dma_no);
    FSi_InitOverlay();
  }
#else
#pragma unused(default_dma_no)
  CARD_Init();
#endif /* FS_IMPLEMENT */
}

BOOL FS_IsAvailable(void) { return is_init; }

void FS_End(void) {
  OSIntrMode bak_psr = OS_DisableInterrupts();
  if (is_init) {
#if defined(FS_IMPLEMENT)
    FSi_EndArchive();
    FSi_EndRomArchive();
#endif /* FS_IMPLEMENT */
    is_init = FALSE;
  }
  (void)OS_RestoreInterrupts(bak_psr);
}

#if defined(FS_IMPLEMENT) && defined(SDK_TWL)

void FS_InitFatDriver(void) {
  static BOOL once = FALSE;
  if (!once) {
    once = TRUE;
    SDK_ASSERT(FS_IsAvailable());
    if (OS_IsRunOnTwl()) {
      FSi_MountDefaultArchives();
    }
  }
}
#endif

#include <nitro.h>

#include "../include/card_utility.h"

static const CARDDmaInterface CARDiDmaUsingFormer = {
    MIi_CardDmaCopy32,
    MI_StopDma,
};
#ifdef SDK_TWL
#include <twl/ltdmain_begin.h>
static CARDDmaInterface CARDiDmaUsingNew = {
    MI_Card_A_NDmaCopy,
    MI_StopNDma,
};
#include <twl/ltdmain_end.h>
#endif

const CARDDmaInterface *CARDi_GetDmaInterface(u32 channel) {
  const CARDDmaInterface *retval = NULL;

  BOOL isNew = ((channel & MI_DMA_USING_NEW) != 0);
  channel &= ~MI_DMA_USING_NEW;
  if (channel <= MI_DMA_MAX_NUM) {

    if (!isNew) {
      retval = &CARDiDmaUsingFormer;
    }

    else if (!OS_IsRunOnTwl()) {
      OS_TPanic("NDMA can use only TWL!");
    }
#ifdef SDK_TWL
    else {
      retval = &CARDiDmaUsingNew;
    }
#endif
  }
  return retval;
}

#ifdef SDK_ARM9

void CARDi_ICInvalidateSmart(void *buffer, u32 length, u32 threshold) {
  if (length >= threshold) {
    IC_InvalidateAll();
  } else {
    IC_InvalidateRange((void *)buffer, length);
  }
}

void CARDi_DCInvalidateSmart(void *buffer, u32 length, u32 threshold) {
  if (length >= threshold) {
    DC_FlushAll();
  } else {
    u32 len = length;
    u32 pos = (u32)buffer;
    u32 mod = (pos & (HW_CACHE_LINE_SIZE - 1));
    if (mod) {
      pos -= mod;
      DC_StoreRange((void *)(pos), HW_CACHE_LINE_SIZE);
      DC_StoreRange((void *)(pos + length), HW_CACHE_LINE_SIZE);
      length += HW_CACHE_LINE_SIZE;
    }
    DC_InvalidateRange((void *)pos, length);
    DC_WaitWriteBufferEmpty();
  }
}

#endif // SDK_ARM9

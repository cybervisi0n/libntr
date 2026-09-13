#ifdef SDK_ARM9
#include <twl.h>
#else
#include <twl_sp.h>
#endif

#include <nitro/os/common/system.h>
#include "../include/mi_ndma.h"

#ifdef SDK_ARM7
#include <twl/ltdwram_begin.h>
#endif

#ifdef SDK_ARM9
#include <twl/ltdmain_begin.h>
#endif

MINDmaConfig MIi_NDmaConfig[MI_NDMA_MAX_NUM + 1];

static void MIi_Wait(u32 ndmaNo);

static inline void MIi_SetSrc(u32 ndmaNo, u32 src) {
  MI_NDMA_REG(ndmaNo, MI_NDMA_REG_SAD_WOFFSET) = src;
}
static inline void MIi_SetDest(u32 ndmaNo, u32 dest) {
  MI_NDMA_REG(ndmaNo, MI_NDMA_REG_DAD_WOFFSET) = dest;
}
static inline void MIi_SetTotalWordCount(u32 ndmaNo, u32 size) {
  MI_NDMA_REG(ndmaNo, MI_NDMA_REG_TCNT_WOFFSET) = size;
}
static inline void MIi_SetWordCount(u32 ndmaNo, u32 size) {
  MI_NDMA_REG(ndmaNo, MI_NDMA_REG_WCNT_WOFFSET) = size;
}
static inline void MIi_SetInterval(u32 ndmaNo, u32 intervalTimer,
                                   u32 prescaler) {
#ifdef SDK_ARM7

  SDK_ASSERT(intervalTimer != 1);
#endif
  MI_NDMA_REG(ndmaNo, MI_NDMA_REG_BCNT_WOFFSET) = intervalTimer | prescaler;
}
static inline void MIi_SetFillData(u32 ndmaNo, u32 data) {
  MI_NDMA_REG(ndmaNo, MI_NDMA_REG_FDATA_WOFFSET) = data;
}
static inline void MIi_SetControl(u32 ndmaNo, u32 contData) {
  MI_NDMA_REG(ndmaNo, MI_NDMA_REG_CNT_WOFFSET) = contData;
}

static u32 MIi_GetControlData(MIiNDmaType ndmaType) {
  u32 contData;

  switch (ndmaType) {
  case MIi_NDMA_TYPE_FILL:
    contData = MIi_SRC_FILLDATA | MIi_DEST_INC;
    break;
  case MIi_NDMA_TYPE_COPY:
    contData = MI_NDMA_SRC_INC | MIi_DEST_INC;
    break;
  case MIi_NDMA_TYPE_SEND:
  case MIi_NDMA_TYPE_GXCOPY:
    contData = MI_NDMA_SRC_INC | MIi_DEST_FIX;
    break;
  case MIi_NDMA_TYPE_GXCOPY_IF:
    contData = MI_NDMA_SRC_INC | MIi_DEST_FIX | MI_NDMA_IF_ENABLE;
    break;
  case MIi_NDMA_TYPE_RECV:
    contData = MI_NDMA_SRC_FIX | MIi_DEST_INC;
    break;
  case MIi_NDMA_TYPE_PIPE:
    contData = MI_NDMA_SRC_FIX | MIi_DEST_FIX;
    break;
  case MIi_NDMA_TYPE_CAMERACONT:
    contData =
        MI_NDMA_SRC_FIX | MIi_DEST_INC_RELOAD | MIi_CONT | MI_NDMA_IF_ENABLE;
    break;
  case MIi_NDMA_TYPE_HBLANK:
    contData = MI_NDMA_SRC_INC | MIi_DEST_INC_RELOAD | MIi_CONT;
    break;
  case MIi_NDMA_TYPE_HBLANK_IF:
    contData =
        MI_NDMA_SRC_INC | MIi_DEST_INC_RELOAD | MIi_CONT | MI_NDMA_IF_ENABLE;
    break;
  case MIi_NDMA_TYPE_MMCOPY:
    contData = MI_NDMA_SRC_INC | MIi_DEST_FIX | MIi_CONT;
    break;
  default:
    contData = 0;
    break;
  }

  return contData;
}

void MIi_NDma_withConfig_Dev(MIiNDmaType ndmaType, u32 ndmaNo, const void *src,
                             void *dest, u32 data, u32 size,
                             const MINDmaConfig *config, MINDmaDevice dev,
                             u32 enable) {
  MIi_ASSERT_DMANO(ndmaNo);
  MIi_WARNING_ADDRINTCM(dest, size);

  if (size > 0) {
    u32 contData;
    OSIntrMode enabled = OS_DisableInterrupts();

    MIi_Wait(ndmaNo);

    if (ndmaType != MIi_NDMA_TYPE_FILL) {
      MIi_SetSrc(ndmaNo, (u32)src);
    }
    MIi_SetDest(ndmaNo, (u32)dest);
    MIi_SetInterval(ndmaNo, config->intervalTimer, config->prescaler);
    if (ndmaType == MIi_NDMA_TYPE_FILL) {
      MIi_SetFillData(ndmaNo, data);
    }
    if (dev == MIi_NDMA_TIMING_IMMIDIATE) {
      MIi_SetWordCount(ndmaNo, size / 4);
    } else {
      MIi_SetTotalWordCount(ndmaNo, size / 4);
      MIi_SetWordCount(ndmaNo, (config->wordCount == MI_NDMA_AT_A_TIME)
                                   ? size / 4
                                   : config->wordCount);
    }

    contData = config->blockWord | (enable & MI_NDMA_ENABLE_MASK);
    contData |= MIi_GetControlData(ndmaType);
    contData |= (dev == MIi_NDMA_TIMING_IMMIDIATE) ? MIi_IMM : dev;

    MIi_SetControl(ndmaNo, contData);

    MIi_Wait(ndmaNo);

    (void)OS_RestoreInterrupts(enabled);
  }
}

void MIi_NDmaAsync_withConfig_Dev(MIiNDmaType ndmaType, u32 ndmaNo,
                                  const void *src, void *dest, u32 data,
                                  u32 size, MINDmaCallback callback, void *arg,
                                  const MINDmaConfig *config, MINDmaDevice dev,
                                  u32 enable) {
  MIi_ASSERT_DMANO(ndmaNo);
  MIi_WARNING_ADDRINTCM(dest, size);

  if (size == 0) {
    MIi_CallCallback(callback, arg);
  } else {
    u32 contData;
    OSIntrMode enabled = OS_DisableInterrupts();

    MIi_Wait(ndmaNo);

    if (callback) {
      OSi_EnterNDmaCallback(ndmaNo, callback, arg);
    }

    if (ndmaType != MIi_NDMA_TYPE_FILL) {
      MIi_SetSrc(ndmaNo, (u32)src);
    }
    MIi_SetDest(ndmaNo, (u32)dest);
    MIi_SetInterval(ndmaNo, config->intervalTimer, config->prescaler);
    if (ndmaType == MIi_NDMA_TYPE_FILL) {
      MIi_SetFillData(ndmaNo, data);
    }
    if (dev == MIi_NDMA_TIMING_IMMIDIATE) {
      MIi_SetWordCount(ndmaNo, size / 4);
    } else {
      MIi_SetTotalWordCount(ndmaNo, size / 4);
      MIi_SetWordCount(ndmaNo, (config->wordCount == MI_NDMA_AT_A_TIME)
                                   ? size / 4
                                   : config->wordCount);
    }

    contData = config->blockWord | (enable & MI_NDMA_ENABLE_MASK);
    contData |= MIi_GetControlData(ndmaType);
    contData |= (dev == MIi_NDMA_TIMING_IMMIDIATE) ? MIi_IMM : dev;

    if (callback) {
      contData |= MI_NDMA_IF_ENABLE;
    }

    MIi_SetControl(ndmaNo, contData);

    (void)OS_RestoreInterrupts(enabled);
  }
}

void MI_InitNDma(void) {
  MI_SetNDmaArbitrament(MI_NDMA_ARBITRAMENT_ROUND, MI_NDMA_RCYCLE_DEFAULT);
  MI_InitNDmaConfig();
}

void MI_NDmaFill(u32 ndmaNo, void *dest, u32 data, u32 size) {
  MIi_NDma(MIi_NDMA_TYPE_FILL, ndmaNo, 0, /*Not used*/
           dest, data, size, MI_NDMA_ENABLE);
}
void MI_NDmaFill_SetUp(u32 ndmaNo, void *dest, u32 data, u32 size) {
  MIi_NDma(MIi_NDMA_TYPE_FILL, ndmaNo, 0, /*Not used*/
           dest, data, size, MI_NDMA_DISABLE);
}

void MI_NDmaCopy(u32 ndmaNo, const void *src, void *dest, u32 size) {
  MIi_NDma(MIi_NDMA_TYPE_COPY, ndmaNo, src, dest, 0, /*Not used*/
           size, MI_NDMA_ENABLE);
}

void MI_NDmaCopy_SetUp(u32 ndmaNo, const void *src, void *dest, u32 size) {
  MIi_NDma(MIi_NDMA_TYPE_COPY, ndmaNo, src, dest, 0, /*Not used*/
           size, MI_NDMA_DISABLE);
}

void MI_NDmaSend(u32 ndmaNo, const void *src, volatile void *dest, u32 size) {
  MIi_NDma(MIi_NDMA_TYPE_SEND, ndmaNo, src, (void *)dest, 0, /*Not used*/
           size, MI_NDMA_ENABLE);
}

void MI_NDmaSend_SetUp(u32 ndmaNo, const void *src, volatile void *dest,
                       u32 size) {
  MIi_NDma(MIi_NDMA_TYPE_SEND, ndmaNo, src, (void *)dest, 0, /*Not used*/
           size, MI_NDMA_DISABLE);
}

void MI_NDmaRecv(u32 ndmaNo, volatile const void *src, void *dest, u32 size) {
  MIi_NDma(MIi_NDMA_TYPE_RECV, ndmaNo, (const void *)src, dest, 0, /*Not used*/
           size, MI_NDMA_ENABLE);
}

void MI_NDmaRecv_SetUp(u32 ndmaNo, volatile const void *src, void *dest,
                       u32 size) {
  MIi_NDma(MIi_NDMA_TYPE_RECV, ndmaNo, (const void *)src, dest, 0, /*Not used*/
           size, MI_NDMA_DISABLE);
}

void MI_NDmaPipe(u32 ndmaNo, volatile const void *src, volatile void *dest,
                 u32 size) {
  MIi_NDma(MIi_NDMA_TYPE_PIPE, ndmaNo, (const void *)src, (void *)dest,
           0, /*Not used*/
           size, MI_NDMA_ENABLE);
}

void MI_NDmaPipe_SetUp(u32 ndmaNo, volatile const void *src,
                       volatile void *dest, u32 size) {
  MIi_NDma(MIi_NDMA_TYPE_PIPE, ndmaNo, (const void *)src, (void *)dest,
           0, /*Not used*/
           size, MI_NDMA_DISABLE);
}

void MI_NDmaFillAsync(u32 ndmaNo, void *dest, u32 data, u32 size,
                      MINDmaCallback callback, void *arg) {
  MIi_NDmaAsync(MIi_NDMA_TYPE_FILL, ndmaNo, 0, /*Not used*/
                dest, data, size, callback, arg, MI_NDMA_ENABLE);
}

void MI_NDmaFillAsync_SetUp(u32 ndmaNo, void *dest, u32 data, u32 size,
                            MINDmaCallback callback, void *arg) {
  MIi_NDmaAsync(MIi_NDMA_TYPE_FILL, ndmaNo, 0, /*Not used*/
                dest, data, size, callback, arg, MI_NDMA_DISABLE);
}

void MI_NDmaCopyAsync(u32 ndmaNo, const void *src, void *dest, u32 size,
                      MINDmaCallback callback, void *arg) {
  MIi_NDmaAsync(MIi_NDMA_TYPE_COPY, ndmaNo, src, dest, 0, /*Not used*/
                size, callback, arg, MI_NDMA_ENABLE);
}

void MI_NDmaCopyAsync_SetUp(u32 ndmaNo, const void *src, void *dest, u32 size,
                            MINDmaCallback callback, void *arg) {
  MIi_NDmaAsync(MIi_NDMA_TYPE_COPY, ndmaNo, src, dest, 0, /*Not used*/
                size, callback, arg, MI_NDMA_DISABLE);
}

void MI_NDmaSendAsync(u32 ndmaNo, const void *src, volatile void *dest,
                      u32 size, MINDmaCallback callback, void *arg) {
  MIi_NDmaAsync(MIi_NDMA_TYPE_SEND, ndmaNo, src, (void *)dest, 0, /*Not used*/
                size, callback, arg, MI_NDMA_ENABLE);
}

void MI_NDmaSendAsync_SetUp(u32 ndmaNo, const void *src, volatile void *dest,
                            u32 size, MINDmaCallback callback, void *arg) {
  MIi_NDmaAsync(MIi_NDMA_TYPE_SEND, ndmaNo, src, (void *)dest, 0, /*Not used*/
                size, callback, arg, MI_NDMA_DISABLE);
}

void MI_NDmaRecvAsync(u32 ndmaNo, volatile const void *src, void *dest,
                      u32 size, MINDmaCallback callback, void *arg) {
  MIi_NDmaAsync(MIi_NDMA_TYPE_RECV, ndmaNo, (const void *)src, dest,
                0, /*Not used*/
                size, callback, arg, MI_NDMA_ENABLE);
}

void MI_NDmaRecvAsync_SetUp(u32 ndmaNo, volatile const void *src, void *dest,
                            u32 size, MINDmaCallback callback, void *arg) {
  MIi_NDmaAsync(MIi_NDMA_TYPE_RECV, ndmaNo, (const void *)src, dest,
                0, /*Not used*/
                size, callback, arg, MI_NDMA_DISABLE);
}

void MI_NDmaPipeAsync(u32 ndmaNo, volatile const void *src, volatile void *dest,
                      u32 size, MINDmaCallback callback, void *arg) {
  MIi_NDmaAsync(MIi_NDMA_TYPE_PIPE, ndmaNo, (const void *)src, (void *)dest,
                0, /*Not used*/
                size, callback, arg, MI_NDMA_ENABLE);
}

void MI_NDmaPipeAsync_SetUp(u32 ndmaNo, volatile const void *src,
                            volatile void *dest, u32 size,
                            MINDmaCallback callback, void *arg) {
  MIi_NDmaAsync(MIi_NDMA_TYPE_PIPE, ndmaNo, (const void *)src, (void *)dest,
                0, /*Not used*/
                size, callback, arg, MI_NDMA_DISABLE);
}

void MI_NDmaFillEx(u32 ndmaNo, void *dest, u32 data, u32 size,
                   const MINDmaConfig *config) {
  MIi_NDma_withConfig_Dev(MIi_NDMA_TYPE_FILL, ndmaNo, 0 /*Not used*/, dest,
                          data, size, config, MIi_NDMA_TIMING_IMMIDIATE,
                          MI_NDMA_ENABLE);
}

void MI_NDmaFillEx_SetUp(u32 ndmaNo, void *dest, u32 data, u32 size,
                         const MINDmaConfig *config) {
  MIi_NDma_withConfig_Dev(MIi_NDMA_TYPE_FILL, ndmaNo, 0 /*Not used*/, dest,
                          data, size, config, MIi_NDMA_TIMING_IMMIDIATE,
                          MI_NDMA_DISABLE);
}

void MI_NDmaCopyEx(u32 ndmaNo, const void *src, void *dest, u32 size,
                   const MINDmaConfig *config) {
  MIi_NDma_withConfig_Dev(MIi_NDMA_TYPE_COPY, ndmaNo, src, dest, 0 /*Not used*/,
                          size, config, MIi_NDMA_TIMING_IMMIDIATE,
                          MI_NDMA_ENABLE);
}

void MI_NDmaCopyEx_SetUp(u32 ndmaNo, const void *src, void *dest, u32 size,
                         const MINDmaConfig *config) {
  MIi_NDma_withConfig_Dev(MIi_NDMA_TYPE_COPY, ndmaNo, src, dest, 0 /*Not used*/,
                          size, config, MIi_NDMA_TIMING_IMMIDIATE,
                          MI_NDMA_DISABLE);
}

void MI_NDmaSendEx(u32 ndmaNo, const void *src, volatile void *dest, u32 size,
                   const MINDmaConfig *config) {
  MIi_NDma_withConfig_Dev(MIi_NDMA_TYPE_SEND, ndmaNo, src, (void *)dest,
                          0 /*Not used*/, size, config,
                          MIi_NDMA_TIMING_IMMIDIATE, MI_NDMA_ENABLE);
}

void MI_NDmaSendEx_SetUp(u32 ndmaNo, const void *src, volatile void *dest,
                         u32 size, const MINDmaConfig *config) {
  MIi_NDma_withConfig_Dev(MIi_NDMA_TYPE_SEND, ndmaNo, src, (void *)dest,
                          0 /*Not used*/, size, config,
                          MIi_NDMA_TIMING_IMMIDIATE, MI_NDMA_DISABLE);
}

void MI_NDmaRecvEx(u32 ndmaNo, volatile const void *src, void *dest, u32 size,
                   const MINDmaConfig *config) {
  MIi_NDma_withConfig_Dev(MIi_NDMA_TYPE_RECV, ndmaNo, (const void *)src, dest,
                          0 /*Not used*/, size, config,
                          MIi_NDMA_TIMING_IMMIDIATE, MI_NDMA_ENABLE);
}

void MI_NDmaRecvEx_SetUp(u32 ndmaNo, volatile const void *src, void *dest,
                         u32 size, const MINDmaConfig *config) {
  MIi_NDma_withConfig_Dev(MIi_NDMA_TYPE_RECV, ndmaNo, (const void *)src, dest,
                          0 /*Not used*/, size, config,
                          MIi_NDMA_TIMING_IMMIDIATE, MI_NDMA_DISABLE);
}

void MI_NDmaPipeEx(u32 ndmaNo, volatile const void *src, volatile void *dest,
                   u32 size, const MINDmaConfig *config) {
  MIi_NDma_withConfig_Dev(MIi_NDMA_TYPE_PIPE, ndmaNo, (const void *)src,
                          (void *)dest, 0 /*Not used*/, size, config,
                          MIi_NDMA_TIMING_IMMIDIATE, MI_NDMA_ENABLE);
}

void MI_NDmaPipeEx_SetUp(u32 ndmaNo, volatile const void *src,
                         volatile void *dest, u32 size,
                         const MINDmaConfig *config) {
  MIi_NDma_withConfig_Dev(MIi_NDMA_TYPE_PIPE, ndmaNo, (const void *)src,
                          (void *)dest, 0 /*Not used*/, size, config,
                          MIi_NDMA_TIMING_IMMIDIATE, MI_NDMA_DISABLE);
}

void MI_NDmaFillExAsync(u32 ndmaNo, void *dest, u32 data, u32 size,
                        MINDmaCallback callback, void *arg,
                        const MINDmaConfig *config) {
  MIi_NDmaAsync_withConfig_Dev(MIi_NDMA_TYPE_FILL, ndmaNo, 0, /*Not used*/
                               dest, data, size, callback, arg, config,
                               MIi_NDMA_TIMING_IMMIDIATE, MI_NDMA_ENABLE);
}

void MI_NDmaFillExAsync_SetUp(u32 ndmaNo, void *dest, u32 data, u32 size,
                              MINDmaCallback callback, void *arg,
                              const MINDmaConfig *config) {
  MIi_NDmaAsync_withConfig_Dev(MIi_NDMA_TYPE_FILL, ndmaNo, 0, /*Not used*/
                               dest, data, size, callback, arg, config,
                               MIi_NDMA_TIMING_IMMIDIATE, MI_NDMA_DISABLE);
}

void MI_NDmaCopyExAsync(u32 ndmaNo, const void *src, void *dest, u32 size,
                        MINDmaCallback callback, void *arg,
                        const MINDmaConfig *config) {
  MIi_NDmaAsync_withConfig_Dev(
      MIi_NDMA_TYPE_COPY, ndmaNo, src, dest, 0, /*Not used*/
      size, callback, arg, config, MIi_NDMA_TIMING_IMMIDIATE, MI_NDMA_ENABLE);
}

void MI_NDmaCopyExAsync_SetUp(u32 ndmaNo, const void *src, void *dest, u32 size,
                              MINDmaCallback callback, void *arg,
                              const MINDmaConfig *config) {
  MIi_NDmaAsync_withConfig_Dev(
      MIi_NDMA_TYPE_COPY, ndmaNo, src, dest, 0, /*Not used*/
      size, callback, arg, config, MIi_NDMA_TIMING_IMMIDIATE, MI_NDMA_DISABLE);
}

void MI_NDmaSendExAsync(u32 ndmaNo, const void *src, volatile void *dest,
                        u32 size, MINDmaCallback callback, void *arg,
                        const MINDmaConfig *config) {
  MIi_NDmaAsync_withConfig_Dev(
      MIi_NDMA_TYPE_SEND, ndmaNo, src, (void *)dest, 0, /*Not used*/
      size, callback, arg, config, MIi_NDMA_TIMING_IMMIDIATE, MI_NDMA_ENABLE);
}

void MI_NDmaSendExAsync_SetUp(u32 ndmaNo, const void *src, volatile void *dest,
                              u32 size, MINDmaCallback callback, void *arg,
                              const MINDmaConfig *config) {
  MIi_NDmaAsync_withConfig_Dev(
      MIi_NDMA_TYPE_SEND, ndmaNo, src, (void *)dest, 0, /*Not used*/
      size, callback, arg, config, MIi_NDMA_TIMING_IMMIDIATE, MI_NDMA_DISABLE);
}

void MI_NDmaRecvExAsync(u32 ndmaNo, volatile const void *src, void *dest,
                        u32 size, MINDmaCallback callback, void *arg,
                        const MINDmaConfig *config) {
  MIi_NDmaAsync_withConfig_Dev(
      MIi_NDMA_TYPE_RECV, ndmaNo, (const void *)src, dest, 0, /*Not used*/
      size, callback, arg, config, MIi_NDMA_TIMING_IMMIDIATE, MI_NDMA_ENABLE);
}

void MI_NDmaRecvExAsync_SetUp(u32 ndmaNo, volatile const void *src, void *dest,
                              u32 size, MINDmaCallback callback, void *arg,
                              const MINDmaConfig *config) {
  MIi_NDmaAsync_withConfig_Dev(
      MIi_NDMA_TYPE_RECV, ndmaNo, (const void *)src, dest, 0, /*Not used*/
      size, callback, arg, config, MIi_NDMA_TIMING_IMMIDIATE, MI_NDMA_DISABLE);
}

void MI_NDmaPipeExAsync(u32 ndmaNo, volatile const void *src,
                        volatile void *dest, u32 size, MINDmaCallback callback,
                        void *arg, const MINDmaConfig *config) {
  MIi_NDmaAsync_withConfig_Dev(MIi_NDMA_TYPE_PIPE, ndmaNo, (const void *)src,
                               (void *)dest, 0, /*Not used*/
                               size, callback, arg, config,
                               MIi_NDMA_TIMING_IMMIDIATE, MI_NDMA_ENABLE);
}

void MI_NDmaPipeExAsync_SetUp(u32 ndmaNo, volatile const void *src,
                              volatile void *dest, u32 size,
                              MINDmaCallback callback, void *arg,
                              const MINDmaConfig *config) {
  MIi_NDmaAsync_withConfig_Dev(MIi_NDMA_TYPE_PIPE, ndmaNo, (const void *)src,
                               (void *)dest, 0, /*Not used*/
                               size, callback, arg, config,
                               MIi_NDMA_TIMING_IMMIDIATE, MI_NDMA_DISABLE);
}

void MI_NDmaFill_Dev(u32 ndmaNo, void *dest, u32 data, u32 size,
                     MINDmaDevice dev) {
  MIi_NDma_withConfig_Dev(MIi_NDMA_TYPE_FILL, ndmaNo, 0 /*Not used*/, dest,
                          data, size, &MIi_NDmaConfig[ndmaNo], dev,
                          MI_NDMA_ENABLE);
}

void MI_NDmaFill_Dev_SetUp(u32 ndmaNo, void *dest, u32 data, u32 size,
                           MINDmaDevice dev) {
  MIi_NDma_withConfig_Dev(MIi_NDMA_TYPE_FILL, ndmaNo, 0 /*Not used*/, dest,
                          data, size, &MIi_NDmaConfig[ndmaNo], dev,
                          MI_NDMA_DISABLE);
}

void MI_NDmaCopy_Dev(u32 ndmaNo, const void *src, void *dest, u32 size,
                     MINDmaDevice dev) {
  MIi_NDma_withConfig_Dev(MIi_NDMA_TYPE_COPY, ndmaNo, src, dest, 0 /*Not used*/,
                          size, &MIi_NDmaConfig[ndmaNo], dev, MI_NDMA_ENABLE);
}

void MI_NDmaCopy_Dev_SetUp(u32 ndmaNo, const void *src, void *dest, u32 size,
                           MINDmaDevice dev) {
  MIi_NDma_withConfig_Dev(MIi_NDMA_TYPE_COPY, ndmaNo, src, dest, 0 /*Not used*/,
                          size, &MIi_NDmaConfig[ndmaNo], dev, MI_NDMA_DISABLE);
}

void MI_NDmaSend_Dev(u32 ndmaNo, const void *src, volatile void *dest, u32 size,
                     MINDmaDevice dev) {
  MIi_NDma_withConfig_Dev(MIi_NDMA_TYPE_SEND, ndmaNo, src, (void *)dest,
                          0 /*Not used*/, size, &MIi_NDmaConfig[ndmaNo], dev,
                          MI_NDMA_ENABLE);
}

void MI_NDmaSend_Dev_SetUp(u32 ndmaNo, const void *src, volatile void *dest,
                           u32 size, MINDmaDevice dev) {
  MIi_NDma_withConfig_Dev(MIi_NDMA_TYPE_SEND, ndmaNo, src, (void *)dest,
                          0 /*Not used*/, size, &MIi_NDmaConfig[ndmaNo], dev,
                          MI_NDMA_DISABLE);
}

void MI_NDmaRecv_Dev(u32 ndmaNo, volatile const void *src, void *dest, u32 size,
                     MINDmaDevice dev) {
  MIi_NDma_withConfig_Dev(MIi_NDMA_TYPE_RECV, ndmaNo, (const void *)src, dest,
                          0 /*Not used*/, size, &MIi_NDmaConfig[ndmaNo], dev,
                          MI_NDMA_ENABLE);
}

void MI_NDmaRecv_Dev_SetUp(u32 ndmaNo, volatile const void *src, void *dest,
                           u32 size, MINDmaDevice dev) {
  MIi_NDma_withConfig_Dev(MIi_NDMA_TYPE_RECV, ndmaNo, (const void *)src, dest,
                          0 /*Not used*/, size, &MIi_NDmaConfig[ndmaNo], dev,
                          MI_NDMA_DISABLE);
}

void MI_NDmaPipe_Dev(u32 ndmaNo, volatile const void *src, volatile void *dest,
                     u32 size, MINDmaDevice dev) {
  MIi_NDma_withConfig_Dev(MIi_NDMA_TYPE_PIPE, ndmaNo, (const void *)src,
                          (void *)dest, 0 /*Not used*/, size,
                          &MIi_NDmaConfig[ndmaNo], dev, MI_NDMA_ENABLE);
}

void MI_NDmaPipe_Dev_SetUp(u32 ndmaNo, volatile const void *src,
                           volatile void *dest, u32 size, MINDmaDevice dev) {
  MIi_NDma_withConfig_Dev(MIi_NDMA_TYPE_PIPE, ndmaNo, (const void *)src,
                          (void *)dest, 0 /*Not used*/, size,
                          &MIi_NDmaConfig[ndmaNo], dev, MI_NDMA_DISABLE);
}

void MI_NDmaFillAsync_Dev(u32 ndmaNo, void *dest, u32 data, u32 size,
                          MINDmaCallback callback, void *arg,
                          MINDmaDevice dev) {
  MIi_NDmaAsync_withConfig_Dev(MIi_NDMA_TYPE_FILL, ndmaNo, 0 /*Not used*/, dest,
                               data, size, callback, arg,
                               &MIi_NDmaConfig[ndmaNo], dev, MI_NDMA_ENABLE);
}

void MI_NDmaFillAsync_Dev_SetUp(u32 ndmaNo, void *dest, u32 data, u32 size,
                                MINDmaCallback callback, void *arg,
                                MINDmaDevice dev) {
  MIi_NDmaAsync_withConfig_Dev(MIi_NDMA_TYPE_FILL, ndmaNo, 0 /*Not used*/, dest,
                               data, size, callback, arg,
                               &MIi_NDmaConfig[ndmaNo], dev, MI_NDMA_DISABLE);
}

void MI_NDmaCopyAsync_Dev(u32 ndmaNo, const void *src, void *dest, u32 size,
                          MINDmaCallback callback, void *arg,
                          MINDmaDevice dev) {
  MIi_NDmaAsync_withConfig_Dev(MIi_NDMA_TYPE_COPY, ndmaNo, src, dest,
                               0 /*Not used*/, size, callback, arg,
                               &MIi_NDmaConfig[ndmaNo], dev, MI_NDMA_ENABLE);
}

void MI_NDmaCopyAsync_Dev_SetUp(u32 ndmaNo, const void *src, void *dest,
                                u32 size, MINDmaCallback callback, void *arg,
                                MINDmaDevice dev) {
  MIi_NDmaAsync_withConfig_Dev(MIi_NDMA_TYPE_COPY, ndmaNo, src, dest,
                               0 /*Not used*/, size, callback, arg,
                               &MIi_NDmaConfig[ndmaNo], dev, MI_NDMA_DISABLE);
}

void MI_NDmaSendAsync_Dev(u32 ndmaNo, const void *src, volatile void *dest,
                          u32 size, MINDmaCallback callback, void *arg,
                          MINDmaDevice dev) {
  MIi_NDmaAsync_withConfig_Dev(MIi_NDMA_TYPE_SEND, ndmaNo, src, (void *)dest,
                               0 /*Not used*/, size, callback, arg,
                               &MIi_NDmaConfig[ndmaNo], dev, MI_NDMA_ENABLE);
}

void MI_NDmaSendAsync_Dev_SetUp(u32 ndmaNo, const void *src,
                                volatile void *dest, u32 size,
                                MINDmaCallback callback, void *arg,
                                MINDmaDevice dev) {
  MIi_NDmaAsync_withConfig_Dev(MIi_NDMA_TYPE_SEND, ndmaNo, src, (void *)dest,
                               0 /*Not used*/, size, callback, arg,
                               &MIi_NDmaConfig[ndmaNo], dev, MI_NDMA_DISABLE);
}

void MI_NDmaRecvAsync_Dev(u32 ndmaNo, volatile const void *src, void *dest,
                          u32 size, MINDmaCallback callback, void *arg,
                          MINDmaDevice dev) {
  MIi_NDmaAsync_withConfig_Dev(MIi_NDMA_TYPE_RECV, ndmaNo, (const void *)src,
                               dest, 0 /*Not used*/, size, callback, arg,
                               &MIi_NDmaConfig[ndmaNo], dev, MI_NDMA_ENABLE);
}

void MI_NDmaRecvAsync_Dev_SetUp(u32 ndmaNo, volatile const void *src,
                                void *dest, u32 size, MINDmaCallback callback,
                                void *arg, MINDmaDevice dev) {
  MIi_NDmaAsync_withConfig_Dev(MIi_NDMA_TYPE_RECV, ndmaNo, (const void *)src,
                               dest, 0 /*Not used*/, size, callback, arg,
                               &MIi_NDmaConfig[ndmaNo], dev, MI_NDMA_DISABLE);
}

void MI_NDmaPipeAsync_Dev(u32 ndmaNo, volatile const void *src,
                          volatile void *dest, u32 size,
                          MINDmaCallback callback, void *arg,
                          MINDmaDevice dev) {
  MIi_NDmaAsync_withConfig_Dev(MIi_NDMA_TYPE_PIPE, ndmaNo, (const void *)src,
                               (void *)dest, 0 /*Not used*/, size, callback,
                               arg, &MIi_NDmaConfig[ndmaNo], dev,
                               MI_NDMA_ENABLE);
}

void MI_NDmaPipeAsync_Dev_SetUp(u32 ndmaNo, volatile const void *src,
                                volatile void *dest, u32 size,
                                MINDmaCallback callback, void *arg,
                                MINDmaDevice dev) {
  MIi_NDmaAsync_withConfig_Dev(MIi_NDMA_TYPE_PIPE, ndmaNo, (const void *)src,
                               (void *)dest, 0 /*Not used*/, size, callback,
                               arg, &MIi_NDmaConfig[ndmaNo], dev,
                               MI_NDMA_DISABLE);
}

void MI_NDmaFillEx_Dev(u32 ndmaNo, void *dest, u32 data, u32 size,
                       const MINDmaConfig *config, MINDmaDevice dev) {
  MIi_NDma_withConfig_Dev(MIi_NDMA_TYPE_FILL, ndmaNo, 0 /*Not used*/, dest,
                          data, size, config, dev, MI_NDMA_ENABLE);
}

void MI_NDmaFillEx_Dev_SetUp(u32 ndmaNo, void *dest, u32 data, u32 size,
                             const MINDmaConfig *config, MINDmaDevice dev) {
  MIi_NDma_withConfig_Dev(MIi_NDMA_TYPE_FILL, ndmaNo, 0 /*Not used*/, dest,
                          data, size, config, dev, MI_NDMA_DISABLE);
}

void MI_NDmaCopyEx_Dev(u32 ndmaNo, const void *src, void *dest, u32 size,
                       const MINDmaConfig *config, MINDmaDevice dev) {
  MIi_NDma_withConfig_Dev(MIi_NDMA_TYPE_COPY, ndmaNo, src, dest, 0 /*Not used*/,
                          size, config, dev, MI_NDMA_ENABLE);
}

void MI_NDmaCopyEx_Dev_SetUp(u32 ndmaNo, const void *src, void *dest, u32 size,
                             const MINDmaConfig *config, MINDmaDevice dev) {
  MIi_NDma_withConfig_Dev(MIi_NDMA_TYPE_COPY, ndmaNo, src, dest, 0 /*Not used*/,
                          size, config, dev, MI_NDMA_DISABLE);
}

void MI_NDmaSendEx_Dev(u32 ndmaNo, const void *src, volatile void *dest,
                       u32 size, const MINDmaConfig *config, MINDmaDevice dev) {
  MIi_NDma_withConfig_Dev(MIi_NDMA_TYPE_SEND, ndmaNo, src, (void *)dest,
                          0 /*Not used*/, size, config, dev, MI_NDMA_ENABLE);
}

void MI_NDmaSendEx_Dev_SetUp(u32 ndmaNo, const void *src, volatile void *dest,
                             u32 size, const MINDmaConfig *config,
                             MINDmaDevice dev) {
  MIi_NDma_withConfig_Dev(MIi_NDMA_TYPE_SEND, ndmaNo, src, (void *)dest,
                          0 /*Not used*/, size, config, dev, MI_NDMA_DISABLE);
}

void MI_NDmaRecvEx_Dev(u32 ndmaNo, volatile const void *src, void *dest,
                       u32 size, const MINDmaConfig *config, MINDmaDevice dev) {
  MIi_NDma_withConfig_Dev(MIi_NDMA_TYPE_RECV, ndmaNo, (const void *)src, dest,
                          0 /*Not used*/, size, config, dev, MI_NDMA_ENABLE);
}

void MI_NDmaRecvEx_Dev_SetUp(u32 ndmaNo, volatile const void *src, void *dest,
                             u32 size, const MINDmaConfig *config,
                             MINDmaDevice dev) {
  MIi_NDma_withConfig_Dev(MIi_NDMA_TYPE_RECV, ndmaNo, (const void *)src, dest,
                          0 /*Not used*/, size, config, dev, MI_NDMA_DISABLE);
}

void MI_NDmaPipeEx_Dev(u32 ndmaNo, volatile const void *src,
                       volatile void *dest, u32 size,
                       const MINDmaConfig *config, MINDmaDevice dev) {
  MIi_NDma_withConfig_Dev(MIi_NDMA_TYPE_PIPE, ndmaNo, (const void *)src,
                          (void *)dest, 0 /*Not used*/, size, config, dev,
                          MI_NDMA_ENABLE);
}

void MI_NDmaPipeEx_Dev_SetUp(u32 ndmaNo, volatile const void *src,
                             volatile void *dest, u32 size,
                             const MINDmaConfig *config, MINDmaDevice dev) {
  MIi_NDma_withConfig_Dev(MIi_NDMA_TYPE_PIPE, ndmaNo, (const void *)src,
                          (void *)dest, 0 /*Not used*/, size, config, dev,
                          MI_NDMA_DISABLE);
}

void MI_NDmaFillExAsync_Dev(u32 ndmaNo, void *dest, u32 data, u32 size,
                            MINDmaCallback callback, void *arg,
                            const MINDmaConfig *config, MINDmaDevice dev) {
  MIi_NDmaAsync_withConfig_Dev(MIi_NDMA_TYPE_FILL, ndmaNo, 0 /*Not used*/, dest,
                               data, size, callback, arg, config, dev,
                               MI_NDMA_ENABLE);
}

void MI_NDmaFillExAsync_Dev_SetUp(u32 ndmaNo, void *dest, u32 data, u32 size,
                                  MINDmaCallback callback, void *arg,
                                  const MINDmaConfig *config,
                                  MINDmaDevice dev) {
  MIi_NDmaAsync_withConfig_Dev(MIi_NDMA_TYPE_FILL, ndmaNo, 0 /*Not used*/, dest,
                               data, size, callback, arg, config, dev,
                               MI_NDMA_DISABLE);
}

void MI_NDmaCopyExAsync_Dev(u32 ndmaNo, const void *src, void *dest, u32 size,
                            MINDmaCallback callback, void *arg,
                            const MINDmaConfig *config, MINDmaDevice dev) {
  MIi_NDmaAsync_withConfig_Dev(MIi_NDMA_TYPE_COPY, ndmaNo, src, dest,
                               0 /*Not used*/, size, callback, arg, config, dev,
                               MI_NDMA_ENABLE);
}

void MI_NDmaCopyExAsync_Dev_SetUp(u32 ndmaNo, const void *src, void *dest,
                                  u32 size, MINDmaCallback callback, void *arg,
                                  const MINDmaConfig *config,
                                  MINDmaDevice dev) {
  MIi_NDmaAsync_withConfig_Dev(MIi_NDMA_TYPE_COPY, ndmaNo, src, dest,
                               0 /*Not used*/, size, callback, arg, config, dev,
                               MI_NDMA_DISABLE);
}

void MI_NDmaSendExAsync_Dev(u32 ndmaNo, const void *src, volatile void *dest,
                            u32 size, MINDmaCallback callback, void *arg,
                            const MINDmaConfig *config, MINDmaDevice dev) {
  MIi_NDmaAsync_withConfig_Dev(MIi_NDMA_TYPE_SEND, ndmaNo, src, (void *)dest,
                               0 /*Not used*/, size, callback, arg, config, dev,
                               MI_NDMA_ENABLE);
}

void MI_NDmaSendExAsync_Dev_SetUp(u32 ndmaNo, const void *src,
                                  volatile void *dest, u32 size,
                                  MINDmaCallback callback, void *arg,
                                  const MINDmaConfig *config,
                                  MINDmaDevice dev) {
  MIi_NDmaAsync_withConfig_Dev(MIi_NDMA_TYPE_SEND, ndmaNo, src, (void *)dest,
                               0 /*Not used*/, size, callback, arg, config, dev,
                               MI_NDMA_DISABLE);
}

void MI_NDmaRecvExAsync_Dev(u32 ndmaNo, volatile const void *src, void *dest,
                            u32 size, MINDmaCallback callback, void *arg,
                            const MINDmaConfig *config, MINDmaDevice dev) {
  MIi_NDmaAsync_withConfig_Dev(MIi_NDMA_TYPE_RECV, ndmaNo, (const void *)src,
                               dest, 0 /*Not used*/, size, callback, arg,
                               config, dev, MI_NDMA_ENABLE);
}

void MI_NDmaRecvExAsync_Dev_SetUp(u32 ndmaNo, volatile const void *src,
                                  void *dest, u32 size, MINDmaCallback callback,
                                  void *arg, const MINDmaConfig *config,
                                  MINDmaDevice dev) {
  MIi_NDmaAsync_withConfig_Dev(MIi_NDMA_TYPE_RECV, ndmaNo, (const void *)src,
                               dest, 0 /*Not used*/, size, callback, arg,
                               config, dev, MI_NDMA_DISABLE);
}

void MI_NDmaPipeExAsync_Dev(u32 ndmaNo, volatile const void *src,
                            volatile void *dest, u32 size,
                            MINDmaCallback callback, void *arg,
                            const MINDmaConfig *config, MINDmaDevice dev) {
  MIi_NDmaAsync_withConfig_Dev(MIi_NDMA_TYPE_PIPE, ndmaNo, (const void *)src,
                               (void *)dest, 0 /*Not used*/, size, callback,
                               arg, config, dev, MI_NDMA_ENABLE);
}

void MI_NDmaPipeExAsync_Dev_SetUp(u32 ndmaNo, volatile const void *src,
                                  volatile void *dest, u32 size,
                                  MINDmaCallback callback, void *arg,
                                  const MINDmaConfig *config,
                                  MINDmaDevice dev) {
  MIi_NDmaAsync_withConfig_Dev(MIi_NDMA_TYPE_PIPE, ndmaNo, (const void *)src,
                               (void *)dest, 0 /*Not used*/, size, callback,
                               arg, config, dev, MI_NDMA_DISABLE);
}

BOOL MI_IsNDmaBusy(u32 ndmaNo) {
  MIi_ASSERT_DMANO(ndmaNo);

  {
    vu32 *dmaCntp = MI_NDMA_REGADDR(ndmaNo, MI_NDMA_REG_CNT_WOFFSET);
    return (BOOL)((*dmaCntp & REG_MI_NDMA0CNT_E_MASK) >>
                  REG_MI_NDMA0CNT_E_SHIFT);
  }
}

void MI_WaitNDma(u32 ndmaNo) {
  MIi_ASSERT_DMANO(ndmaNo);

  {
    OSIntrMode enabled = OS_DisableInterrupts();
    vu32 *dmaCntp = MI_NDMA_REGADDR(ndmaNo, MI_NDMA_REG_CNT_WOFFSET);
    while (*dmaCntp & REG_MI_DMA0CNT_E_MASK) {
    }
    (void)OS_RestoreInterrupts(enabled);
  }
}

void MI_StopNDma(u32 ndmaNo) {
  OSIntrMode enabled = OS_DisableInterrupts();

  vu32 *regCont = MI_NDMA_REGADDR(ndmaNo, MI_NDMA_REG_CNT_WOFFSET);

  *regCont = *regCont & (~MI_NDMA_ENABLE);

  {
    u32 dummy = *regCont;
  }
  {
    u32 dummy = *regCont;
  }

  (void)OS_RestoreInterrupts(enabled);
}

void MI_StopAllNDma(void) {
  MI_StopNDma(0);
  MI_StopNDma(1);
  MI_StopNDma(2);
  MI_StopNDma(3);
}

static void MIi_Wait(u32 ndmaNo) {
  MIi_ASSERT_DMANO(ndmaNo);

  while (MI_IsNDmaBusy(ndmaNo) == TRUE) {
  }
}

void MI_NDmaRestart(u32 ndmaNo) {
  MIi_ASSERT_DMANO(ndmaNo);

  MIi_Wait(ndmaNo);
  MI_NDMA_REG(ndmaNo, MI_NDMA_REG_CNT_WOFFSET) |= MI_NDMA_ENABLE;
}

void MI_SetNDmaArbitrament(u32 mode, u32 cycle) {
  if (mode == MI_NDMA_ARBITRAMENT_ROUND) {
    reg_MI_NDMAGCNT = MI_NDMA_ARBITRAMENT_ROUND | cycle;
  } else {
    reg_MI_NDMAGCNT = MI_NDMA_ARBITRAMENT_FIX;
  }
}

u32 MI_GetNDmaArbitramentMode(void) {
  return (reg_MI_NDMAGCNT & REG_MI_NDMAGCNT_ARBITER_MASK);
}

u32 MI_GetNDmaArbitramentRoundRobinCycle(void) {
  return (reg_MI_NDMAGCNT & MI_NDMA_RCYCLE_MASK);
}

void MI_SetNDmaInterval(u32 ndmaNo, u32 intervalTimer, u32 prescaler) {
  MIi_NDmaConfig[ndmaNo].intervalTimer = intervalTimer;
  MIi_NDmaConfig[ndmaNo].prescaler = prescaler;
}

u32 MI_GetNDmaIntervalTimer(u32 ndmaNo) {
  return MIi_NDmaConfig[ndmaNo].intervalTimer;
}

u32 MI_GetNDmaIntervalPrescaler(u32 ndmaNo) {
  return MIi_NDmaConfig[ndmaNo].prescaler;
}

void MI_SetNDmaBlockWord(u32 ndmaNo, u32 word) {
  MIi_NDmaConfig[ndmaNo].blockWord = word;
}

u32 MI_GetNDmaBlockWord(u32 ndmaNo) { return MIi_NDmaConfig[ndmaNo].blockWord; }

void MI_SetNDmaWordCount(u32 ndmaNo, u32 wordCount) {
  MIi_NDmaConfig[ndmaNo].wordCount = wordCount;
}

u32 MI_GetNDmaWordCount(u32 ndmaNo) { return MIi_NDmaConfig[ndmaNo].wordCount; }

void MI_InitNDmaConfig(void) {
  MINDmaConfig *p = &MIi_NDmaConfig[0];

  int n;
  for (n = 0; n <= MI_NDMA_MAX_NUM; n++) {
    p->intervalTimer = 1;
    p->prescaler = MI_NDMA_INTERVAL_PS_1;
    p->blockWord = MI_NDMA_BWORD_16;
    p->wordCount = MI_NDMA_AT_A_TIME;
    p++;
  }
}

void MI_GetNDmaConfig(u32 ndmaNo, MINDmaConfig *config) {
  MIi_ASSERT_DMANO(ndmaNo);
  MI_CpuCopy32(&MIi_NDmaConfig[ndmaNo], config, sizeof(MINDmaConfig));
}

void MI_SetNDmaConfig(u32 ndmaNo, const MINDmaConfig *config) {
  MIi_ASSERT_DMANO(ndmaNo);
  MI_CpuCopy32(config, &MIi_NDmaConfig[ndmaNo], sizeof(MINDmaConfig));
}

void MI_TimerNDmaCopy(u32 ndmaNo, u32 timerNo, const void *src, void *dest,
                      u32 size) {
  MIi_ASSERT_DMANO(ndmaNo);
  MIi_ASSERT_TIMERNO(timerNo);
  {
    MINDmaDevice dev = MI_NDMA_TIMING_TIMER0 + timerNo;
    MI_NDmaCopy_Dev(ndmaNo, src, dest, size, dev);
  }
}

#ifdef SDK_ARM9

void MI_HBlankNDmaCopy(u32 ndmaNo, const void *src, void *dest, u32 size) {
  MIi_ASSERT_DMANO(ndmaNo);
  MIi_NDma_withConfig_Dev(MIi_NDMA_TYPE_HBLANK, ndmaNo, src, dest,
                          0 /*Not used*/, size, &MIi_NDmaConfig[ndmaNo],
                          MI_NDMA_TIMING_H_BLANK, MI_NDMA_ENABLE);
}
void MI_HBlankNDmaCopyIf(u32 ndmaNo, const void *src, void *dest, u32 size) {
  MIi_ASSERT_DMANO(ndmaNo);
  MIi_NDma_withConfig_Dev(MIi_NDMA_TYPE_HBLANK_IF, ndmaNo, src, dest,
                          0 /*Not used*/, size, &MIi_NDmaConfig[ndmaNo],
                          MI_NDMA_TIMING_H_BLANK, MI_NDMA_ENABLE);
}
#endif

void MI_VBlankNDmaCopy(u32 ndmaNo, const void *src, void *dest, u32 size) {
  MIi_ASSERT_DMANO(ndmaNo);
  MI_NDmaCopy_Dev(ndmaNo, src, dest, size, MI_NDMA_TIMING_V_BLANK);
}

void MI_VBlankNDmaCopyAsync(u32 ndmaNo, const void *src, void *dest, u32 size,
                            MIDmaCallback callback, void *arg) {
  MIi_ASSERT_DMANO(ndmaNo);
  MI_NDmaCopyAsync_Dev(ndmaNo, src, dest, size, callback, arg,
                       MI_NDMA_TIMING_V_BLANK);
}

static void MIi_CardNDmaRecv32(u32 ndmaNo, const void *src, void *dest) {
  MIi_ASSERT_DMANO(ndmaNo);
  {
    const MINDmaConfig *config = &MIi_NDmaConfig[ndmaNo];
    const u32 size = sizeof(u32);
    OSIntrMode enabled = OS_DisableInterrupts();
    u32 contData;
    MIi_Wait(ndmaNo);
    MIi_SetSrc(ndmaNo, (u32)src);
    MIi_SetDest(ndmaNo, (u32)dest);
    MIi_SetTotalWordCount(ndmaNo, size / sizeof(u32));
    MIi_SetWordCount(ndmaNo, size / sizeof(u32));
    contData = (config->blockWord | MI_NDMA_TIMING_CARD | MI_NDMA_SRC_FIX |
                MI_NDMA_DEST_INC | MI_NDMA_DEST_RELOAD_DISABLE |
                MI_NDMA_ENABLE | MI_NDMA_CONTINUOUS_ON);
    MIi_SetControl(ndmaNo, contData);

    (void)OS_RestoreInterrupts(enabled);
  }
}

void MI_Card_NDmaCopy(u32 ndmaNo, const void *src, void *dest, u32 size) {
  (void)size;
  MIi_CardNDmaRecv32(ndmaNo, src, dest);
}

#ifdef SDK_ARM9

#define CAMERA_DMA_BLOCK_SIZE MI_NDMA_BWORD_16
#define CAMERA_DMA_INTERVAL 2
#define CAMERA_DMA_PRESCALER MI_NDMA_INTERVAL_PS_1

void MI_Camera_NDmaRecv(u32 ndmaNo, void *dest, u32 unit, u32 size,
                        BOOL contSw) {
  MINDmaConfig config;

  MIiNDmaType type = (contSw) ? MIi_NDMA_TYPE_CAMERACONT : MIi_NDMA_TYPE_RECV;

  MIi_ASSERT_DMANO(ndmaNo);

  MI_GetNDmaConfig(ndmaNo, &config);
  config.intervalTimer = CAMERA_DMA_INTERVAL;
  config.prescaler = CAMERA_DMA_PRESCALER;
  config.blockWord = CAMERA_DMA_BLOCK_SIZE;
  config.wordCount = unit;

  MIi_NDma_withConfig_Dev(type, ndmaNo, (void *)REG_DAT_ADDR, dest,
                          0 /*Not used*/, size, &config, MI_NDMA_TIMING_CAMERA,
                          MI_NDMA_ENABLE);
}

void MI_Camera_NDmaRecvEx(u32 ndmaNo, void *dest, u32 size, BOOL contSw,
                          const MINDmaConfig *config) {
  MIiNDmaType type = (contSw) ? MIi_NDMA_TYPE_CAMERACONT : MIi_NDMA_TYPE_RECV;

  MIi_ASSERT_DMANO(ndmaNo);

  MIi_NDma_withConfig_Dev(type, ndmaNo, (void *)REG_DAT_ADDR, dest,
                          0 /*Not used*/, size, config, MI_NDMA_TIMING_CAMERA,
                          MI_NDMA_ENABLE);
}

void MI_Camera_NDmaRecvAsync(u32 ndmaNo, void *dest, u32 unit, u32 size,
                             BOOL contSw, MINDmaCallback callback, void *arg) {
  MINDmaConfig config;

  MIiNDmaType type = (contSw) ? MIi_NDMA_TYPE_CAMERACONT : MIi_NDMA_TYPE_RECV;

  MIi_ASSERT_DMANO(ndmaNo);

  MI_GetNDmaConfig(ndmaNo, &config);
  config.intervalTimer = CAMERA_DMA_INTERVAL;
  config.prescaler = CAMERA_DMA_PRESCALER;
  config.blockWord = CAMERA_DMA_BLOCK_SIZE;
  config.wordCount = unit;

  MIi_NDmaAsync_withConfig_Dev(type, ndmaNo, (void *)REG_DAT_ADDR, dest,
                               0 /*Not used*/, size, callback, arg, &config,
                               MI_NDMA_TIMING_CAMERA, MI_NDMA_ENABLE);
}

void MI_Camera_NDmaRecvAsyncEx(u32 ndmaNo, void *dest, u32 size, BOOL contSw,
                               MINDmaCallback callback, void *arg,
                               const MINDmaConfig *config) {
  MIiNDmaType type = (contSw) ? MIi_NDMA_TYPE_CAMERACONT : MIi_NDMA_TYPE_RECV;

  MIi_ASSERT_DMANO(ndmaNo);

  MIi_NDmaAsync_withConfig_Dev(type, ndmaNo, (void *)REG_DAT_ADDR, dest,
                               0 /*Not used*/, size, callback, arg, config,
                               MI_NDMA_TIMING_CAMERA, MI_NDMA_ENABLE);
}
#endif

#if (!defined(SDK_ARM9) && !defined(SDK_PORT))

void MIi_Aes_NDmaSend(u32 ndmaNo, const void *src, u32 size,
                      MINDmaCallback callback, void *arg,
                      const MINDmaConfig *pConfig) {
  MIi_ASSERT_DMANO(ndmaNo);

  MIi_NDmaAsync_withConfig_Dev(MIi_NDMA_TYPE_SEND, ndmaNo, src,
                               (void *)&reg_AES_AES_IFIFO, 0 /*Not used*/, size,
                               callback, arg, pConfig, MI_NDMA_TIMING_AES_IN,
                               MI_NDMA_ENABLE);
}
void MIi_Aes_NDmaRecv(u32 ndmaNo, void *dest, u32 size, MINDmaCallback callback,
                      void *arg, const MINDmaConfig *pConfig) {
  MIi_ASSERT_DMANO(ndmaNo);

  MIi_NDmaAsync_withConfig_Dev(MIi_NDMA_TYPE_RECV, ndmaNo,
                               (const void *)&reg_AES_AES_OFIFO, dest,
                               0 /*Not used*/, size, callback, arg, pConfig,
                               MI_NDMA_TIMING_AES_OUT, MI_NDMA_ENABLE);
}
#endif

void MIi_SetNDmaSrc(u32 ndmaNo, void *src) { MIi_SetSrc(ndmaNo, (u32)src); }
void MIi_SetNDmaDest(u32 ndmaNo, void *dest) { MIi_SetDest(ndmaNo, (u32)dest); }
void MIi_SetNDmaTotalWordCount(u32 ndmaNo, u32 size) {
  MIi_SetTotalWordCount(ndmaNo, size);
}
void MIi_SetNDmaWordCount(u32 ndmaNo, u32 size) {
  MIi_SetWordCount(ndmaNo, size);
}
void MIi_SetNDmaInterval(u32 ndmaNo, u32 intervalTimer, u32 prescaler) {
  MIi_SetInterval(ndmaNo, intervalTimer, prescaler);
}

#ifdef SDK_ARM7
#include <twl/ltdwram_end.h>
#endif

#ifdef SDK_ARM9
#include <twl/ltdmain_end.h>
#endif

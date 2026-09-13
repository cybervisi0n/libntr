#ifndef TWL_COMMON_MI_DMA_H_
#define TWL_COMMON_MI_DMA_H_

#if SDK_VERSION_MAJOR != 5
#error This file is for SDK 5 only!
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define MIi_SRC_INC (MI_NDMA_SRC_INC | MI_NDMA_SRC_RELOAD_DISABLE)
#define MIi_SRC_DEC (MI_NDMA_SRC_DEC | MI_NDMA_SRC_RELOAD_DISABLE)
#define MIi_SRC_FIX (MI_NDMA_SRC_FIX | MI_NDMA_SRC_RELOAD_DISABLE)
#define MIi_SRC_FILLDATA (MI_NDMA_SRC_FILLDATA | MI_NDMA_SRC_RELOAD_DISABLE)

#define MIi_DEST_INC (MI_NDMA_DEST_INC | MI_NDMA_DEST_RELOAD_DISABLE)
#define MIi_DEST_DEC (MI_NDMA_DEST_DEC | MI_NDMA_DEST_RELOAD_DISABLE)
#define MIi_DEST_FIX (MI_NDMA_DEST_FIX | MI_NDMA_DEST_RELOAD_DISABLE)
#define MIi_DEST_INC_RELOAD (MI_NDMA_SRC_INC | MI_NDMA_DEST_RELOAD_ENABLE)

#define MIi_IMM (MI_NDMA_IMM_MODE_ON)

#define MIi_CONT (MI_NDMA_CONTINUOUS_ON)

typedef enum {
  MIi_NDMA_TYPE_FILL = 0,
  MIi_NDMA_TYPE_COPY = 1,
  MIi_NDMA_TYPE_SEND = 2,
  MIi_NDMA_TYPE_RECV = 3,
  MIi_NDMA_TYPE_PIPE = 4,

  MIi_NDMA_TYPE_HBLANK = 5,
  MIi_NDMA_TYPE_HBLANK_IF = 6,
  MIi_NDMA_TYPE_MMCOPY = 7,
  MIi_NDMA_TYPE_GXCOPY = 8,
  MIi_NDMA_TYPE_GXCOPY_IF = 9,
  MIi_NDMA_TYPE_CAMERACONT = 10

} MIiNDmaType;

extern MINDmaConfig MIi_NDmaConfig[MI_NDMA_MAX_NUM + 1];

static inline void MIi_CallCallback(MINDmaCallback callback, void *arg) {
  if (callback) {
    (callback)(arg);
  }
}

#define MIi_ASSERT_DMANO(dmaNo)                                                \
  SDK_TASSERTMSG((dmaNo) <= MI_DMA_MAX_NUM, "illegal DMA No.")
#define MIi_ASSERT_MUL2(size)                                                  \
  SDK_TASSERTMSG(((size) & 1) == 0, "size & 1 must be 0")
#define MIi_ASSERT_MUL4(size)                                                  \
  SDK_TASSERTMSG(((size) & 3) == 0, "size & 3 must be 0")
#define MIi_ASSERT_SRC_ALIGN512(src)                                           \
  SDK_TASSERTMSG(((u32)(src) & 511) == 0,                                      \
                 "source address must be in 512-byte alignment")
#define MIi_ASSERT_SRC_ALIGN4(src)                                             \
  SDK_TASSERTMSG(((u32)(src) & 3) == 0,                                        \
                 "source address must be in 4-byte alignment")
#define MIi_ASSERT_SRC_ALIGN2(src)                                             \
  SDK_TASSERTMSG(((u32)(src) & 1) == 0,                                        \
                 "source address must be in 2-byte alignment")
#define MIi_ASSERT_DEST_ALIGN4(dest)                                           \
  SDK_TASSERTMSG(((u32)(dest) & 3) == 0,                                       \
                 "destination address must be in 4-byte alignment")
#define MIi_ASSERT_DEST_ALIGN2(dest)                                           \
  SDK_TASSERTMSG(((u32)(dest) & 1) == 0,                                       \
                 "destination address must be in 2-byte alignment")
#define MIi_ASSERT_TIMERNO(timerNo)                                            \
  SDK_TASSERTMSG((timerNo) <= 3, "illegal Timer No.");

#if defined(SDK_ARM9) && defined(SDK_DEBUG)
void MIi_CheckAddressInTCM(u32 addr, u32 size);
#define MIi_WARNING_ADDRINTCM(addr, size)                                      \
  MIi_CheckAddressInTCM((u32)addr, (u32)size)
#else
#define MIi_WARNING_ADDRINTCM(addr, size) ((void)0)
#endif

#ifdef SDK_ARM9

#endif

void MIi_NDma_withConfig_Dev(MIiNDmaType ndmaType, u32 ndmaNo, const void *src,
                             void *dest, u32 data, u32 size,
                             const MINDmaConfig *config, MINDmaDevice dev,
                             u32 enable);

void MIi_NDmaAsync_withConfig_Dev(MIiNDmaType ndmaType, u32 ndmaNo,
                                  const void *src, void *dest, u32 data,
                                  u32 size, MINDmaCallback callback, void *arg,
                                  const MINDmaConfig *config, MINDmaDevice dev,
                                  u32 enable);

static inline void MIi_NDma(MIiNDmaType ndmaType, u32 ndmaNo, const void *src,
                            void *dest, u32 data, u32 size, u32 enable) {
  MIi_NDma_withConfig_Dev(ndmaType, ndmaNo, src, dest, data, size,
                          &MIi_NDmaConfig[ndmaNo], MIi_NDMA_TIMING_IMMIDIATE,
                          enable);
}

static inline void MIi_NDmaAsync(MIiNDmaType ndmaType, u32 ndmaNo,
                                 const void *src, void *dest, u32 data,
                                 u32 size, MINDmaCallback callback, void *arg,
                                 u32 enable) {
  MIi_NDmaAsync_withConfig_Dev(ndmaType, ndmaNo, src, dest, data, size,
                               callback, arg, &MIi_NDmaConfig[ndmaNo],
                               MIi_NDMA_TIMING_IMMIDIATE, enable);
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

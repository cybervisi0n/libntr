#ifndef TWL_MI_DMA_H_
#define TWL_MI_DMA_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <twl/misc.h>
#include <twl/types.h>
#include <twl/memorymap.h>

#include <nitro/mi/dma.h>

#define MI_NDMA_MAX_NUM 3

#define MI_NDMA_ARBITRAMENT_FIX (0UL << REG_MI_NDMAGCNT_ARBITER_SHIFT)
#define MI_NDMA_ARBITRAMENT_ROUND (1UL << REG_MI_NDMAGCNT_ARBITER_SHIFT)

#define MI_NDMA_RCYCLE_MASK (REG_MI_NDMAGCNT_CPUCYCLE_MASK)
#define MI_NDMA_RCYCLE_0 (0UL << REG_MI_NDMAGCNT_CPUCYCLE_SHIFT)
#define MI_NDMA_RCYCLE_1 (1UL << REG_MI_NDMAGCNT_CPUCYCLE_SHIFT)
#define MI_NDMA_RCYCLE_2 (2UL << REG_MI_NDMAGCNT_CPUCYCLE_SHIFT)
#define MI_NDMA_RCYCLE_4 (3UL << REG_MI_NDMAGCNT_CPUCYCLE_SHIFT)
#define MI_NDMA_RCYCLE_8 (4UL << REG_MI_NDMAGCNT_CPUCYCLE_SHIFT)
#define MI_NDMA_RCYCLE_16 (5UL << REG_MI_NDMAGCNT_CPUCYCLE_SHIFT)
#define MI_NDMA_RCYCLE_32 (6UL << REG_MI_NDMAGCNT_CPUCYCLE_SHIFT)
#define MI_NDMA_RCYCLE_64 (7UL << REG_MI_NDMAGCNT_CPUCYCLE_SHIFT)
#define MI_NDMA_RCYCLE_128 (8UL << REG_MI_NDMAGCNT_CPUCYCLE_SHIFT)
#define MI_NDMA_RCYCLE_256 (9UL << REG_MI_NDMAGCNT_CPUCYCLE_SHIFT)
#define MI_NDMA_RCYCLE_512 (10UL << REG_MI_NDMAGCNT_CPUCYCLE_SHIFT)
#define MI_NDMA_RCYCLE_1024 (11UL << REG_MI_NDMAGCNT_CPUCYCLE_SHIFT)
#define MI_NDMA_RCYCLE_2048 (12UL << REG_MI_NDMAGCNT_CPUCYCLE_SHIFT)
#define MI_NDMA_RCYCLE_4096 (13UL << REG_MI_NDMAGCNT_CPUCYCLE_SHIFT)
#define MI_NDMA_RCYCLE_8192 (14UL << REG_MI_NDMAGCNT_CPUCYCLE_SHIFT)
#define MI_NDMA_RCYCLE_16384 (15UL << REG_MI_NDMAGCNT_CPUCYCLE_SHIFT)
#ifdef SDK_ARM9
#define MI_NDMA_RCYCLE_DEFAULT MI_NDMA_RCYCLE_32
#else // SDK_ARM7
#define MI_NDMA_RCYCLE_DEFAULT MI_NDMA_RCYCLE_16
#endif // SDK_ARM7

#define MI_NDMA_ENABLE_MASK (1UL << REG_MI_NDMA0CNT_E_SHIFT) // DMA enable mask
#define MI_NDMA_ENABLE (1UL << REG_MI_NDMA0CNT_E_SHIFT)      // DMA enable
#define MI_NDMA_DISABLE (0UL << REG_MI_NDMA0CNT_E_SHIFT)     // DMA disable

#define MI_NDMA_IF_ENABLE (1UL << REG_MI_NDMA0CNT_I_SHIFT)  // interrupt enable
#define MI_NDMA_IF_DISABLE (0UL << REG_MI_NDMA0CNT_I_SHIFT) // interrupt disable

#define MI_NDMA_CONTINUOUS_ON                                                  \
  (1UL << REG_MI_NDMA0CNT_CM_SHIFT) // continuous mode on
#define MI_NDMA_CONTINUOUS_OFF                                                 \
  (0UL << REG_MI_NDMA0CNT_CM_SHIFT) // continuous mode off

#define MI_NDMA_IMM_MODE_ON                                                    \
  (1UL << REG_MI_NDMA0CNT_IM_SHIFT) // immidiate mode on
#define MI_NDMA_IMM_MODE_OFF                                                   \
  (0UL << REG_MI_NDMA0CNT_IM_SHIFT) // immidiate mode on

#define MI_NDMA_TIMING_MASK (REG_MI_NDMA0CNT_MODE_MASK)
#define MI_NDMA_TIMING_SHIFT (REG_MI_NDMA0CNT_MODE_SHIFT)
#define MI_NDMA_TIMING_TIMER0                                                  \
  (0UL << REG_MI_NDMA0CNT_MODE_SHIFT) // start by timer0
#define MI_NDMA_TIMING_TIMER1                                                  \
  (1UL << REG_MI_NDMA0CNT_MODE_SHIFT) // start by timer1
#define MI_NDMA_TIMING_TIMER2                                                  \
  (2UL << REG_MI_NDMA0CNT_MODE_SHIFT) // start by timer2
#define MI_NDMA_TIMING_TIMER3                                                  \
  (3UL << REG_MI_NDMA0CNT_MODE_SHIFT) // start by timer3
#define MI_NDMA_TIMING_CARD                                                    \
  (4UL << REG_MI_NDMA0CNT_MODE_SHIFT) // start by card A
#define MI_NDMA_TIMING_CARD_A MI_NDMA_TIMING_CARD
#define MI_NDMA_TIMING_V_BLANK                                                 \
  (6UL << REG_MI_NDMA0CNT_MODE_SHIFT) // start by VBlank
#if defined(SDK_ARM9) || defined(SDK_PORT)
#define MI_NDMA_TIMING_H_BLANK                                                 \
  (7UL << REG_MI_NDMA0CNT_MODE_SHIFT) // start by HBlank
#define MI_NDMA_TIMING_DISP                                                    \
  (8UL << REG_MI_NDMA0CNT_MODE_SHIFT) // start by display sync
#define MI_NDMA_TIMING_DISP_MMEM                                               \
  (9UL << REG_MI_NDMA0CNT_MODE_SHIFT) // start by main memory display sync
#define MI_NDMA_TIMING_GXFIFO                                                  \
  (10UL << REG_MI_NDMA0CNT_MODE_SHIFT) // start by geometry FIFO
#define MI_NDMA_TIMING_CAMERA                                                  \
  (11UL << REG_MI_NDMA0CNT_MODE_SHIFT) // start by camera
#else                                  // SDK_ARM7
#define MI_NDMA_TIMING_WIRELESS (7UL << REG_MI_NDMA0CNT_MODE_SHIFT) // start by
#define MI_NDMA_TIMING_SD_1                                                    \
  (8UL << REG_MI_NDMA0CNT_MODE_SHIFT) // start by SD I/F 1
#define MI_NDMA_TIMING_SD_2                                                    \
  (9UL << REG_MI_NDMA0CNT_MODE_SHIFT) // start by SD I/F 2
#define MI_NDMA_TIMING_AES_IN                                                  \
  (10UL << REG_MI_NDMA0CNT_MODE_SHIFT) // start by AES in
#define MI_NDMA_TIMING_AES_OUT                                                 \
  (11UL << REG_MI_NDMA0CNT_MODE_SHIFT) // start by AES out
#define MI_NDMA_TIMING_MIC (11UL << REG_MI_NDMA0CNT_MODE_SHIFT) // start by mic
#endif                                                          // SDK_ARM7
#define MIi_NDMA_TIMING_IMMIDIATE 0xffffffff // for internal use

#define MI_NDMA_BWORD_MASK (REG_MI_NDMA0CNT_WORDCNT_MASK)
#define MI_NDMA_BWORD_1 (0UL << REG_MI_NDMA0CNT_WORDCNT_SHIFT)   //     1 word
#define MI_NDMA_BWORD_2 (1UL << REG_MI_NDMA0CNT_WORDCNT_SHIFT)   //     2 words
#define MI_NDMA_BWORD_4 (2UL << REG_MI_NDMA0CNT_WORDCNT_SHIFT)   //     4 words
#define MI_NDMA_BWORD_8 (3UL << REG_MI_NDMA0CNT_WORDCNT_SHIFT)   //     8 words
#define MI_NDMA_BWORD_16 (4UL << REG_MI_NDMA0CNT_WORDCNT_SHIFT)  //    16 words
#define MI_NDMA_BWORD_32 (5UL << REG_MI_NDMA0CNT_WORDCNT_SHIFT)  //    32 words
#define MI_NDMA_BWORD_64 (6UL << REG_MI_NDMA0CNT_WORDCNT_SHIFT)  //    64 words
#define MI_NDMA_BWORD_128 (7UL << REG_MI_NDMA0CNT_WORDCNT_SHIFT) //   128 words
#define MI_NDMA_BWORD_256 (8UL << REG_MI_NDMA0CNT_WORDCNT_SHIFT) //   256 words
#define MI_NDMA_BWORD_512 (9UL << REG_MI_NDMA0CNT_WORDCNT_SHIFT) //   512 words
#define MI_NDMA_BWORD_1024                                                     \
  (10UL << REG_MI_NDMA0CNT_WORDCNT_SHIFT) //  1024 words
#define MI_NDMA_BWORD_2048                                                     \
  (11UL << REG_MI_NDMA0CNT_WORDCNT_SHIFT) //  2048 words
#define MI_NDMA_BWORD_4096                                                     \
  (12UL << REG_MI_NDMA0CNT_WORDCNT_SHIFT) //  4096 words
#define MI_NDMA_BWORD_8192                                                     \
  (13UL << REG_MI_NDMA0CNT_WORDCNT_SHIFT) //  8192 words
#define MI_NDMA_BWORD_16384                                                    \
  (14UL << REG_MI_NDMA0CNT_WORDCNT_SHIFT) // 16384 words
#define MI_NDMA_BWORD_32768                                                    \
  (15UL << REG_MI_NDMA0CNT_WORDCNT_SHIFT) // 32768 words

#define MI_NDMA_SRC_RELOAD_ENABLE                                              \
  (1UL                                                                         \
   << REG_MI_NDMA0CNT_SRL_SHIFT) // reload after transferring specified words
#define MI_NDMA_SRC_RELOAD_DISABLE                                             \
  (0UL << REG_MI_NDMA0CNT_SRL_SHIFT) // not reload

#define MI_NDMA_SRC_INC (0UL << REG_MI_NDMA0CNT_SAR_SHIFT) // increment
#define MI_NDMA_SRC_DEC (1UL << REG_MI_NDMA0CNT_SAR_SHIFT) // decrement
#define MI_NDMA_SRC_FIX (2UL << REG_MI_NDMA0CNT_SAR_SHIFT) // fix
#define MI_NDMA_SRC_FILLDATA                                                   \
  (3UL << REG_MI_NDMA0CNT_SAR_SHIFT) // no address (= use fill data)

#define MI_NDMA_DEST_RELOAD_ENABLE                                             \
  (1UL                                                                         \
   << REG_MI_NDMA0CNT_DRL_SHIFT) // reload after transferring specified words
#define MI_NDMA_DEST_RELOAD_DISABLE                                            \
  (0UL << REG_MI_NDMA0CNT_DRL_SHIFT) // not reload

#define MI_NDMA_DEST_INC (0UL << REG_MI_NDMA0CNT_DAR_SHIFT) // increment
#define MI_NDMA_DEST_DEC (1UL << REG_MI_NDMA0CNT_DAR_SHIFT) // decrement
#define MI_NDMA_DEST_FIX (2UL << REG_MI_NDMA0CNT_DAR_SHIFT) // fix

#define MI_NDMA_INTERVAL_PS_1                                                  \
  (0UL << REG_MI_NDMA0BCNT_PS_SHIFT) // system clock      (33.514MHz)
#define MI_NDMA_INTERVAL_PS_4                                                  \
  (1UL << REG_MI_NDMA0BCNT_PS_SHIFT) // system clock x  4
#define MI_NDMA_INTERVAL_PS_16                                                 \
  (2UL << REG_MI_NDMA0BCNT_PS_SHIFT) // system clock x 16
#define MI_NDMA_INTERVAL_PS_64                                                 \
  (3UL << REG_MI_NDMA0BCNT_PS_SHIFT) // system clock x 64

#define MI_NDMA_NUM_WOFFSET(ndmaNo) (ndmaNo * 7)
#define MI_NDMA_REG_SAD_WOFFSET 0
#define MI_NDMA_REG_DAD_WOFFSET 1
#define MI_NDMA_REG_TCNT_WOFFSET 2
#define MI_NDMA_REG_WCNT_WOFFSET 3
#define MI_NDMA_REG_BCNT_WOFFSET 4
#define MI_NDMA_REG_FDATA_WOFFSET 5
#define MI_NDMA_REG_CNT_WOFFSET 6

#define MI_NDMA_REGADDR(ndmaNo, reg)                                           \
  (((vu32 *)REG_NDMA0SAD_ADDR) + MI_NDMA_NUM_WOFFSET(ndmaNo) + reg)
#define MI_NDMA_REG(ndmaNo, reg) (*(MI_NDMA_REGADDR(ndmaNo, reg)))

#define MI_NDMA_NO_INTERVAL 0
#define MI_NDMA_AT_A_TIME 0xffffffff

typedef struct _MINDmaConfig {
  u32 intervalTimer;
  u32 prescaler;
  u32 blockWord;
  u32 wordCount;
} MINDmaConfig;

typedef void (*MINDmaCallback)(void *);

typedef u32 MINDmaDevice;

void MI_InitNDma(void);

void MI_NDmaFill(u32 ndmaNo, void *dest, u32 data, u32 size);
void MI_NDmaFill_SetUp(u32 ndmaNo, void *dest, u32 data, u32 size);
void MI_NDmaCopy(u32 ndmaNo, const void *src, void *dest, u32 size);
void MI_NDmaCopy_SetUp(u32 ndmaNo, const void *src, void *dest, u32 size);
void MI_NDmaSend(u32 ndmaNo, const void *src, volatile void *dest, u32 size);
void MI_NDmaSend_SetUp(u32 ndmaNo, const void *src, volatile void *dest,
                       u32 size);
void MI_NDmaRecv(u32 ndmaNo, volatile const void *src, void *dest, u32 size);
void MI_NDmaRecv_SetUp(u32 ndmaNo, volatile const void *src, void *dest,
                       u32 size);
void MI_NDmaPipe(u32 ndmaNo, volatile const void *src, volatile void *dest,
                 u32 size);
void MI_NDmaPipe_SetUp(u32 ndmaNo, volatile const void *src,
                       volatile void *dest, u32 size);
#define MI_NDmaClear(ndmaNo, dest, size)                                       \
  MI_NDmaFill((ndmaNo), (dest), 0, (size))
#define MI_NDmaClear_SetUp(ndmaNo, dest, size)                                 \
  MI_NDmaFill_SetUp((ndmaNo), (dest), 0, (size))

void MI_NDmaFillAsync(u32 ndmaNo, void *dest, u32 data, u32 size,
                      MINDmaCallback callback, void *arg);
void MI_NDmaFillAsync_SetUp(u32 ndmaNo, void *dest, u32 data, u32 size,
                            MINDmaCallback callback, void *arg);
void MI_NDmaCopyAsync(u32 ndmaNo, const void *src, void *dest, u32 size,
                      MINDmaCallback callback, void *arg);
void MI_NDmaCopyAsync_SetUp(u32 ndmaNo, const void *src, void *dest, u32 size,
                            MINDmaCallback callback, void *arg);
void MI_NDmaSendAsync(u32 ndmaNo, const void *src, volatile void *dest,
                      u32 size, MINDmaCallback callback, void *arg);
void MI_NDmaSendAsync_SetUp(u32 ndmaNo, const void *src, volatile void *dest,
                            u32 size, MINDmaCallback callback, void *arg);
void MI_NDmaRecvAsync(u32 ndmaNo, volatile const void *src, void *dest,
                      u32 size, MINDmaCallback callback, void *arg);
void MI_NDmaRecvAsync_SetUp(u32 ndmaNo, volatile const void *src, void *dest,
                            u32 size, MINDmaCallback callback, void *arg);
void MI_NDmaPipeAsync(u32 ndmaNo, volatile const void *src, volatile void *dest,
                      u32 size, MINDmaCallback callback, void *arg);
void MI_NDmaPipeAsync_SetUp(u32 ndmaNo, volatile const void *src,
                            volatile void *dest, u32 size,
                            MINDmaCallback callback, void *arg);
#define MI_NDmaClearAsync(ndmaNo, dest, size, callback, arg)                   \
  MI_NDmaFillAsync((ndmaNo), (dest), 0, (size), (callback), (arg))
#define MI_NDmaClearAsync_SetUp(ndmaNo, dest, size, callback, arg)             \
  MI_NDmaFillAsync_SetUp((ndmaNo), (dest), 0, (size), (callback), (arg))

void MI_NDmaFillEx(u32 ndmaNo, void *dest, u32 data, u32 size,
                   const MINDmaConfig *config);
void MI_NDmaFillEx_SetUp(u32 ndmaNo, void *dest, u32 data, u32 size,
                         const MINDmaConfig *config);
void MI_NDmaCopyEx(u32 ndmaNo, const void *src, void *dest, u32 size,
                   const MINDmaConfig *config);
void MI_NDmaCopyEx_SetUp(u32 ndmaNo, const void *src, void *dest, u32 size,
                         const MINDmaConfig *config);
void MI_NDmaSendEx(u32 ndmaNo, const void *src, volatile void *dest, u32 size,
                   const MINDmaConfig *config);
void MI_NDmaSendEx_SetUp(u32 ndmaNo, const void *src, volatile void *dest,
                         u32 size, const MINDmaConfig *config);
void MI_NDmaRecvEx(u32 ndmaNo, volatile const void *src, void *dest, u32 size,
                   const MINDmaConfig *config);
void MI_NDmaRecvEx_SetUp(u32 ndmaNo, volatile const void *src, void *dest,
                         u32 size, const MINDmaConfig *config);
void MI_NDmaPipeEx(u32 ndmaNo, volatile const void *src, volatile void *dest,
                   u32 size, const MINDmaConfig *config);
void MI_NDmaPipeEx_SetUp(u32 ndmaNo, volatile const void *src,
                         volatile void *dest, u32 size,
                         const MINDmaConfig *config);
#define MI_NDmaClearEx(ndmaNo, dest, size, config)                             \
  MI_NDmaFillEx((ndmaNo), (dest), 0, (size), (config))
#define MI_NDmaClearEx_SetUp(ndmaNo, dest, size, config)                       \
  MI_NDmaFillEx_SetUp((ndmaNo), (dest), 0, (size), (config))

void MI_NDmaFillExAsync(u32 ndmaNo, void *dest, u32 data, u32 size,
                        MINDmaCallback callback, void *arg,
                        const MINDmaConfig *config);
void MI_NDmaFillExAsync_SetUp(u32 ndmaNo, void *dest, u32 data, u32 size,
                              MINDmaCallback callback, void *arg,
                              const MINDmaConfig *config);
void MI_NDmaCopyExAsync(u32 ndmaNo, const void *src, void *dest, u32 size,
                        MINDmaCallback callback, void *arg,
                        const MINDmaConfig *config);
void MI_NDmaCopyExAsync_SetUp(u32 ndmaNo, const void *src, void *dest, u32 size,
                              MINDmaCallback callback, void *arg,
                              const MINDmaConfig *config);
void MI_NDmaSendExAsync(u32 ndmaNo, const void *src, volatile void *dest,
                        u32 size, MINDmaCallback callback, void *arg,
                        const MINDmaConfig *config);
void MI_NDmaSendExAsync_SetUp(u32 ndmaNo, const void *src, volatile void *dest,
                              u32 size, MINDmaCallback callback, void *arg,
                              const MINDmaConfig *config);
void MI_NDmaRecvExAsync(u32 ndmaNo, volatile const void *src, void *dest,
                        u32 size, MINDmaCallback callback, void *arg,
                        const MINDmaConfig *config);
void MI_NDmaRecvExAsync_SetUp(u32 ndmaNo, volatile const void *src, void *dest,
                              u32 size, MINDmaCallback callback, void *arg,
                              const MINDmaConfig *config);
void MI_NDmaPipeExAsync(u32 ndmaNo, volatile const void *src,
                        volatile void *dest, u32 size, MINDmaCallback callback,
                        void *arg, const MINDmaConfig *config);
void MI_NDmaPipeExAsync_SetUp(u32 ndmaNo, volatile const void *src,
                              volatile void *dest, u32 size,
                              MINDmaCallback callback, void *arg,
                              const MINDmaConfig *config);
#define MI_NDmaClearExAsync(ndmaNo, dest, size, callback, arg, config)         \
  MI_NDmaFillExAsync((ndmaNo), (dest), 0, (size), (callback), (arg), (config))
#define MI_NDmaClearExAsync_SetUp(ndmaNo, dest, size, callback, arg, config)   \
  MI_NDmaFillExAsync_SetUp((ndmaNo), (dest), 0, (size), (callback), (arg),     \
                           (config))

void MI_NDmaFill_Dev(u32 ndmaNo, void *dest, u32 data, u32 size,
                     MINDmaDevice dev);
void MI_NDmaFill_Dev_SetUp(u32 ndmaNo, void *dest, u32 data, u32 size,
                           MINDmaDevice dev);
void MI_NDmaCopy_Dev(u32 ndmaNo, const void *src, void *dest, u32 size,
                     MINDmaDevice dev);
void MI_NDmaCopy_Dev_SetUp(u32 ndmaNo, const void *src, void *dest, u32 size,
                           MINDmaDevice dev);
void MI_NDmaSend_Dev(u32 ndmaNo, const void *src, volatile void *dest, u32 size,
                     MINDmaDevice dev);
void MI_NDmaSend_Dev_SetUp(u32 ndmaNo, const void *src, volatile void *dest,
                           u32 size, MINDmaDevice dev);
void MI_NDmaRecv_Dev(u32 ndmaNo, volatile const void *src, void *dest, u32 size,
                     MINDmaDevice dev);
void MI_NDmaRecv_Dev_SetUp(u32 ndmaNo, volatile const void *src, void *dest,
                           u32 size, MINDmaDevice dev);
void MI_NDmaPipe_Dev(u32 ndmaNo, volatile const void *src, volatile void *dest,
                     u32 size, MINDmaDevice dev);
void MI_NDmaPipe_Dev_SetUp(u32 ndmaNo, volatile const void *src,
                           volatile void *dest, u32 size, MINDmaDevice dev);
#define MI_NDmaClear_Dev(ndmaNo, dest, size, dev)                              \
  MI_NDmaFill_Dev((ndmaNo), (dest), 0, (size), (dev))
#define MI_NDmaClear_Dev_SetUp(ndmaNo, dest, size, dev)                        \
  MI_NDmaFill_Dev_SetUp((ndmaNo), (dest), 0, (size), (dev))

void MI_NDmaFillAsync_Dev(u32 ndmaNo, void *dest, u32 data, u32 size,
                          MINDmaCallback callback, void *arg, MINDmaDevice dev);
void MI_NDmaFillAsync_Dev_SetUp(u32 ndmaNo, void *dest, u32 data, u32 size,
                                MINDmaCallback callback, void *arg,
                                MINDmaDevice dev);
void MI_NDmaCopyAsync_Dev(u32 ndmaNo, const void *src, void *dest, u32 size,
                          MINDmaCallback callback, void *arg, MINDmaDevice dev);
void MI_NDmaCopyAsync_Dev_SetUp(u32 ndmaNo, const void *src, void *dest,
                                u32 size, MINDmaCallback callback, void *arg,
                                MINDmaDevice dev);
void MI_NDmaSendAsync_Dev(u32 ndmaNo, const void *src, volatile void *dest,
                          u32 size, MINDmaCallback callback, void *arg,
                          MINDmaDevice dev);
void MI_NDmaSendAsync_Dev_SetUp(u32 ndmaNo, const void *src,
                                volatile void *dest, u32 size,
                                MINDmaCallback callback, void *arg,
                                MINDmaDevice dev);
void MI_NDmaRecvAsync_Dev(u32 ndmaNo, volatile const void *src, void *dest,
                          u32 size, MINDmaCallback callback, void *arg,
                          MINDmaDevice dev);
void MI_NDmaRecvAsync_Dev_SetUp(u32 ndmaNo, volatile const void *src,
                                void *dest, u32 size, MINDmaCallback callback,
                                void *arg, MINDmaDevice dev);
void MI_NDmaPipeAsync_Dev(u32 ndmaNo, volatile const void *src,
                          volatile void *dest, u32 size,
                          MINDmaCallback callback, void *arg, MINDmaDevice dev);
void MI_NDmaPipeAsync_Dev_SetUp(u32 ndmaNo, volatile const void *src,
                                volatile void *dest, u32 size,
                                MINDmaCallback callback, void *arg,
                                MINDmaDevice dev);
#define MI_NDmaClearAsync_Dev(ndmaNo, dest, size, callback, arg, dev)          \
  MI_NDmaFillAsync_Dev((ndmaNo), (dest), 0, (size), (callback), (arg), (dev))
#define MI_NDmaClearAsync_Dev_SetUp(ndmaNo, dest, size, callback, arg, dev)    \
  MI_NDmaFillAsync_Dev_SetUp((ndmaNo), (dest), 0, (size), (callback), (arg),   \
                             (dev))

void MI_NDmaFillEx_Dev(u32 ndmaNo, void *dest, u32 data, u32 size,
                       const MINDmaConfig *config, MINDmaDevice dev);
void MI_NDmaFillEx_Dev_SetUp(u32 ndmaNo, void *dest, u32 data, u32 size,
                             const MINDmaConfig *config, MINDmaDevice dev);
void MI_NDmaCopyEx_Dev(u32 ndmaNo, const void *src, void *dest, u32 size,
                       const MINDmaConfig *config, MINDmaDevice dev);
void MI_NDmaCopyEx_Dev_SetUp(u32 ndmaNo, const void *src, void *dest, u32 size,
                             const MINDmaConfig *config, MINDmaDevice dev);
void MI_NDmaSendEx_Dev(u32 ndmaNo, const void *src, volatile void *dest,
                       u32 size, const MINDmaConfig *config, MINDmaDevice dev);
void MI_NDmaSendEx_Dev_SetUp(u32 ndmaNo, const void *src, volatile void *dest,
                             u32 size, const MINDmaConfig *config,
                             MINDmaDevice dev);
void MI_NDmaRecvEx_Dev(u32 ndmaNo, volatile const void *src, void *dest,
                       u32 size, const MINDmaConfig *config, MINDmaDevice dev);
void MI_NDmaRecvEx_Dev_SetUp(u32 ndmaNo, volatile const void *src, void *dest,
                             u32 size, const MINDmaConfig *config,
                             MINDmaDevice dev);
void MI_NDmaPipeEx_Dev(u32 ndmaNo, volatile const void *src,
                       volatile void *dest, u32 size,
                       const MINDmaConfig *config, MINDmaDevice dev);
void MI_NDmaPipeEx_Dev_SetUp(u32 ndmaNo, volatile const void *src,
                             volatile void *dest, u32 size,
                             const MINDmaConfig *config, MINDmaDevice dev);
#define MI_NDmaClearEx_Dev(ndmaNo, dest, size, config, dev)                    \
  MI_NDmaFillEx_Dev((ndmaNo), (dest), 0, (size), (config), (dev))
#define MI_NDmaClearEx_Dev_SetUp(ndmaNo, dest, size, config, dev)              \
  MI_NDmaFillEx_Dev_SetUp((ndmaNo), (dest), 0, (size), (config), (dev))

void MI_NDmaFillExAsync_Dev(u32 ndmaNo, void *dest, u32 data, u32 size,
                            MINDmaCallback callback, void *arg,
                            const MINDmaConfig *config, MINDmaDevice dev);
void MI_NDmaFillExAsync_Dev_SetUp(u32 ndmaNo, void *dest, u32 data, u32 size,
                                  MINDmaCallback callback, void *arg,
                                  const MINDmaConfig *config, MINDmaDevice dev);
void MI_NDmaCopyExAsync_Dev(u32 ndmaNo, const void *src, void *dest, u32 size,
                            MINDmaCallback callback, void *arg,
                            const MINDmaConfig *config, MINDmaDevice dev);
void MI_NDmaCopyExAsync_Dev_SetUp(u32 ndmaNo, const void *src, void *dest,
                                  u32 size, MINDmaCallback callback, void *arg,
                                  const MINDmaConfig *config, MINDmaDevice dev);
void MI_NDmaSendExAsync_Dev(u32 ndmaNo, const void *src, volatile void *dest,
                            u32 size, MINDmaCallback callback, void *arg,
                            const MINDmaConfig *config, MINDmaDevice dev);
void MI_NDmaSendExAsync_Dev_SetUp(u32 ndmaNo, const void *src,
                                  volatile void *dest, u32 size,
                                  MINDmaCallback callback, void *arg,
                                  const MINDmaConfig *config, MINDmaDevice dev);
void MI_NDmaRecvExAsync_Dev(u32 ndmaNo, volatile const void *src, void *dest,
                            u32 size, MINDmaCallback callback, void *arg,
                            const MINDmaConfig *config, MINDmaDevice dev);
void MI_NDmaRecvExAsync_Dev_SetUp(u32 ndmaNo, volatile const void *src,
                                  void *dest, u32 size, MINDmaCallback callback,
                                  void *arg, const MINDmaConfig *config,
                                  MINDmaDevice dev);
void MI_NDmaPipeExAsync_Dev(u32 ndmaNo, volatile const void *src,
                            volatile void *dest, u32 size,
                            MINDmaCallback callback, void *arg,
                            const MINDmaConfig *config, MINDmaDevice dev);
void MI_NDmaPipeExAsync_Dev_SetUp(u32 ndmaNo, volatile const void *src,
                                  volatile void *dest, u32 size,
                                  MINDmaCallback callback, void *arg,
                                  const MINDmaConfig *config, MINDmaDevice dev);
#define MI_NDmaClearExAsync_Dev(ndmaNo, dest, size, callback, arg, config,     \
                                dev)                                           \
  MI_NDmaFillExAsync_Dev((ndmaNo), (dest), 0, (size), (callback), (arg),       \
                         (config), (dev))
#define MI_NDmaClearExAsync_Dev_SetUp(ndmaNo, dest, size, callback, arg,       \
                                      config, dev)                             \
  MI_NDmaFillExAsync_Dev_SetUp((ndmaNo), (dest), 0, (size), (callback), (arg), \
                               (config), (dev))

BOOL MI_IsNDmaBusy(u32 ndmaNo);

void MI_WaitNDma(u32 ndmaNo);

void MI_StopNDma(u32 ndmaNo);

void MI_StopAllNDma(void);

void MI_NDmaRestart(u32 ndmaNo);

void MI_SetNDmaArbitrament(u32 mode, u32 cycle);

u32 MI_GetNDmaArbitramentMode(void);

u32 MI_GetNDmaArbitramentRoundRobinCycle(void);

void MI_SetNDmaInterval(u32 ndmaNo, u32 intervalTime, u32 prescaler);

u32 MI_GetNDmaIntervalTimer(u32 ndmaNo);

u32 MI_GetNDmaIntervalPrescaler(u32 ndmaNo);

void MI_SetNDmaBlockWord(u32 ndmaNo, u32 word);

u32 MI_GetNDmaBlockWord(u32 ndmaNo);

void MI_SetNDmaWordCount(u32 ndmaNo, u32 wordCount);

u32 MI_GetNDmaWordCount(u32 ndmaNo);

void MI_InitNDmaConfig(void);

void MI_GetNDmaConfig(u32 ndmaNo, MINDmaConfig *config);

void MI_SetNDmaConfig(u32 ndmaNo, const MINDmaConfig *config);

void MI_TimerNDmaCopy(u32 ndmaNo, u32 timerNo, const void *src, void *dest,
                      u32 size);

#ifdef SDK_ARM9
void MI_HBlankNDmaCopy(u32 ndmaNo, const void *src, void *dest, u32 size);
void MI_HBlankNDmaCopyIf(u32 ndmaNo, const void *src, void *dest, u32 size);
#endif

void MI_VBlankNDmaCopy(u32 dmaNo, const void *src, void *dest, u32 size);
void MI_VBlankNDmaCopyAsync(u32 dmaNo, const void *src, void *dest, u32 size,
                            MINDmaCallback callback, void *arg);

void MI_Card_NDmaCopy(u32 dmaNo, const void *src, void *dest, u32 size);
#define MI_Card_A_NDmaCopy MI_Card_NDmaCopy

#if defined(SDK_ARM9) || defined(SDK_PORT)
void MI_SendNDmaGXCommand(u32 dmaNo, const void *src, u32 commandLength);
void MI_SendNDmaGXCommandAsync(u32 dmaNo, const void *src, u32 commandLength,
                               MINDmaCallback callback, void *arg);
void MI_SendNDmaGXCommandFast(u32 ndmaNo, const void *src, u32 commandLength);
void MI_SendNDmaGXCommandAsyncFast(u32 ndmaNo, const void *src,
                                   u32 commandLength, MINDmaCallback callback,
                                   void *arg);
#endif

#if defined(SDK_ARM9) || defined(SDK_PORT)
void MI_Camera_NDmaRecv(u32 ndmaNo, void *dest, u32 unit, u32 size,
                        BOOL contSw);
void MI_Camera_NDmaRecvEx(u32 ndmaNo, void *dest, u32 size, BOOL contSw,
                          const MINDmaConfig *config);
void MI_Camera_NDmaRecvAsync(u32 ndmaNo, void *dest, u32 unit, u32 size,
                             BOOL contSw, MINDmaCallback callback, void *arg);
void MI_Camera_NDmaRecvAsyncEx(u32 ndmaNo, void *dest, u32 size, BOOL contSw,
                               MINDmaCallback callback, void *arg,
                               const MINDmaConfig *config);
#endif

#ifndef SDK_ARM9
void MIi_Aes_NDmaSend(u32 ndmaNo, const void *src, u32 size,
                      MINDmaCallback callback, void *arg,
                      const MINDmaConfig *pConfig);
void MIi_Aes_NDmaRecv(u32 ndmaNo, void *dst, u32 size, MINDmaCallback callback,
                      void *arg, const MINDmaConfig *pConfig);
#endif

void MIi_SetNDmaSrc(u32 ndmaNo, void *src);
void MIi_SetNDmaDest(u32 ndmaNo, void *dest);
void MIi_SetNDmaTotalWordCount(u32 ndmaNo, u32 size);
void MIi_SetNDmaWordCount(u32 ndmaNo, u32 size);
void MIi_SetNDmaInterval(u32 ndmaNo, u32 intervalTimer, u32 prescaler);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

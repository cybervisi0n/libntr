#ifndef TWL_OS_INTERRUPT_H_
#define TWL_OS_INTERRUPT_H_

#ifdef SDK_TWL

#ifdef __cplusplus
extern "C" {
#endif

#include <twl/misc.h>
#include <twl/types.h>
#include <twl/memorymap.h>
#include <twl/ioreg.h>
#include <nitro/os/common/thread.h>

void OSi_EnterDmaCallback(u32 dmaNo, void (*callback)(void *), void *arg);
void OSi_EnterNDmaCallback(u32 dmaNo, void (*callback)(void *), void *arg);
void OSi_EnterTimerCallback(u32 timerNo, void (*callback)(void *), void *arg);

#define OS_IME_DISABLE (0UL << REG_OS_IME_IME_SHIFT)
#define OS_IME_ENABLE (1UL << REG_OS_IME_IME_SHIFT)

#define OS_IE_V_BLANK (1UL << REG_OS_IE_VB_SHIFT) //  0 VBlank
#define OS_IE_H_BLANK (1UL << REG_OS_IE_HB_SHIFT) //  1 HBlank
#define OS_IE_V_COUNT (1UL << REG_OS_IE_VE_SHIFT) //  2 VCounter
#define OS_IE_TIMER0 (1UL << REG_OS_IE_T0_SHIFT)  //  3 timer0
#define OS_IE_TIMER1 (1UL << REG_OS_IE_T1_SHIFT)  //  4 timer1
#define OS_IE_TIMER2 (1UL << REG_OS_IE_T2_SHIFT)  //  5 timer2
#define OS_IE_TIMER3 (1UL << REG_OS_IE_T3_SHIFT)  //  6 timer3
#define OS_IE_SIO (1UL << 7) //  7 serial communication (will not occur)
#define OS_IE_DMA0 (1UL << REG_OS_IE_D0_SHIFT)       //  8 DMA0
#define OS_IE_DMA1 (1UL << REG_OS_IE_D1_SHIFT)       //  9 DMA1
#define OS_IE_DMA2 (1UL << REG_OS_IE_D2_SHIFT)       // 10 DMA2
#define OS_IE_DMA3 (1UL << REG_OS_IE_D3_SHIFT)       // 11 DMA3
#define OS_IE_KEY (1UL << REG_OS_IE_K_SHIFT)         // 12 key
#define OS_IE_CARTRIDGE (1UL << REG_OS_IE_I_D_SHIFT) // 13 cartridge
#define OS_IE_CARD_A_DET                                                       \
  (1UL << REG_OS_IE_MCADET_SHIFT) // 14 memory card(A) MC_DET low
#define OS_IE_CARD_B_DET                                                       \
  (1UL << REG_OS_IE_MCBDET_SHIFT)              // 15 memory card(B) MC_DET low
#define OS_IE_SUBP (1UL << REG_OS_IE_A7_SHIFT) // 16 sub processor
#define OS_IE_SPFIFO_SEND                                                      \
  (1UL << REG_OS_IE_IFE_SHIFT) // 17 sub processor send FIFO empty
#define OS_IE_SPFIFO_RECV                                                      \
  (1UL << REG_OS_IE_IFN_SHIFT) // 18 sub processor receive FIFO not empty
#define OS_IE_CARD_DATA                                                        \
  (1UL << REG_OS_IE_MC_SHIFT) // 19 card data transfer finish
#define OS_IE_CARD_IREQ (1UL << REG_OS_IE_MI_SHIFT) // 20 card IREQ
#define OS_IE_CARD_A_DATA                                                      \
  (1UL << REG_OS_IE_MCA_SHIFT) // 19(dup.) card(A) data transfer finish
#define OS_IE_CARD_A_IREQ (1UL << REG_OS_IE_MIA_SHIFT) // 20(dup.) card(A) IREQ

#if defined(SDK_ARM9) || defined(SDK_PORT)
#define OS_IE_GXFIFO (1UL << REG_OS_IE_GF_SHIFT) // 21 geometry command FIFO
#define OS_IE_DEBUG_RECV                                                       \
  (1UL << REG_OS_IE_DRE_SHIFT) // 22 read debug channel RECV buffer
#define OS_IE_DEBUG_SEND                                                       \
  (1UL << REG_OS_IE_DWE_SHIFT) // 23 write debug channel SEND buffer
#define OS_IE_DSP (1UL << REG_OS_IE_DSP_SHIFT)    // 24 DSP
#define OS_IE_CAMERA (1UL << REG_OS_IE_CAM_SHIFT) // 25 camera
#define OS_IE_CARD_B_DATA                                                      \
  (1UL << REG_OS_IE_MCB_SHIFT) // 26 card(B) data transfer finished
#define OS_IE_CARD_B_IREQ (1UL << REG_OS_IE_MIB_SHIFT) // 27 card(B) IREQ
#define OS_IE_NDMA0 (1UL << REG_OS_IE_ND0_SHIFT)       // 28 new DMA0
#define OS_IE_NDMA1 (1UL << REG_OS_IE_ND1_SHIFT)       // 29 new DMA1
#define OS_IE_NDMA2 (1UL << REG_OS_IE_ND2_SHIFT)       // 30 new DMA2
#define OS_IE_NDMA3 (1UL << REG_OS_IE_ND3_SHIFT)       // 31 new DMA3
#else                                                  // SDK_ARM7
#define OS_IE_DEBUG_RECV                                                       \
  (1UL << REG_OS_IE_DRE_SHIFT) // 21 read debug channel RECV buffer
#define OS_IE_POWERMAN (1UL << REG_OS_IE_PM_SHIFT) // 22 Power Management IC
#define OS_IE_SPI (1UL << REG_OS_IE_SPI_SHIFT)     // 23 SPI data transfer
#define OS_IE_WIRELESS (1UL << REG_OS_IE_WL_SHIFT) // 24 Wireless module
#define OS_IE_DEBUG_SEND                                                       \
  (1UL << REG_OS_IE_DWE_SHIFT) // 25 write debug channel SEND buffer
#define OS_IE_CARD_B_DATA                                                      \
  (1UL << REG_OS_IE_MCB_SHIFT) // 26 card(B) data transfer finished
#define OS_IE_CARD_B_IREQ (1UL << REG_OS_IE_MIB_SHIFT) // 27 card(B) IREQ
#define OS_IE_NDMA0 (1UL << REG_OS_IE_ND0_SHIFT)       // 28 new DMA0
#define OS_IE_NDMA1 (1UL << REG_OS_IE_ND1_SHIFT)       // 29 new DMA1
#define OS_IE_NDMA2 (1UL << REG_OS_IE_ND2_SHIFT)       // 30 new DMA2
#define OS_IE_NDMA3 (1UL << REG_OS_IE_ND3_SHIFT)       // 31 new DMA3
#endif

#ifdef SDK_ARM7

#define OS_IE_GPIO18_0 (1UL << REG_OS_IE2_GPIO18_0_SHIFT) //  0 GPIO18[0]
#define OS_IE_GPIO18_1 (1UL << REG_OS_IE2_GPIO18_1_SHIFT) //  1 GPIO18[1]
#define OS_IE_GPIO18_2 (1UL << REG_OS_IE2_GPIO18_2_SHIFT) //  2 GPIO18[2]
#define OS_IE_GPIO33_0 (1UL << REG_OS_IE2_GPIO33_0_SHIFT) //  4 GPIO33[0]
#define OS_IE_GPIO33_1 (1UL << REG_OS_IE2_GPIO33_1_SHIFT) //  5 GPIO33[1]
#define OS_IE_GPIO33_2 (1UL << REG_OS_IE2_GPIO33_2_SHIFT) //  6 GPIO33[2]
#define OS_IE_GPIO33_3 (1UL << REG_OS_IE2_GPIO33_3_SHIFT) //  7 GPIO33[3]
#define OS_IE_SD1 (1UL << REG_OS_IE2_SD1_SHIFT)           //  8 SD card 1
#define OS_IE_SD1ASYNC (1UL << REG_OS_IE2_SD1A_SHIFT) //  9 SDIO card 1 async
#define OS_IE_SD2 (1UL << REG_OS_IE2_SD2_SHIFT)       // 10 SD card 2
#define OS_IE_SD2ASYNC (1UL << REG_OS_IE2_SD2A_SHIFT) // 11 SDIO card 2 async
#define OS_IE_AES (1UL << REG_OS_IE2_AES_SHIFT)       // 12 AES process finished
#define OS_IE_I2C (1UL << REG_OS_IE2_I2C_SHIFT) // 13 I2C transfer finished
#define OS_IE_MIC (1UL << REG_OS_IE2_MIC_SHIFT) // 14 MIC

#define OS_IE_MAINP OS_IE_SUBP
#define OS_IE_MPFIFO_SEND OS_IE_SPFIFO_SEND
#define OS_IE_MPFIFO_RECV OS_IE_SPFIFO_RECV
#define REG_OS_IE_A9_SHIFT REG_OS_IE_A7_SHIFT
#endif // SDK_ARM7

#define OS_IE_FIFO_SEND OS_IE_SPFIFO_SEND
#define OS_IE_FIFO_RECV OS_IE_SPFIFO_RECV

#ifdef SDK_ARM9
#define OS_IRQ_TABLE_MAX 32
#else
#define OS_IRQ_TABLE_MAX 32
#define OS_IRQ_TABLE2_MAX 15
#endif

#ifdef SDK_ARM9
#define OS_IE_MASK_ALL ((OSIrqMask)((1 << OS_IRQ_TABLE_MAX) - 1))
#else
#define OS_IE_MASK_ALL ((OSIrqMask)((1 << OS_IRQ_TABLE_MAX) - 1))
#define OS_IE2_MASK_ALL ((OSIrqMask)((1 << OS_IRQ_TABLE2_MAX) - 1))
#endif

#define OS_IE_USER_FLAG0_SHIFT 31
#define OS_IE_USER_FLAG1_SHIFT 30
#define OS_IE_USER_FLAG0 (1UL << OS_IE_USER_FLAG0_SHIFT) // user 0
#define OS_IE_USER_FLAG1 (1UL << OS_IE_USER_FLAG1_SHIFT) // user 1

#define OSi_IRQCALLBACK_NO_DMA0 0
#define OSi_IRQCALLBACK_NO_DMA1 1
#define OSi_IRQCALLBACK_NO_DMA2 2
#define OSi_IRQCALLBACK_NO_DMA3 3
#define OSi_IRQCALLBACK_NO_NDMA0 4
#define OSi_IRQCALLBACK_NO_NDMA1 5
#define OSi_IRQCALLBACK_NO_NDMA2 6
#define OSi_IRQCALLBACK_NO_NDMA3 7
#define OSi_IRQCALLBACK_NO_TIMER0 8
#define OSi_IRQCALLBACK_NO_TIMER1 9
#define OSi_IRQCALLBACK_NO_TIMER2 10
#define OSi_IRQCALLBACK_NO_TIMER3 11
#ifdef SDK_ARM7
#define OSi_IRQCALLBACK_NO_VBLANK 12
#endif
#ifdef SDK_ARM9
#define OSi_IRQCALLBACK_NUM (11 + 1)
#else
#define OSi_IRQCALLBACK_NUM (12 + 1)
#endif

#define OS_IRQ_MAIN_BUFFER_SIZE (0x200)

typedef void (*OSIrqFunction)(void);

typedef struct {
  void (*func)(void *);
  u32 enable;
  void *arg;
} OSIrqCallbackInfo;

#ifndef OSi_OSIRQMASK_DEFINED
typedef u32 OSIrqMask;
#define OSi_OSIRQMASK_DEFINED
#endif

extern OSIrqFunction OS_IRQTable[];
#ifdef SDK_ARM7
extern OSIrqFunction OS_IRQTable2[];
#endif

extern OSIrqCallbackInfo OSi_IrqCallbackInfo[OSi_IRQCALLBACK_NUM];

void OS_IrqDummy(void);
void OS_IrqHandler(void);
#ifdef SDK_BUILD_ARM
asm
#endif
    void OS_BreakIrqHandler(void);
#ifdef SDK_BUILD_ARM
asm
#endif
    void OS_IrqHandler_ThreadSwitch(void);

static inline BOOL OS_EnableIrq(void) {
  #ifdef SDK_PORT
  return TRUE;
  #else
  u16 prep = reg_OS_IME;
  reg_OS_IME = OS_IME_ENABLE;
  return (BOOL)prep;
  #endif
}

static inline BOOL OS_DisableIrq(void) {
  u16 prep = reg_OS_IME;
  reg_OS_IME = OS_IME_DISABLE;
  return (BOOL)prep;
}

static inline BOOL OS_RestoreIrq(BOOL enable) {
  u16 prep = reg_OS_IME;
  reg_OS_IME = (u16)enable;
  return (BOOL)prep;
}

static inline BOOL OS_GetIrq(void) { return (BOOL)reg_OS_IME; }

OSIrqMask OS_SetIrqMask(OSIrqMask intr);
#ifdef SDK_ARM7
OSIrqMask OS_SetIrqMaskEx(OSIrqMask intr);
#endif

static inline OSIrqMask OS_GetIrqMask(void) { return reg_OS_IE; }

OSIrqMask OS_EnableIrqMask(OSIrqMask intr);
#ifdef SDK_ARM7
OSIrqMask OS_EnableIrqMaskEx(OSIrqMask intr);
#endif

OSIrqMask OS_DisableIrqMask(OSIrqMask intr);
#ifdef SDK_ARM7
OSIrqMask OS_DisableIrqMaskEx(OSIrqMask intr);
#endif

OSIrqMask OS_ResetRequestIrqMask(OSIrqMask intr);
#ifdef SDK_ARM7
OSIrqMask OS_ResetRequestIrqMaskEx(OSIrqMask intr);
#endif

static inline OSIrqMask OS_GetRequestIrqMask(void) { return reg_OS_IF; }

void OS_InitIrqTable(void);

void OS_SetIrqFunction(OSIrqMask intrBit, OSIrqFunction function);
#ifdef SDK_ARM7
void OS_SetIrqFunctionEx(OSIrqMask intrBit, OSIrqFunction function);
#endif

OSIrqFunction OS_GetIrqFunction(OSIrqMask intrBit);
#ifdef SDK_ARM7
OSIrqFunction OS_GetIrqFunctionEx(OSIrqMask intrBit);
#endif

static inline void OS_SetIrqCheckFlag(OSIrqMask intr) {
  *(vu32 *)HW_INTR_CHECK_BUF |= (u32)intr;
}
#ifdef SDK_ARM7
static inline void OS_SetIrqCheckFlagEx(OSIrqMask intr) {
  *(vu32 *)HW_INTR_CHECK2_BUF |= (u32)intr;
}
#endif

static inline void OS_ClearIrqCheckFlag(OSIrqMask intr) {
  *(vu32 *)HW_INTR_CHECK_BUF &= (u32)~intr;
}
#ifdef SDK_ARM7
static inline void OS_ClearIrqCheckFlagEx(OSIrqMask intr) {
  *(vu32 *)HW_INTR_CHECK2_BUF &= (u32)~intr;
}
#endif

static inline volatile OSIrqMask OS_GetIrqCheckFlag(void) {
  return *(volatile OSIrqMask *)HW_INTR_CHECK_BUF;
}
#ifdef SDK_ARM7
static inline volatile OSIrqMask OS_GetIrqCheckFlagEx(void) {
  return *(volatile OSIrqMask *)HW_INTR_CHECK2_BUF;
}
#endif

void OS_WaitIrq(BOOL clear, OSIrqMask irqFlags);
#ifdef SDK_ARM7
void OS_WaitIrqEx(BOOL clear, OSIrqMask irqFlags);
#endif

void OS_WaitAnyIrq(void);
#ifdef SDK_ARM7
void OS_WaitAnyIrqEx(void);
#endif

static inline vu32 OS_GetVBlankCount(void) {
  return *(vu32 *)HW_VBLANK_COUNT_BUF;
}

static inline void OSi_SetVBlankCount(u32 count) {
  *(u32 *)HW_VBLANK_COUNT_BUF = count;
}

extern void OS_SetIrqStackWarningOffset(u32 offset);

extern OSStackStatus OS_GetIrqStackStatus(void);

extern void OS_SetIrqStackChecker(void);

void OSi_CheckIrqStack(char *file, int line);
#if !defined(SDK_FINALROM) && !defined(SDK_NO_MESSAGE)
#define OS_CheckIrqStack() OSi_CheckIrqStack(__FILE__, __LINE__);
#else
#define OS_CheckIrqStack() ((void)0)
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

#endif

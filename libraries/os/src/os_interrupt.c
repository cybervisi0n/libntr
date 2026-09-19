#if SDK_VERSION_MAJOR == 4
#include <nitro.h>
#elif SDK_VERSION_MAJOR == 5
#include <nitro/hw/common/armArch.h>

#ifdef SDK_NITRO
#include <nitro/os/common/interrupt.h>
#else
#include <twl/os/common/interrupt.h>
#endif

#include <nitro/os/common/system.h>
#endif

extern OSThreadQueue OSi_IrqThreadQueue;

void OS_InitIrqTable (void)
{
    OS_InitThreadQueue(&OSi_IrqThreadQueue);

#ifdef SDK_ARM7
    OSi_SetVBlankCount(0);
#endif
}

void OS_SetIrqFunction (OSIrqMask intrBit, OSIrqFunction function)
{
    int i;
    OSIrqCallbackInfo * info;

    for (i = 0; i < OS_IRQ_TABLE_MAX; i++) {
        if (intrBit & 1) {
            info = NULL;

            if (REG_OS_IE_D0_SHIFT <= i && i <= REG_OS_IE_D3_SHIFT) {
                info = &OSi_IrqCallbackInfo[i - REG_OS_IE_D0_SHIFT];
            } 
#if SDK_VERSION_MAJOR == 5
#ifdef SDK_TWL
      else if (REG_OS_IE_ND0_SHIFT <= i && i <= REG_OS_IE_ND3_SHIFT) {
        info = &OSi_IrqCallbackInfo[i - REG_OS_IE_ND0_SHIFT +
                                    OSi_IRQCALLBACK_NO_NDMA0];
      }
#endif
#endif
            else if (REG_OS_IE_T0_SHIFT <= i && i <= REG_OS_IE_T3_SHIFT)   {
                info = &OSi_IrqCallbackInfo[i - REG_OS_IE_T0_SHIFT + OSi_IRQCALLBACK_NO_TIMER0];
            }
#ifdef SDK_ARM7
            else if (REG_OS_IE_VB_SHIFT == i) {
                info = &OSi_IrqCallbackInfo[OSi_IRQCALLBACK_NO_VBLANK];
            }
#endif

            else {
                OS_IRQTable[i] = function;
            }

            if (info) {
                info->func = (void (*)(void *)) function;
                info->arg = 0;
                info->enable = TRUE;
            }

        }

        intrBit >>= 1;
    }
}

#if SDK_VERSION_MAJOR == 5
#if defined(SDK_TWL) && defined(SDK_ARM7)
void OS_SetIrqFunctionEx(OSIrqMask intrBit, OSIrqFunction function) {
  int i;
  OSIrqCallbackInfo *info = NULL;

  for (i = 0; i < OS_IRQ_TABLE2_MAX; i++) {
    if (intrBit & 1) {
      info = NULL;
      OS_IRQTable2[i] = function;

      if (info) {
        info->func = (void (*)(void *))function;
        info->arg = 0;
        info->enable = TRUE;
      }
    }
    intrBit >>= 1;
  }
}
#endif // defined(SDK_TWL) && defined(SDK_ARM7)
#endif

OSIrqFunction OS_GetIrqFunction (OSIrqMask intrBit)
{
    int i;
    OSIrqFunction * funcPtr = &OS_IRQTable[0];

    for (i = 0; i < OS_IRQ_TABLE_MAX; i++) {
        if (intrBit & 1) {

            if (REG_OS_IE_D0_SHIFT <= i && i <= REG_OS_IE_D3_SHIFT) {
                return (void (*)(void)) OSi_IrqCallbackInfo[i - REG_OS_IE_D0_SHIFT].func;
            } 
#if (SDK_VERSION_MAJOR == 5) && defined(SDK_TWL)

      else if (REG_OS_IE_ND0_SHIFT <= i && i <= REG_OS_IE_ND3_SHIFT) {
        return (void (*)(void))OSi_IrqCallbackInfo[i - REG_OS_IE_D0_SHIFT +
                                                   OSi_IRQCALLBACK_NO_NDMA0].func;
      }
#endif
            else if (REG_OS_IE_T0_SHIFT <= i && i <= REG_OS_IE_T3_SHIFT)   {
                return (void (*)(void)) OSi_IrqCallbackInfo[i - REG_OS_IE_T0_SHIFT +
                                                            OSi_IRQCALLBACK_NO_TIMER0].func;
            }
#ifdef SDK_ARM7
            else if (REG_OS_IE_VB_SHIFT == i) {
                return (void (*)(void)) OSi_IrqCallbackInfo[OSi_IRQCALLBACK_NO_VBLANK].func;
            }
#endif

            return *funcPtr;
        }

        intrBit >>= 1;
        funcPtr++;
    }

    return 0;
}

#if SDK_VERSION_MAJOR == 5
#if defined(SDK_TWL) && defined(SDK_ARM7)
OSIrqFunction OS_GetIrqFunctionEx(OSIrqMask intrBit) {
  int i;
  OSIrqFunction *funcPtr = &OS_IRQTable2[0]; // skip IE part

  for (i = 0; i < OS_IRQ_TABLE2_MAX; i++) {
    if (intrBit & 1) {
      return *funcPtr;
    }
    intrBit >>= 1;
    funcPtr++;
  }
  return 0;
}
#endif // defined(SDK_TWL) && defined(SDK_ARM7)
#endif

void OSi_EnterDmaCallback (u32 dmaNo, void (*callback)(void *), void * arg)
{
    OSIrqMask imask = (1UL << (REG_OS_IE_D0_SHIFT + dmaNo));

    OSi_IrqCallbackInfo[dmaNo].func = callback;
    OSi_IrqCallbackInfo[dmaNo].arg = arg;
    OSi_IrqCallbackInfo[dmaNo].enable = OS_EnableIrqMask(imask) & imask;
}

#if SDK_VERSION_MAJOR == 5
#ifdef SDK_TWL
#include <twl/ltdmain_begin.h>
static void OSi_EnterNDmaCallback_ltdmain(u32 dmaNo, void (*callback)(void *),
                                          void *arg);
static void OSi_EnterNDmaCallback_ltdmain(u32 dmaNo, void (*callback)(void *),
                                          void *arg) {
  OSIrqMask imask = (1UL << (REG_OS_IE_ND0_SHIFT + dmaNo));

  OSi_IrqCallbackInfo[dmaNo + OSi_IRQCALLBACK_NO_NDMA0].func = callback;
  OSi_IrqCallbackInfo[dmaNo + OSi_IRQCALLBACK_NO_NDMA0].arg = arg;

  OSi_IrqCallbackInfo[dmaNo + OSi_IRQCALLBACK_NO_NDMA0].enable =
      OS_EnableIrqMask(imask) & imask;
}
#include <twl/ltdmain_end.h>

void OSi_EnterNDmaCallback(u32 dmaNo, void (*callback)(void *), void *arg) {
  OSi_EnterNDmaCallback_ltdmain(dmaNo, callback, arg);
}
#endif
#endif

void OSi_EnterTimerCallback (u32 timerNo, void (*callback)(void *), void * arg)
{
    OSIrqMask imask = (1UL << (REG_OS_IE_T0_SHIFT + timerNo));

    OSi_IrqCallbackInfo[timerNo + 4].func = callback;
    OSi_IrqCallbackInfo[timerNo + 4].arg = arg;

    (void)OS_EnableIrqMask(imask);
    OSi_IrqCallbackInfo[timerNo + 4].enable = TRUE;
}

#if defined(SDK_TCM_APPLY) && defined(SDK_ARM9)
    #include <nitro/itcm_begin.h>
#endif

OSIrqMask OS_SetIrqMask (OSIrqMask intr)
{
    BOOL ime = OS_DisableIrq();
    OSIrqMask prep = reg_OS_IE;
    reg_OS_IE = intr;
    (void)OS_RestoreIrq(ime);
    return prep;
}

#if SDK_VERSION_MAJOR == 5
#if defined(SDK_TWL) && defined(SDK_ARM7)
OSIrqMask OS_SetIrqMaskEx(OSIrqMask intr) {
  BOOL ime = OS_DisableIrq(); // IME disable
  OSIrqMask prep = reg_OS_IE2;
  reg_OS_IE2 = (u16)intr;
  (void)OS_RestoreIrq(ime);
  return prep;
}
#endif
#endif

OSIrqMask OS_EnableIrqMask (OSIrqMask intr)
{
    BOOL ime = OS_DisableIrq();
    OSIrqMask prep = reg_OS_IE;
    reg_OS_IE = prep | intr;
    (void)OS_RestoreIrq(ime);
    return prep;
}

#if SDK_VERSION_MAJOR == 5
#if defined(SDK_TWL) && defined(SDK_ARM7)
OSIrqMask OS_EnableIrqMaskEx(OSIrqMask intr) {
  BOOL ime = OS_DisableIrq(); // IME disable
  OSIrqMask prep = reg_OS_IE2;
  reg_OS_IE2 = (u16)(prep | intr);
  (void)OS_RestoreIrq(ime);
  return prep;
}
#endif
#endif

OSIrqMask OS_DisableIrqMask (OSIrqMask intr)
{
    BOOL ime = OS_DisableIrq();
    OSIrqMask prep = reg_OS_IE;
    reg_OS_IE = prep & ~intr;
    (void)OS_RestoreIrq(ime);
    return prep;
}

#if SDK_VERSION_MAJOR == 5
#if defined(SDK_TWL) & defined(SDK_ARM7)
OSIrqMask OS_DisableIrqMaskEx(OSIrqMask intr) {
  BOOL ime = OS_DisableIrq(); // IME disable
  OSIrqMask prep = reg_OS_IE2;
  reg_OS_IE2 = (u16)(prep & ~intr);
  (void)OS_RestoreIrq(ime);
  return prep;
}
#endif
#endif

OSIrqMask OS_ResetRequestIrqMask (OSIrqMask intr)
{
    BOOL ime = OS_DisableIrq();
    OSIrqMask prep = reg_OS_IF;
    reg_OS_IF = intr;
    (void)OS_RestoreIrq(ime);
    return prep;
}

#if SDK_VERSION_MAJOR == 5
#if defined(SDK_TWL) && defined(SDK_ARM7)
OSIrqMask OS_ResetRequestIrqMaskEx(OSIrqMask intr) {
  BOOL ime = OS_DisableIrq(); // IME disable
  OSIrqMask prep = reg_OS_IF2;
  reg_OS_IF2 = (u16)intr;
  (void)OS_RestoreIrq(ime);
  return prep;
}
#endif
#endif

#if defined(SDK_TCM_APPLY) && defined(SDK_ARM9)
    #include <nitro/itcm_end.h>
#endif

extern void SDK_IRQ_STACKSIZE(void);
#if SDK_VERSION_MAJOR == 5
#define OSi_IRQ_STACKSIZE ((int)SDK_IRQ_STACKSIZE)
#endif

#if (defined(SDK_ARM9) || defined(SDK_PORT))
    #define  OSi_IRQ_STACK_TOP                (HW_DTCM_SVC_STACK - ((s32)SDK_IRQ_STACKSIZE))
    #define  OSi_IRQ_STACK_BOTTOM             HW_DTCM_SVC_STACK
#endif
#ifdef SDK_ARM7
    #define  OSi_IRQ_STACK_TOP                (HW_PRV_WRAM_IRQ_STACK_END - ((s32)SDK_IRQ_STACKSIZE))
    #define  OSi_IRQ_STACK_BOTTOM             HW_PRV_WRAM_IRQ_STACK_END
#endif

#if (defined(SDK_ARM9) || defined(SDK_PORT))
    #define  OSi_IRQ_STACK_CHECKNUM_BOTTOM     0xfddb597dUL
    #define  OSi_IRQ_STACK_CHECKNUM_TOP        0x7bf9dd5bUL
    #define  OSi_IRQ_STACK_CHECKNUM_WARN       0x597dfbd9UL
#else
    #define  OSi_IRQ_STACK_CHECKNUM_BOTTOM     0xd73bfdf7UL
    #define  OSi_IRQ_STACK_CHECKNUM_TOP        0xfbdd37bbUL
    #define  OSi_IRQ_STACK_CHECKNUM_WARN       0xbdf7db3dUL
#endif

static u32 OSi_IrqStackWarningOffset = 0;

void OS_SetIrqStackChecker (void)
{
    #ifndef SDK_PORT
    *(u32 *)(OSi_IRQ_STACK_BOTTOM - sizeof(u32)) = OSi_IRQ_STACK_CHECKNUM_BOTTOM;
    *(u32 *)(OSi_IRQ_STACK_TOP) = OSi_IRQ_STACK_CHECKNUM_TOP;
    #endif
}

void OS_SetIrqStackWarningOffset (u32 offset)
{
    SDK_ASSERTMSG((offset & 3) == 0, "Offset must be aligned by 4");
    SDK_ASSERTMSG(offset > 0, "Cannot set warning level to stack top.");
    SDK_ASSERTMSG(offset < ((u32)SDK_IRQ_STACKSIZE), "Cannot set warning level over stack bottom.");

    OSi_IrqStackWarningOffset = offset;

    if (offset != 0) {
        *(u32 *)(OSi_IRQ_STACK_TOP + offset) = OSi_IRQ_STACK_CHECKNUM_WARN;
    }
}

OSStackStatus OS_GetIrqStackStatus (void)
{

    if (*(u32 *)(OSi_IRQ_STACK_TOP) != OSi_IRQ_STACK_CHECKNUM_TOP) {
        return OS_STACK_OVERFLOW;
    } else if (OSi_IrqStackWarningOffset
               && *(u32 *)(OSi_IRQ_STACK_TOP + OSi_IrqStackWarningOffset) !=
               OSi_IRQ_STACK_CHECKNUM_WARN) {
        return OS_STACK_ABOUT_TO_OVERFLOW;
    } else if (*(u32 *)(OSi_IRQ_STACK_BOTTOM - sizeof(u32)) != OSi_IRQ_STACK_CHECKNUM_BOTTOM)   {
        return OS_STACK_UNDERFLOW;
    } else {
        return OS_STACK_NO_ERROR;
    }
}

static char * OSi_CheckIrqStack_mesg[] = {
    "overflow", "about to overflow", "underflow"
};

#ifndef SDK_FINALROM
    #ifndef SDK_NO_MESSAGE
        void OSi_CheckIrqStack (char * file, int line)
        {
            OSStackStatus st = OS_GetIrqStackStatus();

            if (st == OS_STACK_NO_ERROR) {
                return;
            }

            OSi_Panic(file, line, "irq stack %s.\nirq stack area: %08x-%08x, warning offset: %x",
                    OSi_CheckIrqStack_mesg[(int)st - 1],
                    OSi_IRQ_STACK_TOP, OSi_IRQ_STACK_BOTTOM, OSi_IrqStackWarningOffset);

        }
    #endif
#endif
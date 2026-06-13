#include <nitro/os.h>
#include <nitro/code32.h>

#ifdef SDK_PORT
OSIntrMode OS_EnableInterrupts( void )
{
    return 0;
}

OSIntrMode OS_DisableInterrupts( void )
{
    return 0;
}

OSProcMode OS_GetProcMode( void )
{
    return 0;
}

OSIntrMode OS_DisableInterrupts_IrqAndFiq( void )
{
    return 0;
}

OSIntrMode OS_RestoreInterrupts( OSIntrMode state )
{
    return 0;
}

OSIntrMode OS_EnableInterrupts_IrqAndFiq( void )
{
    return 0;
}

OSIntrMode OS_RestoreInterrupts_IrqAndFiq( OSIntrMode state )
{
    return 0;
}

OSIntrMode_Irq OS_GetCpsrIrq( void )
{
    return 0;
}
#else
asm OSIntrMode OS_EnableInterrupts (void)
{
    mrs r0, cpsr
    bic r1, r0, #HW_PSR_IRQ_DISABLE
    msr cpsr_c, r1
    and r0, r0, #HW_PSR_IRQ_DISABLE
    bx lr
}

asm OSIntrMode OS_DisableInterrupts (void)
{
    mrs r0, cpsr
    orr r1, r0, #HW_PSR_IRQ_DISABLE
    msr cpsr_c, r1
    and r0, r0, #HW_PSR_IRQ_DISABLE
    bx lr
}

asm OSIntrMode OS_RestoreInterrupts (register OSIntrMode state)
{
    mrs r1, cpsr
    bic r2, r1, #HW_PSR_IRQ_DISABLE
    orr r2, r2, r0
    msr cpsr_c, r2
    and r0, r1, #HW_PSR_IRQ_DISABLE
    bx lr
}

asm OSIntrMode OS_EnableInterrupts_IrqAndFiq (void)
{
    mrs r0, cpsr
    bic r1, r0, #HW_PSR_IRQ_FIQ_DISABLE
    msr cpsr_c, r1
    and r0, r0, #HW_PSR_IRQ_FIQ_DISABLE
    bx lr
}

asm OSIntrMode OS_DisableInterrupts_IrqAndFiq (void)
{
    mrs r0, cpsr
    orr r1, r0, #HW_PSR_IRQ_FIQ_DISABLE
    msr cpsr_c, r1
    and r0, r0, #HW_PSR_IRQ_FIQ_DISABLE
    bx lr
}

asm OSIntrMode OS_RestoreInterrupts_IrqAndFiq (register OSIntrMode state)
{
    mrs r1, cpsr
    bic r2, r1, #HW_PSR_IRQ_FIQ_DISABLE
    orr r2, r2, r0
    msr cpsr_c, r2
    and r0, r1, #HW_PSR_IRQ_FIQ_DISABLE
    bx lr
}

asm OSIntrMode_Irq OS_GetCpsrIrq (void)
{
    mrs r0, cpsr
    and r0, r0, #HW_PSR_IRQ_DISABLE
    bx lr
}

asm OSProcMode OS_GetProcMode (void)
{
    mrs r0, cpsr
    and r0, r0, #HW_PSR_CPU_MODE_MASK
    bx lr
}

#include <nitro/codereset.h>
#endif

#ifdef SDK_ARM9
    #include <nitro/code32.h>
    asm void OS_SpinWait (u32 cycle)
    {
    _1: subs r0, r0, #4
        bcs _1
        bx lr
    }
    #include <nitro/codereset.h>
#else
    void OS_SpinWait (u32 cycle)
    {
        SVC_WaitByLoop((s32)cycle / 4);
    }
#endif

void OS_WaitInterrupt (BOOL clear, OSIrqMask irqFlags)
{
    OSIntrMode cpsrIrq = OS_DisableInterrupts();
    BOOL ime = OS_EnableIrq();

    if (clear) {
        (void)OS_ClearIrqCheckFlag(irqFlags);
    }

    while (!(OS_GetIrqCheckFlag() & irqFlags)) {
        #ifdef SDK_BUILD_ARM
        OS_Halt();
        #endif
        (void)OS_EnableInterrupts();
        (void)OS_DisableInterrupts();
    }

    (void)OS_ClearIrqCheckFlag(irqFlags);
    (void)OS_RestoreIrq(ime);
    (void)OS_RestoreInterrupts(cpsrIrq);
}

void OS_WaitVBlankIntr (void)
{
    SVC_WaitByLoop(1);
#if defined(SDK_ENABLE_ARM7_PRINT) && defined(SDK_ARM9)

    OS_PrintServer();
#endif
    OS_WaitIrq(TRUE, OS_IE_V_BLANK);
}

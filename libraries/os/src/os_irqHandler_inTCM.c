#include <nitro/code32.h>
#include <nitro/types.h>
#include <nitro/os/common/interrupt.h>
#include <nitro/os/common/thread.h>
#include <nitro/os/common/systemCall.h>

#if defined(SDK_ARM9) || defined(SDK_PORT)
#ifdef SDK_PORT
#include <nitro/hw/X86/mmap_global.h>
#include <nitro/hw/X86/ioreg_OS.h>
#else
#include <nitro/hw/ARM9/mmap_global.h>
#include <nitro/hw/ARM9/ioreg_OS.h>
#endif
#else // SDK_ARM9
#include <nitro/hw/ARM7/mmap_global.h>
#include <nitro/hw/ARM7/ioreg_OS.h>
#endif // SDK_ARM9

OSThreadQueue OSi_IrqThreadQueue;

#ifdef SDK_PORT
void OS_IrqHandler(void) {}
void OS_IrqHandler_ThreadSwitch(void) {}
#else
asm void OS_IrqHandler(void) {
#ifdef SDK_NO_THREAD
#else
  stmfd   sp!, { lr }                     // Save LR
#endif

  mov r12, #HW_REG_BASE add r12, r12,
      #REG_IE_OFFSET // r12: REG_IE address

      ldr r1,
      [ r12, #REG_IME_ADDR - REG_IE_ADDR ] // r1: IME

      cmp r1,
      #0
#ifdef SDK_NO_THREAD
      bxeq lr
#else
        ldmeqfd sp!, { pc }
#endif

          ldmia r12,
      {r1 - r2} // r1: IE, r2: IF
  ands r1,
      r1,
      r2 // r1: IE & IF

#ifdef SDK_NO_THREAD
          bxeq lr
#else
        ldmeqfd sp!, { pc }
#endif

#if defined(SDK_ARM9) && !defined(SDK_CWBUG_PROC_OPT)

              mov r3,
      #1 << 31 @1 : clz r0,
      r1 // Count zero of high bit
          bics r1,
      r1, r3,
      LSR r0 bne @1

      mov r1,
      r3, LSR r0 str r1,
      [ r12, #REG_IF_ADDR - REG_IE_ADDR ]

      rsbs r0,
      r0,
      #31

#else  // defined(SDK_ARM9) && !defined(SDK_CWBUG_PROC_OPT)



        mov     r3, #1
        mov     r0, #0
@1:     ands    r2, r1, r3, LSL r0              // Count zero of high bit
        addeq   r0, r0, #1
        beq     @1


        str     r2, [ r12, #REG_IF_ADDR - REG_IE_ADDR ]
#endif // defined(SDK_ARM9) && !defined(SDK_CWBUG_PROC_OPT)

#ifdef SDK_DEBUG
      cmp r0,
      #OS_IRQ_TABLE_MAX @2 : bge @2 // Error Trap
#endif // SDK_DEBUG
                             ldr r1,
                             = OS_IRQTable ldr r0,
                             [ r1, r0, LSL #2 ]

#ifdef SDK_NO_THREAD
                             bx r0
#else  // SDK_NO_THREAD
        ldr     lr, =OS_IrqHandler_ThreadSwitch
        bx      r0      // Set return address for thread rescheduling
#endif // SDK_NO_THREAD
}

asm void OS_IrqHandler_ThreadSwitch(void) {

#ifdef SDK_NO_THREAD
#else

  ldr r12, = OSi_IrqThreadQueue
#if (OS_THREAD_MAX_NUM <= 16)
               ldrh r3,
      [r12] // r3 = OSi_IrqThreadQueue
      mov r0,
      #0 cmp r3,
      #0 beq @thread_switch // If r3 == 0 exit
          strh r0,
      [r12] // OSi_IrqThreadQueue = 0
#else  // OS_THREAD_MAX_NUM
               ldr r3,
      [r12] mov r0, #0 cmp r3, #0 beq @thread_switch str r0,
      [r12]
#endif // OS_THREAD_MAX_NUM

      ldr r12,
      = OSi_ThreadInfo // isNeedRescheduling=OS_THREADINFO_RESCHEDULING_DISABLE_LATER
          mov r1,
      #1 strh r1, [ r12, #OS_THREADINFO_OFFSET_ISNEEDRESCHEDULING ] ldr r12,
      [ r12, #OS_THREADINFO_OFFSET_LIST ] // r12 = OSi_ThreadInfo.list
      mov r2,
      #OS_THREAD_STATE_READY @1 : cmp r12,
      #0 beq @thread_switch ldr r0,
      [ r12, #OS_THREAD_OFFSET_ID ] tst r3,
      r1,
      LSL r0 // OSi_IrqThreadQueue & (1<<thread->id)
                                      strne r2,
      [ r12, #OS_THREAD_OFFSET_STATE ] ldr r12,
      [ r12, #OS_THREAD_OFFSET_NEXT ] b @1 @thread_switch :

      ldr r12,
      = OSi_ThreadInfo ldrh r1,
      [ r12, #OS_THREADINFO_OFFSET_ISNEEDRESCHEDULING ] cmp r1,
      #0 ldreq pc,
      [sp],
      #4 // Return if OSi_IsNeedResceduling == 0
      mov r1,
      #0 strh r1,
      [ r12, #OS_THREADINFO_OFFSET_ISNEEDRESCHEDULING ]

      mov r3,
      #HW_PSR_IRQ_MODE | HW_PSR_FIQ_DISABLE | HW_PSR_IRQ_DISABLE |
          HW_PSR_ARM_STATE msr cpsr_c,
      r3

          add r2,
      r12,
      #OS_THREADINFO_OFFSET_LIST // r2 = &OSi_ThreadInfo.list
      ldr r1,
      [r2] // r1 = *r2 = TopOfList
      @11 : cmp r1,
      #0 ldrneh r0,
      [ r1, #OS_THREAD_OFFSET_STATE ] // r0 = t->state
            cmpne r0,
      #OS_THREAD_STATE_READY ldrne r1,
      [ r1, #OS_THREAD_OFFSET_NEXT ] bne @11

            cmp r1,
      #0 bne @12

            _dont_switched_ : mov r3,
      #HW_PSR_IRQ_MODE | HW_PSR_IRQ_DISABLE | HW_PSR_ARM_STATE msr cpsr_c,
      r3 ldr pc,
      [sp],
      #4 // Return to irq master handler

                              @12 :

      ldr r0,
      [ r12, #OS_THREADINFO_OFFSET_CURRENT ] cmp r1,
      r0 beq _dont_switched_ // Return if no thread switching

          ldr r3,
      [ r12, #OS_THREADINFO_OFFSET_SWITCHCALLBACK ] cmp r3,
      #0 beq @13 // Skip calling callback when callback == 0
      stmfd sp !,
      {r0, r1, r12} mov lr,
      pc bx r3 ldmfd sp !,
  {
    r0, r1, r12
  }

  @13 :

      str r1,
      [ r12, #OS_THREADINFO_OFFSET_CURRENT ]

      mrs r2,
      SPSR str r2,
      [ r0, #OS_THREAD_OFFSET_CONTEXT ] ! // *r0=context:CPSR

#if defined(SDK_ARM9) && !defined(SDK_CP_NO_SAFE)

      stmfd sp !,
      {r0, r1} add r0,
      r0,
      #OS_THREAD_OFFSET_CONTEXT add r0,
      r0,
      #OS_CONTEXT_CP_CONTEXT ldr r1,
      = CP_SaveContext blx r1 ldmfd sp !,
      {r0, r1}
#endif

      ldmib sp !,
      {r2, r3} // Get R0,R1    // *sp=stack:R1
      stmib r0 !,
      {r2, r3} // Put R0,R1    // *r0=context:R1

      ldmib sp !,
      {r2, r3, r12, r14} // Get R2,R3,R12,LR / *sp=stack:LR
      stmib r0 !,
      {r2 - r14} ^ // Put R2-R14^  // *r0=context:R14
          stmib r0 !,
      {r14} // Put R14_irq  // *r0=context:R15+4
#ifdef SDK_CONTEXT_HAS_SP_SVC
      mov r3,
      #HW_PSR_SVC_MODE | HW_PSR_FIQ_DISABLE | HW_PSR_IRQ_DISABLE |
          HW_PSR_ARM_STATE msr cpsr_c,
      r3 stmib r0 !,
      {sp}
#endif

#if defined(SDK_ARM9) && !defined(SDK_CP_NO_SAFE)

      stmfd sp !,
      {r1} add r0,
      r1,
      #OS_THREAD_OFFSET_CONTEXT add r0,
      r0,
      #OS_CONTEXT_CP_CONTEXT ldr r1,
      = CP_RestoreContext blx r1

#if 0  // Don't need because we still spend more than 34 cycles for divider.

        ldr     r0, =REG_DIVCNT_ADDR
@00:
        ldr     r1, [ r0 ]
        and     r1, r1, #REG_CP_DIVCNT_BUSY_MASK
        bne     @00
#endif // If 0
          ldmfd sp !,
      {r1}
#endif // If defined(SDK_ARM9) && !defined(SDK_CP_NO_SAFE)

#ifdef SDK_CONTEXT_HAS_SP_SVC
      ldr sp,
      [ r1, #OS_THREAD_OFFSET_CONTEXT + OS_CONTEXT_SP_SVC ] mov r3,
      #HW_PSR_IRQ_MODE | HW_PSR_FIQ_DISABLE | HW_PSR_IRQ_DISABLE |
          HW_PSR_ARM_STATE msr cpsr_c,
      r3
#endif
          ldr r2,
      [ r1, #OS_THREAD_OFFSET_CONTEXT ] ! // *r1=context:CPSR
      msr SPSR,
      r2 // Put SPSR

          ldr r14,
      [ r1, #OS_CONTEXT_PC_PLUS4 - OS_CONTEXT_CPSR ] // Get R15
      ldmib r1 !,
      {r0 - r14} ^ // Get R0-R14^  // *r1=over written
          nop stmda sp !,
      {r0 - r3, r12, r14} // Put R0-R3,R12,LR / *sp=stack:LR
      ldmfd sp !,
  {
    pc
  } // Return to irq master handler
#endif
}
#endif

void OS_WaitIrq(BOOL clear, OSIrqMask irqFlags) {
#ifdef SDK_NO_THREAD
  OS_WaitInterrupt(clear, irqFlags);

#else
  OSIntrMode enabled = OS_DisableInterrupts();

  if (clear) {
    (void)OS_ClearIrqCheckFlag(irqFlags);
  }

  while (!(OS_GetIrqCheckFlag() & irqFlags)) {
    OS_SleepThread(&OSi_IrqThreadQueue);
  }

  (void)OS_RestoreInterrupts(enabled);
#endif // ifdef SDK_NO_THREAD
}

void OS_WaitAnyIrq(void) {
#ifdef SDK_NO_THREAD
  OS_Halt();
#else
  OS_SleepThread(&OSi_IrqThreadQueue);
#endif
}

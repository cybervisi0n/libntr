#include <nitro/os.h>
#include <nitro/mi/wram.h>
#include <nitro/pxi.h>
#if SDK_VERSION_MAJOR == 4
#include <nitro/spi.h>

#ifndef SDK_TEG
    #include <nitro/ctrdg.h>
#endif
#elif SDK_VERSION_MAJOR == 5
#include <nitro/ctrdg/common/ctrdg_common.h>
#include <nitro/card/common.h>
#if defined(SDK_ARM9) || defined(SDK_PORT)
#include <nitro/spi/ARM9/pm.h>
#else // SDK_ARM7
#include <nitro/spi/ARM7/pm.h>
#endif
#ifdef SDK_TWL
#include <application_jump_private.h>
#include <twl/scfg.h>
#endif
#endif

#include <nitro/code32.h>

#ifdef SDK_PORT
static void OSi_WaitVCount0( void )
{
    return;
}
#else
static asm void OSi_WaitVCount0 (void)
{
    mov r12, #HW_REG_BASE
    ldr r1, [r12, #REG_IME_OFFSET]
    str r12, [r12, #REG_IME_OFFSET]
@wait_vcount_0:
    ldrh r0, [r12, #REG_VCOUNT_OFFSET]
    cmp r0, #0
    bne @wait_vcount_0
    str r1, [r12, #REG_IME_OFFSET]
    bx lr
}
#endif

#include <nitro/codereset.h>

#if SDK_VERSION_MAJOR == 5
#if (defined(SDK_ARM9) || defined(SDK_PORT)) && defined(SDK_TWLLTD)
#include <os_attention.h>
#endif

void OSi_InitCommon(void);
#endif

#pragma profile off

void OS_Init (void)
{
#if SDK_VERSION_MAJOR == 4
#if defined(SDK_ARM9) || defined(SDK_PORT)
    SDK_ASSERT((u32) & (OS_GetSystemWork()->command_area) == HW_CMD_AREA);
#ifdef SDK_ENABLE_ARM7_PRINT
    OS_InitPrintServer();
#endif
    OS_InitArena();

    PXI_Init();

    OS_InitLock();
    OS_InitArenaEx();
    OS_InitIrqTable();
    OS_SetIrqStackChecker();
    OS_InitException();

    MI_Init();

    OS_InitVAlarm();
    OSi_InitVramExclusive();

#ifndef SDK_NO_THREAD
    OS_InitThread();
#endif

#ifndef SDK_SMALL_BUILD
    OS_InitReset();
#endif

#ifndef SDK_TEG
    CTRDG_Init();
#endif

#ifndef SDK_SMALL_BUILD
    CARD_Init();
#endif

#ifndef SDK_TEG
    PM_Init();
#endif

    OSi_WaitVCount0();

#else
    OS_InitArena();
    PXI_Init();
    OS_InitLock();
    OS_InitIrqTable();

#define SDK_EXCEPTION_BUG
#ifndef SDK_EXCEPTION_BUG
    OS_InitException();
#endif
    OS_InitTick();
    OS_InitAlarm();
    OS_InitThread();

#ifndef SDK_SMALL_BUILD
    OS_InitReset();
#endif

#ifndef SDK_TEG
    CTRDG_Init();
#endif

#endif
#elif SDK_VERSION_MAJOR == 5
#ifdef SDK_ARM9

  SDK_ASSERT((u32) & (OS_GetSystemWork()->command_area) == HW_CMD_AREA);
#endif // SDK_ARM9

  OSi_InitCommon();
#endif
}

#ifdef SDK_PORT
void OS_Init7(void)
{
    OS_InitArena7();
}
#endif

#pragma profile reset

#if SDK_VERSION_MAJOR == 5
#pragma profile off
void OSi_InitCommon(void) {
#if defined(SDK_ARM9) || defined(SDK_PORT)
#ifdef SDK_ENABLE_ARM7_PRINT
  OS_InitPrintServer();
#endif
  PXI_Init();
  OS_InitArena();
  OS_InitLock();
  OS_InitArenaEx();
  OS_InitIrqTable();
  OS_SetIrqStackChecker();
  OS_InitException();
  MI_Init();
  OS_InitVAlarm();
  OSi_InitVramExclusive();
#ifndef SDK_NO_THREAD
  OS_InitThread();
#endif
#ifndef SDK_SMALL_BUILD
  OS_InitReset();
#endif
  CTRDG_Init();
#ifndef SDK_SMALL_BUILD
  CARD_Init();
#endif
#ifdef SDK_TWL
  MI_InitWramManager();
#endif
#ifdef SDK_TWL
  if (OS_IsRunOnTwl() == TRUE) {
    SCFG_Init();
  }
#endif
  PM_Init();
  OSi_WaitVCount0();
#ifdef SDK_TWL
  if (OS_IsRunOnTwl() == TRUE) {
    OSi_InitPrevTitleId();
  }
#endif

#else // SDK_ARM9
  PXI_Init();
  OS_InitArena();
  OS_InitLock();
  OS_InitIrqTable();
  OS_InitException();
#ifdef SDK_TWL
  MI_Init();
#endif
  OS_InitTick();
  OS_InitAlarm();
  OS_InitThread();
#ifndef SDK_SMALL_BUILD
  OS_InitReset();
#endif

#ifndef SDK_TWLLTD
  CTRDG_Init();
#endif

#ifdef SDK_TWL
  MI_InitWramManager();
#endif

#ifdef SDK_TWL
  if (OS_IsRunOnTwl() == TRUE) {
    SCFG_Init();
  }
#endif

#endif // SDK_ARM9
}
#pragma profile reset
#endif

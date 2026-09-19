#include <nitro/os/common/system.h>
#if SDK_VERSION_MAJOR == 4
#ifdef SDK_ARM7
#include <nitro/ctrdg.h>
#endif
#elif SDK_VERSION_MAJOR == 5
#include <nitro/os/common/interrupt.h>
#include <nitro/os/common/emulator.h>
#include <nitro/os/common/pxi.h>
#include <nitro/os/common/reset.h>

#ifdef SDK_TWL
#include <twl/fatfs.h>
#endif
#endif

#if SDK_VERSION_MAJOR == 4
#ifdef SDK_PORT
void OS_Terminate (void) {

}

void OS_Halt() {

}

void OS_Exit (int status) {

}
#else

SDK_WEAK_SYMBOL void OS_Terminate (void)
{
#ifdef SDK_ARM7
    CTRDG_VibPulseEdgeUpdate(NULL);
#endif
    while (1) {
        (void)OS_DisableInterrupts();
        OS_Halt();
    }
}

#ifdef SDK_ARM9
#include <nitro/code32.h>

SDK_WEAK_SYMBOL asm void OS_Halt (void)
{
    mov r0, #0
    mcr p15, 0, r0, c7, c0, 4
    bx lr
}

#include <nitro/codereset.h>

SDK_WEAK_SYMBOL void OS_Exit (int status)
{
#ifdef SDK_FINALROM
#pragma unused(status)
#endif
    (void)OS_DisableInterrupts();
    OS_Printf("\n" OS_EXIT_STRING, status);
    OS_Terminate();
}
#endif
#endif
#else
OSTerminateCallback OSi_TerminateCallback = NULL;
void *OSi_TerminateCallbackArg = NULL;

SDK_WEAK_SYMBOL void OS_Terminate(void) {
#ifdef SDK_TWL
  static BOOL terminated = FALSE;
#endif

  if (OSi_TerminateCallback) {
    OSTerminateCallback callback = OSi_TerminateCallback;
    OSi_TerminateCallback = NULL;
    callback(OSi_TerminateCallbackArg);
  }

#ifdef SDK_TWL
  if (!terminated) {
    terminated = TRUE;

    if (OS_GetProcMode() != OS_PROCMODE_IRQ) {
      (void)OS_EnableIrq();
      (void)OS_EnableInterrupts();
    }

    if (OS_IsRunOnTwl()) {
      if (FATFSi_IsInitialized()) {
        (void)FATFS_UnmountAll();
      }
    }

    OSi_SendToPxi(OS_PXI_COMMAND_TERMINATE);
  }
#endif

  OSi_TerminateCore();
}

void OSi_TerminateCore(void) {
  (void)OS_DisableInterrupts();
  while (1) {
    OS_Halt();
  }
}

void OSi_SetTerminateCallback(OSTerminateCallback callback, void *arg) {
  OSi_TerminateCallback = callback;
  OSi_TerminateCallbackArg = arg;
}

#ifdef SDK_PORT

#else
#include <nitro/code32.h>
SDK_WEAK_SYMBOL asm void OS_Halt(void){mov r0, #0 mcr p15, 0,      r0,
                                       c7,     c0,         4 bx lr}
#include <nitro/codereset.h>
#endif

SDK_WEAK_SYMBOL void OS_Exit(int status) {
  OS_FExit(OS_PRINT_OUTPUT_ERROR, status);
}

SDK_WEAK_SYMBOL void OS_FExit(int console, int status) {
#ifdef SDK_FINALROM
#pragma unused(console, status)
#endif
  (void)OS_DisableInterrupts();
  OS_FPrintf(console, "\n" OS_EXIT_STRING, status);
  OS_Terminate();
}
#endif
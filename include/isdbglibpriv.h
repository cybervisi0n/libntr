#ifndef ISDEBUGGER_LIBRARY_PRIVATE_H
#define ISDEBUGGER_LIBRARY_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

void _ISDbgLib_Initialize(void);

void _ISDbgLib_AllocateEmualtor(void);
int _ISDbgLib_TryAllocateEmulator(void); // ret=TRUE/FALSE

void _ISDbgLib_FreeEmulator(void);

void _ISDbgLib_FreeEmulator(void);

typedef enum { OVERLAYPROC_ARM9, OVERLAYPROC_ARM7 } OVERLAYPROC;
int _ISDbgLib_RegistOverlayInfo(OVERLAYPROC nProc, u32 nAddrRAM, u32 nAddrROM,
                                u32 nSize);

int _ISDbgLib_UnregistOverlayInfo(OVERLAYPROC nProc, u32 nAddrRAM, u32 nSize);

int _ISDbgLib_IsDebuggerPresent(void);

void _ISDbgLib_OnLoadChildBinary(void);

#ifdef __cplusplus
}
#endif
#endif /* ISDEBUGGER_LIBRARY_PRIVATE_H */

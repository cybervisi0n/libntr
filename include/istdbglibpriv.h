#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define ISTDDBGLIB_VERSION 0x00000008

#define ISTDDBGLIB_RELEASE 2009052422

void _ISTDbgLib_Initialize(void);

typedef enum { ISTDOVERLAYPROC_ARM9, ISTDOVERLAYPROC_ARM7 } ISTDOVERLAYPROC;
BOOL _ISTDbgLib_RegistOverlayInfoByAddr(ISTDOVERLAYPROC nProc, u32 nAddrRAM,
                                        u32 nSlot, u32 nAddrROM, u32 nSize);
BOOL _ISTDbgLib_RegistOverlayInfoByFID(ISTDOVERLAYPROC nProc, u32 nAddrRAM,
                                       u32 nFileID);

BOOL _ISTDbgLib_UnregistOverlayInfoByAddr(ISTDOVERLAYPROC nProc, u32 nAddrRAM,
                                          u32 nSize);
BOOL _ISTDbgLib_UnregistOverlayInfoByFID(ISTDOVERLAYPROC nProc, u32 nAddrRAM,
                                         u32 nFileID);

void _ISTDbgLib_StartRegistRelocationInfo(ISTDOVERLAYPROC nProc);
BOOL _ISTDbgLib_RegistRelocationInfo(ISTDOVERLAYPROC nProc, u32 nFileID,
                                     u32 nELF, u32 nSection, u32 nLMA);
void _ISTDbgLib_EndRegistRelocationInfo(ISTDOVERLAYPROC nProc);

BOOL _ISTDbgLib_UnregistRelocationInfo(ISTDOVERLAYPROC nProc, u32 nFileID);

typedef enum {

  ISTDVENEERTYPE_ARM,

  ISTDVENEERTYPE_ARMV4T
} ISTDVENEERTYPE;
BOOL _ISTDbgLib_RegistVeneerInfo(ISTDOVERLAYPROC nProc, ISTDVENEERTYPE vType,
                                 u32 nVeneerAddress, u32 nJumpAddress);
BOOL _ISTDbgLib_UnregistVeneerInfo(ISTDOVERLAYPROC nProc, ISTDVENEERTYPE vType,
                                   u32 nVeneerAddress);
BOOL _ISTDbgLib_UnregistExVeneerInfo(ISTDOVERLAYPROC nProc, u32 nStartAddress,
                                     u32 nEndAddress);

BOOL _ISTDbgLib_IsDebuggerPresent(void);

void _ISTDbgLib_OnLoadChildBinary(void);

#ifdef SDK_ARM9

void _ISTDbgLib_OnBeforeResetHard();
#endif // SDK_ARM9

#ifdef __cplusplus
}
#endif

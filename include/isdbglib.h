#ifndef ISDEBUGGER_LIBRARY_H
#define ISDEBUGGER_LIBRARY_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NDEBUG
#define ISDPrint(ARG_)
#define ISDPrintf(ARG_...)
#define ISDPrintSetBlockingMode(ENABLE_)
#define ISDPrintGetBlockingMode() 0
#define ISDPrintGetBufferLength() 0x3FFF
#define ISDAssert(FILE_, LINE_, INFO_)
#else
void ISDPrint(const char *pBuf);
void ISDPrintf(const char *pBuf, ...);
void ISDPrintSetBlockingMode(int bBlocking);
int ISDPrintGetBlockingMode(void);
int ISDPrintGetWritableLength();
void ISDAssert(const char *pFile, int nLine, const char *pExpression,
               int nStopProgram);
#endif

#undef ISDASSERT
#ifdef NDEBUG
#define ISDASSERT(exp)
#else
#define ISDASSERT(exp)                                                         \
  (exp) ? ((void)0) : ISDAssert(__FILE__, __LINE__, #exp, 1);
#endif

#undef ISDWARNING
#ifdef NDEBUG
#define ISDWARNING(exp)
#else
#define ISDWARNING(exp)                                                        \
  (exp) ? ((void)0) : ISDAssert(__FILE__, __LINE__, #exp, 0);
#endif

#define NITROSTM_SENDMAX 8192

#define NITROMID_RECVMAX 2048

enum {
  NITRODEVID_NULL,
  NITRODEVID_CGBEMULATOR, // IS-CGB-EMULATOR
  NITRODEVID_NITEMULATOR, // NITRO-EMULATOR
  NITRODEVID_NITUIC,      // IS-NITRO-UIC CARTRIDGE
  NITRODEVID_AGBMIDI,     // IS-AGB-MIDI CARTRIDGE
  NITRODEVID_MAX
};

enum {
  NITRORESID_POLL = 0,
  NITRORESID_TIMER = 1,
  NITRORESID_VBLANK = 2,
  NITRORESID_CARTRIDGE = 3,
  NITRORESID_MAX
};

#define NITROMASK_RESOURCE_POLL (1 << NITRORESID_POLL)
#define NITROMASK_RESOURCE_TIMER (1 << NITRORESID_TIMER)
#define NITROMASK_RESOURCE_VBLANK (1 << NITRORESID_VBLANK)
#define NITROMASK_RESOURCE_CARTRIDGE (1 << NITRORESID_CARTRIDGE)

typedef struct tagNITRODEVCAPS {
  u32 m_nSizeStruct;    // Size of CAPS structure
  u32 m_nDeviceID;      // Device recognition ID
  u32 m_dwMaskResource; // Resources needed to operate this device
} NITRODEVCAPS;
typedef void (*NITROSTREAMCALLBACKFUNC)(u32 dwUserData, u32 nChnStream,
                                        const void *pRecv, u32 dwRecvSize);

void NITROToolAPIInit(void);

int NITROToolAPIGetMaxCaps(void);
const NITRODEVCAPS *NITROToolAPIGetDeviceCaps(int nCaps);

int NITROToolAPIOpen(const NITRODEVCAPS *pCaps);
int NITROToolAPIClose(void);

int NITROToolAPIStreamGetWritableLength(u32 *pnLength);
int NITROToolAPIWriteStream(u32 nChnStream, const void *pSrc, u32 dwSize);
int NITROToolAPISetReceiveStreamCallBackFunction(NITROSTREAMCALLBACKFUNC cbFunc,
                                                 u32 dwUserData);

void NITROToolAPIPollingIdle(void);        // For EMULATOR use
void NITROToolAPITimerInterrupt(void);     // Timer interrupt
void NITROToolAPIVBlankInterrupt(void);    // V-Blank interrupt
void NITROToolAPICartridgeInterrupt(void); // Cartridge (Game Pak) interrupt

typedef struct tagNITROMIDICAPS {
  u32 m_nSizeStruct;    // Size of CAPS structure
  u32 m_nDeviceID;      // Device recognition ID (common with the TOOLAPI)
  u32 m_nMaxFIFO;       // Receive buffer size
  u32 m_dwMaskResource; // Resources needed to operate this device (common with
                        // the TOOLAPI)
} NITROMIDICAPS;

void NITROMIDIAPIInit(void);

int NITROMIDIAPIGetMaxCaps(void);
const NITROMIDICAPS *NITROMIDIAPIGetDeviceCaps(int nCaps);

int NITROMIDIAPIOpen(const NITROMIDICAPS *pCaps);
int NITROMIDIAPIClose(void);

void NITROMIDIAPIReset(void);

int NITROMIDIAPIGetReadableLength(void);

int NITROMIDIAPIRead(void *pBuf, u32 nBufSize);

void NITROMIDIAPICartridgeInterrupt(void);

#ifdef __cplusplus
}
#endif
#endif /* ISDEBUGGER_LIBRARY_H */

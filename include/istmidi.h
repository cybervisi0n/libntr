#ifndef ISTWL_MIDI_LIB_H__
#define ISTWL_MIDI_LIB_H__
#ifdef __cplusplus
extern "C" {
#endif

#define ISTMIDLIB_VERSION 0x00000002

#define ISTMIDLIB_RELEASE 2009012211

enum {

  ISTMDEVID_TWLMIDI,
  ISTMDEVID_MAX
};

typedef struct tagTWLMIDICAPS {
  u32 m_nSizeStruct; // Size of CAPS structure
  u32 m_nDeviceID;   // Device ID (ISTMDEVID*)
  u32 m_nMaxFIFO; // Receive buffer size (ISTMID_GetReadableLength, ISTMID_Read)
  u32 m_dwOpts;
} ISTMIDCAPS;

void ISTMID_Initialize(void);

u32 ISTMID_GetMaxCaps(void);

const ISTMIDCAPS *ISTMID_GetDeviceCaps(u32 nCaps);

BOOL ISTMID_Open(const ISTMIDCAPS *pCaps);

BOOL ISTMID_Close(void);

BOOL ISTMID_Reset(void);

s32 ISTMID_GetReadableLength(void);

BOOL ISTMID_Read(void *p, u32 nLength);

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif // ISTWL_MIDI_LIB_H__

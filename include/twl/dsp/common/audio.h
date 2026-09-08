#ifndef TWLSDK_DSP_AUDIO_H__
#define TWLSDK_DSP_AUDIO_H__

#include <twl/dsp/common/byteaccess.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DSPAudioCodecCommand {
  DSPByte32 ctrl;
  DSPAddrInARM src;
  DSPAddrInARM dst;
  DSPByte32 len;
} DSPAudioCodecCommand;

typedef struct DSPAudioDriverCommand {
  DSPByte32 ctrl;
  DSPAddrInARM buf;
  DSPByte32 len;
  DSPByte32 opt;
} DSPAudioDriverCommand;

typedef struct DSPAudioDriverResponse {
  DSPByte32 ctrl;
  DSPByte32 result;
} DSPAudioDriverResponse;

typedef DSPByte32 DSPAudioCodecMode;
#define DSP_AUDIO_CODEC_TYPE_MASK (DSPAudioCodecMode)0x0F00
#define DSP_AUDIO_CODEC_MODE_MASK (DSPAudioCodecMode)0x00FF
#define DSP_AUDIO_CODEC_TYPE_ENCODE (DSPAudioCodecMode)0x0100
#define DSP_AUDIO_CODEC_TYPE_DECODE (DSPAudioCodecMode)0x0200
#define DSP_AUDIO_CODEC_MODE_G711_ALAW (DSPAudioCodecMode)0x0001
#define DSP_AUDIO_CODEC_MODE_G711_ULAW (DSPAudioCodecMode)0x0002

#define DSP_AUDIO_DRIVER_TARGET_MASK (DSPByte32)0xF000
#define DSP_AUDIO_DRIVER_CONTROL_MASK (DSPByte32)0x0F00
#define DSP_AUDIO_DRIVER_MODE_MASK (DSPByte32)0x00FF
#define DSP_AUDIO_DRIVER_TARGET_OUTPUT (DSPByte32)0x1000
#define DSP_AUDIO_DRIVER_TARGET_INPUT (DSPByte32)0x2000
#define DSP_AUDIO_DRIVER_TARGET_CACHE (DSPByte32)0x3000
#define DSP_AUDIO_DRIVER_CONTROL_START (DSPByte32)0x0100
#define DSP_AUDIO_DRIVER_CONTROL_STOP (DSPByte32)0x0200
#define DSP_AUDIO_DRIVER_CONTROL_LOAD (DSPByte32)0x0300
#define DSP_AUDIO_DRIVER_CONTROL_STORE (DSPByte32)0x0400
#define DSP_AUDIO_DRIVER_MODE_MONAURAL (DSPByte32)0x0000
#define DSP_AUDIO_DRIVER_MODE_STEREO (DSPByte32)0x0001
#define DSP_AUDIO_DRIVER_MODE_HALFVOL (DSPByte32)0x0002

#ifdef SDK_TWL

void DSPi_OpenStaticComponentAudioCore(FSFile *file);
BOOL DSPi_LoadAudioCore(FSFile *file, int slotB, int slotC);
void DSPi_UnloadAudioCore(void);

SDK_INLINE void DSP_OpenStaticComponentAudio(FSFile *file) {
  if (OS_IsRunOnTwl()) {
    DSPi_OpenStaticComponentAudioCore(file);
  }
}

SDK_INLINE BOOL DSP_LoadAudio(FSFile *file, int slotB, int slotC) {
  if (OS_IsRunOnTwl()) {
    return DSPi_LoadAudioCore(file, slotB, slotC);
  }
  return FALSE;
}

SDK_INLINE void DSP_UnloadAudio(void) {
  if (OS_IsRunOnTwl()) {
    DSPi_UnloadAudioCore();
  }
}

#else

#endif // SDK_TWL

#ifdef __cplusplus
}
#endif

#endif // TWLSDK_DSP_AUDIO_H__

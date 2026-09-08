#ifndef DSP_AUDIO_G711_H__
#define DSP_AUDIO_G711_H__

#include <twl/dsp/common/pipe.h>
#include <twl/dsp/common/audio.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SDK_TWL

void DSPi_OpenStaticComponentG711Core(FSFile *file);
BOOL DSPi_LoadG711Core(FSFile *file, int slotB, int slotC);
void DSPi_UnloadG711Core(void);
void DSPi_EncodeG711Core(void *dst, const void *src, u32 len,
                         DSPAudioCodecMode mode);
void DSPi_DecodeG711Core(void *dst, const void *src, u32 len,
                         DSPAudioCodecMode mode);
BOOL DSPi_TryWaitForG711Core(void);
void DSPi_WaitForG711Core(void);

SDK_INLINE void DSP_OpenStaticComponentG711(FSFile *file) {
  if (OS_IsRunOnTwl()) {
    DSPi_OpenStaticComponentG711Core(file);
  }
}

SDK_INLINE BOOL DSP_LoadG711(FSFile *file, int slotB, int slotC) {
  if (OS_IsRunOnTwl()) {
    return DSPi_LoadG711Core(file, slotB, slotC);
  }
  return FALSE;
}

SDK_INLINE void DSP_UnloadG711(void) {
  if (OS_IsRunOnTwl()) {
    DSPi_UnloadG711Core();
  }
}

SDK_INLINE void DSP_EncodeG711(void *dst, const void *src, u32 len,
                               DSPAudioCodecMode mode) {
  if (OS_IsRunOnTwl()) {
    DSPi_EncodeG711Core(dst, src, len, mode);
  }
}

SDK_INLINE void DSP_DecodeG711(void *dst, const void *src, u32 len,
                               DSPAudioCodecMode mode) {
  if (OS_IsRunOnTwl()) {
    DSPi_DecodeG711Core(dst, src, len, mode);
  }
}

SDK_INLINE BOOL DSP_TryWaitForG711(void) {
  if (OS_IsRunOnTwl()) {
    return DSPi_TryWaitForG711Core();
  }
  return FALSE;
}

SDK_INLINE void DSP_WaitForG711(void) {
  if (OS_IsRunOnTwl()) {
    DSPi_WaitForG711Core();
  }
}

#else

void DSP_EncodeG711(void *dst, const void *src, u32 len,
                    DSPAudioCodecMode mode);
void DSP_DecodeG711(void *dst, const void *src, u32 len,
                    DSPAudioCodecMode mode);

#endif // SDK_TWL

#ifdef __cplusplus
}
#endif

#endif // DSP_AUDIO_G711_H__

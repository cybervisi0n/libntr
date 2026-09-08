#ifndef TWL_DSP_UTIL_H_
#define TWL_DSP_UTIL_H_

#include <twl/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DSP_SLOT_B_COMPONENT_G711 1
#define DSP_SLOT_C_COMPONENT_G711 2

#define DSP_SLOT_B_COMPONENT_JPEGENCODER 3
#define DSP_SLOT_C_COMPONENT_JPEGENCODER 4

#define DSP_SLOT_B_COMPONENT_JPEGDECODER 2
#define DSP_SLOT_C_COMPONENT_JPEGDECODER 4

#define DSP_SLOT_B_COMPONENT_GRAPHICS 1
#define DSP_SLOT_C_COMPONENT_GRAPHICS 4

#define DSP_SLOT_B_COMPONENT_AACDECODER 2
#define DSP_SLOT_C_COMPONENT_AACDECODER 4

void DSPi_PlaySoundCore(const void *src, u32 len, BOOL stereo);
BOOL DSPi_PlayShutterSoundCore(const void *src, u32 len);
void DSPi_StopSoundCore(void);
BOOL DSPi_IsSoundPlayingCore(void);
BOOL DSPi_IsShutterSoundPlayingCore(void);
void DSPi_StartSamplingCore(void *buffer, u32 length);
void DSPi_StopSamplingCore(void);
void DSPi_SyncSamplingBufferCore(void);
const u8 *DSPi_GetLastSamplingAddressCore(void);

SDK_INLINE void DSP_PlaySound(const void *src, u32 len, BOOL stereo) {
  if (OS_IsRunOnTwl()) {
    DSPi_PlaySoundCore(src, len, stereo);
  }
}

SDK_INLINE BOOL DSP_PlayShutterSound(const void *src, u32 len) {
  if (OS_IsRunOnTwl()) {
    return DSPi_PlayShutterSoundCore(src, len);
  }
  return FALSE;
}

SDK_INLINE BOOL DSP_IsSoundPlaying(void) {
  BOOL retval = FALSE;
  if (OS_IsRunOnTwl()) {
    retval = DSPi_IsSoundPlayingCore();
  }
  return retval;
}

SDK_INLINE BOOL DSP_IsShutterSoundPlaying(void) {
  BOOL retval = FALSE;
  if (OS_IsRunOnTwl()) {
    retval = DSPi_IsShutterSoundPlayingCore();
  }
  return retval;
}

SDK_INLINE void DSP_StopSound(void) {
  if (OS_IsRunOnTwl()) {
    DSPi_StopSoundCore();
  }
}

SDK_INLINE void DSP_StartSampling(void *buffer, u32 length) {
  if (OS_IsRunOnTwl()) {
    DSPi_StartSamplingCore(buffer, length);
  }
}

SDK_INLINE void DSP_StopSampling(void) {
  if (OS_IsRunOnTwl()) {
    DSPi_StopSamplingCore();
  }
}

SDK_INLINE void DSP_SyncSamplingBuffer(void) {
  if (OS_IsRunOnTwl()) {
    DSPi_SyncSamplingBufferCore();
  }
}

SDK_INLINE const u8 *DSP_GetLastSamplingAddress(void) {
  const u8 *retval = NULL;
  if (OS_IsRunOnTwl()) {
    retval = DSPi_GetLastSamplingAddressCore();
  }
  return retval;
}

void DSP_HookPostStartProcess(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TWL_DSP_UTIL_H_ */

#ifndef TWL_SND_ARM9_SNDEX_H_
#define TWL_SND_ARM9_SNDEX_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <twl/snd/common/sndex_common.h>

typedef enum SNDEXResult {
  SNDEX_RESULT_SUCCESS = 0,
  SNDEX_RESULT_BEFORE_INIT = 1,
  SNDEX_RESULT_INVALID_PARAM = 2,
  SNDEX_RESULT_EXCLUSIVE = 3,
  SNDEX_RESULT_ILLEGAL_STATE = 4,
  SNDEX_RESULT_PXI_SEND_ERROR = 5,
  SNDEX_RESULT_DEVICE_ERROR = 6,
  SNDEX_RESULT_FATAL_ERROR = 7,
  SNDEX_RESULT_MAX

} SNDEXResult;

typedef enum SNDEXMute {
  SNDEX_MUTE_OFF = 0,
  SNDEX_MUTE_ON = 1,
  SNDEX_MUTE_MAX

} SNDEXMute;

typedef enum SNDEXFrequency {
  SNDEX_FREQUENCY_32730 = 0,
  SNDEX_FREQUENCY_47610 = 1,
  SNDEX_FREQUENCY_MAX

} SNDEXFrequency;

typedef struct _SNDEXIirFilterParam {
  u16 n0;
  u16 n1;
  u16 n2;
  u16 d1;
  u16 d2;
} SNDEXIirFilterParam;

typedef enum _SNDEXIirFilterTarget {
  SNDEX_IIR_FILTER_ADC_1 = 0,
  SNDEX_IIR_FILTER_ADC_2,
  SNDEX_IIR_FILTER_ADC_3,
  SNDEX_IIR_FILTER_ADC_4,
  SNDEX_IIR_FILTER_ADC_5,
  SNDEX_IIR_FILTER_DAC_LEFT_1,
  SNDEX_IIR_FILTER_DAC_LEFT_2,
  SNDEX_IIR_FILTER_DAC_LEFT_3,
  SNDEX_IIR_FILTER_DAC_LEFT_4,
  SNDEX_IIR_FILTER_DAC_LEFT_5,
  SNDEX_IIR_FILTER_DAC_RIGHT_1,
  SNDEX_IIR_FILTER_DAC_RIGHT_2,
  SNDEX_IIR_FILTER_DAC_RIGHT_3,
  SNDEX_IIR_FILTER_DAC_RIGHT_4,
  SNDEX_IIR_FILTER_DAC_RIGHT_5,
  SNDEX_IIR_FILTER_DAC_BOTH_1,
  SNDEX_IIR_FILTER_DAC_BOTH_2,
  SNDEX_IIR_FILTER_DAC_BOTH_3,
  SNDEX_IIR_FILTER_DAC_BOTH_4,
  SNDEX_IIR_FILTER_DAC_BOTH_5,
  SNDEX_IIR_FILTER_TARGET_MAX
} SNDEXIirFilterTarget;

typedef enum SNDEXHeadphone {
  SNDEX_HEADPHONE_UNCONNECT = 0,
  SNDEX_HEADPHONE_CONNECT = 1,
  SNDEX_HEADPHONE_MAX
} SNDEXHeadphone;

#define SNDEX_DSP_MIX_RATE_MIN 0
#define SNDEX_DSP_MIX_RATE_MAX 8

#define SNDEX_VOLUME_MIN 0
#define SNDEX_VOLUME_MAX 7
#define SNDEX_VOLUME_MAX_EX 31

typedef void (*SNDEXCallback)(SNDEXResult result, void *arg);

#define SNDEXVolumeSwitchCallbackInfo SNDEXWork

void SNDEXi_Init(void);

SNDEXResult SNDEXi_GetMuteAsync(SNDEXMute *mute, SNDEXCallback callback,
                                void *arg);
SNDEXResult SNDEXi_GetMute(SNDEXMute *mute);
SNDEXResult SNDEXi_GetI2SFrequencyAsync(SNDEXFrequency *freq,
                                        SNDEXCallback callback, void *arg);
SNDEXResult SNDEXi_GetI2SFrequency(SNDEXFrequency *freq);
SNDEXResult SNDEXi_GetDSPMixRateAsync(u8 *rate, SNDEXCallback callback,
                                      void *arg);
SNDEXResult SNDEXi_GetDSPMixRate(u8 *rate);
SNDEXResult SNDEXi_GetVolumeAsync(u8 *volume, SNDEXCallback callback, void *arg,
                                  BOOL eightlv, BOOL keep);
SNDEXResult SNDEXi_GetVolume(u8 *volume, BOOL eightlv, BOOL keep);
SNDEXResult SNDEXi_GetVolumeExAsync(u8 *volume, SNDEXCallback callback,
                                    void *arg);
SNDEXResult SNDEXi_GetVolumeEx(u8 *volume);
SNDEXResult SNDEXi_GetCurrentVolumeExAsync(u8 *volume, SNDEXCallback callback,
                                           void *arg);
SNDEXResult SNDEXi_GetCurrentVolumeEx(u8 *volume);

SNDEXResult SNDEXi_SetMuteAsync(SNDEXMute mute, SNDEXCallback callback,
                                void *arg);
SNDEXResult SNDEXi_SetMute(SNDEXMute mute);
SNDEXResult SNDEXi_SetI2SFrequencyAsync(SNDEXFrequency freq,
                                        SNDEXCallback callback, void *arg);
SNDEXResult SNDEXi_SetI2SFrequency(SNDEXFrequency freq);
SNDEXResult SNDEXi_SetDSPMixRateAsync(u8 rate, SNDEXCallback callback,
                                      void *arg);
SNDEXResult SNDEXi_SetDSPMixRate(u8 rate);
SNDEXResult SNDEXi_SetVolumeAsync(u8 volume, SNDEXCallback callback, void *arg,
                                  BOOL eightlv);
SNDEXResult SNDEXi_SetVolume(u8 volume, BOOL eightlv);
SNDEXResult SNDEXi_SetVolumeExAsync(u8 volume, SNDEXCallback callback,
                                    void *arg);
SNDEXResult SNDEXi_SetVolumeEx(u8 volume);

SNDEXResult SNDEXi_SetIirFilterAsync(SNDEXIirFilterTarget target,
                                     const SNDEXIirFilterParam *pParam,
                                     SNDEXCallback callback, void *arg);
SNDEXResult SNDEXi_SetIirFilter(SNDEXIirFilterTarget target,
                                const SNDEXIirFilterParam *pParam);

SNDEXResult SNDEXi_IsConnectedHeadphoneAsync(SNDEXHeadphone *hp,
                                             SNDEXCallback callback, void *arg);
SNDEXResult SNDEXi_IsConnectedHeadphone(SNDEXHeadphone *hp);

void SNDEXi_SetVolumeSwitchCallback(SNDEXCallback callback, void *arg);

SNDEXResult SNDEXi_SetIgnoreHWVolume(u8 volume, BOOL eightlv);
SNDEXResult SNDEXi_ResetIgnoreHWVolume(void);

SNDEXResult SNDEXi_PreProcessForShutterSound(void);
SNDEXResult SNDEXi_PostProcessForShutterSound(SNDEXCallback callback,
                                              void *arg);

static inline void SNDEX_Init(void) {
  if (OS_IsRunOnTwl() == TRUE) {
    SNDEXi_Init();
  }
}

static inline SNDEXResult
SNDEX_GetMuteAsync(SNDEXMute *mute, SNDEXCallback callback, void *arg) {
  return (SNDEXResult)((OS_IsRunOnTwl() == TRUE)
                           ? SNDEXi_GetMuteAsync(mute, callback, arg)
                           : SNDEX_RESULT_ILLEGAL_STATE);
}

static inline SNDEXResult SNDEX_GetMute(SNDEXMute *mute) {
  return (SNDEXResult)((OS_IsRunOnTwl() == TRUE) ? SNDEXi_GetMute(mute)
                                                 : SNDEX_RESULT_ILLEGAL_STATE);
}

static inline SNDEXResult SNDEX_GetI2SFrequencyAsync(SNDEXFrequency *freq,
                                                     SNDEXCallback callback,
                                                     void *arg) {
  return (SNDEXResult)((OS_IsRunOnTwl() == TRUE)
                           ? SNDEXi_GetI2SFrequencyAsync(freq, callback, arg)
                           : SNDEX_RESULT_ILLEGAL_STATE);
}

static inline SNDEXResult SNDEX_GetI2SFrequency(SNDEXFrequency *freq) {
  return (SNDEXResult)((OS_IsRunOnTwl() == TRUE) ? SNDEXi_GetI2SFrequency(freq)
                                                 : SNDEX_RESULT_ILLEGAL_STATE);
}

static inline SNDEXResult
SNDEX_GetDSPMixRateAsync(u8 *rate, SNDEXCallback callback, void *arg) {
  return (SNDEXResult)((OS_IsRunOnTwl() == TRUE)
                           ? SNDEXi_GetDSPMixRateAsync(rate, callback, arg)
                           : SNDEX_RESULT_ILLEGAL_STATE);
}

static inline SNDEXResult SNDEX_GetDSPMixRate(u8 *rate) {
  return (SNDEXResult)((OS_IsRunOnTwl() == TRUE) ? SNDEXi_GetDSPMixRate(rate)
                                                 : SNDEX_RESULT_ILLEGAL_STATE);
}

static inline SNDEXResult
SNDEX_GetVolumeAsync(u8 *volume, SNDEXCallback callback, void *arg) {
  return (SNDEXResult)((OS_IsRunOnTwl() == TRUE)
                           ? SNDEXi_GetVolumeAsync(volume, callback, arg, TRUE,
                                                   TRUE)
                           : SNDEX_RESULT_ILLEGAL_STATE);
}

static inline SNDEXResult
SNDEX_GetCurrentVolumeAsync(u8 *volume, SNDEXCallback callback, void *arg) {
  return (SNDEXResult)((OS_IsRunOnTwl() == TRUE)
                           ? SNDEXi_GetVolumeAsync(volume, callback, arg, TRUE,
                                                   FALSE)
                           : SNDEX_RESULT_ILLEGAL_STATE);
}

static inline SNDEXResult SNDEX_GetVolume(u8 *volume) {
  return (SNDEXResult)((OS_IsRunOnTwl() == TRUE)
                           ? SNDEXi_GetVolume(volume, TRUE, TRUE)
                           : SNDEX_RESULT_ILLEGAL_STATE);
}

static inline SNDEXResult SNDEX_GetCurrentVolume(u8 *volume) {
  return (SNDEXResult)((OS_IsRunOnTwl() == TRUE)
                           ? SNDEXi_GetVolume(volume, TRUE, FALSE)
                           : SNDEX_RESULT_ILLEGAL_STATE);
}

static inline SNDEXResult
SNDEX_SetMuteAsync(SNDEXMute mute, SNDEXCallback callback, void *arg) {
  return (SNDEXResult)((OS_IsRunOnTwl() == TRUE)
                           ? SNDEXi_SetMuteAsync(mute, callback, arg)
                           : SNDEX_RESULT_ILLEGAL_STATE);
}

static inline SNDEXResult SNDEX_SetMute(SNDEXMute mute) {
  return (SNDEXResult)((OS_IsRunOnTwl() == TRUE) ? SNDEXi_SetMute(mute)
                                                 : SNDEX_RESULT_ILLEGAL_STATE);
}

static inline SNDEXResult SNDEX_SetI2SFrequencyAsync(SNDEXFrequency freq,
                                                     SNDEXCallback callback,
                                                     void *arg) {
  return (SNDEXResult)((OS_IsRunOnTwl() == TRUE)
                           ? SNDEXi_SetI2SFrequencyAsync(freq, callback, arg)
                           : SNDEX_RESULT_ILLEGAL_STATE);
}

static inline SNDEXResult SNDEX_SetI2SFrequency(SNDEXFrequency freq) {
  return (SNDEXResult)((OS_IsRunOnTwl() == TRUE) ? SNDEXi_SetI2SFrequency(freq)
                                                 : SNDEX_RESULT_ILLEGAL_STATE);
}

static inline SNDEXResult
SNDEX_SetDSPMixRateAsync(u8 rate, SNDEXCallback callback, void *arg) {
  return (SNDEXResult)((OS_IsRunOnTwl() == TRUE)
                           ? SNDEXi_SetDSPMixRateAsync(rate, callback, arg)
                           : SNDEX_RESULT_ILLEGAL_STATE);
}

static inline SNDEXResult SNDEX_SetDSPMixRate(u8 rate) {
  return (SNDEXResult)((OS_IsRunOnTwl() == TRUE) ? SNDEXi_SetDSPMixRate(rate)
                                                 : SNDEX_RESULT_ILLEGAL_STATE);
}

static inline SNDEXResult
SNDEX_SetVolumeAsync(u8 volume, SNDEXCallback callback, void *arg) {
  return (SNDEXResult)((OS_IsRunOnTwl() == TRUE)
                           ? SNDEXi_SetVolumeAsync(volume, callback, arg, TRUE)
                           : SNDEX_RESULT_ILLEGAL_STATE);
}

static inline SNDEXResult SNDEX_SetVolume(u8 volume) {
  return (SNDEXResult)((OS_IsRunOnTwl() == TRUE)
                           ? SNDEXi_SetVolume(volume, TRUE)
                           : SNDEX_RESULT_ILLEGAL_STATE);
}

static inline SNDEXResult
SNDEX_SetIirFilterAsync(SNDEXIirFilterTarget target,
                        const SNDEXIirFilterParam *pParam,
                        SNDEXCallback callback, void *arg) {
  return (
      SNDEXResult)((OS_IsRunOnTwl() == TRUE)
                       ? SNDEXi_SetIirFilterAsync(target, pParam, callback, arg)
                       : SNDEX_RESULT_ILLEGAL_STATE);
}

static inline SNDEXResult
SNDEX_SetIirFilter(SNDEXIirFilterTarget target,
                   const SNDEXIirFilterParam *pParam) {
  return (SNDEXResult)((OS_IsRunOnTwl() == TRUE)
                           ? SNDEXi_SetIirFilter(target, pParam)
                           : SNDEX_RESULT_ILLEGAL_STATE);
}

static inline SNDEXResult
SNDEX_IsConnectedHeadphoneAsync(SNDEXHeadphone *hp, SNDEXCallback callback,
                                void *arg) {
  return (SNDEXResult)((OS_IsRunOnTwl() == TRUE)
                           ? SNDEXi_IsConnectedHeadphoneAsync(hp, callback, arg)
                           : SNDEX_RESULT_ILLEGAL_STATE);
}

static inline SNDEXResult SNDEX_IsConnectedHeadphone(SNDEXHeadphone *hp) {
  return (SNDEXResult)((OS_IsRunOnTwl() == TRUE)
                           ? SNDEXi_IsConnectedHeadphone(hp)
                           : SNDEX_RESULT_ILLEGAL_STATE);
}

static inline void SNDEX_SetVolumeSwitchCallback(SNDEXCallback callback,
                                                 void *arg) {
  if (OS_IsRunOnTwl() == TRUE) {
    SNDEXi_SetVolumeSwitchCallback(callback, arg);
  }
}

static inline SNDEXResult SNDEX_SetIgnoreHWVolume(u8 volume) {
  return (SNDEXResult)((OS_IsRunOnTwl() == TRUE)
                           ? SNDEXi_SetIgnoreHWVolume(volume, TRUE)
                           : SNDEX_RESULT_ILLEGAL_STATE);
}

static inline SNDEXResult SNDEX_ResetIgnoreHWVolume(void) {
  return (SNDEXResult)((OS_IsRunOnTwl() == TRUE) ? SNDEXi_ResetIgnoreHWVolume()
                                                 : SNDEX_RESULT_ILLEGAL_STATE);
}

#ifdef __cplusplus
} // extern "C"
#endif
#endif // TWL_SND_ARM9_SNDEX_H_

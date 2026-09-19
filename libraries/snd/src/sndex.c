#include <twl/os.h>
#include <twl/snd/ARM9/sndex.h>
#include <nitro/pxi.h>
#include <twl/os/common/codecmode.h>

#ifdef SDK_DEBUG
#define SNDEXi_Warning OS_TWarning
#else
#define SNDEXi_Warning(...) ((void)0)
#endif

typedef struct SNDEXWork {
  SNDEXCallback callback;
  void *cbArg;
  void *dest;
  u8 command;
  u8 padding[3];

} SNDEXWork;

typedef enum SNDEXState {
  SNDEX_STATE_BEFORE_INIT = 0,
  SNDEX_STATE_INITIALIZING = 1,
  SNDEX_STATE_INITIALIZED = 2,
  SNDEX_STATE_LOCKED = 3,
  SNDEX_STATE_PLAY_SHUTTER =
      4, // Shutter sound is playing (exclude all SNDEX functions other than
         // PreProcessForShutterSound and PostProcessForShutterSound)
  SNDEX_STATE_POST_PROC_SHUTTER =
      5, // Currently postprocessing the shutter sound playback (exclude all
         // SNDEX functions)
  SNDEX_STATE_MAX

} SNDEXState;

typedef enum SNDEXDevice {
  SNDEX_DEVICE_AUTO = 0,
  SNDEX_DEVICE_SPEAKER = 1,
  SNDEX_DEVICE_HEADPHONE = 2,
  SNDEX_DEVICE_BOTH = 3,
  SNDEX_DEVICE_MAX

} SNDEXDevice;

static volatile SNDEXState sndexState = SNDEX_STATE_BEFORE_INIT;
static SNDEXWork sndexWork;
static SNDEXVolumeSwitchCallbackInfo SNDEXi_VolumeSwitchCallbackInfo = {
    NULL, NULL, NULL, 0};

PMSleepCallbackInfo sleepCallbackInfo;
PMExitCallbackInfo exitCallbackInfo;
static volatile BOOL isIirFilterSetting =
    FALSE; // TRUE if already executing SetIirFilterAsync
static volatile BOOL isLockSpi =
    FALSE; // Whether SPI exclusion control is currently being applied inside
           // SNDEX_SetIirFilter[Async]

static volatile BOOL isPlayShutter =
    FALSE; // TRUE if currently processing shutter sound playback
static volatile BOOL isStoreVolume =
    FALSE; // Whether a volume has been saved by SNDEXi_SetIgnoreHWVolume
static u8 storeVolume = 0; // Value of the volume before it is temporarily
                           // changed by SNDEXi_SetIgnoreHWVolume

#ifdef SDK_PORT
static void CommonCallback(PXIFifoTag tag, u64 data, BOOL err);
#else
static void CommonCallback(PXIFifoTag tag, u32 data, BOOL err);
#endif
static void SyncCallback(SNDEXResult result, void *arg);
static BOOL SendCommand(u8 command, u8 param);
static BOOL SendCommandEx(u8 command, u16 param);
static SNDEXResult CheckState(void);
static void ReplyCallback(SNDEXResult result);
static void SetSndexWork(SNDEXCallback cb, void *cbarg, void *dst, u8 command);

static void SndexSleepAndExitCallback(void *arg);

static void WaitIirFilterSetting(void);
static void WaitResetSoundCallback(SNDEXResult result, void *arg);
static void ResetTempVolume(void);

SNDEXResult SNDEXi_GetDeviceAsync(SNDEXDevice *device, SNDEXCallback callback,
                                  void *arg);
SNDEXResult SNDEXi_GetDevice(SNDEXDevice *device);
SNDEXResult SNDEXi_SetDeviceAsync(SNDEXDevice device, SNDEXCallback callback,
                                  void *arg);
SNDEXResult SNDEXi_SetDevice(SNDEXDevice device);

void SNDEXi_Init(void) {
  OSIntrMode e = OS_DisableInterrupts();

  if (sndexState != SNDEX_STATE_BEFORE_INIT) {
    (void)OS_RestoreInterrupts(e);
    SNDEXi_Warning("%s: Already initialized.\n", __FUNCTION__);
    return;
  }
  sndexState = SNDEX_STATE_INITIALIZING;
  (void)OS_RestoreInterrupts(e);

  SetSndexWork(NULL, NULL, NULL, 0);

  PXI_Init();
  while (!PXI_IsCallbackReady(PXI_FIFO_TAG_SNDEX, PXI_PROC_ARM7)) {
    SVC_WaitByLoop(10);
  }

  PXI_SetFifoRecvCallback(PXI_FIFO_TAG_SNDEX, CommonCallback);

  PM_SetExitCallbackInfo(&exitCallbackInfo, SndexSleepAndExitCallback, NULL);
  PMi_InsertPostExitCallbackEx(&exitCallbackInfo, PM_CALLBACK_PRIORITY_SNDEX);

  PM_SetSleepCallbackInfo(&sleepCallbackInfo, SndexSleepAndExitCallback, NULL);
  PMi_InsertPreSleepCallbackEx(&sleepCallbackInfo, PM_CALLBACK_PRIORITY_SNDEX);

  sndexState = SNDEX_STATE_INITIALIZED;
}

SNDEXResult SNDEXi_GetMuteAsync(SNDEXMute *mute, SNDEXCallback callback,
                                void *arg) {

  {
    SNDEXResult result = CheckState();
    if (result != SNDEX_RESULT_SUCCESS) {
      return result;
    }
  }

  SetSndexWork(callback, arg, (void *)mute, SNDEX_PXI_COMMAND_GET_SMIX_MUTE);

  if (FALSE == SendCommand(sndexWork.command, 0)) {
    return SNDEX_RESULT_PXI_SEND_ERROR;
  }
  return SNDEX_RESULT_SUCCESS;
}

SNDEXResult SNDEXi_GetMute(SNDEXMute *mute) {
  SNDEXResult result;
  OSMessageQueue msgQ;
  OSMessage msg[1];

  if (OS_GetCurrentThread() == NULL) {
    SNDEXi_Warning(
        "%s: Syncronous API could not process in exception handler.\n",
        __FUNCTION__);
    return SNDEX_RESULT_ILLEGAL_STATE;
  }

  OS_InitMessageQueue(&msgQ, msg, 1);

  result = SNDEXi_GetMuteAsync(mute, SyncCallback, (void *)(&msgQ));
  if (result == SNDEX_RESULT_SUCCESS) {
    (void)OS_ReceiveMessage(&msgQ, (OSMessage *)(&result), OS_MESSAGE_BLOCK);
  }
  return result;
}

SNDEXResult SNDEXi_GetI2SFrequencyAsync(SNDEXFrequency *freq,
                                        SNDEXCallback callback, void *arg) {

  {
    SNDEXResult result = CheckState();
    if (result != SNDEX_RESULT_SUCCESS) {
      return result;
    }
  }

  SetSndexWork(callback, arg, (void *)freq, SNDEX_PXI_COMMAND_GET_SMIX_FREQ);

  if (FALSE == SendCommand(sndexWork.command, 0)) {
    return SNDEX_RESULT_PXI_SEND_ERROR;
  }
  return SNDEX_RESULT_SUCCESS;
}

SNDEXResult SNDEXi_GetI2SFrequency(SNDEXFrequency *freq) {
  SNDEXResult result;
  OSMessageQueue msgQ;
  OSMessage msg[1];

  if (OS_GetCurrentThread() == NULL) {
    SNDEXi_Warning(
        "%s: Syncronous API could not process in exception handler.\n",
        __FUNCTION__);
    return SNDEX_RESULT_ILLEGAL_STATE;
  }

  OS_InitMessageQueue(&msgQ, msg, 1);

  result = SNDEXi_GetI2SFrequencyAsync(freq, SyncCallback, (void *)(&msgQ));
  if (result == SNDEX_RESULT_SUCCESS) {
    (void)OS_ReceiveMessage(&msgQ, (OSMessage *)(&result), OS_MESSAGE_BLOCK);
  }
  return result;
}

SNDEXResult SNDEXi_GetDSPMixRateAsync(u8 *rate, SNDEXCallback callback,
                                      void *arg) {

  {
    SNDEXResult result = CheckState();
    if (result != SNDEX_RESULT_SUCCESS) {
      return result;
    }
  }

  SetSndexWork(callback, arg, (void *)rate, SNDEX_PXI_COMMAND_GET_SMIX_DSP);

  if (FALSE == SendCommand(sndexWork.command, 0)) {
    return SNDEX_RESULT_PXI_SEND_ERROR;
  }
  return SNDEX_RESULT_SUCCESS;
}

SNDEXResult SNDEXi_GetDSPMixRate(u8 *rate) {
  SNDEXResult result;
  OSMessageQueue msgQ;
  OSMessage msg[1];

  if (OS_GetCurrentThread() == NULL) {
    SNDEXi_Warning(
        "%s: Syncronous API could not process in exception handler.\n",
        __FUNCTION__);
    return SNDEX_RESULT_ILLEGAL_STATE;
  }

  OS_InitMessageQueue(&msgQ, msg, 1);

  result = SNDEXi_GetDSPMixRateAsync(rate, SyncCallback, (void *)(&msgQ));
  if (result == SNDEX_RESULT_SUCCESS) {
    (void)OS_ReceiveMessage(&msgQ, (OSMessage *)(&result), OS_MESSAGE_BLOCK);
  }
  return result;
}

SNDEXResult SNDEXi_GetVolumeAsync(u8 *volume, SNDEXCallback callback, void *arg,
                                  BOOL eightlv, BOOL keep) {

  {
    SNDEXResult result = CheckState();
    if (result != SNDEX_RESULT_SUCCESS) {
      return result;
    }
  }

  SetSndexWork(callback, arg, (void *)volume, SNDEX_PXI_COMMAND_GET_VOLUME);

  if (FALSE ==
      SendCommand(sndexWork.command,
                  (u8)((keep << SNDEX_PXI_PARAMETER_SHIFT_VOL_KEEP) |
                       (eightlv << SNDEX_PXI_PARAMETER_SHIFT_VOL_8LV)))) {
    return SNDEX_RESULT_PXI_SEND_ERROR;
  }
  return SNDEX_RESULT_SUCCESS;
}

SNDEXResult SNDEXi_GetVolume(u8 *volume, BOOL eightlv, BOOL keep) {
  SNDEXResult result;
  OSMessageQueue msgQ;
  OSMessage msg[1];

  if (OS_GetCurrentThread() == NULL) {
    SNDEXi_Warning(
        "%s: Syncronous API could not process in exception handler.\n",
        __FUNCTION__);
    return SNDEX_RESULT_ILLEGAL_STATE;
  }

  OS_InitMessageQueue(&msgQ, msg, 1);

  result = SNDEXi_GetVolumeAsync(volume, SyncCallback, (void *)(&msgQ), eightlv,
                                 keep);
  if (result == SNDEX_RESULT_SUCCESS) {
    (void)OS_ReceiveMessage(&msgQ, (OSMessage *)(&result), OS_MESSAGE_BLOCK);
  }
  return result;
}

SNDEXResult SNDEXi_GetVolumeExAsync(u8 *volume, SNDEXCallback callback,
                                    void *arg) {
  if (!OS_IsRunOnTwl()) {
    return SNDEX_RESULT_ILLEGAL_STATE;
  }

  return SNDEXi_GetVolumeAsync(volume, callback, arg, FALSE, TRUE);
}

SNDEXResult SNDEXi_GetVolumeEx(u8 *volume) {
  if (!OS_IsRunOnTwl()) {
    return SNDEX_RESULT_ILLEGAL_STATE;
  }

  return SNDEXi_GetVolume(volume, FALSE, TRUE);
}

SNDEXResult SNDEXi_GetCurrentVolumeExAsync(u8 *volume, SNDEXCallback callback,
                                           void *arg) {
  if (!OS_IsRunOnTwl()) {
    return SNDEX_RESULT_ILLEGAL_STATE;
  }

  return SNDEXi_GetVolumeAsync(volume, callback, arg, FALSE, FALSE);
}

SNDEXResult SNDEXi_GetCurrentVolumeEx(u8 *volume) {
  if (!OS_IsRunOnTwl()) {
    return SNDEX_RESULT_ILLEGAL_STATE;
  }

  return SNDEXi_GetVolume(volume, FALSE, FALSE);
}

SNDEXResult SNDEXi_GetDeviceAsync(SNDEXDevice *device, SNDEXCallback callback,
                                  void *arg) {

  {
    SNDEXResult result = CheckState();
    if (result != SNDEX_RESULT_SUCCESS) {
      return result;
    }
  }

  SetSndexWork(callback, arg, (void *)device, SNDEX_PXI_COMMAND_GET_SND_DEVICE);

  if (FALSE == SendCommand(sndexWork.command, 0)) {
    return SNDEX_RESULT_PXI_SEND_ERROR;
  }
  return SNDEX_RESULT_SUCCESS;
}

SNDEXResult SNDEXi_GetDevice(SNDEXDevice *device) {
  SNDEXResult result;
  OSMessageQueue msgQ;
  OSMessage msg[1];

  if (OS_GetCurrentThread() == NULL) {
    SNDEXi_Warning(
        "%s: Syncronous API could not process in exception handler.\n",
        __FUNCTION__);
    return SNDEX_RESULT_ILLEGAL_STATE;
  }

  OS_InitMessageQueue(&msgQ, msg, 1);

  result = SNDEXi_GetDeviceAsync(device, SyncCallback, (void *)(&msgQ));
  if (result == SNDEX_RESULT_SUCCESS) {
    (void)OS_ReceiveMessage(&msgQ, (OSMessage *)(&result), OS_MESSAGE_BLOCK);
  }
  return result;
}

SNDEXResult SNDEXi_SetMuteAsync(SNDEXMute mute, SNDEXCallback callback,
                                void *arg) {

  if (mute >= SNDEX_MUTE_MAX) {
    SNDEXi_Warning("%s: Invalid parameter (mute: %d)\n", __FUNCTION__, mute);
    return SNDEX_RESULT_INVALID_PARAM;
  }

  {
    SNDEXResult result = CheckState();
    if (result != SNDEX_RESULT_SUCCESS) {
      return result;
    }
  }

  SetSndexWork(callback, arg, NULL, SNDEX_PXI_COMMAND_SET_SMIX_MUTE);

  if (FALSE == SendCommand(sndexWork.command, (u8)mute)) {
    return SNDEX_RESULT_PXI_SEND_ERROR;
  }
  return SNDEX_RESULT_SUCCESS;
}

SNDEXResult SNDEXi_SetMute(SNDEXMute mute) {
  SNDEXResult result;
  OSMessageQueue msgQ;
  OSMessage msg[1];

  if (OS_GetCurrentThread() == NULL) {
    SNDEXi_Warning(
        "%s: Syncronous API could not process in exception handler.\n",
        __FUNCTION__);
    return SNDEX_RESULT_ILLEGAL_STATE;
  }

  OS_InitMessageQueue(&msgQ, msg, 1);

  result = SNDEXi_SetMuteAsync(mute, SyncCallback, (void *)(&msgQ));
  if (result == SNDEX_RESULT_SUCCESS) {
    (void)OS_ReceiveMessage(&msgQ, (OSMessage *)(&result), OS_MESSAGE_BLOCK);
  }
  return result;
}

SNDEXResult SNDEXi_SetI2SFrequencyAsync(SNDEXFrequency freq,
                                        SNDEXCallback callback, void *arg) {

  if (freq >= SNDEX_FREQUENCY_MAX) {
    SNDEXi_Warning("%s: Invalid parameter (freq: %d)\n", __FUNCTION__, freq);
    return SNDEX_RESULT_INVALID_PARAM;
  }

  if (OSi_IsCodecTwlMode() == FALSE) {
    return SNDEX_RESULT_ILLEGAL_STATE;
  }

  {
    SNDEXResult result = CheckState();
    if (result != SNDEX_RESULT_SUCCESS) {
      return result;
    }
  }

  SetSndexWork(callback, arg, NULL, SNDEX_PXI_COMMAND_SET_SMIX_FREQ);

  if (FALSE == SendCommand(sndexWork.command, (u8)freq)) {
    return SNDEX_RESULT_PXI_SEND_ERROR;
  }
  return SNDEX_RESULT_SUCCESS;
}

SNDEXResult SNDEXi_SetI2SFrequency(SNDEXFrequency freq) {
  SNDEXResult result;
  OSMessageQueue msgQ;
  OSMessage msg[1];

  if (OS_GetCurrentThread() == NULL) {
    SNDEXi_Warning(
        "%s: Syncronous API could not process in exception handler.\n",
        __FUNCTION__);
    return SNDEX_RESULT_ILLEGAL_STATE;
  }

  OS_InitMessageQueue(&msgQ, msg, 1);

  result = SNDEXi_SetI2SFrequencyAsync(freq, SyncCallback, (void *)(&msgQ));
  if (result == SNDEX_RESULT_SUCCESS) {
    (void)OS_ReceiveMessage(&msgQ, (OSMessage *)(&result), OS_MESSAGE_BLOCK);
  }
  return result;
}

SNDEXResult SNDEXi_SetDSPMixRateAsync(u8 rate, SNDEXCallback callback,
                                      void *arg) {

  if (OSi_IsCodecTwlMode() == FALSE) {
    SNDEXi_Warning("%s: Illegal state\n", __FUNCTION__);
    return SNDEX_RESULT_ILLEGAL_STATE;
  }

  if (rate > SNDEX_DSP_MIX_RATE_MAX) {
    SNDEXi_Warning("%s: Invalid parameter (rate: %u)\n", __FUNCTION__, rate);
    return SNDEX_RESULT_INVALID_PARAM;
  }

  {
    SNDEXResult result = CheckState();
    if (result != SNDEX_RESULT_SUCCESS) {
      return result;
    }
  }

  SetSndexWork(callback, arg, NULL, SNDEX_PXI_COMMAND_SET_SMIX_DSP);

  if (FALSE == SendCommand(sndexWork.command, (u8)rate)) {
    return SNDEX_RESULT_PXI_SEND_ERROR;
  }
  return SNDEX_RESULT_SUCCESS;
}

SNDEXResult SNDEXi_SetDSPMixRate(u8 rate) {
  SNDEXResult result;
  OSMessageQueue msgQ;
  OSMessage msg[1];

  if (OS_GetCurrentThread() == NULL) {
    SNDEXi_Warning(
        "%s: Syncronous API could not process in exception handler.\n",
        __FUNCTION__);
    return SNDEX_RESULT_ILLEGAL_STATE;
  }

  OS_InitMessageQueue(&msgQ, msg, 1);

  result = SNDEXi_SetDSPMixRateAsync(rate, SyncCallback, (void *)(&msgQ));
  if (result == SNDEX_RESULT_SUCCESS) {
    (void)OS_ReceiveMessage(&msgQ, (OSMessage *)(&result), OS_MESSAGE_BLOCK);
  }
  return result;
}

SNDEXResult SNDEXi_SetVolumeAsync(u8 volume, SNDEXCallback callback, void *arg,
                                  BOOL eightlv) {

  if (volume > (eightlv ? SNDEX_VOLUME_MAX : SNDEX_VOLUME_MAX_EX)) {
    SNDEXi_Warning("%s: Invalid parameter (volume: %u)\n", __FUNCTION__,
                   volume);
    return SNDEX_RESULT_INVALID_PARAM;
  }

  {
    SNDEXResult result = CheckState();
    if (result != SNDEX_RESULT_SUCCESS) {
      return result;
    }
  }

  SetSndexWork(callback, arg, NULL, SNDEX_PXI_COMMAND_SET_VOLUME);

  if (FALSE ==
      SendCommand(sndexWork.command,
                  (u8)((eightlv << SNDEX_PXI_PARAMETER_SHIFT_VOL_8LV) |
                       (SNDEX_PXI_PARAMETER_MASK_VOL_VALUE & volume)))) {
    return SNDEX_RESULT_PXI_SEND_ERROR;
  }
  return SNDEX_RESULT_SUCCESS;
}

SNDEXResult SNDEXi_SetVolume(u8 volume, BOOL eightlv) {
  SNDEXResult result;
  OSMessageQueue msgQ;
  OSMessage msg[1];

  if (OS_GetCurrentThread() == NULL) {
    SNDEXi_Warning(
        "%s: Syncronous API could not process in exception handler.\n",
        __FUNCTION__);
    return SNDEX_RESULT_ILLEGAL_STATE;
  }

  OS_InitMessageQueue(&msgQ, msg, 1);

  result =
      SNDEXi_SetVolumeAsync(volume, SyncCallback, (void *)(&msgQ), eightlv);
  if (result == SNDEX_RESULT_SUCCESS) {
    (void)OS_ReceiveMessage(&msgQ, (OSMessage *)(&result), OS_MESSAGE_BLOCK);
  }
  return result;
}

SNDEXResult SNDEXi_SetVolumeExAsync(u8 volume, SNDEXCallback callback,
                                    void *arg) {
  if (!OS_IsRunOnTwl()) {
    return SNDEX_RESULT_ILLEGAL_STATE;
  }

  return SNDEXi_SetVolumeAsync(volume, callback, arg, FALSE);
}

SNDEXResult SNDEXi_SetVolumeEx(u8 volume) {
  if (!OS_IsRunOnTwl()) {
    return SNDEX_RESULT_ILLEGAL_STATE;
  }

  return SNDEXi_SetVolume(volume, FALSE);
}

SNDEXResult SNDEXi_SetDeviceAsync(SNDEXDevice device, SNDEXCallback callback,
                                  void *arg) {

  if (device >= SNDEX_DEVICE_MAX) {
    SNDEXi_Warning("%s: Invalid parameter (device: %d)\n", __FUNCTION__,
                   device);
    return SNDEX_RESULT_INVALID_PARAM;
  }

  if (OSi_IsCodecTwlMode() == FALSE) {
    return SNDEX_RESULT_ILLEGAL_STATE;
  }

  {
    SNDEXResult result = CheckState();
    if (result != SNDEX_RESULT_SUCCESS) {
      return result;
    }
  }

  SetSndexWork(callback, arg, NULL, SNDEX_PXI_COMMAND_SET_SND_DEVICE);

  if (FALSE == SendCommand(sndexWork.command, (u8)device)) {
    return SNDEX_RESULT_PXI_SEND_ERROR;
  }
  return SNDEX_RESULT_SUCCESS;
}

SNDEXResult SNDEXi_SetDevice(SNDEXDevice device) {
  SNDEXResult result;
  OSMessageQueue msgQ;
  OSMessage msg[1];

  if (OS_GetCurrentThread() == NULL) {
    SNDEXi_Warning(
        "%s: Syncronous API could not process in exception handler.\n",
        __FUNCTION__);
    return SNDEX_RESULT_ILLEGAL_STATE;
  }

  OS_InitMessageQueue(&msgQ, msg, 1);

  result = SNDEXi_SetDeviceAsync(device, SyncCallback, (void *)(&msgQ));
  if (result == SNDEX_RESULT_SUCCESS) {
    (void)OS_ReceiveMessage(&msgQ, (OSMessage *)(&result), OS_MESSAGE_BLOCK);
  }
  return result;
}

SNDEXResult SNDEXi_SetIirFilterAsync(SNDEXIirFilterTarget target,
                                     const SNDEXIirFilterParam *pParam,
                                     SNDEXCallback callback, void *arg) {

  if (isIirFilterSetting) {
    return SNDEX_RESULT_EXCLUSIVE;
  }
  isIirFilterSetting = TRUE;

  if (OSi_IsCodecTwlMode() == FALSE) {
    SNDEXi_Warning("%s: Illegal state\n", __FUNCTION__);
    isIirFilterSetting = FALSE;
    return SNDEX_RESULT_ILLEGAL_STATE;
  }

  {
    SNDEXResult result = CheckState();
    if (result != SNDEX_RESULT_SUCCESS) {
      isIirFilterSetting = FALSE;
      return result;
    }
  }

  SetSndexWork(NULL, NULL, NULL, SNDEX_PXI_COMMAND_SET_IIRFILTER_TARGET);

  if (FALSE == SendCommandEx(sndexWork.command, (u16)target)) {
    isIirFilterSetting = FALSE;
    return SNDEX_RESULT_PXI_SEND_ERROR;
  }

  OS_SpinWait(67 * 1000); // Approximately 1 ms

  while (CheckState() != SNDEX_RESULT_SUCCESS) {
    OS_SpinWait(67 * 1000); // Approximately 1 ms
  }

  SetSndexWork(NULL, NULL, NULL, SNDEX_PXI_COMMAND_SET_IIRFILTER_PARAM_N0);

  if (FALSE == SendCommandEx(sndexWork.command, pParam->n0)) {
    isIirFilterSetting = FALSE;
    return SNDEX_RESULT_PXI_SEND_ERROR;
  }
  OS_SpinWait(67 * 1000); // Approximately 1 ms

  while (CheckState() != SNDEX_RESULT_SUCCESS) {
    OS_SpinWait(67 * 1000); // Approximately 1 ms
  }

  SetSndexWork(NULL, NULL, NULL, SNDEX_PXI_COMMAND_SET_IIRFILTER_PARAM_N1);

  if (FALSE == SendCommandEx(sndexWork.command, pParam->n1)) {
    isIirFilterSetting = FALSE;
    return SNDEX_RESULT_PXI_SEND_ERROR;
  }
  OS_SpinWait(67 * 1000); // Approximately 1 ms

  while (CheckState() != SNDEX_RESULT_SUCCESS) {
    OS_SpinWait(67 * 1000); // Approximately 1 ms
  }

  SetSndexWork(NULL, NULL, NULL, SNDEX_PXI_COMMAND_SET_IIRFILTER_PARAM_N2);

  if (FALSE == SendCommandEx(sndexWork.command, pParam->n2)) {
    isIirFilterSetting = FALSE;
    return SNDEX_RESULT_PXI_SEND_ERROR;
  }
  OS_SpinWait(67 * 1000); // Approximately 1 ms

  while (CheckState() != SNDEX_RESULT_SUCCESS) {
    OS_SpinWait(67 * 1000); // Approximately 1 ms
  }

  SetSndexWork(NULL, NULL, NULL, SNDEX_PXI_COMMAND_SET_IIRFILTER_PARAM_D1);

  if (FALSE == SendCommandEx(sndexWork.command, pParam->d1)) {
    isIirFilterSetting = FALSE;
    return SNDEX_RESULT_PXI_SEND_ERROR;
  }
  OS_SpinWait(67 * 1000); // Approximately 1 ms

  while (CheckState() != SNDEX_RESULT_SUCCESS) {
    OS_SpinWait(67 * 1000); // Approximately 1 ms
  }

  SetSndexWork(NULL, NULL, NULL, SNDEX_PXI_COMMAND_SET_IIRFILTER_PARAM_D2);

  if (FALSE == SendCommandEx(sndexWork.command, pParam->d2)) {
    isIirFilterSetting = FALSE;
    return SNDEX_RESULT_PXI_SEND_ERROR;
  }
  OS_SpinWait(67 * 1000); // Approximately 1 ms

  while (CheckState() != SNDEX_RESULT_SUCCESS) {
    OS_SpinWait(67 * 1000); // Approximately 1 ms
  }

  SetSndexWork(callback, arg, NULL, SNDEX_PXI_COMMAND_SET_IIRFILTER);

  if (FALSE == SendCommandEx(sndexWork.command, 0)) {
    isIirFilterSetting = FALSE;
    return SNDEX_RESULT_PXI_SEND_ERROR;
  }

  return SNDEX_RESULT_SUCCESS;
}

SNDEXResult SNDEXi_SetIirFilter(SNDEXIirFilterTarget target,
                                const SNDEXIirFilterParam *pParam) {
  SNDEXResult result;
  OSMessageQueue msgQ;
  OSMessage msg[1];

  if (OS_GetCurrentThread() == NULL) {
    SNDEXi_Warning(
        "%s: Syncronous API could not process in exception handler.\n",
        __FUNCTION__);
    return SNDEX_RESULT_ILLEGAL_STATE;
  }

  OS_InitMessageQueue(&msgQ, msg, 1);

  result =
      SNDEXi_SetIirFilterAsync(target, pParam, SyncCallback, (void *)(&msgQ));
  if (result == SNDEX_RESULT_SUCCESS) {
    (void)OS_ReceiveMessage(&msgQ, (OSMessage *)(&result), OS_MESSAGE_BLOCK);
  }

  isLockSpi = FALSE;

  isIirFilterSetting = FALSE;

  return result;
}

SNDEXResult SNDEXi_IsConnectedHeadphoneAsync(SNDEXHeadphone *hp,
                                             SNDEXCallback callback,
                                             void *arg) {

  {
    SNDEXResult result = CheckState();
    if (result != SNDEX_RESULT_SUCCESS) {
      return result;
    }
  }

  SetSndexWork(callback, arg, (void *)hp, SNDEX_PXI_COMMAND_HP_CONNECT);

  if (FALSE == SendCommand(sndexWork.command, 0)) {
    return SNDEX_RESULT_PXI_SEND_ERROR;
  }
  return SNDEX_RESULT_SUCCESS;
}

SNDEXResult SNDEXi_IsConnectedHeadphone(SNDEXHeadphone *hp) {
  SNDEXResult result;
  OSMessageQueue msgQ;
  OSMessage msg[1];

  if (OS_GetCurrentThread() == NULL) {
    SNDEXi_Warning(
        "%s: Syncronous API could not process in exception handler.\n",
        __FUNCTION__);
    return SNDEX_RESULT_ILLEGAL_STATE;
  }

  OS_InitMessageQueue(&msgQ, msg, 1);

  result = SNDEXi_IsConnectedHeadphoneAsync(hp, SyncCallback, (void *)(&msgQ));
  if (result == SNDEX_RESULT_SUCCESS) {
    (void)OS_ReceiveMessage(&msgQ, (OSMessage *)(&result), OS_MESSAGE_BLOCK);
  }
  return result;
}

void SNDEXi_SetVolumeSwitchCallback(SNDEXCallback callback, void *arg) {
  SNDEXi_VolumeSwitchCallbackInfo.callback = callback;
  SNDEXi_VolumeSwitchCallbackInfo.cbArg = arg;
}

SNDEXResult SNDEXi_SetIgnoreHWVolume(u8 volume, BOOL eightlv) {
  SNDEXResult result;

  if (!isStoreVolume) {
    result = SNDEXi_GetVolumeEx(&storeVolume);
    if (result != SNDEX_RESULT_SUCCESS) {
      return result;
    }
  }

  result = eightlv ? SNDEX_SetVolume(volume) : SNDEXi_SetVolumeEx(volume);
  if (result != SNDEX_RESULT_SUCCESS) {
    return result;
  }

  isStoreVolume = TRUE;
  return result; // SNDEX_RESULT_SUCCESS
}

SNDEXResult SNDEXi_ResetIgnoreHWVolume(void) {
  SNDEXResult result;
  if ((result = SNDEXi_SetVolumeEx(storeVolume)) != SNDEX_RESULT_SUCCESS) {
    return result;
  }

  isStoreVolume = FALSE;
  return result; // SNDEX_RESULT_SUCCESS
}

SNDEXResult SNDEXi_PreProcessForShutterSound(void) {
  SNDEXResult result;
  OSMessageQueue msgQ;
  OSMessage msg[1];

  if (OSi_IsCodecTwlMode() == FALSE) {
    return SNDEX_RESULT_ILLEGAL_STATE;
  }

  if (OS_GetCurrentThread() == NULL) {
    SNDEXi_Warning(
        "%s: Syncronous API could not process in exception handler.\n",
        __FUNCTION__);
    return SNDEX_RESULT_ILLEGAL_STATE;
  }

  if (sndexState != SNDEX_STATE_PLAY_SHUTTER) {

    result = CheckState();
    if (result != SNDEX_RESULT_SUCCESS) {
      return result;
    }
  } else if (sndexState == SNDEX_STATE_POST_PROC_SHUTTER) {
    return SNDEX_RESULT_EXCLUSIVE;
  }

  OS_InitMessageQueue(&msgQ, msg, 1);

  SetSndexWork(SyncCallback, (void *)(&msgQ), NULL,
               SNDEX_PXI_COMMAND_PRE_PROC_SHUTTER);

  if (FALSE == SendCommand(sndexWork.command, 0)) {
    return SNDEX_RESULT_PXI_SEND_ERROR;
  }

  (void)OS_ReceiveMessage(&msgQ, (OSMessage *)(&result), OS_MESSAGE_BLOCK);

  return result;
}

SNDEXResult SNDEXi_PostProcessForShutterSound(SNDEXCallback callback,
                                              void *arg) {

  if (OSi_IsCodecTwlMode() == FALSE || sndexState != SNDEX_STATE_PLAY_SHUTTER) {
    return SNDEX_RESULT_ILLEGAL_STATE;
  }

  sndexState = SNDEX_STATE_POST_PROC_SHUTTER;

  SetSndexWork(callback, arg, NULL, SNDEX_PXI_COMMAND_POST_PROC_SHUTTER);

  if (FALSE == SendCommand(sndexWork.command, 0)) {
    return SNDEX_RESULT_PXI_SEND_ERROR;
  }

  return SNDEX_RESULT_SUCCESS;
}

#ifdef SDK_PORT
static void CommonCallback(PXIFifoTag tag, u64 data, BOOL err)
#else
static void CommonCallback(PXIFifoTag tag, u32 data, BOOL err)
#endif
{
  u8 command = (u8)((data & SNDEX_PXI_COMMAND_MASK) >> SNDEX_PXI_COMMAND_SHIFT);
  u8 result = (u8)((data & SNDEX_PXI_RESULT_MASK) >> SNDEX_PXI_RESULT_SHIFT);
  u8 param =
      (u8)((data & SNDEX_PXI_PARAMETER_MASK) >> SNDEX_PXI_PARAMETER_SHIFT);

  if (command == SNDEX_PXI_COMMAND_PRESS_VOLSWITCH) {
    SNDEXResult result = CheckState();
    if (result != SNDEX_RESULT_SUCCESS) {
      return;
    }

    SetSndexWork(NULL, NULL, NULL, SNDEX_PXI_COMMAND_PRESS_VOLSWITCH);
  }

  if ((tag != PXI_FIFO_TAG_SNDEX) || (err == TRUE) ||
      (sndexState != SNDEX_STATE_LOCKED &&
       sndexState != SNDEX_STATE_PLAY_SHUTTER &&
       sndexState != SNDEX_STATE_POST_PROC_SHUTTER) ||
      (sndexWork.command != command)) {

    OS_TWarning("SNDEX: Library state is inconsistent.\n");
    ReplyCallback(SNDEX_RESULT_FATAL_ERROR);
    return;
  }

  switch (result) {
  case SNDEX_PXI_RESULT_SUCCESS:

    switch (command) {
    case SNDEX_PXI_COMMAND_GET_SMIX_MUTE:
      if (sndexWork.dest != NULL) {
        *((SNDEXMute *)sndexWork.dest) = (SNDEXMute)param;
      }
      break;
    case SNDEX_PXI_COMMAND_GET_SMIX_FREQ:
      if (sndexWork.dest != NULL) {
        *((SNDEXFrequency *)sndexWork.dest) = (SNDEXFrequency)param;
      }
      break;
    case SNDEX_PXI_COMMAND_GET_SMIX_DSP:
    case SNDEX_PXI_COMMAND_GET_VOLUME:
      if (sndexWork.dest != NULL) {
        *((u8 *)sndexWork.dest) = param;
      }
      break;
    case SNDEX_PXI_COMMAND_GET_SND_DEVICE:
      if (sndexWork.dest != NULL) {
        *((SNDEXDevice *)sndexWork.dest) = (SNDEXDevice)param;
      }
      break;
    case SNDEX_PXI_COMMAND_PRESS_VOLSWITCH:

      if (SNDEXi_VolumeSwitchCallbackInfo.callback != NULL) {
        (SNDEXi_VolumeSwitchCallbackInfo.callback)(
            (SNDEXResult)result, SNDEXi_VolumeSwitchCallbackInfo.cbArg);
      }
      break;
    case SNDEX_PXI_COMMAND_HP_CONNECT:
      if (sndexWork.dest != NULL) {
        *((SNDEXHeadphone *)sndexWork.dest) = (SNDEXHeadphone)param;
      }
      break;
    }
    ReplyCallback(SNDEX_RESULT_SUCCESS);
    break;
  case SNDEX_PXI_RESULT_INVALID_PARAM:
    ReplyCallback(SNDEX_RESULT_INVALID_PARAM);
    break;
  case SNDEX_PXI_RESULT_EXCLUSIVE:
    ReplyCallback(SNDEX_RESULT_EXCLUSIVE);
    break;
  case SNDEX_PXI_RESULT_ILLEGAL_STATE:
    ReplyCallback(SNDEX_RESULT_ILLEGAL_STATE);
    break;
  case SNDEX_PXI_RESULT_DEVICE_ERROR:
    if (command == SNDEX_PXI_COMMAND_GET_VOLUME) {
      if (sndexWork.dest != NULL) {
        *((u8 *)sndexWork.dest) = SNDEX_VOLUME_MIN;
      }
    }
    ReplyCallback(SNDEX_RESULT_DEVICE_ERROR);
    break;
  case SNDEX_PXI_RESULT_INVALID_COMMAND:
  default:
    ReplyCallback(SNDEX_RESULT_FATAL_ERROR);
    break;
  }
}

static void SyncCallback(SNDEXResult result, void *arg) {
  SDK_NULL_ASSERT(arg);

  if (FALSE == OS_SendMessage((OSMessageQueue *)arg, (OSMessage)result,
                              OS_MESSAGE_NOBLOCK)) {

    OS_TWarning("SNDEX: Temporary message queue is full.\n");
  }
}

static BOOL SendCommand(u8 command, u8 param) {
  OSIntrMode e = OS_DisableInterrupts();
  u32 packet =
      (u32)(((command << SNDEX_PXI_COMMAND_SHIFT) & SNDEX_PXI_COMMAND_MASK) |
            ((param << SNDEX_PXI_PARAMETER_SHIFT) & SNDEX_PXI_PARAMETER_MASK));

  if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_SNDEX, packet, 0)) {

    sndexState = SNDEX_STATE_INITIALIZED;
    (void)OS_RestoreInterrupts(e);
    SNDEXi_Warning("SNDEX: Failed to send PXI command.\n");
    return FALSE;
  }

  (void)OS_RestoreInterrupts(e);
  return TRUE;
}

static BOOL SendCommandEx(u8 command, u16 param) {
  OSIntrMode e = OS_DisableInterrupts();
  u32 packet =
      (u32)(((command << SNDEX_PXI_COMMAND_SHIFT) & SNDEX_PXI_COMMAND_MASK) |
            ((param << SNDEX_PXI_PARAMETER_SHIFT) &
             SNDEX_PXI_PARAMETER_MASK_IIR));

  if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_SNDEX, packet, 0)) {

    sndexState = SNDEX_STATE_INITIALIZED;
    (void)OS_RestoreInterrupts(e);
    SNDEXi_Warning("SNDEX: Failed to send PXI command.\n");
    return FALSE;
  }
  if (command == SNDEX_PXI_COMMAND_SET_IIRFILTER) {
    isLockSpi = TRUE;
  }

  (void)OS_RestoreInterrupts(e);
  return TRUE;
}

static SNDEXResult CheckState(void) {
  OSIntrMode e = OS_DisableInterrupts();

  switch (sndexState) {
  case SNDEX_STATE_BEFORE_INIT:
  case SNDEX_STATE_INITIALIZING:
    (void)OS_RestoreInterrupts(e);
    SNDEXi_Warning("SNDEX: Library is not initialized yet.\n");
    return SNDEX_RESULT_BEFORE_INIT;
  case SNDEX_STATE_LOCKED:
  case SNDEX_STATE_PLAY_SHUTTER:
  case SNDEX_STATE_POST_PROC_SHUTTER:
    (void)OS_RestoreInterrupts(e);
    SNDEXi_Warning("SNDEX: Another request is in progress.\n");
    return SNDEX_RESULT_EXCLUSIVE;
  }

  sndexState = SNDEX_STATE_LOCKED;
  (void)OS_RestoreInterrupts(e);
  return SNDEX_RESULT_SUCCESS;
}

static void ReplyCallback(SNDEXResult result) {
  OSIntrMode e = OS_DisableInterrupts();
  SNDEXCallback callback = sndexWork.callback;
  void *cbArg = sndexWork.cbArg;
  u8 command = sndexWork.command;

  if (sndexWork.command == SNDEX_PXI_COMMAND_SET_IIRFILTER) {
    isLockSpi = FALSE;
    isIirFilterSetting = FALSE;
  }

  SetSndexWork(NULL, NULL, NULL, 0);

  if (command == SNDEX_PXI_COMMAND_PRE_PROC_SHUTTER) {

    sndexState = SNDEX_STATE_PLAY_SHUTTER;
  } else {
    sndexState = SNDEX_STATE_INITIALIZED;
  }
  (void)OS_RestoreInterrupts(e);

  if (callback != NULL) {
    callback(result, cbArg);
  }
}

static void SetSndexWork(SNDEXCallback cb, void *cbarg, void *dst, u8 command) {
  sndexWork.callback = cb;
  sndexWork.cbArg = cbarg;
  sndexWork.dest = dst;
  sndexWork.command = command;
}

static void SndexSleepAndExitCallback(void *arg) {
#pragma unused(arg)

  WaitIirFilterSetting();

  ResetTempVolume();
}

static void WaitIirFilterSetting(void) {

  while (isLockSpi) {
    OS_SpinWait(67 * 1000); // Approximately 1 ms
    PXIi_HandlerRecvFifoNotEmpty();
  }
}

static void WaitResetSoundCallback(SNDEXResult result, void *arg) {
  static u32 i = 0; // Retry up to 5 times
#pragma unused(arg)
  if (result != SNDEX_RESULT_SUCCESS && i < 5) {
    (void)SNDEXi_SetVolumeExAsync(storeVolume, WaitResetSoundCallback, NULL);
    i++;
    return;
  }
  isStoreVolume = FALSE;
}

static void ResetTempVolume(void) {
  if (isStoreVolume) {
    static
#ifdef SDK_PORT
        int
#endif
            i = 0; // Retry up to 5 times

    while (SNDEX_RESULT_SUCCESS !=
               SNDEXi_SetVolumeExAsync(storeVolume, WaitResetSoundCallback,
                                       NULL) &&
           i < 5) {
      i++;
    }
    while (isStoreVolume) {
      OS_SpinWait(67 * 1000); // Approximately 1 ms
      PXIi_HandlerRecvFifoNotEmpty();
    }
  }
}

#include <twl.h>
#include <twl/dsp.h>
#include <twl/dsp/common/pipe.h>
#include <twl/dsp/common/audio.h>

#include "dsp_process.h"

#define DSP_SOUND_PRIORITY_SHUTTER 0
#define DSP_SOUND_PRIORITY_NORMAL 16
#define DSP_SOUND_PRIORITY_NONE 32

#define MAKE_FOURCC(cc1, cc2, cc3, cc4)                                        \
  (u32)((cc1) | (cc2 << 8) | (cc3 << 16) | (cc4 << 24))
#define MAKE_TAG_DATA(ca)                                                      \
  (u32)((*(ca)) | (*(ca + 1) << 8) | (*(ca + 2) << 16) | (*(ca + 3) << 24))
#define FOURCC_RIFF MAKE_FOURCC('R', 'I', 'F', 'F')
#define FOURCC_WAVE MAKE_FOURCC('W', 'A', 'V', 'E')
#define FOURCC_fmt MAKE_FOURCC('f', 'm', 't', ' ')
#define FOURCC_data MAKE_FOURCC('d', 'a', 't', 'a')

#define DSP_WAIT_SHUTTER 60

static BOOL DSPiSoundPlaying = FALSE;
static OSAlarm DSPiSoundAlarm;

static int DSPiSoundPriority = DSP_SOUND_PRIORITY_NONE;

typedef struct DSPAudioCaptureInfo {
  DSPAddr bufferAddress;
  DSPWord bufferLength;
  DSPWord currentPosition;
} DSPAudioCaptureInfo;
static DSPAudioCaptureInfo DSPiAudioCapture;
extern DSPAddr DSPiAudioCaptureAddress;
static DSPAddr DSPiReadPosition = 0;

static u32 DSPiLocalRingLength = 0;
static u8 *DSPiLocalRingBuffer = NULL;
static int DSPiLocalRingOffset = 0;
DSPAddr DSPiAudioCaptureAddress = 0;

static BOOL DSPiPlayingShutter = FALSE;

static void DSPiShutterPostProcessCallback(SNDEXResult result, void *arg) {
#pragma unused(arg)
  if (result == SNDEX_RESULT_EXCLUSIVE) {

    OS_TPanic("SNDEXi_PostProcessForShutterSound SNDEX_RESULT_EXCLUSIVE ... "
              "DSP_PlayShutterSound\n");
  }
  if (result != SNDEX_RESULT_SUCCESS) {

    OS_TPanic(
        "SNDEXi_PostProcessForShutterSound Error ... DSP_PlayShutterSound\n");
  }
  DSPiPlayingShutter = FALSE;
}

static void sound_handler(void *arg) {
#pragma unused(arg)
  DSPiSoundPlaying = FALSE;
}

static void DSPi_PipeCallbackForSound(void *userdata, int port, int peer) {
  (void)userdata;
  if (peer == DSP_PIPE_INPUT) {

    DSPAudioDriverResponse response;
    DSPPipe pipe[1];
    (void)DSP_LoadPipe(pipe, port, peer);
    if (DSP_GetPipeReadableSize(pipe) >= sizeof(response)) {
      DSP_ReadPipe(pipe, &response, sizeof(response));
      response.ctrl = DSP_32BIT_TO_DSP(response.ctrl);
      response.result = DSP_32BIT_TO_DSP(response.result);

      if ((response.ctrl & DSP_AUDIO_DRIVER_TARGET_MASK) ==
          DSP_AUDIO_DRIVER_TARGET_OUTPUT) {

        if ((response.ctrl & DSP_AUDIO_DRIVER_CONTROL_MASK) ==
            DSP_AUDIO_DRIVER_CONTROL_STOP) {

          OS_CreateAlarm(&DSPiSoundAlarm);
          OS_SetAlarm(&DSPiSoundAlarm, OS_MilliSecondsToTicks(30),
                      sound_handler, NULL);
          if (DSPiSoundPriority == DSP_SOUND_PRIORITY_SHUTTER) {

            (void)SNDEXi_PostProcessForShutterSound(
                DSPiShutterPostProcessCallback, 0);
          }
          DSPiSoundPriority = DSP_SOUND_PRIORITY_NONE;
        }
      }

      if ((response.ctrl & DSP_AUDIO_DRIVER_TARGET_MASK) ==
          DSP_AUDIO_DRIVER_TARGET_INPUT) {

        if ((response.ctrl & DSP_AUDIO_DRIVER_CONTROL_MASK) ==
            DSP_AUDIO_DRIVER_CONTROL_START) {
          DSPiAudioCaptureAddress = (DSPAddr)response.result;
        }
      }
    }
  }
}

static void DSPi_PlaySoundEx(const void *src, u32 len, u32 ctrl, int priority) {

  DSPProcessContext *context = DSP_FindProcess(NULL);
  if (context) {

    if (DSPiSoundPriority < priority) {
      OS_TWarning("still now playing high-priority sound.\n");
    } else {
      DSPiSoundPriority = priority;
      ctrl |= DSP_AUDIO_DRIVER_TARGET_OUTPUT;
      ctrl |= DSP_AUDIO_DRIVER_CONTROL_START;

      len >>= 1;

      DSP_SetPipeCallback(DSP_PIPE_AUDIO, DSPi_PipeCallbackForSound, NULL);
      DSPiSoundPlaying = TRUE;
      {
        DSPAudioDriverCommand command;
        command.ctrl = DSP_32BIT_TO_DSP(ctrl);
        command.buf = DSP_32BIT_TO_DSP(src);
        command.len = DSP_32BIT_TO_DSP(len);
        command.opt = DSP_32BIT_TO_DSP(0);
        DSP_WriteProcessPipe(context, DSP_PIPE_AUDIO, &command,
                             sizeof(command));
      }
    }
  }
}

void DSPi_PlaySoundCore(const void *src, u32 len, BOOL stereo) {
  u32 ctrl = (stereo != FALSE) ? DSP_AUDIO_DRIVER_MODE_STEREO
                               : DSP_AUDIO_DRIVER_MODE_MONAURAL;
  DSPi_PlaySoundEx(src, len, ctrl, DSP_SOUND_PRIORITY_NORMAL);
}

#if 0 // Do not apply a thorough WAVE check up to here
BOOL DSPi_PlayShutterSoundCore(const void *src, u32 len)
{
    u8* wave_data = (u8*)src;
    u32 cur = 0;
    u32 tag;
    u32 wave_len;
    u32 raw_len;
    BOOL    fFmt = FALSE, fData = FALSE;

    static SNDEXFrequency freq;
    u32 sampling;
    u32 chunkSize;


    if(len < cur+12)
        return FALSE;
    tag = MAKE_TAG_DATA(wave_data+cur);
    if(tag != FOURCC_RIFF)
        return FALSE;
    cur+=4;

    wave_len = MAKE_TAG_DATA(wave_data+cur);
    cur+=4;

    tag = MAKE_TAG_DATA(wave_data+cur);
    if(tag != FOURCC_WAVE)
        return FALSE;
    cur+=4;

    while (wave_len > 0)
    {
        if(len < cur+8)
            return FALSE;
        tag = MAKE_TAG_DATA(wave_data+cur);
        cur+=4;
        chunkSize = MAKE_TAG_DATA(wave_data+cur);
        cur+=4;

        if(len < cur+chunkSize)
            return FALSE;

        switch (tag)
        {
        case FOURCC_fmt:


            if(!DSPi_IsShutterSoundPlayingCore())
            {
                if(SNDEX_GetI2SFrequency(&freq) != SNDEX_RESULT_SUCCESS)
                    return FALSE;
            }
            sampling = MAKE_TAG_DATA(wave_data+cur+4);
            cur+=chunkSize;
            if( ((freq == SNDEX_FREQUENCY_32730)&&(sampling != 32730))||((freq == SNDEX_FREQUENCY_47610)&&(sampling != 47610)) )
                return FALSE;
            fFmt = TRUE;
            break;
        case FOURCC_data:
            raw_len = chunkSize;
            fData = TRUE;
            break;
        default:
            cur+=chunkSize;
            break;
        }
        if(fFmt && fData)
            break;
        wave_len -= chunkSize;
    }
    if(!(fFmt && fData))
        return FALSE;




    OS_SpinWait(67 * DSP_WAIT_SHUTTER * 1000);	// Approximately 60 msec


    while(SNDEXi_PreProcessForShutterSound() != SNDEX_RESULT_SUCCESS)
    {
        OS_Sleep(1); // Retry until successful
    }

    {
        u32     ctrl = DSP_AUDIO_DRIVER_MODE_MONAURAL;

        ctrl |= DSP_AUDIO_DRIVER_MODE_HALFVOL;
        DSPi_PlaySoundEx((wave_data+cur), raw_len, ctrl, DSP_SOUND_PRIORITY_SHUTTER);
        DSPiPlayingShutter = TRUE;
    }

    return TRUE;
}
#else // The purpose of the check is only for not making the shutter sound with
      // a different frequency
BOOL DSPi_PlayShutterSoundCore(const void *src, u32 len) {
#pragma unused(len)
  u32 cur;
  u32 sampling;
  u32 raw_len;
  u8 *wave_data = (u8 *)src;
  static SNDEXFrequency freq;

  if (len < 44)
    return FALSE;

  if (MAKE_TAG_DATA(wave_data) != FOURCC_RIFF)
    return FALSE;

  if (MAKE_TAG_DATA(wave_data + 8) != FOURCC_WAVE)
    return FALSE;

  cur = 24;
  sampling = MAKE_TAG_DATA(wave_data + cur);

  if (!DSPi_IsShutterSoundPlayingCore()) {
    if (SNDEX_GetI2SFrequency(&freq) != SNDEX_RESULT_SUCCESS)
      return FALSE;
  }
  if (((freq == SNDEX_FREQUENCY_32730) && (sampling != 32730)) ||
      ((freq == SNDEX_FREQUENCY_47610) && (sampling != 47610)))
    return FALSE;

  cur += 16;
  raw_len = MAKE_TAG_DATA(wave_data + cur);
  cur += 4;

  if (len < cur + raw_len)
    return FALSE;

  OS_SpinWait(67 * DSP_WAIT_SHUTTER * 1000); // Approximately 60 msec

  while (SNDEXi_PreProcessForShutterSound() != SNDEX_RESULT_SUCCESS) {
    OS_Sleep(1); // Retry until successful
  }

  {
    u32 ctrl = DSP_AUDIO_DRIVER_MODE_MONAURAL;

    ctrl |= DSP_AUDIO_DRIVER_MODE_HALFVOL;
    DSPi_PlaySoundEx((wave_data + cur), raw_len, ctrl,
                     DSP_SOUND_PRIORITY_SHUTTER);
    DSPiPlayingShutter = TRUE;
  }

  return TRUE;
}
#endif

void DSPi_StopSoundCore(void) {

  DSPProcessContext *context = DSP_FindProcess(NULL);
  if (context && DSPiSoundPlaying) {
    int ctrl = 0;
    ctrl |= DSP_AUDIO_DRIVER_TARGET_OUTPUT;
    ctrl |= DSP_AUDIO_DRIVER_CONTROL_STOP;
    {
      DSPAudioDriverCommand command;
      command.ctrl = DSP_32BIT_TO_DSP(ctrl);
      command.buf = DSP_32BIT_TO_DSP(0);
      command.len = DSP_32BIT_TO_DSP(0);
      command.opt = DSP_32BIT_TO_DSP(0);
      DSP_WriteProcessPipe(context, DSP_PIPE_AUDIO, &command, sizeof(command));
    }
  }
}

BOOL DSPi_IsSoundPlayingCore(void) { return DSPiSoundPlaying; }

BOOL DSPi_IsShutterSoundPlayingCore(void) {
  return (DSPiSoundPlaying | DSPiPlayingShutter);
}

void DSPi_StartSamplingCore(void *buffer, u32 length) {
  SDK_ALIGN2_ASSERT(buffer);
  SDK_ALIGN2_ASSERT(length);
  {

    DSPProcessContext *context = DSP_FindProcess(NULL);
    if (context) {
      int ctrl = 0;
      ctrl |= DSP_AUDIO_DRIVER_TARGET_INPUT;
      ctrl |= DSP_AUDIO_DRIVER_CONTROL_START;
      DSPiLocalRingLength = length;
      DSPiLocalRingBuffer = (u8 *)buffer;
      DSPiLocalRingOffset = 0;
      DSPiAudioCaptureAddress = 0;
      DSPiReadPosition = 0;

      DSP_SetPipeCallback(DSP_PIPE_AUDIO, DSPi_PipeCallbackForSound, NULL);
      {
        DSPAudioDriverCommand command;
        command.ctrl = DSP_32BIT_TO_DSP(ctrl);
        command.buf = DSP_32BIT_TO_DSP(0);
        command.len = DSP_32BIT_TO_DSP(0);
        command.opt = DSP_32BIT_TO_DSP(0);
        DSP_WriteProcessPipe(context, DSP_PIPE_AUDIO, &command,
                             sizeof(command));
      }
    }
  }
}

void DSPi_StopSamplingCore(void) {

  DSPProcessContext *context = DSP_FindProcess(NULL);
  if (context) {
    int ctrl = 0;
    ctrl |= DSP_AUDIO_DRIVER_TARGET_INPUT;
    ctrl |= DSP_AUDIO_DRIVER_CONTROL_STOP;

    DSP_SetPipeCallback(DSP_PIPE_AUDIO, DSPi_PipeCallbackForSound, NULL);
    {
      DSPAudioDriverCommand command;
      command.ctrl = DSP_32BIT_TO_DSP(ctrl);
      command.buf = DSP_32BIT_TO_DSP(0);
      command.len = DSP_32BIT_TO_DSP(0);
      command.opt = DSP_32BIT_TO_DSP(0);
      DSP_WriteProcessPipe(context, DSP_PIPE_AUDIO, &command, sizeof(command));
    }
  }
}

void DSPi_SyncSamplingBufferCore(void) {

  if (DSPiAudioCaptureAddress != 0) {

    DSP_LoadData(DSP_ADDR_TO_ARM(DSPiAudioCaptureAddress), &DSPiAudioCapture,
                 sizeof(DSPiAudioCapture));
    if (DSPiAudioCapture.currentPosition != DSPiReadPosition) {

      int cur = DSPiAudioCapture.currentPosition;
      int end = (DSPiReadPosition > cur) ? DSPiAudioCapture.bufferLength : cur;
      int len = end - DSPiReadPosition;
      while (len > 0) {
        int segment = (int)MATH_MIN(
            len, DSP_WORD_TO_DSP32(DSPiLocalRingLength - DSPiLocalRingOffset));
        DSP_LoadData(
            DSP_ADDR_TO_ARM(DSPiAudioCapture.bufferAddress + DSPiReadPosition),
            &DSPiLocalRingBuffer[DSPiLocalRingOffset],
            DSP_WORD_TO_ARM(segment));
        len -= segment;
        DSPiReadPosition += segment;
        if (DSPiReadPosition >= DSPiAudioCapture.bufferLength) {
          DSPiReadPosition -= DSPiAudioCapture.bufferLength;
        }
        DSPiLocalRingOffset += (int)DSP_WORD_TO_ARM32(segment);
        if (DSPiLocalRingOffset >= DSPiLocalRingLength) {
          DSPiLocalRingOffset -= DSPiLocalRingLength;
        }
      }
    }
  }
}

const u8 *DSPi_GetLastSamplingAddressCore(void) {
  int offset = DSPiLocalRingOffset - (int)sizeof(u16);
  if (offset < 0) {
    offset += DSPiLocalRingLength;
  }
  return &DSPiLocalRingBuffer[offset];
}

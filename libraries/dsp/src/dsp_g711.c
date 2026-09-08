#include <twl.h>
#include <twl/dsp.h>

#include <twl/dsp/common/pipe.h>
#include <twl/dsp/common/g711.h>

#include "dsp_process.h"

static BOOL DSPiG711Available = FALSE;
static u32 DSPiG711CommandSend = 0;
static u32 DSPiG711CommandRecv = 0;
static DSPProcessContext DSPiProcessG711[1];

void DSPi_OpenStaticComponentG711Core(FSFile *file) {
  extern const u8 DSPiFirmware_audio[];
  (void)DSPi_CreateMemoryFile(file, DSPiFirmware_audio);
}

static void DSPi_SendCodecG711(void *dst, const void *src, u32 len,
                               DSPAudioCodecMode flags) {
  DSPAudioCodecCommand command;
  command.ctrl = DSP_32BIT_TO_DSP(flags);
  command.src = DSP_32BIT_TO_DSP((DSPAddrInARM)src);
  command.dst = DSP_32BIT_TO_DSP((DSPAddrInARM)dst);
  command.len = DSP_32BIT_TO_DSP(len);
  DSP_WriteProcessPipe(DSPiProcessG711, DSP_PIPE_BINARY, &command,
                       sizeof(command));

  ++DSPiG711CommandSend;
}

BOOL DSPi_LoadG711Core(FSFile *file, int slotB, int slotC) {
  if (!DSPiG711Available) {
    DSP_InitProcessContext(DSPiProcessG711, "g711");
    if (DSP_ExecuteProcess(DSPiProcessG711, file, slotB, slotC)) {
      DSPiG711Available = TRUE;
    }
  }
  return DSPiG711Available;
}

void DSPi_UnloadG711Core(void) {
  if (DSPiG711Available) {
    DSP_QuitProcess(DSPiProcessG711);
    DSPiG711Available = FALSE;
  }
}

void DSPi_EncodeG711Core(void *dst, const void *src, u32 len,
                         DSPAudioCodecMode mode) {
  DSPi_SendCodecG711(dst, src, len,
                     (DSPAudioCodecMode)(DSP_AUDIO_CODEC_TYPE_ENCODE |
                                         (mode & DSP_AUDIO_CODEC_MODE_MASK)));
}

void DSPi_DecodeG711Core(void *dst, const void *src, u32 len,
                         DSPAudioCodecMode mode) {
  DSPi_SendCodecG711(dst, src, len,
                     (DSPAudioCodecMode)(DSP_AUDIO_CODEC_TYPE_DECODE |
                                         (mode & DSP_AUDIO_CODEC_MODE_MASK)));
}

BOOL DSPi_TryWaitForG711Core(void) {
  BOOL retval = (DSPiG711CommandSend == DSPiG711CommandRecv);
  if (!retval) {
    DSPByte32 ack;
    DSPPipe binin[1];
    (void)DSP_LoadPipe(binin, DSP_PIPE_BINARY, DSP_PIPE_INPUT);
    if (DSP_GetPipeReadableSize(binin) >= sizeof(ack)) {
      DSP_ReadPipe(binin, &ack, sizeof(ack));
      ++DSPiG711CommandRecv;
      retval = (DSPiG711CommandSend == DSPiG711CommandRecv);
    }
  }
  return retval;
}

void DSPi_WaitForG711Core(void) {
  if (DSPiG711CommandSend != DSPiG711CommandRecv) {
    DSPByte32 ack;
    DSPPipe binin[1];
    (void)DSP_LoadPipe(binin, DSP_PIPE_BINARY, DSP_PIPE_INPUT);
    while (DSPiG711CommandSend != DSPiG711CommandRecv) {
      DSP_ReadPipe(binin, &ack, sizeof(ack));
      ++DSPiG711CommandRecv;
    }
  }
}

#include <twl.h>
#include <twl/dsp.h>
#include <twl/dsp/common/graphics.h>

#include "dsp_process.h"

extern const u8 DSPiFirmware_graphics[];

static DSPPipe binOut[1];
static BOOL DSPiGraphicsAvailable = FALSE;
static DSPProcessContext DSPiProcessGraphics[1];

static u16 replyReg;

static BOOL CheckLimitation(f32 rx, f32 ry, DSPGraphicsScalingMode mode,
                            u16 src_width);

static BOOL CheckLimitation(f32 rx, f32 ry, DSPGraphicsScalingMode mode,
                            u16 src_width) {

  if (rx * ry >= 1.0f) {
    switch (mode) {
    case DSP_GRAPHICS_SCALING_MODE_N_NEIGHBOR:
    case DSP_GRAPHICS_SCALING_MODE_BILINEAR:
      if (DSP_CALC_SCALING_SIZE(src_width, rx) * ry >= 8192) {
        return FALSE;
      }
      break;
    case DSP_GRAPHICS_SCALING_MODE_BICUBIC:
      if (DSP_CALC_SCALING_SIZE(src_width, rx) * ry >= 4096) {
        return FALSE;
      }
      break;
    }
  } else // When the processed data size is less than the original data size
  {
    switch (mode) {
    case DSP_GRAPHICS_SCALING_MODE_N_NEIGHBOR:
    case DSP_GRAPHICS_SCALING_MODE_BILINEAR:
      if (src_width >= 8192) {
        return FALSE;
      }
      break;
    case DSP_GRAPHICS_SCALING_MODE_BICUBIC:
      if (src_width >= 4096) {
        return FALSE;
      }
      break;
    }
  }
  return TRUE;
}

void DSPi_OpenStaticComponentGraphicsCore(FSFile *file) {
  extern const u8 DSPiFirmware_graphics[];
  (void)DSPi_CreateMemoryFile(file, DSPiFirmware_graphics);
}

static void DSPi_GraphicsEvents(void *userdata) {
  (void)userdata;

  if (isAsync) {
    replyReg = DSP_RecvData(DSP_GRAPHICS_REP_REGISTER);
    if (replyReg == DSP_STATE_SUCCESS) {
      isBusy = FALSE;
      isAsync = FALSE;

      if (callBackFunc != NULL) {
        callBackFunc();
      }
    } else if (replyReg == DSP_STATE_FAIL) {
      OS_TWarning("a process on DSP is failed.\n");
      isBusy = FALSE;
      isAsync = FALSE;
    } else {
      OS_TWarning("unknown error occured.\n");
      isBusy = FALSE;
      isAsync = FALSE;
    }
  } else // Synchronous version
  {
  }
}

BOOL DSPi_LoadGraphicsCore(FSFile *file, int slotB, int slotC) {
  if (!DSPiGraphicsAvailable) {
    isBusy = FALSE;
    isAsync = FALSE;
    DSP_InitProcessContext(DSPiProcessGraphics, "graphics");

    DSPiProcessGraphics->flags |= DSP_PROCESS_FLAG_USING_OS;
    DSP_SetProcessHook(DSPiProcessGraphics,
                       DSP_HOOK_REPLY_(DSP_GRAPHICS_REP_REGISTER),
                       DSPi_GraphicsEvents, NULL);
    if (DSP_ExecuteProcess(DSPiProcessGraphics, file, slotB, slotC)) {
      DSPiGraphicsAvailable = TRUE;
    }

    (void)DSP_LoadPipe(binOut, DSP_PIPE_BINARY, DSP_PIPE_OUTPUT);
  }

  return DSPiGraphicsAvailable;
}

void DSPi_UnloadGraphicsCore(void) {
  if (DSPiGraphicsAvailable) {
    DSP_QuitProcess(DSPiProcessGraphics);
    DSPiGraphicsAvailable = FALSE;
  }
}

BOOL DSPi_ConvertYuvToRgbCore(const void *src, void *dst, u32 size,
                              DSP_GraphicsCallback callback, BOOL async) {
  DSPYuv2RgbParam yr_param;
  u32 offset = 0;
  u16 command;

  if (isBusy) {
    OS_TPrintf("dsp is busy.\n");
    return FALSE;
  }
  isBusy = TRUE;

  callBackFunc = callback;
  isAsync = async;

  if (async) {
    DSP_SetProcessHook(DSPiProcessGraphics,
                       DSP_HOOK_REPLY_(DSP_GRAPHICS_REP_REGISTER),
                       DSPi_GraphicsEvents, NULL);
  } else {
    DSP_SetProcessHook(DSPiProcessGraphics,
                       DSP_HOOK_REPLY_(DSP_GRAPHICS_REP_REGISTER), NULL, NULL);
  }

  command = DSP_G_FUNCID_YUV2RGB;
  DSP_SendData(DSP_GRAPHICS_COM_REGISTER, command);

  yr_param.size = size;
  yr_param.src = (u32)((u32)src + offset);
  yr_param.dst = (u32)((u32)dst + offset);

  DSP_WritePipe(binOut, &yr_param, sizeof(DSPYuv2RgbParam));

  if (async) {
    return TRUE;
  } else {

    while (!DSP_RecvDataIsReady(DSP_GRAPHICS_REP_REGISTER)) {
      OS_Sleep(1);
    }
    replyReg = DSP_RecvData(DSP_GRAPHICS_REP_REGISTER);
    isBusy = FALSE;

    if (replyReg == DSP_STATE_SUCCESS) {
      return TRUE;
    } else {
      return FALSE;
    }
  }

  return FALSE;
}

BOOL DSPi_ScalingCore(const void *src, void *dst, u16 img_width, u16 img_height,
                      f32 rx, f32 ry, DSPGraphicsScalingMode mode, u16 x, u16 y,
                      u16 width, u16 height, DSP_GraphicsCallback callback,
                      BOOL async) {
  DSPScalingParam sc_param;

  u16 command;

  SDK_TASSERTMSG(CheckLimitation(rx, ry, mode, width),
                 "DSP_Scaling: arguments exceed the limit.");

  if (isBusy) {
    OS_TPrintf("dsp is busy.\n");
    return FALSE;
  }
  isBusy = TRUE;

  callBackFunc = callback;
  isAsync = async;
  if (async) {
    DSP_SetProcessHook(DSPiProcessGraphics,
                       DSP_HOOK_REPLY_(DSP_GRAPHICS_REP_REGISTER),
                       DSPi_GraphicsEvents, NULL);
  } else {
    DSP_SetProcessHook(DSPiProcessGraphics,
                       DSP_HOOK_REPLY_(DSP_GRAPHICS_REP_REGISTER), NULL, NULL);
  }

  command = DSP_G_FUNCID_SCALING;
  DSP_SendData(DSP_GRAPHICS_COM_REGISTER, command);

  sc_param.src = (u32)src;
  sc_param.dst = (u32)dst;
  sc_param.mode = mode;
  sc_param.img_width = img_width;
  sc_param.img_height = img_height;
  sc_param.rate_w = (u16)(rx * 1000);
  sc_param.rate_h = (u16)(ry * 1000);
  sc_param.block_x = x;
  sc_param.block_y = y;
  sc_param.width = width;
  sc_param.height = height;

  DSP_WritePipe(binOut, &sc_param, sizeof(DSPScalingParam));

  if (isAsync) {
    return TRUE;
  } else {

    while (!DSP_RecvDataIsReady(DSP_GRAPHICS_REP_REGISTER)) {
      OS_Sleep(1);
    }

    replyReg = DSP_RecvData(DSP_GRAPHICS_REP_REGISTER);
    isBusy = FALSE;

    if (replyReg == DSP_STATE_SUCCESS) {
      return TRUE;
    } else {
      return FALSE;
    }
  }

  return FALSE;
}

BOOL DSPi_ScalingFxCore(const void *src, void *dst, u16 img_width,
                        u16 img_height, fx32 rx, fx32 ry,
                        DSPGraphicsScalingMode mode, u16 x, u16 y, u16 width,
                        u16 height, DSP_GraphicsCallback callback, BOOL async) {
  DSPScalingParam sc_param;

  u16 command;

  SDK_TASSERTMSG(
      CheckLimitation(FX_FX32_TO_F32(rx), FX_FX32_TO_F32(ry), mode, width),
      "DSP_Scaling: arguments exceed the limit.");

  if (isBusy) {
    OS_TPrintf("dsp is busy.\n");
    return FALSE;
  }
  isBusy = TRUE;

  callBackFunc = callback;
  isAsync = async;

  if (async) {
    DSP_SetProcessHook(DSPiProcessGraphics,
                       DSP_HOOK_REPLY_(DSP_GRAPHICS_REP_REGISTER),
                       DSPi_GraphicsEvents, NULL);
  } else {
    DSP_SetProcessHook(DSPiProcessGraphics,
                       DSP_HOOK_REPLY_(DSP_GRAPHICS_REP_REGISTER), NULL, NULL);
  }

  command = DSP_G_FUNCID_SCALING;
  DSP_SendData(DSP_GRAPHICS_COM_REGISTER, command);

  sc_param.src = (u32)src;
  sc_param.dst = (u32)dst;
  sc_param.mode = mode;
  sc_param.img_width = img_width;
  sc_param.img_height = img_height;
  sc_param.rate_w = (u16)(rx / 4096.0f * 1000);
  sc_param.rate_h = (u16)(ry / 4096.0f * 1000);
  sc_param.block_x = x;
  sc_param.block_y = y;
  sc_param.width = width;
  sc_param.height = height;

  DSP_WritePipe(binOut, &sc_param, sizeof(DSPScalingParam));

  if (isAsync) {
    return TRUE;
  } else {

    while (!DSP_RecvDataIsReady(DSP_GRAPHICS_REP_REGISTER)) {
      OS_Sleep(1);
    }
    replyReg = DSP_RecvData(DSP_GRAPHICS_REP_REGISTER);
    isBusy = FALSE;

    if (replyReg == DSP_STATE_SUCCESS) {
      return TRUE;
    } else {
      return FALSE;
    }
  }

  return FALSE;
}

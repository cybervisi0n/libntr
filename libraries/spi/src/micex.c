#include <nitro/os.h>
#include <nitro/spi.h>
#include "micex.h"

#include <twl/spi/common/mic_common.h>

static void MicexSyncCallback(MICResult result, void *arg);
static BOOL MicexStartLimitedSampling(void *buf, u32 size, u32 rate, u8 flags);
static BOOL MicexStopLimitedSampling(void);
static BOOL MicexAdjustLimitedSampling(u32 rate);

MICResult MICEXi_StartLimitedSampling(const MICAutoParam *param) {
  MICResult result;
  OSMessageQueue msgQ;
  OSMessage msg[1];

  if (OS_GetCurrentThread() == NULL) {
#ifdef SDK_DEBUG
    OS_TWarning("%s: Could not process in exception handler.\n", __FUNCTION__);
#endif
    return MIC_RESULT_ILLEGAL_STATUS;
  }

  OS_InitMessageQueue(&msgQ, msg, 1);

  result = MICEXi_StartLimitedSamplingAsync(param, MicexSyncCallback,
                                            (void *)(&msgQ));
  if (result == MIC_RESULT_SUCCESS) {
    (void)OS_ReceiveMessage(&msgQ, (OSMessage *)(&result), OS_MESSAGE_BLOCK);
  }
  return result;
}

MICResult MICEXi_StartLimitedSamplingAsync(const MICAutoParam *param,
                                           MICCallback callback, void *arg) {
  OSIntrMode e;
  MICWork *w = MICi_GetSysWork();
  u8 flags;

  SDK_NULL_ASSERT(callback);
  SDK_NULL_ASSERT(param->buffer);

  {

    if (param->size <= 0) {
#ifdef SDK_DEBUG
      OS_TWarning("%s: Illegal desination buffer size. (%d)\n", __FUNCTION__,
                  param->size);
#endif
      return MIC_RESULT_ILLEGAL_PARAMETER;
    }

    if (((u32)(param->buffer) % HW_CACHE_LINE_SIZE != 0) ||
        ((param->size % HW_CACHE_LINE_SIZE) != 0)) {
#ifdef SDK_DEBUG
      OS_TWarning("%s: Destination buffer (%p - %p) is not aligned on %d bytes "
                  "boundary.\n",
                  __FUNCTION__, param->buffer,
                  (void *)((u32)param->buffer + param->size),
                  HW_CACHE_LINE_SIZE);
#endif
      return MIC_RESULT_ILLEGAL_PARAMETER;
    }

    switch (param->type) {
    case MIC_SAMPLING_TYPE_8BIT:
      flags = SPI_MIC_SAMPLING_TYPE_8BIT;
      break;
    case MIC_SAMPLING_TYPE_12BIT:
      flags = SPI_MIC_SAMPLING_TYPE_12BIT;
      break;
    case MIC_SAMPLING_TYPE_SIGNED_8BIT:
      flags = SPI_MIC_SAMPLING_TYPE_S8BIT;
      break;
    case MIC_SAMPLING_TYPE_SIGNED_12BIT:
      flags = SPI_MIC_SAMPLING_TYPE_S12BIT;
      break;
    case MIC_SAMPLING_TYPE_12BIT_FILTER_OFF:
      flags = (SPI_MIC_SAMPLING_TYPE_12BIT | SPI_MIC_SAMPLING_TYPE_FILTER_OFF);
      break;
    case MIC_SAMPLING_TYPE_SIGNED_12BIT_FILTER_OFF:
      flags = (SPI_MIC_SAMPLING_TYPE_S12BIT | SPI_MIC_SAMPLING_TYPE_FILTER_OFF);
      break;
    default:
#ifdef SDK_DEBUG
      OS_TWarning("%s: Illegal sampling type. (%d)\n", __FUNCTION__,
                  param->type);
#endif
      return MIC_RESULT_ILLEGAL_PARAMETER;
    }

    switch (param->rate) {
    case MIC_SAMPLING_RATE_32730:
    case MIC_SAMPLING_RATE_16360:
    case MIC_SAMPLING_RATE_10910:
    case MIC_SAMPLING_RATE_8180:
    case MIC_SAMPLING_RATE_47610:
    case MIC_SAMPLING_RATE_23810:
    case MIC_SAMPLING_RATE_15870:
    case MIC_SAMPLING_RATE_11900:
      break;
    default:
#ifdef SDK_TWL
      OS_TWarning("%s: Illegal sampling rate. (%d)\n", __FUNCTION__,
                  param->rate);
#endif
      return MIC_RESULT_ILLEGAL_PARAMETER;
    }

    if (param->loop_enable) {
      flags = (u8)((flags & ~SPI_MIC_SAMPLING_TYPE_LOOP_MASK) |
                   SPI_MIC_SAMPLING_TYPE_LOOP_ON);
    } else {
      flags = (u8)((flags & ~SPI_MIC_SAMPLING_TYPE_LOOP_MASK) |
                   SPI_MIC_SAMPLING_TYPE_LOOP_OFF);
    }

    flags = (u8)((flags & ~SPI_MIC_SAMPLING_TYPE_CORRECT_MASK) |
                 SPI_MIC_SAMPLING_TYPE_CORRECT_OFF);
  }

  e = OS_DisableInterrupts();
  if (w->lock != MIC_LOCK_OFF) {
    (void)OS_RestoreInterrupts(e);
    return MIC_RESULT_BUSY;
  }
  w->lock = MIC_LOCK_ON;
  (void)OS_RestoreInterrupts(e);

  w->callback = callback;
  w->callbackArg = arg;
  w->full = param->full_callback;
  w->fullArg = param->full_arg;
  if (MicexStartLimitedSampling(param->buffer, param->size, param->rate,
                                flags)) {
    return MIC_RESULT_SUCCESS;
  }
  w->lock = MIC_LOCK_OFF;
  return MIC_RESULT_SEND_ERROR;
}

MICResult MICEXi_StopLimitedSampling(void) {
  MICResult result;
  OSMessageQueue msgQ;
  OSMessage msg[1];

  if (OS_GetCurrentThread() == NULL) {
#ifdef SDK_DEBUG
    OS_TWarning("%s: Could not process in exception handler.\n", __FUNCTION__);
#endif
    return MIC_RESULT_ILLEGAL_STATUS;
  }

  OS_InitMessageQueue(&msgQ, msg, 1);

  result = MICEXi_StopLimitedSamplingAsync(MicexSyncCallback, (void *)(&msgQ));
  if (result == MIC_RESULT_SUCCESS) {
    (void)OS_ReceiveMessage(&msgQ, (OSMessage *)(&result), OS_MESSAGE_BLOCK);
  }
  return result;
}

MICResult MICEXi_StopLimitedSamplingAsync(MICCallback callback, void *arg) {
  OSIntrMode e;
  MICWork *w = MICi_GetSysWork();

  SDK_NULL_ASSERT(callback);

  e = OS_DisableInterrupts();
  if (w->lock != MIC_LOCK_OFF) {
    (void)OS_RestoreInterrupts(e);
    return MIC_RESULT_BUSY;
  }
  w->lock = MIC_LOCK_ON;
  (void)OS_RestoreInterrupts(e);

  w->callback = callback;
  w->callbackArg = arg;
  if (MicexStopLimitedSampling()) {
    return MIC_RESULT_SUCCESS;
  }
  w->lock = MIC_LOCK_OFF;
  return MIC_RESULT_SEND_ERROR;
}

MICResult MICEXi_AdjustLimitedSampling(u32 rate) {
  MICResult result;
  OSMessageQueue msgQ;
  OSMessage msg[1];

  if (OS_GetCurrentThread() == NULL) {
#ifdef SDK_DEBUG
    OS_TWarning("%s: Could not process in exception handler.\n", __FUNCTION__);
#endif
    return MIC_RESULT_ILLEGAL_STATUS;
  }

  OS_InitMessageQueue(&msgQ, msg, 1);

  result = MICEXi_AdjustLimitedSamplingAsync(rate, MicexSyncCallback,
                                             (void *)(&msgQ));
  if (result == MIC_RESULT_SUCCESS) {
    (void)OS_ReceiveMessage(&msgQ, (OSMessage *)(&result), OS_MESSAGE_BLOCK);
  }
  return result;
}

MICResult MICEXi_AdjustLimitedSamplingAsync(u32 rate, MICCallback callback,
                                            void *arg) {
  OSIntrMode e;
  MICWork *w = MICi_GetSysWork();

  SDK_NULL_ASSERT(callback);

  switch (rate) {
  case MIC_SAMPLING_RATE_32730:
  case MIC_SAMPLING_RATE_16360:
  case MIC_SAMPLING_RATE_10910:
  case MIC_SAMPLING_RATE_8180:
  case MIC_SAMPLING_RATE_47610:
  case MIC_SAMPLING_RATE_23810:
  case MIC_SAMPLING_RATE_15870:
  case MIC_SAMPLING_RATE_11900:
    break;
  default:
#ifdef SDK_DEBUG
    OS_TWarning("%s: Illegal sampling rate. (%d)\n", __FUNCTION__, rate);
#endif
    return MIC_RESULT_ILLEGAL_PARAMETER;
  }

  e = OS_DisableInterrupts();
  if (w->lock != MIC_LOCK_OFF) {
    (void)OS_RestoreInterrupts(e);
    return MIC_RESULT_BUSY;
  }
  w->lock = MIC_LOCK_ON;
  (void)OS_RestoreInterrupts(e);

  w->callback = callback;
  w->callbackArg = arg;
  if (MicexAdjustLimitedSampling(rate)) {
    return MIC_RESULT_SUCCESS;
  }
  w->lock = MIC_LOCK_OFF;
  return MIC_RESULT_SEND_ERROR;
}

static void MicexSyncCallback(MICResult result, void *arg) {
  SDK_NULL_ASSERT(arg);

  (void)OS_SendMessage((OSMessageQueue *)arg, (OSMessage)result,
                       OS_MESSAGE_NOBLOCK);
}

static BOOL MicexStartLimitedSampling(void *buf, u32 size, u32 rate, u8 flags) {

  if (0 >
      PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
                         SPI_PXI_START_BIT | (0 << SPI_PXI_INDEX_SHIFT) |
                             (SPI_PXI_COMMAND_MIC_LTDAUTO_ON << 8) | (u32)flags,
                         0)) {
    return FALSE;
  }

  if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
                             (1 << SPI_PXI_INDEX_SHIFT) | ((u32)buf >> 16),
                             0)) {
    return FALSE;
  }

  if (0 > PXI_SendWordByFifo(
              PXI_FIFO_TAG_MIC,
              (2 << SPI_PXI_INDEX_SHIFT) | ((u32)buf & 0x0000ffff), 0)) {
    return FALSE;
  }

  if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
                             (3 << SPI_PXI_INDEX_SHIFT) | (size >> 16), 0)) {
    return FALSE;
  }

  if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
                             (4 << SPI_PXI_INDEX_SHIFT) | (size & 0x0000ffff),
                             0)) {
    return FALSE;
  }

  if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
                             (5 << SPI_PXI_INDEX_SHIFT) | (rate >> 16), 0)) {
    return FALSE;
  }

  if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
                             SPI_PXI_END_BIT | (6 << SPI_PXI_INDEX_SHIFT) |
                                 (rate & 0x0000ffff),
                             0)) {
    return FALSE;
  }
  return TRUE;
}

static BOOL MicexStopLimitedSampling(void) {

  if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
                             SPI_PXI_START_BIT | SPI_PXI_END_BIT |
                                 (0 << SPI_PXI_INDEX_SHIFT) |
                                 (SPI_PXI_COMMAND_MIC_LTDAUTO_OFF << 8),
                             0)) {
    return FALSE;
  }
  return TRUE;
}

static BOOL MicexAdjustLimitedSampling(u32 rate) {

  if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
                             SPI_PXI_START_BIT | (0 << SPI_PXI_INDEX_SHIFT) |
                                 (SPI_PXI_COMMAND_MIC_LTDAUTO_ADJUST << 8),
                             0)) {
    return FALSE;
  }

  if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
                             (1 << SPI_PXI_INDEX_SHIFT) | (rate >> 16), 0)) {
    return FALSE;
  }

  if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
                             SPI_PXI_END_BIT | (2 << SPI_PXI_INDEX_SHIFT) |
                                 (rate & 0x0000ffff),
                             0)) {
    return FALSE;
  }
  return TRUE;
}

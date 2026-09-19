#include <twl.h>

#include "nwm_common_private.h"
#include "nwm_arm9_private.h"

static u32 *NwmGetCommandBuffer4Arm7(void);

void NWMi_SetCallbackTable(NWMApiid id, NWMCallbackFunc callback) {
  NWMArm9Buf *sys = NWMi_GetSystemWork();

  SDK_NULL_ASSERT(sys);

  sys->callbackTable[id] = callback;
}

void NWMi_SetReceiveCallbackTable(NWMFramePort port, NWMCallbackFunc callback) {
  NWMArm9Buf *sys = NWMi_GetSystemWork();

  SDK_NULL_ASSERT(sys);

  sys->recvCallbackTable[port] = callback;
}

NWMRetCode NWMi_SendCommand(NWMApiid id, u16 paramNum, ...) {
  va_list vlist;
  s32 i;
  int result;
  u32 *tmpAddr;
  NWMArm9Buf *sys = NWMi_GetSystemWork();

  tmpAddr = NwmGetCommandBuffer4Arm7();
  if (tmpAddr == NULL) {
    NWM_WARNING("Failed to get command buffer.\n");
    return NWM_RETCODE_FIFO_ERROR;
  }

  *(u16 *)tmpAddr = (u16)id;

  va_start(vlist, paramNum);
  for (i = 0; i < paramNum; i++) {
    tmpAddr[i + 1] = va_arg(vlist, u32);
  }
  va_end(vlist);

  DC_StoreRange(tmpAddr, NWM_APIFIFO_BUF_SIZE);

  result = PXI_SendWordByFifo(PXI_FIFO_TAG_WMW, (u32)tmpAddr, FALSE);

  (void)OS_SendMessage(&sys->apibufQ.q, tmpAddr, OS_MESSAGE_BLOCK);

  if (result < 0) {
    NWM_WARNING("Failed to send command through FIFO.\n");
    return NWM_RETCODE_FIFO_ERROR;
  }

  return NWM_RETCODE_OPERATING;
}

NWMRetCode NWMi_SendCommandDirect(void *data, u32 length) {
  int result;
  u32 *tmpAddr;
  NWMArm9Buf *sys = NWMi_GetSystemWork();

  SDK_ASSERT(length <= NWM_APIFIFO_BUF_SIZE);

  tmpAddr = NwmGetCommandBuffer4Arm7();
  if (tmpAddr == NULL) {
    NWM_WARNING("Failed to get command buffer.\n");
    return NWM_RETCODE_FIFO_ERROR;
  }

  MI_CpuCopy8(data, tmpAddr, length);

  DC_StoreRange(tmpAddr, length);

  result = PXI_SendWordByFifo(PXI_FIFO_TAG_WMW, (u32)tmpAddr, FALSE);

  (void)OS_SendMessage(&sys->apibufQ.q, tmpAddr, OS_MESSAGE_BLOCK);

  if (result < 0) {
    NWM_WARNING("Failed to send command through FIFO.\n");
    return NWM_RETCODE_FIFO_ERROR;
  }

  return NWM_RETCODE_OPERATING;
}

u32 *NwmGetCommandBuffer4Arm7(void) {
  u32 *tmpAddr = NULL;
  NWMArm9Buf *sys = NWMi_GetSystemWork();

  do {
    if (FALSE == OS_ReceiveMessage(&sys->apibufQ.q, (OSMessage *)&tmpAddr,
                                   OS_MESSAGE_NOBLOCK)) {
      return NULL;
    }

    DC_InvalidateRange(sys->apibuf, NWM_APIBUF_NUM * NWM_APIFIFO_BUF_SIZE);

    DC_InvalidateRange(tmpAddr, sizeof(u16));

    if ((*((u16 *)tmpAddr) & NWM_API_REQUEST_ACCEPTED) == 0) {

      (void)OS_SendMessage(&sys->apibufQ.q, tmpAddr, OS_MESSAGE_BLOCK);
      tmpAddr = NULL;
      continue;
    }
  } while (tmpAddr == NULL);

  return tmpAddr;
}

#include <twl.h>
#include "nwm_common_private.h"
#include "nwm_arm9_private.h"

NWMRetCode NWM_End(void) {
  NWMRetCode result;
  OSIntrMode e;
  extern NWMArm9Buf *nwm9buf;
  extern u8 nwmInitialized;

  SDK_NULL_ASSERT(nwm9buf);

  result = NWMi_CheckState(1, NWM_STATE_INITIALIZED);
  NWM_CHECK_RESULT(result);

  e = OS_DisableInterrupts();

  nwm9buf->status->state =
      NWM_STATE_NONE; // As an exception, set the initial state here alone (this
                      // was originally done by the ARM7)

  NWMi_ClearFifoRecvFlag();

  PXI_SetFifoRecvCallback(PXI_FIFO_TAG_WMW, NULL);
  nwm9buf = 0;

  nwmInitialized = 0;
  (void)OS_RestoreInterrupts(e);
  return NWM_RETCODE_SUCCESS;
}

#include <nitro.h>

static vu16 OSi_IsResetOccurred = FALSE;
static vu16 OSi_IsTerminateOccurred = FALSE;

BOOL OS_IsResetOccurred(void) { return OSi_IsResetOccurred; }

#ifdef SDK_TWL

BOOL OSi_IsTerminatePxiOccurred(void) { return OSi_IsTerminateOccurred; }

void OSi_SetTerminatePxiOccurred(void) { OSi_IsTerminateOccurred = TRUE; }

#endif // SDK_TWL

#ifdef SDK_PORT
void OSi_CommonCallback(PXIFifoTag tag, u64 data, BOOL err)
#else
void OSi_CommonCallback(PXIFifoTag tag, u32 data, BOOL err)
#endif
{
#pragma unused(tag, err)
  u16 command;
  u16 commandArg;

  command = (u16)((data & OS_PXI_COMMAND_MASK) >> OS_PXI_COMMAND_SHIFT);
  commandArg = (u16)((data & OS_PXI_DATA_MASK) >> OS_PXI_DATA_SHIFT);

#ifdef SDK_ARM9

  if (command == OS_PXI_COMMAND_RESET) {
    OSi_IsResetOccurred = TRUE;
  }
#ifdef SDK_TWL

  else if (command == OS_PXI_COMMAND_TERMINATE) {
    OS_TPrintf("[ARM9] received terminate command\n");
    OSi_IsTerminateOccurred = TRUE;
    OS_Terminate();
  }
#endif // SDK_TWL

  else {
    OS_TPanic("[ARM9] unknown command");
  }

#else

  if (command == OS_PXI_COMMAND_RESET) {
    OSi_IsResetOccurred = TRUE;
  }
#ifdef SDK_TWL

  else if (command == OS_PXI_COMMAND_TERMINATE) {
    OSi_IsTerminateOccurred = TRUE;
    OS_Terminate();
  }
#endif // SDK_TWL

  else {
    OS_TPanic("[ARM7] unknown command");
  }
#endif
}

void OSi_SendToPxi(u16 data) {
  u32 pxi_send_data;

  pxi_send_data = ((u32)data) << OS_PXI_COMMAND_SHIFT;
  while (PXI_SendWordByFifo(PXI_FIFO_TAG_OS, pxi_send_data, FALSE) !=
         PXI_FIFO_SUCCESS) {
  }
}

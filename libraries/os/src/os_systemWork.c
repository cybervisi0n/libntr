#include <nitro/os.h>
#ifdef SDK_TWL
#include <nitro/os/common/emulator.h>
#include <twl/os/common/codecmode.h>
#endif

OSBootType OS_GetBootType(void) { return OS_GetBootInfo()->boot_type; }

const OSBootInfo *OS_GetBootInfo(void) {
  return ((const OSBootInfo *)HW_WM_BOOT_BUF);
}

#ifdef SDK_TWL

BOOL OSi_IsCodecTwlMode(void) {
  static BOOL retval;
  static BOOL initialized = FALSE;

  if (initialized == FALSE) {
    retval = ((OS_IsRunOnTwl() == TRUE) &&
              ((*((u8 *)(HW_TWL_ROM_HEADER_BUF + 0x01bf)) & 0x01) ==
               OS_CODECMODE_TWL));
  }
  initialized = TRUE;

  return retval;
}

#endif // SDK_TWL

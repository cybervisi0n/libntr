#include <nitro.h>
#include <twl/os/common/msJump.h>

#define TITLE_ID_MACHINE_SETTING                                               \
  0x00030015484e4241 /* HNBA and location of destination do not matter */

#ifdef SDK_TWL
#include <twl/ltdmain_begin.h>
#endif

BOOL OSi_JumpToMachineSetting(u8 dest) {
  switch (dest) {
    OSTWLRegion region = OS_GetRegion();

  case OS_TWL_MACHINE_SETTING_COUNTRY:

    switch (OS_GetRegion()) {
    case OS_TWL_REGION_AMERICA:
    case OS_TWL_REGION_EUROPE:
    case OS_TWL_REGION_AUSTRALIA:

      break;

    case OS_TWL_REGION_JAPAN:

    default:
      OS_TWarning("Region Error.");
      return FALSE;
    }
    break;

  case OS_TWL_MACHINE_SETTING_LANGUAGE:

    switch (OS_GetRegion()) {
    case OS_TWL_REGION_AMERICA:
    case OS_TWL_REGION_EUROPE:

      break;

    case OS_TWL_REGION_JAPAN:
    case OS_TWL_REGION_AUSTRALIA:

    default:
      OS_TWarning("Region Error.");
      return FALSE;
    }
    break;

  case OS_TWL_MACHINE_SETTING_PAGE_1:
  case OS_TWL_MACHINE_SETTING_PAGE_2:
  case OS_TWL_MACHINE_SETTING_PAGE_3:
  case OS_TWL_MACHINE_SETTING_PAGE_4:
  case OS_TWL_MACHINE_SETTING_APP_MANAGER:
  case OS_TWL_MACHINE_SETTING_WIRELESS_SW:
  case OS_TWL_MACHINE_SETTING_BRIGHTNESS:
  case OS_TWL_MACHINE_SETTING_USER_INFO:
  case OS_TWL_MACHINE_SETTING_DATE:
  case OS_TWL_MACHINE_SETTING_TIME:
  case OS_TWL_MACHINE_SETTING_ALARM:
  case OS_TWL_MACHINE_SETTING_TP_CALIBRATION:
  case OS_TWL_MACHINE_SETTING_PARENTAL_CONTROL:
  case OS_TWL_MACHINE_SETTING_NETWORK_SETTING:
  case OS_TWL_MACHINE_SETTING_NETWORK_EULA:
  case OS_TWL_MACHINE_SETTING_NETWORK_OPTION:
  case OS_TWL_MACHINE_SETTING_SYSTEM_UPDATE:
  case OS_TWL_MACHINE_SETTING_SYSTEM_INITIALIZE:
    break;
  default:
    OS_TWarning("Unknown Destination");
    return FALSE;
  }

  {
    OSDeliverArgInfo argInfo;
    int result;

    OS_InitDeliverArgInfo(&argInfo, 0);
    (void)OS_DecodeDeliverArg(); // There are cases where the DeliverArg is not
                                 // set in advance, so continue processing
                                 // regardless of whether it is right or wrong
    OSi_SetDeliverArgState(OS_DELIVER_ARG_BUF_ACCESSIBLE |
                           OS_DELIVER_ARG_BUF_WRITABLE);
    result = OS_SetSysParamToDeliverArg((u16)dest);

    if (result != OS_DELIVER_ARG_SUCCESS) {
      OS_TWarning("Failed to Set DeliverArgument.");
      return FALSE;
    }
    result = OS_EncodeDeliverArg();
    if (result != OS_DELIVER_ARG_SUCCESS) {
      OS_TWarning("Failed to Encode DeliverArgument.");
      return FALSE;
    }
  }

  return OS_DoApplicationJump(TITLE_ID_MACHINE_SETTING, OS_APP_JUMP_NORMAL);

  return FALSE;
}

BOOL OSi_JumpToEulaDirect(void) {
  return OSi_JumpToMachineSetting(OS_TWL_MACHINE_SETTING_NETWORK_EULA);
}

BOOL OSi_JumpToApplicationManagerDirect(void) {
  return OSi_JumpToMachineSetting(OS_TWL_MACHINE_SETTING_APP_MANAGER);
}

BOOL OSi_JumpToNetworkSettngDirect(void) {
  return OSi_JumpToMachineSetting(OS_TWL_MACHINE_SETTING_NETWORK_SETTING);
}

BOOL OSi_JumpToCountrySettingDirect(void) {
  return OSi_JumpToMachineSetting(OS_TWL_MACHINE_SETTING_COUNTRY);
}

BOOL OSi_JumpToSystemUpdateDirect(void) {
  return OSi_JumpToMachineSetting(OS_TWL_MACHINE_SETTING_SYSTEM_UPDATE);
}

#ifdef SDK_TWL
#include <twl/ltdmain_end.h>
#endif

BOOL OS_JumpToInternetSetting(void) {
  BOOL result = FALSE;
#ifdef SDK_TWL
  if (OS_IsRunOnTwl()) {
    result = OSi_JumpToNetworkSettngDirect();
  } else
#endif
  {
    OS_TWarning("This Hardware don't support this funciton");
  }
  return result;
}

BOOL OS_JumpToEULAViewer(void) {
  BOOL result = FALSE;
#ifdef SDK_TWL
  if (OS_IsRunOnTwl()) {
    result = OSi_JumpToEulaDirect();
  } else
#endif
  {
    OS_TWarning("This Hardware don't support this funciton");
  }
  return result;
}

BOOL OS_JumpToWirelessSetting(void) {
  BOOL result = FALSE;
#ifdef SDK_TWL
  if (OS_IsRunOnTwl()) {
    result = OSi_JumpToMachineSetting(OS_TWL_MACHINE_SETTING_WIRELESS_SW);
  } else
#endif
  {
    OS_TWarning("This Hardware don't support this funciton");
  }
  return result;
}

#include <nitro/os/common/ownerInfo.h>
#include <nitro/os/common/systemWork.h>
#include <nitro/os/common/emulator.h>
#include <twl/os/common/ownerInfoEx.h>
#include <twl/os/common/ownerInfoEx_private.h>
#include <twl/hw/common/mmap_parameter.h>

#ifdef SDK_ARM9
#include <twl/hw/ARM9/mmap_main.h>
#elif defined(SDK_PORT)
#include <twl/hw/X86/mmap_main.h>
#else // SDK_ARM7
#include <twl/hw/ARM7/mmap_main.h>
#endif

#define OS_ADDR_TWL_SETTINGSDATA                                               \
  ((OSTWLSettingsData *)HW_PARAM_TWL_SETTINGS_DATA)
#define OS_ADDR_TWL_HWNORMALINFO                                               \
  ((OSTWLHWNormalInfo *)HW_PARAM_TWL_HW_NORMAL_INFO)

void OS_GetOwnerInfoEx(OSOwnerInfoEx *info) {
  NVRAMConfig *src;
  OSTWLSettingsData *twlSettings;

  src = (NVRAMConfig *)(OS_GetSystemWork()->nvramUserInfo);

  info->favoriteColor = (u8)(src->ncd.owner.favoriteColor);
  info->birthday.month = (u8)(src->ncd.owner.birthday.month);
  info->birthday.day = (u8)(src->ncd.owner.birthday.day);
  info->nickNameLength = (u16)(src->ncd.owner.nickname.length);
  info->commentLength = (u16)(src->ncd.owner.comment.length);
  MI_CpuCopy16(src->ncd.owner.nickname.str, info->nickName,
               OS_OWNERINFO_NICKNAME_MAX * sizeof(u16));
  MI_CpuCopy16(src->ncd.owner.comment.str, info->comment,
               OS_OWNERINFO_COMMENT_MAX * sizeof(u16));
  info->nickName[OS_OWNERINFO_NICKNAME_MAX] = 0;
  info->comment[OS_OWNERINFO_COMMENT_MAX] = 0;

  if (OS_IsRunOnTwl()) {
    twlSettings = (OSTWLSettingsData *)OS_ADDR_TWL_SETTINGSDATA;
    info->country = twlSettings->country;
    info->language = twlSettings->language;
  } else {
    info->country = 0;
    info->language = (u8)(src->ncd.option.language);
  }
}

#ifdef SDK_TWL
#include <twl/ltdmain_begin.h>
static BOOL OS_IsAvailableWireless_ltdmain(void);
static BOOL OS_IsAvailableWireless_ltdmain(void) {
  OSTWLSettingsData *p;
  p = (OSTWLSettingsData *)OS_ADDR_TWL_SETTINGSDATA;
  return (p->flags.isAvailableWireless == 0x1);
}
#include <twl/ltdmain_end.h>
#endif

BOOL OS_IsAvailableWireless(void) {
  BOOL result;
  if (OS_IsRunOnTwl()) {
#ifdef SDK_TWL
    result = OS_IsAvailableWireless_ltdmain();
#endif
  } else {
    result = TRUE;
  }
  return (result);
}

BOOL OS_IsAgreeEULA(void) {
  OSTWLSettingsData *p;
  if (OS_IsRunOnTwl()) {
    p = (OSTWLSettingsData *)OS_ADDR_TWL_SETTINGSDATA;
    return (p->flags.isAgreeEULAFlagList & 0x01) ? TRUE : FALSE;
  } else {
    return FALSE;
  }
}

u8 OS_GetAgreedEULAVersion(void) {
  OSTWLSettingsData *p;
  if (OS_IsRunOnTwl()) {
    p = (OSTWLSettingsData *)OS_ADDR_TWL_SETTINGSDATA;
    return p->agreeEulaVersion[0];
  } else {
    return 0;
  }
}

u8 OS_GetROMHeaderEULAVersion(void) {
  if (OS_IsRunOnTwl()) {

    return *(u8 *)(HW_TWL_ROM_HEADER_BUF + 0x020e);
  } else {
    return 0;
  }
}

const OSTWLParentalControl *OS_GetParentalControlInfoPtr(void) {
  OSTWLSettingsData *p;
  if (OS_IsRunOnTwl()) {
    p = (OSTWLSettingsData *)OS_ADDR_TWL_SETTINGSDATA;
    return &(p->parentalControl);
  } else {
    return NULL;
  }
}

#ifdef SDK_TWL
#include <twl/ltdmain_begin.h>
#endif
BOOL OSi_IsParentalControledAppCore(u8 *appRatingInfo);

BOOL OSi_IsParentalControledAppCore(u8 *appRatingInfo) {
  OSTWLParentalControl *p =
      &(((OSTWLSettingsData *)OS_ADDR_TWL_SETTINGSDATA)->parentalControl);

  if (p->flags.isSetParentalControl) {
    if (p->ogn < OS_TWL_PCTL_OGN_MAX) {
      if (appRatingInfo == NULL) {

        OS_TWarning("Invalid pointer to Application rating information.\n");
        return TRUE;
      } else {
        if ((appRatingInfo[p->ogn] & OS_TWL_PCTL_OGNINFO_ENABLE_MASK) == 0) {

          OS_TWarning("Application doesn't have rating information for the "
                      "organization.\n");
          return TRUE;
        } else {
          if ((appRatingInfo[p->ogn] & OS_TWL_PCTL_OGNINFO_ALWAYS_MASK) != 0) {
            return TRUE;
          } else {
            if ((appRatingInfo[p->ogn] & OS_TWL_PCTL_OGNINFO_AGE_MASK) >
                p->ratingAge) {
              return TRUE;
            }
          }
        }
      }
    } else {

      OS_TWarning("Invalid rating organization index (%d) in LCFG.\n", p->ogn);
    }
  }
  return FALSE;
}

#ifdef SDK_TWL
#include <twl/ltdmain_end.h>
#endif

BOOL OS_IsParentalControledApp(u8 *appRatingInfo) {
  if (OS_IsRunOnTwl() == TRUE) {
    return OSi_IsParentalControledAppCore(appRatingInfo);
  }
  return FALSE;
}

void OS_GetMovableUniqueID(u8 *pUniqueID) {
  OSTWLHWNormalInfo *p;
  if (OS_IsRunOnTwl()) {
    p = (OSTWLHWNormalInfo *)OS_ADDR_TWL_HWNORMALINFO;
    MI_CpuCopy8(p->movableUniqueID, pUniqueID,
                OS_TWL_HWINFO_MOVABLE_UNIQUE_ID_LEN * sizeof(u8));
  } else {
    MI_CpuFill8(pUniqueID, 0, OS_TWL_HWINFO_MOVABLE_UNIQUE_ID_LEN * sizeof(u8));
  }
}

u32 OS_GetValidLanguageBitmap(void) {
  OSTWLHWSecureInfo *p;
  if (OS_IsRunOnTwl()) {
    p = (OSTWLHWSecureInfo *)HW_HW_SECURE_INFO;
    return p->validLanguageBitmap;
  } else {
    return 0;
  }
}

const u8 *OS_GetMovableUniqueIDPtr(void) {
  OSTWLHWNormalInfo *p;
  if (OS_IsRunOnTwl()) {
    p = (OSTWLHWNormalInfo *)OS_ADDR_TWL_HWNORMALINFO;
    return (p->movableUniqueID);
  } else {
    return NULL;
  }
}

#ifdef SDK_TWL
#include <twl/ltdmain_begin.h>
static BOOL OS_IsForceDisableWireless_ltdmain(void);
static BOOL OS_IsForceDisableWireless_ltdmain(void) {
  OSTWLHWSecureInfo *p;
  p = (OSTWLHWSecureInfo *)HW_HW_SECURE_INFO;
  return (p->flags.forceDisableWireless == 0x1);
}
#include <twl/ltdmain_end.h>
#endif

BOOL OS_IsForceDisableWireless(void) {
  BOOL result;
  if (OS_IsRunOnTwl()) {
#ifdef SDK_TWL
    result = OS_IsForceDisableWireless_ltdmain();
#endif
  } else {
    result = FALSE;
  }
  return (result);
}

OSTWLRegion OS_GetRegion(void) {
  OSTWLRegion result;
  OSTWLHWSecureInfo *p;
  if (OS_IsRunOnTwl()) {
    p = (OSTWLHWSecureInfo *)HW_HW_SECURE_INFO;
    result = (OSTWLRegion)(p->region);
  } else {
    result = (OSTWLRegion)0;
  }
  return (result);
}

void OS_GetSerialNo(u8 *serialNo) {
  OSTWLHWSecureInfo *p;
  if (OS_IsRunOnTwl()) {
    p = (OSTWLHWSecureInfo *)HW_HW_SECURE_INFO;
    MI_CpuCopy8(p->serialNo, serialNo,
                OS_TWL_HWINFO_SERIALNO_LEN_MAX * sizeof(u8));
  } else {
    MI_CpuFill8(serialNo, 0, OS_TWL_HWINFO_SERIALNO_LEN_MAX * sizeof(u8));
  }
}

OSTWLWirelessFirmwareData *OS_GetWirelessFirmwareData(void) {
  if (OS_IsRunOnTwl()) {
    return (OSTWLWirelessFirmwareData *)HW_PARAM_WIRELESS_FIRMWARE_DATA;
  } else {
    return NULL;
  }
}

#ifdef SDK_TWL
#include <twl/ltdmain_begin.h>
static const char *OSi_GetRegionCodeA3_ltdmain(OSTWLRegion region);
static const char *OSi_GetRegionCodeA3_ltdmain(OSTWLRegion region) {
  const char *regionList[] = {
      "JPN", // OS_TWL_REGION_JAPAN     = 0,  // NCL
      "USA", // OS_TWL_REGION_AMERICA   = 1,  // NOA
      "EUR", // OS_TWL_REGION_EUROPE    = 2,  // NOE
      "AUS", // OS_TWL_REGION_AUSTRALIA = 3,  // NAL
      "CHN", // OS_TWL_REGION_CHINA     = 4,  // IQue
      "KOR"  // OS_TWL_REGION_KOREA     = 5,  // NOK
  };

  if (region >= sizeof(regionList) / sizeof(regionList[0])) {
    OS_TWarning("Invalide region code.(%d)", region);
    return NULL;
  }
  return regionList[region];
}
#include <twl/ltdmain_end.h>
#endif /* SDK_TWL */

const char *OS_GetRegionCodeA3(OSTWLRegion region) {
  if (OS_IsRunOnTwl()) {
#ifdef SDK_TWL
    return OSi_GetRegionCodeA3_ltdmain(region);
#else  /* SDK_TWL */
    return NULL;
#endif /* SDK_TWL */
  } else {
    return NULL;
  }
}

extern const int OSi_CountryCodeListNumEntries;
extern const char *OSi_CountryCodeList[];

const char *OS_GetISOCountryCodeA2(u8 twlCountry) {
  const char *cca2 = NULL;
  if (OS_IsRunOnTwl()) {
#ifdef SDK_TWL
    if (twlCountry < OSi_CountryCodeListNumEntries) {
      cca2 = OSi_CountryCodeList[twlCountry];
    }
#ifndef SDK_FINALROM
    if (!cca2) {
      OS_TWarning("Invalid country code(%d)\n", twlCountry);
    }
#endif /* SDK_FINALROM */
#endif /* SDK_TWL */
    return cca2;
  } else // When not running in TWL
  {
    return NULL;
  }
}

#undef OS_TWL_COUNTRY_NAME_MAX

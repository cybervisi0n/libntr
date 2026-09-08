#ifndef TWL_OS_COMMON_OWNERINFO_PRIVATE_H_
#define TWL_OS_COMMON_OWNERINFO_PRIVATE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <twl/types.h>
#include <twl/spec.h>
#ifndef SDK_TWL
#include <nitro/hw/common/mmap_shared.h>
#else
#include <twl/hw/common/mmap_shared.h>
#endif

#include <twl/os/common/ownerInfoEx.h>

typedef struct OSTWLDate {
  u8 month;  // Month: 01 to 12
  u8 day;    // Day: 01 to 31
} OSTWLDate; // 2 bytes

typedef struct OSTWLOwnerInfo {
  u8 userColor : 4;   // Favorite color
  u8 rsv : 4;         // Reserved
  u8 pad;             // Padding
  OSTWLDate birthday; // Birthdate
  u16 nickname[OS_TWL_NICKNAME_LENGTH +
               1]; // Nickname (with terminating character)
  u16 comment[OS_TWL_COMMENT_LENGTH +
              1]; // Comment (with terminating character)
} OSTWLOwnerInfo; // 80 bytes

typedef struct OSTWLSettingsData {
  union {
    struct {
      u32 isFinishedInitialSetting : 1; // Initial settings complete?
      u32 isFinishedInitialSetting_Launcher
          : 1;               // Initial launcher settings complete?
      u32 isSetLanguage : 1; // Language code set?
      u32 isAvailableWireless
          : 1;      // Enabling/disabling the wireless module's RF unit
      u32 rsv : 20; // Reserved
      u32 isAgreeEULAFlagList : 8; // List of EULA acceptance flags
    };
    u32 raw;
  } flags;
  u8 rsv[1];         // Reserved
  u8 country;        // Country code
  u8 language;       // Language (the 8-bit data size differs from NTR systems)
  u8 rtcLastSetYear; // The year previously set with the RTC
  s64 rtcOffset; // Offset value when configuring the RTC (each time the user
                 // changes the configuration of the RTC, this fluctuates in
                 // response to that value)j

  u8 agreeEulaVersion[8]; //    8 bytes: The accepted EULA version
  u8 pad1[2];
  u8 pad2[6];           //    6 bytes
  u8 pad3[16];          //   16bytes
  u8 pad4[20];          //   20 bytes
  OSTWLOwnerInfo owner; //   80 bytes: Owner information
  OSTWLParentalControl
      parentalControl; //  148 bytes: Parental Controls information
} OSTWLSettingsData;   // 296 bytes

typedef struct OSTWLHWNormalInfo {
  u8 rtcAdjust; // RTC adjustment value
  u8 rsv[3];
  u8 movableUniqueID[OS_TWL_HWINFO_MOVABLE_UNIQUE_ID_LEN]; // A unique ID that
                                                           // can be moved
} OSTWLHWNormalInfo;                                       // 20 bytes

typedef struct OSTWLHWSecureInfo {
  u32 validLanguageBitmap; // A bitmap of language codes valid on this system
  struct {
    u8 forceDisableWireless : 1;
    u8 : 7;
  } flags;
  u8 pad[3];
  u8 region;                                   // Region
  u8 serialNo[OS_TWL_HWINFO_SERIALNO_LEN_MAX]; // Serial number (an ASCII string
                                               // with a terminating character)
} OSTWLHWSecureInfo;                           // 24 bytes

typedef struct OSTWLWirelessFirmwareData {
  u8 data;
  u8 rsv[3];
} OSTWLWirelessFirmwareData;

OSTWLWirelessFirmwareData *OS_GetWirelessFirmwareData(void);

u32 OS_GetValidLanguageBitmap(void);

void OS_GetSerialNo(u8 *serialNo);

static inline BOOL OS_IsRestrictDSDownloadBoot(void) {
  return (BOOL)OS_GetParentalControlInfoPtr()->flags.isSetParentalControl &&
         (BOOL)OS_GetParentalControlInfoPtr()->flags.dsDownload;
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TWL_OS_COMMON_OWNERINFO_EX_PRIVATE_H_ */

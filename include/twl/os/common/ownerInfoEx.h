#ifndef TWL_OS_COMMON_OWNERINFO_EX_H_
#define TWL_OS_COMMON_OWNERINFO_EX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <twl/types.h>
#include <twl/spec.h>
#include <nitro/std.h>
#ifndef SDK_TWL
#include <nitro/hw/common/mmap_shared.h>
#else
#include <twl/hw/common/mmap_shared.h>
#endif

#include <nitro/os/common/ownerInfo.h>

typedef enum OSTWLRegionCode {
  OS_TWL_REGION_JAPAN = 0,
  OS_TWL_REGION_AMERICA = 1,
  OS_TWL_REGION_EUROPE = 2,
  OS_TWL_REGION_AUSTRALIA = 3,
  OS_TWL_REGION_CHINA = 4,
  OS_TWL_REGION_KOREA = 5,
  OS_TWL_REGION_MAX
} OSTWLRegion;

#define OS_TWL_HWINFO_SERIALNO_LEN_MAX                                         \
  15 // The maximum length for a system's serial number (this can extend as far
     // as 14 bytes because it includes the terminating character)
#define OS_TWL_HWINFO_MOVABLE_UNIQUE_ID_LEN 16 // A unique ID that can be moved
#define OS_TWL_NICKNAME_LENGTH OS_OWNERINFO_NICKNAME_MAX // Nickname Length
#define OS_TWL_COMMENT_LENGTH OS_OWNERINFO_COMMENT_MAX   // Comment Length
#define OS_TWL_PCTL_PASSWORD_LENGTH 4 // Number of password digits
#define OS_TWL_PCTL_SECRET_ANSWER_LENGTH_MAX                                   \
  64 // The maximum (MAX) number of UTF-16 characters in the answer to the
     // secret question

typedef struct OSOwnerInfoEx {
  u8 language;         // Language code
  u8 favoriteColor;    // Favorite color (0 to 15)
  OSBirthday birthday; // Birthdate
  u16 nickName[OS_OWNERINFO_NICKNAME_MAX + 1];

  u16 nickNameLength; // Number of characters in nickname (0 to 10)
  u16 comment[OS_OWNERINFO_COMMENT_MAX + 1];

  u16 commentLength; // Number of characters in comment (0 to 26)

  u8 country; // Country and region code
  u8 padding;

} OSOwnerInfoEx;

typedef enum OSTWLRatingOgn {
  OS_TWL_PCTL_OGN_CERO = 0, // CERO                         // Japan
  OS_TWL_PCTL_OGN_ESRB = 1, // ESRB                         // North America
  OS_TWL_PCTL_OGN_RESERVED2 = 2, // BBFC [obsolete]
  OS_TWL_PCTL_OGN_USK = 3,       // USK                          // Germany
  OS_TWL_PCTL_OGN_PEGI_GEN = 4,  // PEGI general                 // Europe
  OS_TWL_PCTL_OGN_RESERVED5 = 5, // PEGI Finland [obsolete]
  OS_TWL_PCTL_OGN_PEGI_PRT = 6,  // PEGI Portugal                // Portugal
  OS_TWL_PCTL_OGN_PEGI_BBFC = 7, // PEGI and BBFC Great Britain   // England
  OS_TWL_PCTL_OGN_COB =
      8, // COB                          // Australia, New Zealand
  OS_TWL_PCTL_OGN_GRB = 9, // GRB                          // South Korea

  OS_TWL_PCTL_OGN_MAX = 16

} OSTWLRatingOgn;

typedef struct OSTWLParentalControl {
  struct {
    u32 isSetParentalControl : 1; // Have Parental Controls already been set?
    u32 pictoChat : 1;            // Is starting PictoChat restricted?
    u32 dsDownload : 1;           // Is starting DS Download Play restricted?
    u32 browser : 1;              // Is starting a full browser restricted?
    u32 prepaidPoint : 1;         // Is the use of points restricted?
    u32 photoExchange : 1;        // Is exchanging photos restricted?
    u32 ugc : 1;                  // Is user-created content restricted?
    u32 rsv : 25;
  } flags;
  u8 rsv1[3];
  u8 ogn;                // Rating organizations
  u8 ratingAge;          // Rating value (age)
  u8 secretQuestionID;   // Secret question ID
  u8 secretAnswerLength; // Number of characters in the answer to the secret
                         // question
  u8 rsv[2];
  char password[OS_TWL_PCTL_PASSWORD_LENGTH +
                1]; // Password (with terminating character)
  u16 secretAnswer[OS_TWL_PCTL_SECRET_ANSWER_LENGTH_MAX +
                   1];  // UTF-16 answer (with terminating character) to the
                        // secret question
} OSTWLParentalControl; //  148 bytes

#define OS_TWL_PCTL_OGNINFO_ENABLE_MASK 0x80
#define OS_TWL_PCTL_OGNINFO_ALWAYS_MASK 0x40
#define OS_TWL_PCTL_OGNINFO_AGE_MASK 0x1f

void OS_GetOwnerInfoEx(OSOwnerInfoEx *info);

BOOL OS_IsAvailableWireless(void);

BOOL OS_IsAgreeEULA(void);

u8 OS_GetAgreedEULAVersion(void);

u8 OS_GetROMHeaderEULAVersion(void);

const OSTWLParentalControl *OS_GetParentalControlInfoPtr(void);

BOOL OS_IsParentalControledApp(u8 *appRatingInfo);

void OS_GetMovableUniqueID(u8 *pUniqueID);

const u8 *OS_GetMovableUniqueIDPtr(void);

BOOL OS_IsForceDisableWireless(void);

OSTWLRegion OS_GetRegion(void);

const char *OS_GetRegionCodeA3(OSTWLRegion region);

const char *OS_GetISOCountryCodeA2(u8 twlCountry);

static inline BOOL OS_IsRestrictPictoChatBoot(void) {
  return (BOOL)OS_GetParentalControlInfoPtr()->flags.isSetParentalControl &&
         (BOOL)OS_GetParentalControlInfoPtr()->flags.pictoChat;
}

static inline BOOL OS_IsRestrictBrowserBoot(void) {
  return (BOOL)OS_GetParentalControlInfoPtr()->flags.isSetParentalControl &&
         (BOOL)OS_GetParentalControlInfoPtr()->flags.browser;
}

static inline BOOL OS_IsRestrictPrepaidPoint(void) {
  return (BOOL)OS_GetParentalControlInfoPtr()->flags.isSetParentalControl &&
         (BOOL)OS_GetParentalControlInfoPtr()->flags.prepaidPoint;
}

static inline BOOL OS_IsRestrictPhotoExchange(void) {
  return (BOOL)OS_GetParentalControlInfoPtr()->flags.isSetParentalControl &&
         (BOOL)OS_GetParentalControlInfoPtr()->flags.photoExchange;
}

static inline BOOL OS_IsRestrictUGC(void) {
  return (BOOL)OS_GetParentalControlInfoPtr()->flags.isSetParentalControl &&
         (BOOL)OS_GetParentalControlInfoPtr()->flags.ugc;
}

static inline BOOL OS_CheckParentalControlPassword(const char *password) {
  return password &&
         (STD_CompareLString(password, OS_GetParentalControlInfoPtr()->password,
                             OS_TWL_PCTL_PASSWORD_LENGTH) == 0);
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TWL_OS_COMMON_OWNERINFO_EX_H_ */

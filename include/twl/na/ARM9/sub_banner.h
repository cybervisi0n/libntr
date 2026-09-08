#ifndef TWL_NA_ARM9_SUB_BANNER_H_
#define TWL_NA_ARM9_SUB_BANNER_H_

#include <twl/misc.h>
#include <twl/types.h>
#include <nitro/os/common/emulator.h>
#include <nitro/gx/gxcommon.h>
#include <nitro/gx/struct_2d.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NA_SUBBANNER_VERSION 3
#define NA_SUBBANNER_PLATFORM 1
#define NA_SUBBANNER_PALETTE_NUM 16
#define NA_SUBBANNER_CHARACTER_NUM 16
#define NA_SUBBANNER_PATTERN_NUM 8
#define NA_SUBBANNER_CONTROL_NUM 64

typedef struct NASubBannerHeader {
  u8 version;
  u8 platform;
  u16 crc16_v1;
  u16 crc16_v2;
  u16 crc16_v3;
  u16 crc16_anime;
  u8 reserved_[22];
} NASubBannerHeader;

typedef struct NASubBannerControl {
  u8 frameCount : 8;

  union {
    struct {
      u8 cellNo : 3;
      u8 plttNo : 3;
      u8 flipH : 1;
      u8 flipV : 1;
    } frameInfo;

    u8 animeType;
  };
} NASubBannerControl;

typedef struct NASubBannerAnime {
  GXCharFmt16 image[NA_SUBBANNER_PATTERN_NUM][NA_SUBBANNER_CHARACTER_NUM];
  GXRgb pltt[NA_SUBBANNER_PATTERN_NUM][NA_SUBBANNER_PALETTE_NUM];
  NASubBannerControl control[NA_SUBBANNER_CONTROL_NUM];
} NASubBannerAnime;

typedef struct NASubBanner {
  NASubBannerHeader header;
  NASubBannerAnime anime;
} NASubBanner;

BOOL NAi_SaveSubBanner(const NASubBanner *banner);
BOOL NAi_LoadSubBanner(NASubBanner *banner);
BOOL NAi_DeleteSubBanner(NASubBanner *workBuffer);
void NAi_MakeSubBannerHeader(NASubBanner *banner);

static inline BOOL NA_SaveSubBanner(const NASubBanner *banner) {
  return OS_IsRunOnTwl() ? NAi_SaveSubBanner(banner) : FALSE;
}
static inline BOOL NA_LoadSubBanner(NASubBanner *banner) {
  return OS_IsRunOnTwl() ? NAi_LoadSubBanner(banner) : FALSE;
}
static inline BOOL NA_DeleteSubBanner(NASubBanner *workBuffer) {
  return OS_IsRunOnTwl() ? NAi_DeleteSubBanner(workBuffer) : FALSE;
}
static inline void NA_MakeSubBannerHeader(NASubBanner *banner) {
  if (OS_IsRunOnTwl())
    NAi_MakeSubBannerHeader(banner);
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TWL_NA_ARM9_SUB_BANNER_H_ */

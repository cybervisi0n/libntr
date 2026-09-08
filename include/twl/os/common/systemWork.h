#ifndef TWL_OS_COMMON_SYSTEMWORK_H_
#define TWL_OS_COMMON_SYSTEMWORK_H_

#if !(defined(SDK_WIN32) || defined(SDK_FROM_TOOL))

#ifndef SDK_ASM

#include <twl/types.h>
#include <twl/hw/common/mmap_shared.h>
#ifdef SDK_TWL
#ifdef SDK_PORT
#include <twl/hw/X86/mmap_global.h>
#else
#ifdef SDK_ARM9
#include <twl/hw/ARM9/mmap_global.h>
#else // SDK_ARM7
#include <twl/hw/ARM7/mmap_global.h>
#endif
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SDK_TWL

typedef enum OSMountDevice {
  OS_MOUNT_DEVICE_SD = 0,
  OS_MOUNT_DEVICE_NAND = 1,
  OS_MOUNT_DEVICE_MAX = 2
} OSMountDevice;

typedef enum OSMountTarget {
  OS_MOUNT_TGT_ROOT = 0,
  OS_MOUNT_TGT_FILE = 1,
  OS_MOUNT_TGT_DIR = 2,
  OS_MOUNT_TGT_MAX = 3
} OSMountTarget;

typedef enum OSMountPermission {
  OS_MOUNT_USR_X = 0x01,
  OS_MOUNT_USR_W = 0x02,
  OS_MOUNT_USR_R = 0x04
} OSMountPermission;

typedef enum OSMountResource {
  OS_MOUNT_RSC_MMEM = 0,
  OS_MOUNT_RSC_WRAM = 1
} OSMountResource;

#define OS_MOUNT_PARTITION_MAX_NUM                                             \
  3 // The maximum number of mountable partitions
#define OS_MOUNT_DRIVE_START                                                   \
  'A' // The first drive name (you can specify only uppercase letters from 'A'
      // to 'Z')
#define OS_MOUNT_DRIVE_END 'Z'       // The last drive name
#define OS_MOUNT_ARCHIVE_NAME_LEN 16 // The maximum length of an archive name
#define OS_MOUNT_PATH_LEN 64         // The maximum length of a path
#define OS_MOUNT_INFO_MAX                                                      \
  (size_t)((HW_TWL_FS_BOOT_SRL_PATH_BUF - HW_TWL_FS_MOUNT_INFO_BUF) /          \
           sizeof(OSMountInfo))

#define OS_TITLEIDLIST_MAX                                                     \
  118 // The maximum number of items retained by the title ID list

typedef struct OSMountInfo {
  u8 drive[1];
  u8 device : 3;
  u8 target : 2;
  u8 partitionIndex : 2;
  u8 resource : 1;
  u8 userPermission
      : 3; // Specifies whether this is readable and writable by the user
  u8 rsv_A : 5;
  u8 rsv_B;
  char archiveName[OS_MOUNT_ARCHIVE_NAME_LEN]; // Size with a terminating '\0'
  char path[OS_MOUNT_PATH_LEN];                // Size with a terminating '\0'
} OSMountInfo;                                 // 84 bytes

typedef struct OSTitleIDList {
  u8 num;
  u8 rsv[15];
  u8 publicFlag[16];    // Flag indicating whether there is public save data
  u8 privateFlag[16];   // Flag indicating whether there is private save data
  u8 appJumpFlag[16];   // Flag indicating whether an application jump can be
                        // performed
  u8 sameMakerFlag[16]; // Flag indicating whether the manufacturer is the same
  u64 TitleID[OS_TITLEIDLIST_MAX];
} OSTitleIDList; // 1024 bytes

typedef struct OSHotBootStatus {
  u8 isDisable : 1;
  u8 rsv : 7;
} OSHotBootStatus;

static inline const OSMountInfo *OS_GetMountInfo(void) {
  return (const OSMountInfo *)HW_TWL_FS_MOUNT_INFO_BUF;
}

static inline const char *OS_GetBootSRLPath(void) {
  return (const char *)HW_TWL_FS_BOOT_SRL_PATH_BUF;
}

static inline u64 OS_GetTitleId(void) {
  return *(u64 *)(HW_TWL_ROM_HEADER_BUF + 0x230);
}

static inline u16 OS_GetMakerCode(void) {
  return *(u16 *)(HW_TWL_ROM_HEADER_BUF + 0x10);
}

static inline const u8 *OSi_GetSystemMenuVersionInfoContentID(void) {
  return (const u8 *)HW_SYSM_VER_INFO_CONTENT_ID;
}

static inline u8 OSi_GetSystemMenuVersionInfoLastGameCode(void) {
  return *(u8 *)HW_SYSM_VER_INFO_CONTENT_LAST_INITIAL_CODE;
}

static inline BOOL OSi_IsEnableHotBoot(void) {
  return ((OSHotBootStatus *)HW_SYSM_DISABLE_SET_HOTBOOT)->isDisable ? 0 : 1;
}

static inline void OSi_SetEnableHotBoot(BOOL isEnable) {
  ((OSHotBootStatus *)HW_SYSM_DISABLE_SET_HOTBOOT)->isDisable =
      isEnable ? 0 : 1;
}

#endif // SDK_TWL

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // SDK_ASM

#endif // SDK_FROM_TOOL

#endif

#ifndef NITRO_OS_COMMON_SYSTEMWORK_H_
#define NITRO_OS_COMMON_SYSTEMWORK_H_

#if !(defined(SDK_WIN32) || defined(SDK_FROM_TOOL))

#ifndef SDK_ASM
#include <nitro/types.h>


#ifdef SDK_PORT
#include <nitro/hw/X86/mmap_shared.h>
#else
#if SDK_VERSION_MAJOR == 4
#include <nitro/hw/common/mmap_shared.h>
#elif SDK_VERSION_MAJOR == 5
#ifndef SDK_TWL
#include <nitro/hw/common/mmap_shared.h>
#else // SDK_TWL
#include <twl/hw/common/mmap_shared.h>
#endif
#endif
#endif

#include <nitro/os/common/thread.h>
#include <nitro/os/common/spinLock.h>
#include <nitro/os/common/arena.h>

typedef union {
	u32 b32;
	u16 b16;
} OSDmaClearSrc;

typedef struct {
	u8 bootCheckInfo[0x20];
	u32 resetParameter;
	#if (SDK_VERSION_MAJOR == 5) && defined(SDK_TWL)
	u8 bootSync[0x8];
	#else
	u8 padding5[0x8];
	#endif
	u32 romBaseOffset;
	u8 cartridgeModuleInfo[12];
	u32 vblankCount;
	u8 wmBootBuf[0x40];
	#if (SDK_VERSION_MAJOR == 5) && defined(SDK_TWL)
  	u8 nvramUserInfo[0xe8];
  	u8 HW_secure_info[0x18];
	#else
	u8 nvramUserInfo[0x100];
	#endif
	u8 isd_reserved1[0x20];
	u8 arenaInfo[0x48];
	u8 real_time_clock[8];
	#if (SDK_VERSION_MAJOR == 5)
	u8 sys_conf[6];
	u8 printWindowArm9;
	u8 printWindowArm7;
	u8 printWindowArm9Err;
	u8 printWindowArm7Err;
	#ifdef SDK_TWL
	u8 nandFirmHotStartFlag;
	u8 REDLauncherVersion;
	u32 preloadParameterAddr;
	#else
	u8 padding1[6];
	#endif
	#else
	u32 dmaClearBuf[4];
	#endif
	u8 rom_header[0x160];
	u8 isd_reserved2[32];
	u32 pxiSignalParam[2];
	u32 pxiHandleChecker[2];
	u32 mic_last_address;
	u16 mic_sampling_data;
	u16 wm_callback_control;
	u16 wm_rssi_pool;
	u8 ctrdg_SetModuleInfoFlag;
	u8 ctrdg_IsExisting;
	u32 component_param;
	OSThreadInfo * threadinfo_mainp;
	OSThreadInfo * threadinfo_subp;
	u16 button_XY;
	u8 touch_panel[4];
	u16 autoloadSync;
	u32 lockIDFlag_mainp[2];
	u32 lockIDFlag_subp[2];
    #if(defined(SDK_PORT) && defined(__cplusplus))
    OSLockWord lock_VRAM_C;
    OSLockWord lock_VRAM_D;
    OSLockWord lock_WRAM_BLOCK0;
    OSLockWord lock_WRAM_BLOCK1;
    OSLockWord lock_CARD;
    OSLockWord lock_CARTRIDGE;
    OSLockWord lock_INIT;
    #else
	struct OSLockWord lock_VRAM_C;
	struct OSLockWord lock_VRAM_D;
	struct OSLockWord lock_WRAM_BLOCK0;
	struct OSLockWord lock_WRAM_BLOCK1;
	struct OSLockWord lock_CARD;
	struct OSLockWord lock_CARTRIDGE;
	struct OSLockWord lock_INIT;
    #endif
	u16 mmem_checker_mainp;
	u16 mmem_checker_subp;
	u8 padding4[2];
	u16 command_area;
} OSSystemWork;

#define OS_GetSystemWork()      ((OSSystemWork *)HW_MAIN_MEM_SYSTEM)

#if SDK_VERSION_MAJOR == 5
#ifdef SDK_TWL
typedef struct {
  #ifdef SDK_BUILD_ARM
  struct 
  #endif
  OSLockWord
      lock_WRAM_ex;  // 000-003:   4 bytes: Lock buffer for WRAM-A, B, and C
  u32 reset_flag;    // 004-007:   4 bytes: Reset flags (hardware reset)
  u8 padding[0x178]; // 008-17f:  (376 bytes)
} OSSystemWork2;

#define OS_GetSystemWork2() ((OSSystemWork2 *)HW_PSEG1_RESERVED_0)
#endif // SDK_TWL

#ifndef SDK_TWL
#define OS_IsCodecTwlMode() (FALSE)
#endif

typedef u16 OSBootType;
#define OS_BOOTTYPE_ILLEGAL 0     // Illegal status
#define OS_BOOTTYPE_ROM 1         // Boot from ROM
#define OS_BOOTTYPE_DOWNLOAD_MB 2 // Start a downloaded application
#define OS_BOOTTYPE_NAND 3        // Start an application in NAND memory
#define OS_BOOTTYPE_MEMORY 4      //

typedef struct OSBootInfo {
  OSBootType boot_type; // 2

  u16 length;     // 4
  u16 rssi;       // 6
  u16 bssid[3];   // 12
  u16 ssidLength; // 14
  u8 ssid[32];    // 46
  u16 capaInfo;   // 48
  struct {
    u16 basic;   // 50
    u16 support; // 52
  } rateSet;
  u16 beaconPeriod;   // 54
  u16 dtimPeriod;     // 56
  u16 channel;        // 58
  u16 cfpPeriod;      // 60
  u16 cfpMaxDuration; // 62
  u16 rsv1;           // 64
} OSBootInfo;

OSBootType OS_GetBootType(void);

const OSBootInfo *OS_GetBootInfo(void);
#endif

#endif

#endif

#endif

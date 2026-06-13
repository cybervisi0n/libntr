#ifndef NITRO_HW_COMMON_MMAP_SHARED_H_
#define NITRO_HW_COMMON_MMAP_SHARED_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/os/common/arena.h>


//----------------------------------------------------------------------
// MEMORY MAP of SYSTEM SHARED AREA
//----------------------------------------------------------------------
//
#define HW_RED_RESERVED                 (HW_MAIN_MEM + 0x007ff800)      // maybe change later
#define HW_RED_RESERVED_END             (HW_MAIN_MEM + 0x007ffa00)

/* original ROM-header of NITRO-CARD for downloaded program */
#define HW_CARD_ROM_HEADER_SIZE           0x160
/* maximun size of parameters for downloaded program */
#define HW_DOWNLOAD_PARAMETER_SIZE        0x20

//---- default address for shared memory arena
#define  HW_SHARED_ARENA_LO_DEFAULT       HW_MAIN_MEM_SHARED
#ifdef   HW_RED_RESERVED
# define HW_SHARED_ARENA_HI_DEFAULT       (HW_RED_RESERVED - HW_CARD_ROM_HEADER_SIZE - HW_DOWNLOAD_PARAMETER_SIZE)
#else
# define HW_SHARED_ARENA_HI_DEFAULT       (HW_MAIN_MEM_SYSTEM - HW_CARD_ROM_HEADER_SIZE - HW_DOWNLOAD_PARAMETER_SIZE)
#endif

// 0x027ffa00-0x027ffa7f:(128byte):UNUSED

/* 0x027ffa80-0x027ffbdf:(352byte): original ROM-header */
#define HW_CARD_ROM_HEADER              (HW_MAIN_MEM + 0x007ffa80)

/* 0x027ffbe0-0x027ffbff:( 32byte): parameters for downloaded program */
#define HW_DOWNLOAD_PARAMETER           (HW_MAIN_MEM + 0x007ffbe0)

//-------------------------------- MainMemory shared area
#define HW_MAIN_MEM_SYSTEM_SIZE         0x400
//#define HW_MAIN_MEM_SYSTEM              (HW_MAIN_MEM + 0x007ffc00)
extern u8 s_HW_MAIN_MEM_SYSTEM[HW_MAIN_MEM_SYSTEM_SIZE];
#define HW_MAIN_MEM_SYSTEM              (u64)(s_HW_MAIN_MEM_SYSTEM)

#define HW_BOOT_CHECK_INFO_BUF          (HW_MAIN_MEM_SYSTEM + 0x0)      // Boot check info (END-0x400)
#define HW_BOOT_CHECK_INFO_BUF_END      (HW_MAIN_MEM_SYSTEM + 0x20)      //                 (END-0x3e0)

#define HW_RESET_PARAMETER_BUF          (HW_MAIN_MEM_SYSTEM + 0x20)      // reset parameter (END-0x3e0)

// 0x027ffc24-0x027ffc2b:(8byte):UNUSED

#define HW_ROM_BASE_OFFSET_BUF          (HW_MAIN_MEM_SYSTEM + 0x2c)      // ROM offset of own program (END-0x3d4)
#define HW_ROM_BASE_OFFSET_BUF_END      (HW_MAIN_MEM_SYSTEM + 0x30)      //                           (END-0x3d0)

#define HW_CTRDG_MODULE_INFO_BUF        (HW_MAIN_MEM_SYSTEM + 0x30)      // Cartridge module info (END-0x3d0)
#define HW_CTRDG_MODULE_INFO_BUF_END    (HW_MAIN_MEM_SYSTEM + 0x3c)      //                       (END-0x3c4)

//#define HW_VBLANK_COUNT_BUF             (HW_MAIN_MEM + 0x007ffc3c)      // VBlank counter (END-0x3c4)
//static u8 s_HW_VBLANK_COUNT_BUF[964];
#define HW_VBLANK_COUNT_BUF				      (HW_MAIN_MEM_SYSTEM + 0x3c)

#define HW_WM_BOOT_BUF                  (HW_MAIN_MEM_SYSTEM + 0x40)      // WM buffer for Multi-Boot (END-0x3c0)
#define HW_WM_BOOT_BUF_END              (HW_MAIN_MEM_SYSTEM + 0x80)      //                          (END-0x380)

#define HW_NVRAM_USER_INFO              (HW_MAIN_MEM_SYSTEM + 0x80)      // NVRAM user info (END-0x380)
#define HW_NVRAM_USER_INFO_END          (HW_MAIN_MEM_SYSTEM + 0x180)      //                 (END-0x280)

#define HW_BIOS_EXCP_STACK_MAIN         (HW_MAIN_MEM_SYSTEM + 0x180)      // MAINPデバッガモニタ例外ハンドラ (END-0x280)
#define HW_BIOS_EXCP_STACK_MAIN_END     (HW_MAIN_MEM_SYSTEM + 0x19C)      //                                 (END-0x264)
#define HW_EXCP_VECTOR_MAIN             (HW_MAIN_MEM_SYSTEM + 0x19C)      // MAINP用HW_EXCP_VECTOR_BUF       (END-0x264)


//extern u8 s_HW_ARENA_INFO_BUF[608];
//#define HW_ARENA_INFO_BUF (void *)(&s_HW_ARENA_INFO_BUF)
//#define HW_ARENA_INFO_BUF               (HW_MAIN_MEM_SYSTEM + 0x007ffda0)      // アリーナ情報構造体(27F_FDA0〜27F_FDE7) (END-0x260)
//static u8 s_HW_ARENA_INFO_BUF[608];


extern u8 s_HW_ARENA_INFO_BUF[sizeof(OSArenaInfo)];
//#define HW_ARENA_INFO_BUF               (HW_MAIN_MEM_SYSTEM + 0x1A0)
#define HW_ARENA_INFO_BUF               ((u64)&s_HW_ARENA_INFO_BUF)
#define HW_REAL_TIME_CLOCK_BUF          (HW_MAIN_MEM_SYSTEM + 0x1E8)      // RTC

#define HW_DMA_CLEAR_DATA_BUF           (HW_MAIN_MEM_SYSTEM + 0x1F0)      // DMAクリアデータ・バッファ (END-0x210)
#define HW_DMA_CLEAR_DATA_BUF_END       (HW_MAIN_MEM_SYSTEM + 0x200)      //  (この領域は ARM9-TEG にのみ使用する)

#define HW_ROM_HEADER_BUF               (HW_MAIN_MEM_SYSTEM + 0x200)      // ＲＯＭ内登録エリアデータ・バッファ (END-0x200)
#define HW_ROM_HEADER_BUF_END           (HW_MAIN_MEM_SYSTEM + 0x360)      //                                    (END-0x0a0)
#define HW_ISD_RESERVED                 (HW_MAIN_MEM_SYSTEM + 0x360)      // IS DEBUGGER Reserved (END-0xa0)
#define HW_ISD_RESERVED_END             (HW_MAIN_MEM_SYSTEM + 0x380)      //                      (END-0x80)

#define HW_PXI_SIGNAL_PARAM_ARM9        (HW_MAIN_MEM_SYSTEM + 0x380)      // PXI Signal Param for ARM9
#define HW_PXI_SIGNAL_PARAM_ARM7        (HW_MAIN_MEM_SYSTEM + 0x384)      // PXI Signal Param for ARM7
#define HW_PXI_HANDLE_CHECKER_ARM9      (HW_MAIN_MEM_SYSTEM + 0x388)      // PXI Handle Checker for ARM9
#define HW_PXI_HANDLE_CHECKER_ARM7      (HW_MAIN_MEM_SYSTEM + 0x38C)      // PXI Handle Checker for ARM7

#define HW_MIC_LAST_ADDRESS             (HW_MAIN_MEM_SYSTEM + 0x390)      // MIC 最新サンプリングデータ格納アドレス
#define HW_MIC_SAMPLING_DATA            (HW_MAIN_MEM_SYSTEM + 0x394)      // MIC 単体サンプリング結果

#define HW_WM_CALLBACK_CONTROL          (HW_MAIN_MEM_SYSTEM + 0x396)      // WM コールバック同期用パラメータ
#define HW_WM_RSSI_POOL                 (HW_MAIN_MEM_SYSTEM + 0x398)      // WM 受信強度による乱数源

#define HW_SET_CTRDG_MODULE_INFO_ONCE   (HW_MAIN_MEM_SYSTEM + 0x39A)      // set ctrdg module info flag
#define HW_IS_CTRDG_EXIST               (HW_MAIN_MEM_SYSTEM + 0x39B)      // ctrdg exist flag

#define HW_COMPONENT_PARAM              (HW_MAIN_MEM_SYSTEM + 0x39C)      // Component 同期用パラメータ

//#define HW_THREADINFO_MAIN              (HW_MAIN_MEM + 0x007fffa0)      // ThreadInfo for Main processor
//static u8 s_HW_THREADINFO_MAIN[4];
#define HW_THREADINFO_MAIN              (HW_MAIN_MEM_SYSTEM + 0x3A0)
//#define HW_THREADINFO_SUB               (HW_MAIN_MEM + 0x007fffa4)      // ThreadInfo for Sub  processor
//static u8 s_HW_THREADINFO_SUB[4];
#define HW_THREADINFO_SUB               (HW_MAIN_MEM_SYSTEM + 0x3A4)
//#define HW_BUTTON_XY_BUF                (HW_MAIN_MEM + 0x007fffa8)      // buffer for X and Y button
//static u8 s_HW_BUTTON_XY_BUF[2];
#define HW_BUTTON_XY_BUF                (HW_MAIN_MEM_SYSTEM + 0x3A8)
//#define HW_TOUCHPANEL_BUF               (HW_MAIN_MEM + 0x007fffaa)      // buffer for touch panel
//static u8 s_HW_TOUCHPANEL_BUF[4];
#define HW_TOUCHPANEL_BUF               (HW_MAIN_MEM_SYSTEM + 0x3AA)
#define HW_AUTOLOAD_SYNC_BUF            (HW_MAIN_MEM_SYSTEM + 0x3AE)      // buffer for autoload sync

#define HW_LOCK_ID_FLAG_MAIN            (HW_MAIN_MEM_SYSTEM + 0x3B0)      // lockID flag for Main processor
#define HW_LOCK_ID_FLAG_SUB             (HW_MAIN_MEM_SYSTEM + 0x3B8)      // lockID flag for Sub processor

// SpinLock Mutex
#define HW_VRAM_C_LOCK_BUF              (HW_MAIN_MEM_SYSTEM + 0x3C0)      // ＶＲＡＭ−Ｃ・ロックバッファ (END-0x40)
#define HW_VRAM_D_LOCK_BUF              (HW_MAIN_MEM_SYSTEM + 0x3C8)      // ＶＲＡＭ−Ｄ・ロックバッファ (END-0x38)
#define HW_WRAM_BLOCK0_LOCK_BUF         (HW_MAIN_MEM_SYSTEM + 0x3D0)      // ＣＰＵ内部ワークＲＡＭ・ブロック０・ロックバッファ (END-0x30)
#define HW_WRAM_BLOCK1_LOCK_BUF         (HW_MAIN_MEM_SYSTEM + 0x3D8)      // ＣＰＵ内部ワークＲＡＭ・ブロック１・ロックバッファ (END-0x28)
#define HW_CARD_LOCK_BUF                (HW_MAIN_MEM_SYSTEM + 0x3E0)      // カード・ロックバッファ (END-0x20)
#define HW_CTRDG_LOCK_BUF               (HW_MAIN_MEM_SYSTEM + 0x3E8)      // カートリッジ・ロックバッファ (END-0x18)
//#define HW_INIT_LOCK_BUF                (HW_MAIN_MEM + 0x007ffff0)      // 初期化ロックバッファ (END-0x10)
//static volatile u8 s_HW_INIT_LOCK_BUF[8];
#define HW_INIT_LOCK_BUF                (HW_MAIN_MEM_SYSTEM + 0x3F0)

#define HW_MMEMCHECKER_MAIN             (HW_MAIN_MEM_SYSTEM + 0x3F8)      // MainMomory Size Checker for Main processor (END-8)
#define HW_MMEMCHECKER_SUB              (HW_MAIN_MEM_SYSTEM + 0x3FA)      // MainMomory Size Checker for Sub processor  (END-6)

#define HW_CMD_AREA                     (HW_MAIN_MEM_SYSTEM + 0x3FE)      // メインメモリコマンド発行エリア（使用禁止エリア）(END-2)

//--------------------------------------------------------------------
//---- Lock area


#define HW_SHARED_LOCK_BUF              (HW_VRAM_C_LOCK_BUF)
#define HW_SHARED_LOCK_BUF_END          (HW_INIT_LOCK_BUF + 8)

#define HW_CHECK_DEBUGGER_SW     0x027ffc10     // (u16)debugger check switch. if 0 check buf1, else buf2.
#define HW_CHECK_DEBUGGER_BUF1   0x027ff814     // (u16)debugger checker. 1 if run on debugger.
#define HW_CHECK_DEBUGGER_BUF2   0x027ffc14     // (u16)debugger checker. 1 if run on debugger.


#ifdef __cplusplus
} /* extern "C" */
#endif
/* NITRO_HW_COMMON_MMAP_SHARED_H_ */
#endif

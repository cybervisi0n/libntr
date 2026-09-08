#ifndef TWL_HW_COMMON_MMAP_SHARED_H_
#define TWL_HW_COMMON_MMAP_SHARED_H_
#ifdef __cplusplus
extern "C" {
#endif

#define HW_TWL_CARD_ROM_HEADER_BUF (HW_MAIN_MEM + 0x00ffc000)
#define HW_TWL_CARD_ROM_HEADER_BUF_SIZE 0x1000
#define HW_TWL_CARD_ROM_HEADER_BUF_END                                         \
  (HW_TWL_CARD_ROM_HEADER_BUF + HW_TWL_CARD_ROM_HEADER_BUF_SIZE)

#define HW_TWL_SHARED_RESERVED HW_TWL_CARD_ROM_HEADER_BUF_END
#define HW_TWL_SHARED_RESERVED_END HW_SYSM_VER_INFO_CONTENT_ID

#define HW_SYSM_VER_INFO_CONTENT_ID                                            \
  (HW_MAIN_MEM + 0x00ffd7b0) // 8-byte, NULL-terminated contentID
#define HW_SYSM_VER_INFO_CONTENT_LAST_INITIAL_CODE (HW_MAIN_MEM + 0x00ffd7b9)

#define HW_SYSM_DISABLE_SET_HOTBOOT (HW_MAIN_MEM + 0x00ffd7ba)

#define HW_SD_NAND_CONTEXT_BUF                                                 \
  (HW_MAIN_MEM +                                                               \
   0x00ffd7bc) // NAND context data for the SD driver taken from the launcher
#define HW_SD_NAND_CONTEXT_BUF_END (HW_MAIN_MEM + 0x00ffd800)

#define HW_OS_TITLE_ID_LIST (HW_MAIN_MEM + 0x00ffd800) // User TitleID list
#define HW_OS_TITLE_ID_LIST_SIZE 0x400

#define HW_TWL_FS_MOUNT_INFO_BUF                                               \
  (HW_MAIN_MEM +                                                               \
   0x00ffdc00) // Mount information (A size of 0x3c0 is allocated. You can
               // specify up to 11 mount points. 11*84+1=0x39d)
#define HW_TWL_FS_BOOT_SRL_PATH_BUF                                            \
  (HW_MAIN_MEM + 0x00ffdfc0) // Path to the SRL file for startup

#define HW_TWL_ROM_HEADER_BUF (HW_MAIN_MEM + 0x00ffe000)
#define HW_TWL_ROM_HEADER_BUF_SIZE 0x1000
#define HW_TWL_ROM_HEADER_BUF_END                                              \
  (HW_TWL_ROM_HEADER_BUF + HW_TWL_ROM_HEADER_BUF_SIZE)

#define HW_SHARED_ARENA_LO_DEFAULT HW_MAIN_MEM_SHARED
#define HW_SHARED_ARENA_SIZE_DEFAULT                                           \
  (HW_SHARED_ARENA_HI_DEFAULT - HW_SHARED_ARENA_LO_DEFAULT) // 1664 bytes
#define HW_SHARED_ARENA_HI_DEFAULT HW_PSEG1_RESERVED_0

#define HW_PSEG1_RESERVED_0 (HW_PSEG1_RESERVED_0_END - HW_PSEG1_RESERVED_0_SIZE)
#define HW_PSEG1_RESERVED_0_SIZE 0x180 // 384 bytes
#define HW_PSEG1_RESERVED_0_END HW_RED_RESERVED

#define HW_RED_RESERVED (HW_MAIN_MEM + 0x00fff800)
#define HW_RED_RESERVED_SIZE 0x200 // 512 bytes
#define HW_RED_RESERVED_END (HW_RED_RESERVED + HW_RED_RESERVED_SIZE)

#define HW_PSEG1_RESERVED_1 HW_RED_RESERVED_END
#define HW_PSEG1_RESERVED_1_SIZE                                               \
  (HW_PSEG1_RESERVED_1_END - HW_PSEG1_RESERVED_1) // 128 bytes
#define HW_PSEG1_RESERVED_1_END HW_CARD_ROM_HEADER

#define HW_CARD_ROM_HEADER (HW_MAIN_MEM + 0x00fffa80)
#define HW_CARD_ROM_HEADER_SIZE 0x160 // 352 bytes
#define HW_CARD_ROM_HEADER_END (HW_CARD_ROM_HEADER + HW_CARD_ROM_HEADER_SIZE)

#define HW_DOWNLOAD_PARAMETER (HW_MAIN_MEM + 0x00fffbe0)
#define HW_DOWNLOAD_PARAMETER_SIZE 0x20 // 32 bytes
#define HW_DOWNLOAD_PARAMETER_END                                              \
  (HW_DOWNLOAD_PARAMETER + HW_DOWNLOAD_PARAMETER_SIZE)

#define HW_MAIN_MEM_SYSTEM (HW_MAIN_MEM + 0x00fffc00)
#define HW_MAIN_MEM_SYSTEM_SIZE                                                \
  (HW_MAIN_MEM_SYSTEM_END - HW_MAIN_MEM_SYSTEM) // 1024 bytes
#define HW_MAIN_MEM_SYSTEM_END HW_MAIN_MEM_SHARED_END

#define HW_WRAM_EX_LOCK_BUF                                                    \
  (HW_MAIN_MEM + 0x00fff680) // WRAM lock buffer (END-0x180)
#define HW_WRAM_EX_LOCK_BUF_END                                                \
  (HW_MAIN_MEM + 0x00fff684) //                  (END-0x17c)
#define HW_RESET_LOCK_FLAG_BUF                                                 \
  (HW_MAIN_MEM + 0x00fff684) //                  (END-0x17c)
#define HW_RESET_LOCK_FLAG_BUF_END                                             \
  (HW_MAIN_MEM + 0x00fff688) //                  (END-0x17c)

#define HW_BOOT_CHECK_INFO_BUF                                                 \
  (HW_MAIN_MEM + 0x00fffc00) // Boot check info (END-0x400)
#define HW_BOOT_CHECK_INFO_BUF_END                                             \
  (HW_MAIN_MEM + 0x00fffc20) //                 (END-0x3e0)

#define HW_RESET_PARAMETER_BUF                                                 \
  (HW_MAIN_MEM + 0x00fffc20) // reset parameter (END-0x3e0)

#define HW_BOOT_SHAKEHAND_9                                                    \
  (HW_MAIN_MEM + 0x00fffc24) // to shake hand with ARM7 boot sequence
#define HW_BOOT_SHAKEHAND_7                                                    \
  (HW_MAIN_MEM + 0x00fffc26) // to shake hand with ARM9 boot sequence
#define HW_BOOT_SYNC_PHASE                                                     \
  (HW_MAIN_MEM + 0x00fffc28) // to synchronize ARM7/ARM9 boot sequences

#define HW_ROM_BASE_OFFSET_BUF                                                 \
  (HW_MAIN_MEM + 0x00fffc2c) // ROM offset of own program (END-0x3d4)
#define HW_ROM_BASE_OFFSET_BUF_END                                             \
  (HW_MAIN_MEM + 0x00fffc30) //                           (END-0x3d0)

#define HW_CTRDG_MODULE_INFO_BUF                                               \
  (HW_MAIN_MEM + 0x00fffc30) // Cartridge module info (END-0x3d0)
#define HW_CTRDG_MODULE_INFO_BUF_END                                           \
  (HW_MAIN_MEM + 0x00fffc3c) //                       (END-0x3c4)

#define HW_VBLANK_COUNT_BUF                                                    \
  (HW_MAIN_MEM + 0x00fffc3c) // VBlank counter (END-0x3c4)

#define HW_WM_BOOT_BUF                                                         \
  (HW_MAIN_MEM + 0x00fffc40) // WM buffer for Multi-Boot (END-0x3c0)
#define HW_WM_BOOT_BUF_END                                                     \
  (HW_MAIN_MEM + 0x00fffc80) //                          (END-0x380)

#define HW_NVRAM_USER_INFO                                                     \
  (HW_MAIN_MEM + 0x00fffc80) // NVRAM user info (END-0x380)
#define HW_NVRAM_USER_INFO_END                                                 \
  (HW_MAIN_MEM + 0x00fffd68) //                           (END-0x298)
#define HW_HW_SECURE_INFO                                                      \
  (HW_MAIN_MEM + 0x00fffd68) // Secure system information (END-0x298)
#define HW_HW_SECURE_INFO_END                                                  \
  (HW_MAIN_MEM + 0x00fffd80) //                           (END-0x280)

#define HW_BIOS_EXCP_STACK_MAIN                                                \
  (HW_MAIN_MEM +                                                               \
   0x00fffd80) // MAINP Debugger monitor exception handler(END-0x280)
#define HW_BIOS_EXCP_STACK_MAIN_END                                            \
  (HW_MAIN_MEM + 0x00fffd9c) //                                 (END-0x264)
#define HW_EXCP_VECTOR_MAIN                                                    \
  (HW_MAIN_MEM + 0x00fffd9c) // HW_EXCP_VECTOR_BUF for MAINP      (END-0x264)

#define HW_ARENA_INFO_BUF                                                      \
  (HW_MAIN_MEM +                                                               \
   0x00fffda0) // Arena data structure (27F_FDA0 to 27F_FDE7) (END-0x260)
#define HW_REAL_TIME_CLOCK_BUF (HW_MAIN_MEM + 0x00fffde8) // RTC

#define HW_SYS_CONF_BUF                                                        \
  (HW_MAIN_MEM + 0x00fffdf0) // System config data (END-0x210)
#define HW_SYS_CONF_BUF_END                                                    \
  (HW_MAIN_MEM + 0x00fffdf6) //                    (END-0x20a)

#define HW_PRINT_OUTPUT_ARM9                                                   \
  (HW_MAIN_MEM + 0x00fffdf6) // debug print window for ARM9
#define HW_PRINT_OUTPUT_ARM7                                                   \
  (HW_MAIN_MEM + 0x00fffdf7) // debug print window for ARM7
#define HW_PRINT_OUTPUT_ARM9ERR                                                \
  (HW_MAIN_MEM + 0x00fffdf8) // debug print window for ARM9 error
#define HW_PRINT_OUTPUT_ARM7ERR                                                \
  (HW_MAIN_MEM + 0x00fffdf9) // debug print window for ARM7 error

#define HW_NAND_FIRM_HOTSTART_FLAG                                             \
  (HW_MAIN_MEM + 0x00fffdfa) // debug print window for ARM7 error
#define HW_TWL_RED_LAUNCHER_VER                                                \
  (HW_MAIN_MEM + 0x00fffdfb) // RED launcher version
#define HW_PRELOAD_PARAMETER_ADDR                                              \
  (HW_MAIN_MEM + 0x00fffdfc) // preload parameter address

#define HW_ROM_HEADER_BUF                                                      \
  (HW_MAIN_MEM +                                                               \
   0x00fffe00) // ROM-internal registration area data buffer (END-0x200)
#define HW_ROM_HEADER_BUF_END                                                  \
  (HW_MAIN_MEM + 0x00ffff60) //                                    (END-0x0a0)
#define HW_ISD_RESERVED                                                        \
  (HW_MAIN_MEM + 0x00ffff60) // IS DEBUGGER Reserved (END-0xa0)
#define HW_ISD_RESERVED_END                                                    \
  (HW_MAIN_MEM + 0x00ffff80) //                      (END-0x80)

#define HW_PXI_SIGNAL_PARAM_ARM9                                               \
  (HW_MAIN_MEM + 0x00ffff80) // PXI Signal Param for ARM9
#define HW_PXI_SIGNAL_PARAM_ARM7                                               \
  (HW_MAIN_MEM + 0x00ffff84) // PXI Signal Param for ARM7
#define HW_PXI_HANDLE_CHECKER_ARM9                                             \
  (HW_MAIN_MEM + 0x00ffff88) // PXI Handle Checker for ARM9
#define HW_PXI_HANDLE_CHECKER_ARM7                                             \
  (HW_MAIN_MEM + 0x00ffff8c) // PXI Handle Checker for ARM7

#define HW_MIC_LAST_ADDRESS                                                    \
  (HW_MAIN_MEM + 0x00ffff90) // MIC new sampling data storage address
#define HW_MIC_SAMPLING_DATA                                                   \
  (HW_MAIN_MEM + 0x00ffff94) // MIC individual sampling results

#define HW_WM_CALLBACK_CONTROL                                                 \
  (HW_MAIN_MEM + 0x00ffff96) // Parameter to synchronize WM callback
#define HW_WM_RSSI_POOL                                                        \
  (HW_MAIN_MEM +                                                               \
   0x00ffff98) // Random number source depends on WM received signal intensity

#define HW_SET_CTRDG_MODULE_INFO_ONCE                                          \
  (HW_MAIN_MEM + 0x00ffff9a) // set ctrdg module info flag
#define HW_IS_CTRDG_EXIST (HW_MAIN_MEM + 0x00ffff9b) // ctrdg exist flag

#define HW_COMPONENT_PARAM                                                     \
  (HW_MAIN_MEM + 0x00ffff9c) // Parameter for component synchronization

#define HW_THREADINFO_MAIN                                                     \
  (HW_MAIN_MEM + 0x00ffffa0) // ThreadInfo for Main processor
#define HW_THREADINFO_SUB                                                      \
  (HW_MAIN_MEM + 0x00ffffa4) // ThreadInfo for subprocessor
#define HW_BUTTON_XY_BUF (HW_MAIN_MEM + 0x00ffffa8) // buffer for X and Y button
#define HW_TOUCHPANEL_BUF (HW_MAIN_MEM + 0x00ffffaa) // buffer for touch panel
#define HW_AUTOLOAD_SYNC_BUF                                                   \
  (HW_MAIN_MEM + 0x00ffffae) // buffer for autoload sync

#define HW_LOCK_ID_FLAG_MAIN                                                   \
  (HW_MAIN_MEM + 0x00ffffb0) // lockID flag for Main processor
#define HW_LOCK_ID_FLAG_SUB                                                    \
  (HW_MAIN_MEM + 0x00ffffb8) // lockID flag for subprocessor

#define HW_VRAM_C_LOCK_BUF                                                     \
  (HW_MAIN_MEM + 0x00ffffc0) // VRAM-C lock buffer (END-0x40)
#define HW_VRAM_D_LOCK_BUF                                                     \
  (HW_MAIN_MEM + 0x00ffffc8) // VRAM-D lock buffer (END-0x38)
#define HW_WRAM_BLOCK0_LOCK_BUF                                                \
  (HW_MAIN_MEM +                                                               \
   0x00ffffd0) // CPU internal work RAM - Block0 - lock buffer (END-0x30)
#define HW_WRAM_BLOCK1_LOCK_BUF                                                \
  (HW_MAIN_MEM +                                                               \
   0x00ffffd8) // CPU internal work RAM - Block1 - lock buffer (END-0x28)
#define HW_CARD_LOCK_BUF                                                       \
  (HW_MAIN_MEM + 0x00ffffe0) // Game Card - lock buffer (END-0x20)
#define HW_CTRDG_LOCK_BUF                                                      \
  (HW_MAIN_MEM + 0x00ffffe8) // DS Pak - lock buffer (END-0x18)
#define HW_INIT_LOCK_BUF                                                       \
  (HW_MAIN_MEM + 0x00fffff0) // Initialization lock buffer  (END-0x10)

#define HW_MMEMCHECKER_MAIN                                                    \
  (HW_MAIN_MEM +                                                               \
   0x00fffff8) // MainMemory Size Checker for Main processor (END-8)
#define HW_MMEMCHECKER_SUB                                                     \
  (HW_MAIN_MEM +                                                               \
   0x00fffffa) // MainMemory Size Checker for Sub processor  (END-6)

#define HW_CHIPTYPE_FLAG (HW_MAIN_MEM + 0x00fffffc) // chiptype flag (END-4)

#define HW_CMD_AREA                                                            \
  (HW_MAIN_MEM +                                                               \
   0x00fffffe) // Main memory command issue area (Prohibited use area) (END-2)

#define HW_SHARED_LOCK_BUF (HW_VRAM_C_LOCK_BUF)
#define HW_SHARED_LOCK_BUF_END (HW_INIT_LOCK_BUF + 8)

#define HW_CHECK_DEBUGGER_SW                                                   \
  0x027ffc10 // (u16) debugger check switch. if 0 check buf1, else buf2.
#define HW_CHECK_DEBUGGER_BUF1                                                 \
  0x027ff814 // (u16) debugger checker. 1 if run on debugger.
#define HW_CHECK_DEBUGGER_BUF2                                                 \
  0x027ffc14 // (u16) debugger checker. 1 if run on debugger.

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* TWL_HW_COMMON_MMAP_SHARED_H_ */

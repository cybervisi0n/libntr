#ifndef NITRO_CARD_TYPES_H_
#define NITRO_CARD_TYPES_H_

#include <nitro/misc.h>
#include <nitro/types.h>

#if SDK_VERSION_MAJOR != 5
#error This file is for SDK Version 5 only!
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CARDRomRegion {
  u32 offset;
  u32 length;
} CARDRomRegion;

typedef struct CARDRomHeader {

  char game_name[12];
  u32 game_code;
  u16 maker_code;
  u8 product_id;
  u8 device_type;
  u8 device_size;
  u8 reserved_A[9];
  u8 game_version;
  u8 property;

  #ifdef SDK_PORT
	u32 main_rom_offset;
	u32 main_entry_address;
	u32 main_ram_address;
	u32 main_size;
	u32 sub_rom_offset;
	u32 sub_entry_address;
	u32 sub_ram_address;
	u32 sub_size;
	#else
  void *main_rom_offset;
  void *main_entry_address;
  void *main_ram_address;
  u32 main_size;
  void *sub_rom_offset;
  void *sub_entry_address;
  void *sub_ram_address;
  u32 sub_size;
  #endif

  CARDRomRegion fnt;
  CARDRomRegion fat;

  CARDRomRegion main_ovt;
  CARDRomRegion sub_ovt;

  u8 rom_param_A[8];
  u32 banner_offset;
  u16 secure_crc;
  u8 rom_param_B[2];

  void *main_autoload_done;
  void *sub_autoload_done;

  u8 rom_param_C[8];
  u32 rom_size;
  u32 header_size;
  u32 main_module_param_offset;
  u32 sub_module_param_offset;

  u16 normal_area_rom_offset;
  u16 twl_ltd_area_rom_offset;
  u8 reserved_B[0x2C];

  u8 logo_data[0x9C];
  u16 logo_crc;
  u16 header_crc;

} CARDRomHeader;

typedef CARDRomHeader CARDRomHeaderNTR;

typedef struct CARDRomHeaderTWL {
  CARDRomHeaderNTR ntr;
  u8 debugger_reserved[0x20];
  u8 config1[0x34];

  struct {
    u32 : 5;
    u32 game_card_on : 1;
    u32 : 2;
    u32 game_card_nitro_mode : 1;
    u32 : 2;
    u32 photo_access_read : 1;
    u32 photo_access_write : 1;
    u32 sdmc_access_read : 1;
    u32 sdmc_access_write : 1;
    u32 backup_access_read : 1;
    u32 backup_access_write : 1;
    u32 : 0;
  } access_control;
  u8 reserved_0x1B8[8]; // Reserved (all 0's)
  u32 main_ltd_rom_offset;
  u8 reserved_0x1C4[4]; // Reserved (all 0's)
  void *main_ltd_ram_address;
  u32 main_ltd_size;
  u32 sub_ltd_rom_offset;
  u8 reserved_0x1D4[4]; // Reserved (all 0's)
  void *sub_ltd_ram_address;
  u32 sub_ltd_size;
  CARDRomRegion digest_area_ntr;
  CARDRomRegion digest_area_ltd;
  CARDRomRegion digest_tabel1;
  CARDRomRegion digest_tabel2;
  u32 digest_table1_size;
  u32 digest_table2_sectors;
  u8 config2[0xF8]; // Group of flags used internally
  u8 main_static_digest[0x14];
  u8 sub_static_digest[0x14];
  u8 digest_tabel2_digest[0x14];
  u8 banner_digest[0x14];
  u8 main_ltd_static_digest[0x14];
  u8 sub_ltd_static_digest[0x14];
} CARDRomHeaderTWL;

SDK_COMPILER_ASSERT(sizeof(CARDRomHeader) == 0x160);
SDK_COMPILER_ASSERT(sizeof(CARDRomHeaderTWL) == 0x378);

#define CARD_ROM_PAGE_SIZE 512

#define CARD_ROM_DOWNLOAD_SIGNATURE_SIZE 136

typedef enum CARDResult {
  CARD_RESULT_SUCCESS = 0,
  CARD_RESULT_FAILURE,
  CARD_RESULT_INVALID_PARAM,
  CARD_RESULT_UNSUPPORTED,
  CARD_RESULT_TIMEOUT,
  CARD_RESULT_ERROR,
  CARD_RESULT_NO_RESPONSE,
  CARD_RESULT_CANCELED
} CARDResult;

#define CARD_ROM_HEADER_EXE_NTR_OFF 0x01
#define CARD_ROM_HEADER_EXE_TWL_ON 0x02

SDK_INLINE BOOL CARD_IsExecutableOnNTR(const CARDRomHeader *header) {
  return ((header->product_id & CARD_ROM_HEADER_EXE_NTR_OFF) == 0);
}

SDK_INLINE BOOL CARD_IsExecutableOnTWL(const CARDRomHeader *header) {
  return ((header->product_id & CARD_ROM_HEADER_EXE_TWL_ON) != 0);
}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // NITRO_CARD_TYPES_H_

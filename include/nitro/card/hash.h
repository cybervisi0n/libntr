#ifndef NITRO_CARD_HASH_H_
#define NITRO_CARD_HASH_H_

#if SDK_VERSION_MAJOR < 5
#error This file is for SDK version 5 only!
#endif

#include <nitro/misc.h>
#include <nitro/types.h>
#include <nitro/card/types.h>
#include <nitro/mi/device.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CARD_ROM_HASH_SIZE 20

struct CARDRomHashSector;
struct CARDRomHashBlock;
struct CARDRomHashContext;

typedef struct CARDRomHashSector {
  struct CARDRomHashSector *next;
  u32 index;
  u32 offset;
  void *image;
} CARDRomHashSector;

typedef struct CARDRomHashBlock {
  struct CARDRomHashBlock *next;
  u32 index;
  u32 offset;
  u8 *hash;
  u8 *hash_aligned;
} CARDRomHashBlock;

typedef struct CARDRomHashContext {

  CARDRomRegion area_ntr;
  CARDRomRegion area_ltd;
  CARDRomRegion sector_hash;
  CARDRomRegion block_hash;
  u32 bytes_per_sector;
  u32 sectors_per_block;
  u32 block_max;
  u32 sector_max;

  void *userdata;
  MIDeviceReadFunction ReadSync;
  MIDeviceReadFunction ReadAsync;

  OSThread *loader;
  void *recent_load;

  CARDRomHashSector *loading_sector; // Sector waiting for media to load
  CARDRomHashSector *loaded_sector;  // Sector waiting for hash verification
  CARDRomHashSector *valid_sector;   // Sector that has already been verified
  CARDRomHashBlock *loading_block;   // Block waiting for media to load
  CARDRomHashBlock *loaded_block;    // Block waiting for hash verification
  CARDRomHashBlock *valid_block;     // Block that has already been verified

  u8 *master_hash;            // A block hash array
  u8 *images;                 // Sector image
  u8 *hashes;                 // The hash array within a block
  CARDRomHashSector *sectors; // Sector data
  CARDRomHashBlock *blocks;   // Block data
} CARDRomHashContext;

void CARD_InitRomHashContext(CARDRomHashContext *context,
                             const CARDRomHeaderTWL *header, void *buffer,
                             u32 length, MIDeviceReadFunction sync,
                             MIDeviceReadFunction async, void *userdata);

u32 CARD_CalcRomHashBufferLength(const CARDRomHeaderTWL *header);

void CARD_NotifyRomHashReadAsync(CARDRomHashContext *context);

void CARD_ReadRomHashImage(CARDRomHashContext *context, void *buffer,
                           u32 offset, u32 length);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // NITRO_CARD_HASH_H_

#ifndef NITRO_LIBRARIES_CARD_COMMAND_H__
#define NITRO_LIBRARIES_CARD_COMMAND_H__

#if SDK_VERSION_MAJOR != 5
#error This file is for SDK version 5 only!
#endif

#include <nitro/card/common.h>
#include <nitro/card/backup.h>

#include <nitro/pxi.h>
#include <nitro/os.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CARD_BACKUP_CAPS_AVAILABLE (u32)(CARD_BACKUP_CAPS_READ - 1)
#define CARD_BACKUP_CAPS_READ (u32)(1 << CARD_REQ_READ_BACKUP)
#define CARD_BACKUP_CAPS_WRITE (u32)(1 << CARD_REQ_WRITE_BACKUP)
#define CARD_BACKUP_CAPS_PROGRAM (u32)(1 << CARD_REQ_PROGRAM_BACKUP)
#define CARD_BACKUP_CAPS_VERIFY (u32)(1 << CARD_REQ_VERIFY_BACKUP)
#define CARD_BACKUP_CAPS_ERASE_PAGE (u32)(1 << CARD_REQ_ERASE_PAGE_BACKUP)
#define CARD_BACKUP_CAPS_ERASE_SECTOR (u32)(1 << CARD_REQ_ERASE_SECTOR_BACKUP)
#define CARD_BACKUP_CAPS_ERASE_CHIP (u32)(1 << CARD_REQ_ERASE_CHIP_BACKUP)
#define CARD_BACKUP_CAPS_READ_STATUS (u32)(1 << CARD_REQ_READ_STATUS)
#define CARD_BACKUP_CAPS_WRITE_STATUS (u32)(1 << CARD_REQ_WRITE_STATUS)
#define CARD_BACKUP_CAPS_ERASE_SUBSECTOR                                       \
  (u32)(1 << CARD_REQ_ERASE_SUBSECTOR_BACKUP)

typedef struct CARDiCommandArg {
  CARDResult result;   /* Return value */
  CARDBackupType type; /* Device type */
  u32 id;              /* Card ID. */
  u32 src;             /* Transfer source */
  u32 dst;             /* Transfer destination */
  u32 len;             /* Transfer length */

  struct {

    u32 total_size;   /* Memory capacity (BYTE) */
    u32 sect_size;    /* Write-sector (BYTE) */
    u32 subsect_size; /* Write-sector (BYTE) */
    u32 page_size;    /* Write-page (BYTE) */
    u32 addr_width;   /* Address width on command (BYTE) */

    u32 program_page;
    u32 write_page;
    u32 write_page_total;
    u32 erase_chip;
    u32 erase_chip_total;
    u32 erase_sector;
    u32 erase_sector_total;
    u32 erase_subsector;
    u32 erase_subsector_total;
    u32 erase_page;

    u8 initial_status;
    u8 padding1[3];
    u32 caps;
    u8 padding2[4];
  } spec;

} CARDiCommandArg;

SDK_COMPILER_ASSERT(sizeof(CARDiCommandArg) % 32 == 0);

SDK_INLINE void CARDi_SendPxi(u32 data) {
#ifdef SDK_BUILD_ARM
  while (PXI_SendWordByFifo(PXI_FIFO_TAG_FS, data, TRUE) < 0) {
  }
#endif
}

#if defined(SDK_ARM7)

void CARDi_LockMutexForARM7(void);

void CARDi_UnlockMutexForARM7(void);
#endif // if defined(SDK_ARM7)

#ifdef SDK_PORT
void CARDi_OnFifoRecv(PXIFifoTag tag, u64 data, BOOL err);
#else
void CARDi_OnFifoRecv(PXIFifoTag tag, u32 data, BOOL err);
#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // NITRO_LIBRARIES_CARD_COMMAND_H__

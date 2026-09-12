#if !defined(NITRO_CARD_BACKUP_H_)
#define NITRO_CARD_BACKUP_H_

#include <nitro/types.h>

#if SDK_VERSION_MAJOR == 5
#include <nitro/card/types.h>
#endif

#ifdef SDK_PORT
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

#ifdef __cplusplus
extern  "C"
{
#endif

#if SDK_VERSION_MAJOR == 5
#define CARD_BACKUP_TYPE_DEVICE_SHIFT 0
#define CARD_BACKUP_TYPE_DEVICE_MASK 0xFF
#define CARD_BACKUP_TYPE_DEVICE_EEPROM 1
#define CARD_BACKUP_TYPE_DEVICE_FLASH 2
#define CARD_BACKUP_TYPE_DEVICE_FRAM 3
#define CARD_BACKUP_TYPE_SIZEBIT_SHIFT 8
#define CARD_BACKUP_TYPE_SIZEBIT_MASK 0xFF
#define CARD_BACKUP_TYPE_VENDER_SHIFT 16
#define CARD_BACKUP_TYPE_VENDER_MASK 0xFF
#define CARD_BACKUP_TYPE_DEFINE(type, size, vender)                            \
  (((CARD_BACKUP_TYPE_DEVICE_##type) << CARD_BACKUP_TYPE_DEVICE_SHIFT) |       \
   ((size) << CARD_BACKUP_TYPE_SIZEBIT_SHIFT) |                                \
   ((vender) << CARD_BACKUP_TYPE_VENDER_SHIFT))

typedef enum CARDBackupType {
  CARD_BACKUP_TYPE_EEPROM_4KBITS = CARD_BACKUP_TYPE_DEFINE(EEPROM, 9, 0),
  CARD_BACKUP_TYPE_EEPROM_64KBITS = CARD_BACKUP_TYPE_DEFINE(EEPROM, 13, 0),
  CARD_BACKUP_TYPE_EEPROM_512KBITS = CARD_BACKUP_TYPE_DEFINE(EEPROM, 16, 0),
  CARD_BACKUP_TYPE_EEPROM_1MBITS = CARD_BACKUP_TYPE_DEFINE(EEPROM, 17, 0),
  CARD_BACKUP_TYPE_FLASH_2MBITS = CARD_BACKUP_TYPE_DEFINE(FLASH, 18, 0),
  CARD_BACKUP_TYPE_FLASH_4MBITS = CARD_BACKUP_TYPE_DEFINE(FLASH, 19, 0),
  CARD_BACKUP_TYPE_FLASH_8MBITS = CARD_BACKUP_TYPE_DEFINE(FLASH, 20, 0),
  CARD_BACKUP_TYPE_FLASH_16MBITS = CARD_BACKUP_TYPE_DEFINE(FLASH, 21, 0),
  CARD_BACKUP_TYPE_FLASH_64MBITS = CARD_BACKUP_TYPE_DEFINE(FLASH, 23, 0),
  CARD_BACKUP_TYPE_FRAM_256KBITS = CARD_BACKUP_TYPE_DEFINE(FRAM, 15, 0),
  CARD_BACKUP_TYPE_NOT_USE = 0
} CARDBackupType;

#define CARD_BACKUP_TYPE_FLASH_64MBITS_EX                                      \
  (CARDBackupType) CARD_BACKUP_TYPE_DEFINE(FLASH, 23, 1)

typedef enum CARDRequest {
  CARD_REQ_INIT = 0,               /* initialize (setting from ARM9) */
  CARD_REQ_ACK,                    /* request done (acknowledge from ARM7) */
  CARD_REQ_IDENTIFY,               /* CARD_IdentifyBackup */
  CARD_REQ_READ_ID,                /* CARD_ReadRomID (TEG && ARM9) */
  CARD_REQ_READ_ROM,               /* CARD_ReadRom (TEG && ARM9) */
  CARD_REQ_WRITE_ROM,              /* (reserved) */
  CARD_REQ_READ_BACKUP,            /* CARD_ReadBackup */
  CARD_REQ_WRITE_BACKUP,           /* CARD_WriteBackup */
  CARD_REQ_PROGRAM_BACKUP,         /* CARD_ProgramBackup */
  CARD_REQ_VERIFY_BACKUP,          /* CARD_VerifyBackup */
  CARD_REQ_ERASE_PAGE_BACKUP,      /* CARD_EraseBackupPage */
  CARD_REQ_ERASE_SECTOR_BACKUP,    /* CARD_EraseBackupSector */
  CARD_REQ_ERASE_CHIP_BACKUP,      /* CARD_EraseBackupChip */
  CARD_REQ_READ_STATUS,            /* CARD_ReadStatus */
  CARD_REQ_WRITE_STATUS,           /* CARD_WriteStatus */
  CARD_REQ_ERASE_SUBSECTOR_BACKUP, /* CARD_EraseBackupSubSector */
  CARD_REQ_MAX
} CARDRequest;

typedef enum CARDRequestMode {
  CARD_REQUEST_MODE_RECV,        /* Receive data */
  CARD_REQUEST_MODE_SEND,        /* Send data (including single verify) */
  CARD_REQUEST_MODE_SEND_VERIFY, /* Send data + verify */
  CARD_REQUEST_MODE_SPECIAL      /* Special operations like sector deletion */
} CARDRequestMode;

#define CARD_RETRY_COUNT_MAX 10

#define CARD_PXI_COMMAND_MASK 0x0000003f // command part
#define CARD_PXI_COMMAND_SHIFT 0
#define CARD_PXI_COMMAND_PARAM_MASK 0x01ffffc0 // parameter part
#define CARD_PXI_COMMAND_PARAM_SHIFT 6

#define CARD_PXI_COMMAND_TERMINATE 0x0001  // arm9->arm7 terminate command
#define CARD_PXI_COMMAND_PULLED_OUT 0x0011 // arm7->arm9 pulled out message
#define CARD_PXI_COMMAND_RESET_SLOT 0x0002 // arm7->arm9 reset-slot message

BOOL CARD_IdentifyBackup(CARDBackupType type);
CARDBackupType CARD_GetCurrentBackupType(void);
u32 CARD_GetBackupTotalSize(void);
u32 CARD_GetBackupSectorSize(void);
u32 CARD_GetBackupPageSize(void);
#endif


  void CARD_LockBackup(u16 lock_id);
  void CARD_UnlockBackup(u16 lock_id);
  BOOL CARD_TryWaitBackupAsync(void);
  BOOL CARD_WaitBackupAsync(void);
  void CARD_CancelBackupAsync(void);

  BOOL CARDi_RequestStreamCommand(u32 src, u32 dst, u32 len,
                                  MIDmaCallback callback, void * arg, BOOL is_async,
                                  CARDRequest req_type, int req_retry, CARDRequestMode req_mode);

  BOOL CARDi_RequestWriteSectorCommand(u32 src, u32 dst, u32 len, BOOL verify,
                                       MIDmaCallback callback, void * arg, BOOL is_async);


  SDK_INLINE BOOL CARDi_ReadBackup(u32 src, void *dst, u32 len,
                                   MIDmaCallback callback, void *arg, BOOL is_async)
  {
    #ifdef SDK_PORT
    FILE * backupFilePtr;
    backupFilePtr = fopen("save.bin", "rb");
    if(!backupFilePtr) {
      // File does not exist. Create the file
      backupFilePtr = fopen("save.bin", "wb");

      // Zero out the file with the len
      u8 * buf = (u8*)malloc(len + src);
      memset(buf, 0, len + src);
      fwrite(buf, 1, len + src, backupFilePtr);
      fclose(backupFilePtr);
      free(buf);

      // Reopen file
      backupFilePtr = fopen("save.bin", "rb");
    }
    fseek(backupFilePtr, src, SEEK_SET);
    fread(dst, 1, len, backupFilePtr);
    fclose(backupFilePtr);
    if( callback != NULL )
    {
      callback(arg);
    }

    return TRUE;
    #else
    return CARDi_RequestStreamCommand((u32)src, (u32)dst, len,
                                      callback, arg, is_async,
                                      CARD_REQ_READ_BACKUP, 1, CARD_REQUEST_MODE_RECV);
    #endif
  }

  SDK_INLINE BOOL CARDi_ProgramBackup(u32 dst, const void *src, u32 len,
                                      MIDmaCallback callback, void *arg, BOOL is_async)
  {
    #ifdef SDK_PORT
    return CARDi_RequestStreamCommand((u64)src, (u64)dst, len, callback, arg, is_async,
                                      CARD_REQ_PROGRAM_BACKUP, CARD_RETRY_COUNT_MAX,
                                      CARD_REQUEST_MODE_SEND);
    #else
    return CARDi_RequestStreamCommand((u32)src, (u32)dst, len, callback, arg, is_async,
                                      CARD_REQ_PROGRAM_BACKUP, CARD_RETRY_COUNT_MAX,
                                      CARD_REQUEST_MODE_SEND);
    #endif
  }

  SDK_INLINE BOOL CARDi_WriteBackup(u32 dst, const void *src, u32 len,
                                    MIDmaCallback callback, void *arg, BOOL is_async)
  {
    #ifdef SDK_PORT
    FILE * backupFilePtr;
    backupFilePtr = fopen("save.bin", "rb+");
    fseek(backupFilePtr, dst, SEEK_SET);
    fwrite(src, 1, len, backupFilePtr);
    fclose(backupFilePtr);
    if( callback != NULL )
    {
      callback(arg);
    }
    return TRUE;
    #else
    return CARDi_RequestStreamCommand((u32)src, (u32)dst, len, callback, arg, is_async,
                                      CARD_REQ_WRITE_BACKUP, CARD_RETRY_COUNT_MAX,
                                      CARD_REQUEST_MODE_SEND);
    #endif
  }

  SDK_INLINE BOOL CARDi_VerifyBackup(u32 dst, const void *src, u32 len,
                                     MIDmaCallback callback, void *arg, BOOL is_async)
  {
    #ifdef SDK_PORT
    return CARDi_RequestStreamCommand((u64)src, (u64)dst, len, callback, arg, is_async,
                                      CARD_REQ_VERIFY_BACKUP, 1, CARD_REQUEST_MODE_SEND);
    #else
    return CARDi_RequestStreamCommand((u32)src, (u32)dst, len, callback, arg, is_async,
                                      CARD_REQ_VERIFY_BACKUP, 1, CARD_REQUEST_MODE_SEND);
    #endif
  }

  SDK_INLINE BOOL CARDi_ProgramAndVerifyBackup(u32 dst, const void *src, u32 len,
                                               MIDmaCallback callback, void *arg, BOOL is_async)
  {
    #ifdef SDK_PORT
    return CARDi_RequestStreamCommand((u64)src, (u64)dst, len, callback, arg, is_async,
                                      CARD_REQ_PROGRAM_BACKUP, CARD_RETRY_COUNT_MAX,
                                      CARD_REQUEST_MODE_SEND_VERIFY);
    #else
    return CARDi_RequestStreamCommand((u32)src, (u32)dst, len, callback, arg, is_async,
                                      CARD_REQ_PROGRAM_BACKUP, CARD_RETRY_COUNT_MAX,
                                      CARD_REQUEST_MODE_SEND_VERIFY);
    #endif
  }

  SDK_INLINE BOOL CARDi_WriteAndVerifyBackup(u32 dst, const void *src, u32 len,
                                             MIDmaCallback callback, void *arg, BOOL is_async)
  {
    #ifdef SDK_PORT
    FILE * backupFilePtr;
    backupFilePtr = fopen("save.bin", "rb+");
    fseek(backupFilePtr, dst, SEEK_SET);
    fwrite(src, 1, len, backupFilePtr);
    fclose(backupFilePtr);
    if( callback != NULL )
    {
      callback(arg);
    }
    return CARD_RESULT_SUCCESS;
    #else
    return CARDi_RequestStreamCommand((u32)src, (u32)dst, len, callback, arg, is_async,
                                      CARD_REQ_WRITE_BACKUP, CARD_RETRY_COUNT_MAX,
                                      CARD_REQUEST_MODE_SEND_VERIFY);
    #endif
  }

  SDK_INLINE BOOL CARDi_EraseBackupSector (u32 dst, u32 len,
                                           MIDmaCallback callback, void * arg, BOOL is_async)
  {
    return CARDi_RequestStreamCommand(0, (u32)dst, len, callback, arg, is_async,
                                      CARD_REQ_ERASE_SECTOR_BACKUP, CARD_RETRY_COUNT_MAX,
                                      CARD_REQUEST_MODE_SPECIAL);
  }

  SDK_INLINE BOOL CARDi_EraseBackupSubSector (u32 dst, u32 len,
                                              MIDmaCallback callback, void * arg, BOOL is_async)
  {
    return CARDi_RequestStreamCommand(0, (u32)dst, len, callback, arg, is_async,
                                      CARD_REQ_ERASE_SUBSECTOR_BACKUP, CARD_RETRY_COUNT_MAX,
                                      CARD_REQUEST_MODE_SPECIAL);
  }

  SDK_INLINE BOOL CARDi_EraseBackupChip (MIDmaCallback callback, void * arg, BOOL is_async)
  {
    return CARDi_RequestStreamCommand(0, 0, 0, callback, arg, is_async,
                                      CARD_REQ_ERASE_CHIP_BACKUP, 1, CARD_REQUEST_MODE_SPECIAL);
  }

  SDK_INLINE void CARD_ReadBackupAsync (u32 src, void * dst, u32 len, MIDmaCallback callback, void * arg)
  {
    (void)CARDi_ReadBackup(src, dst, len, callback, arg, TRUE);
  }

  SDK_INLINE BOOL CARD_ReadBackup (u32 src, void * dst, u32 len)
  {
    return CARDi_ReadBackup(src, dst, len, NULL, NULL, FALSE);
  }

  SDK_INLINE void CARD_ProgramBackupAsync (u32 dst, const void * src, u32 len,
                                           MIDmaCallback callback, void * arg)
  {
    (void)CARDi_ProgramBackup(dst, src, len, callback, arg, TRUE);
  }

  SDK_INLINE BOOL CARD_ProgramBackup (u32 dst, const void * src, u32 len)
  {
    return CARDi_ProgramBackup(dst, src, len, NULL, NULL, FALSE);
  }

  SDK_INLINE void CARD_WriteBackupAsync (u32 dst, const void * src, u32 len,
                                         MIDmaCallback callback, void * arg)
  {
    (void)CARDi_WriteBackup(dst, src, len, callback, arg, TRUE);
  }

  SDK_INLINE BOOL CARD_WriteBackup (u32 dst, const void * src, u32 len)
  {
    return CARDi_WriteBackup(dst, src, len, NULL, NULL, FALSE);
  }

  SDK_INLINE void CARD_VerifyBackupAsync (u32 dst, const void * src, u32 len,
                                          MIDmaCallback callback, void * arg)
  {
    (void)CARDi_VerifyBackup(dst, src, len, callback, arg, TRUE);
  }

  SDK_INLINE BOOL CARD_VerifyBackup (u32 dst, const void * src, u32 len)
  {
    return CARDi_VerifyBackup(dst, src, len, NULL, NULL, FALSE);
  }

  SDK_INLINE void CARD_ProgramAndVerifyBackupAsync (u32 dst, const void * src, u32 len,
                                                    MIDmaCallback callback, void * arg)
  {
    (void)CARDi_ProgramAndVerifyBackup(dst, src, len, callback, arg, TRUE);
  }

  SDK_INLINE BOOL CARD_ProgramAndVerifyBackup (u32 dst, const void * src, u32 len)
  {
    return CARDi_ProgramAndVerifyBackup(dst, src, len, NULL, NULL, FALSE);
  }

  SDK_INLINE void CARD_WriteAndVerifyBackupAsync (u32 dst, const void * src, u32 len,
                                                  MIDmaCallback callback, void * arg)
  {
    (void)CARDi_WriteAndVerifyBackup(dst, src, len, callback, arg, TRUE);
  }

  SDK_INLINE BOOL CARD_WriteAndVerifyBackup (u32 dst, const void * src, u32 len)
  {
    return CARDi_WriteAndVerifyBackup(dst, src, len, NULL, NULL, FALSE);
  }

  SDK_INLINE void CARD_EraseBackupSectorAsync (u32 dst, u32 len, MIDmaCallback callback, void * arg)
  {
    (void)CARDi_EraseBackupSector(dst, len, callback, arg, TRUE);
  }

  SDK_INLINE BOOL CARD_EraseBackupSector (u32 dst, u32 len)
  {
    return CARDi_EraseBackupSector(dst, len, NULL, NULL, FALSE);
  }

  SDK_INLINE void CARD_EraseBackupSubSectorAsync (u32 dst, u32 len, MIDmaCallback callback, void * arg)
  {
    (void)CARDi_EraseBackupSubSector(dst, len, callback, arg, TRUE);
  }

  SDK_INLINE BOOL CARD_EraseBackupSubSector (u32 dst, u32 len)
  {
    return CARDi_EraseBackupSubSector(dst, len, NULL, NULL, FALSE);
  }

  SDK_INLINE void CARD_EraseBackupChipAsync (MIDmaCallback callback, void * arg)
  {
    (void)CARDi_EraseBackupChip(callback, arg, TRUE);
  }

  SDK_INLINE BOOL CARD_EraseBackupChip (void)
  {
    return CARDi_EraseBackupChip(NULL, NULL, FALSE);
  }

  SDK_INLINE void CARD_WriteBackupSectorAsync (u32 dst, const void * src, u32 len,
                                               MIDmaCallback callback, void * arg)
  {
    #ifdef SDK_PORT
    (void)CARDi_RequestWriteSectorCommand((u64)src, dst, len, FALSE, callback, arg, TRUE);
    #else
    (void)CARDi_RequestWriteSectorCommand((u32)src, dst, len, FALSE, callback, arg, TRUE);
    #endif
  }

  SDK_INLINE BOOL CARD_WriteBackupSector (u32 dst, const void * src, u32 len)
  {
    #ifdef SDK_PORT
    return CARDi_RequestWriteSectorCommand((u64)src, dst, len, FALSE, NULL, NULL, FALSE);
    #else
    return CARDi_RequestWriteSectorCommand((u32)src, dst, len, FALSE, NULL, NULL, FALSE);
    #endif
  }

  SDK_INLINE void CARD_WriteAndVerifyBackupSectorAsync(u32 dst, const void *src, u32 len,
                                                       MIDmaCallback callback, void *arg)
  {
    #ifdef SDK_PORT
    (void)CARDi_RequestWriteSectorCommand((u64)src, dst, len, TRUE, callback, arg, TRUE);
    #else
    (void)CARDi_RequestWriteSectorCommand((u32)src, dst, len, TRUE, callback, arg, TRUE);
    #endif
  }

  SDK_INLINE BOOL CARD_WriteAndVerifyBackupSector(u32 dst, const void *src, u32 len)
  {
    #ifdef SDK_PORT
    return CARDi_RequestWriteSectorCommand((u64)src, dst, len, TRUE, NULL, NULL, FALSE);
    #else
    return CARDi_RequestWriteSectorCommand((u32)src, dst, len, TRUE, NULL, NULL, FALSE);
    #endif
  }

  int CARDi_AccessStatus(CARDRequest command, u8 value);

  SDK_INLINE int CARDi_ReadStatus (void)
  {
    return CARDi_AccessStatus(CARD_REQ_READ_STATUS, 0);
  }

  SDK_INLINE BOOL CARDi_WriteStatus (u8 value)
  {
    return (CARDi_AccessStatus(CARD_REQ_WRITE_STATUS, value) >= 0);
  }

  #ifdef __cplusplus
}
#endif

#endif

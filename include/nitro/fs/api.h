#if !defined(NITRO_FS_API_H_)
#define NITRO_FS_API_H_

#if SDK_VERSION_MAJOR != 5
#error This file is for SDK version 5 only!
#endif

#include <nitro/fs/archive.h>
#include <nitro/fs/file.h>
#include <nitro/fs/romfat.h>
#include <nitro/fs/overlay.h>
#include <nitro/std.h>

#ifdef SDK_TWL
#include <twl/fatfs.h>
#endif // SDK_TWL

#ifdef __cplusplus
extern "C" {
#endif

void FS_Init(u32 default_dma_no);

#if defined(SDK_TWL)
void FS_InitFatDriver(void);
#endif

BOOL FS_IsAvailable(void);

void FS_End(void);

u32 FS_GetDefaultDMA(void);

u32 FS_SetDefaultDMA(u32 dma_no);

BOOL FS_SetCurrentDirectory(const char *path);

BOOL FS_CreateFileFromMemory(FSFile *file, void *buf, u32 size);

BOOL FS_CreateFileFromRom(FSFile *file, u32 offset, u32 size);

BOOL FS_OpenTopLevelDirectory(FSFile *dir);

u32 FS_TryLoadTable(void *mem, u32 size);

SDK_INLINE u32 FS_GetTableSize(void) { return FS_TryLoadTable(NULL, 0); }

SDK_INLINE BOOL FS_LoadTable(void *mem, u32 size) {
  return (FS_TryLoadTable(mem, size) <= size);
}

SDK_INLINE void *FS_UnloadTable(void) {
  return FS_UnloadArchiveTables(FS_FindArchive("rom", 3));
}

void FS_ForceToEnableLatencyEmulation(void);

#define FS_TMPBUF_LENGTH 2048
#define FS_MOUNTDRIVE_MAX OS_MOUNT_INFO_MAX
#define FS_TEMPORARY_BUFFER_MAX (FS_TMPBUF_LENGTH * FS_MOUNTDRIVE_MAX)

#ifdef SDK_TWL
typedef struct FSFATFSArchiveContext {
  FSArchive arc[1];
  char fullpath[2][FATFS_PATH_MAX];
  u8 *tmpbuf;
  FATFSDriveResource resource[1];
} FSFATFSArchiveContext;

typedef struct FSFATFSArchiveWork {
  u8 tmpbuf[FS_TMPBUF_LENGTH];
  FSFATFSArchiveContext context;
  int slot;
} FSFATFSArchiveWork;

extern FSFATFSArchiveContext *FSiFATFSDrive;

extern FATFSRequestBuffer *FSiFATFSAsyncRequest;

FSResult FSi_MountSpecialArchive(u64 param, const char *arcname,
                                 FSFATFSArchiveWork *pWork);

FSResult FSi_FormatSpecialArchive(const char *path);

#endif // SDK_TWL

extern u8
    *FSiTemporaryBuffer /* [FS_TEMPORARY_BUFFER_MAX] ATTRIBUTE_ALIGN(32)*/;

void FSi_SetupFATBuffers(void);

BOOL FSi_OverrideRomArchive(FSArchive *arc);

BOOL FSi_IsValidAddressForARM7(const void *buffer, u32 length);

void FSi_SetSwitchableWramSlots(int bitsB, int bitsC);

void FSi_UnmountRomAndCloseNANDSRL(void);

STDResult FSi_ConvertStringSjisToUnicode(u16 *dst, int *dst_len,
                                         const char *src, int *src_len,
                                         STDConvertUnicodeCallback callback);

STDResult FSi_ConvertStringUnicodeToSjis(char *dst, int *dst_len,
                                         const u16 *src, int *src_len,
                                         STDConvertSjisCallback callback);

u16 *FSi_GetUnicodeBuffer(const char *src);

void FSi_ReleaseUnicodeBuffer(const void *buf);

#define FS_DMA_NOT_USE MI_DMA_NOT_USE

BOOL FS_ChangeDir(const char *path);

#ifdef SDK_ARM7
#define FS_CreateReadServerThread(priority)                                    \
  (void)CARD_SetThreadPriority(priority)
#endif // SDK_ARM7

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NITRO_FS_API_H_ */

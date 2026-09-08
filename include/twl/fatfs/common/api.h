#ifndef NITRO_FATFS_API_H_
#define NITRO_FATFS_API_H_

#include <twl/fatfs/common/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(SDK_ARM9) || defined(SDK_PORT)
void FATFS_Init(void);
#else
BOOL FATFS_Init(u32 dma1, u32 dma2, u32 priority);
#endif

BOOL FATFSi_IsInitialized(void);

const char *FATFSi_GetArcnameList(void);

u32 FATFS_GetLastError(void);

void FATFS_RegisterResultBuffer(FATFSResultBuffer *buffer, BOOL enable);

BOOL FATFS_MountDrive(const char *name, FATFSMediaType media, u32 partition);

SDK_INLINE BOOL FATFS_MountNAND(const char *name, u32 partition) {
  return FATFS_MountDrive(name, FATFS_MEDIA_TYPE_NAND, partition);
}

BOOL FATFS_UnmountDrive(const char *name);

BOOL FATFS_SetDefaultDrive(const char *path);

BOOL FATFS_FormatDrive(const char *path);

BOOL FATFSi_FormatDriveEx(const char *path, BOOL formatMedia);

SDK_INLINE BOOL FATFSi_FormatMedia(const char *path) {
  return FATFSi_FormatDriveEx(path, TRUE);
}

BOOL FATFS_CheckDisk(const char *name, FATFSDiskInfo *info, BOOL verbose,
                     BOOL fixProblems, BOOL writeChains);

BOOL FATFS_GetDriveResource(const char *path, FATFSDriveResource *resource);

int FATFS_GetDiskSpace(const char *name, u32 *totalBlocks, u32 *freeBlocks);

BOOL FATFS_GetFileInfo(const char *path, FATFSFileInfo *info);

BOOL FATFS_SetFileInfo(const char *path, const FATFSFileInfo *info);

BOOL FATFS_CreateFile(const char *path, BOOL trunc, const char *permit);

BOOL FATFS_DeleteFile(const char *path);

BOOL FATFS_RenameFile(const char *path, const char *newpath);

BOOL FATFS_CreateDirectory(const char *path, const char *permit);

BOOL FATFS_DeleteDirectory(const char *path);

BOOL FATFS_RenameDirectory(const char *path, const char *newpath);

FATFSFileHandle FATFS_OpenFile(const char *path, const char *mode);

BOOL FATFS_CloseFile(FATFSFileHandle file);

int FATFS_ReadFile(FATFSFileHandle file, void *buffer, int length);

int FATFS_WriteFile(FATFSFileHandle file, const void *buffer, int length);

BOOL FATFS_SetSeekCache(FATFSFileHandle file, void *buf, u32 buf_size);

int FATFS_SeekFile(FATFSFileHandle file, int offset, FATFSSeekMode origin);

BOOL FATFS_FlushFile(FATFSFileHandle file);

int FATFS_GetFileLength(FATFSFileHandle file);

BOOL FATFS_SetFileLength(FATFSFileHandle file, int length);

FATFSDirectoryHandle FATFS_OpenDirectory(const char *path, const char *mode);

BOOL FATFS_CloseDirectory(FATFSDirectoryHandle dir);

BOOL FATFS_ReadDirectory(FATFSDirectoryHandle dir, FATFSFileInfo *info);

BOOL FATFS_FlushAll(void);

BOOL FATFS_UnmountAll(void);

BOOL FATFS_MountSpecial(u64 param, const char *arcname, int *slot);

BOOL FATFS_FormatSpecial(const char *path);

BOOL FATFS_SetLatencyEmulation(BOOL enable);

BOOL FATFS_SearchWildcard(const char *directory, const char *prefix,
                          const char *suffix, void *buffer, u32 length);

BOOL FATFS_GetDriveResourceW(const u16 *path, FATFSDriveResource *resource);

BOOL FATFS_GetFileInfoW(const u16 *path, FATFSFileInfoW *info);

BOOL FATFS_SetFileInfoW(const u16 *path, const FATFSFileInfoW *info);

BOOL FATFS_CreateFileW(const u16 *path, BOOL trunc, const char *permit);

BOOL FATFS_DeleteFileW(const u16 *path);

BOOL FATFS_RenameFileW(const u16 *path, const u16 *newpath);

BOOL FATFS_CreateDirectoryW(const u16 *path, const char *permit);

BOOL FATFS_DeleteDirectoryW(const u16 *path);

BOOL FATFS_RenameDirectoryW(const u16 *path, const u16 *newpath);

FATFSFileHandle FATFS_OpenFileW(const u16 *path, const char *mode);

FATFSDirectoryHandle FATFS_OpenDirectoryW(const u16 *path, const char *mode);

BOOL FATFS_ReadDirectoryW(FATFSDirectoryHandle dir, FATFSFileInfoW *info);

extern char
    *FATFSiArcnameList /* [MATH_ROUNDUP(OS_MOUNT_ARCHIVE_NAME_LEN *
                          OS_MOUNT_INFO_MAX + 1, 32)] ATTRIBUTE_ALIGN(32) */
    ;

extern u8
    *FATFSiCommandBuffer /* [FATFS_COMMAND_BUFFER_MAX] ATTRIBUTE_ALIGN(32)*/;

void FATFSi_GetUnicodeConversionTable(const u8 **u2s, const u16 **s2u);

void FATFSi_SetNdmaParameters(u32 ndmaNo, u32 blockWord, u32 intervalTimer,
                              u32 prescaler);

void FATFSi_SetRequestBuffer(FATFSRequestBuffer *buffer,
                             void (*callback)(FATFSRequestBuffer *),
                             void *userdata);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

#ifndef NITRO_FATFS_TYPES_H_
#define NITRO_FATFS_TYPES_H_

#include <twl/types.h>
#include <twl/misc.h>
#include <twl/os/common/systemWork.h>
#include <nitro/os/common/thread.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FATFS_FILE_HANDLE_MAX 8
#define FATFS_DIRECTORY_HANDLE_MAX 8

#define FATFS_DMA_NOT_USE 0xFF
#define FATFS_DMA_4 0
#define FATFS_DMA_5 1
#define FATFS_DMA_6 2
#define FATFS_DMA_7 3

#define FATFS_COMMAND_BUFFER_SIZE 2048

#define FATFS_COMMAND_BUFFER_MAX (FATFS_COMMAND_BUFFER_SIZE * 2)

#define FATFS_INVALID_HANDLE 0

typedef u32 FATFSResult;
#define FATFS_RESULT_SUCCESS 0
#define FATFS_RESULT_ERROR 1
#define FATFS_RESULT_BUSY 2
#define FATFS_RESULT_FAILURE 3
#define FATFS_RESULT_UNSUPPORTED 4
#define FATFS_RESULT_INVALIDPARAM 5
#define FATFS_RESULT_NOMORERESOURCE 6
#define FATFS_RESULT_ALREADYDONE 7
#define FATFS_RESULT_PERMISSIONDENIDED 8
#define FATFS_RESULT_MEDIAFATAL 9
#define FATFS_RESULT_NOENTRY 10
#define FATFS_RESULT_MEDIANOTHING 11
#define FATFS_RESULT_MEDIAUNKNOWN 12
#define FATFS_RESULT_BADFORMAT 13
#define FATFS_RESULT_CANCELED 14
#define FATFS_RESULT_CANCELLED FATFS_RESULT_CANCELED

typedef u32 FATFSCommandID;
#define FATFS_COMMAND_MOUNT_DRIVE 0
#define FATFS_COMMAND_UNMOUNT_DRIVE 1
#define FATFS_COMMAND_FORMAT_DRIVE 2
#define FATFS_COMMAND_CHECK_DISK 3
#define FATFS_COMMAND_GET_DRIVE_RESOURCE 4
#define FATFS_COMMAND_SET_DEFAULT_DRIVE 5
#define FATFS_COMMAND_GET_FILEINFO 6
#define FATFS_COMMAND_SET_FILEINFO 7
#define FATFS_COMMAND_CREATE_FILE 8
#define FATFS_COMMAND_DELETE_FILE 9
#define FATFS_COMMAND_RENAME_FILE 10
#define FATFS_COMMAND_CREATE_DIRECTORY 11
#define FATFS_COMMAND_DELETE_DIRECTORY 12
#define FATFS_COMMAND_RENAME_DIRECTORY 13
#define FATFS_COMMAND_OPEN_FILE 14
#define FATFS_COMMAND_CLOSE_FILE 15
#define FATFS_COMMAND_READ_FILE 16
#define FATFS_COMMAND_WRITE_FILE 17
#define FATFS_COMMAND_SEEK_FILE 18
#define FATFS_COMMAND_FLUSH_FILE 19
#define FATFS_COMMAND_GET_FILE_LENGTH 20
#define FATFS_COMMAND_SET_FILE_LENGTH 21
#define FATFS_COMMAND_OPEN_DIRECTORY 22
#define FATFS_COMMAND_CLOSE_DIRECTORY 23
#define FATFS_COMMAND_READ_DIRECTORY 24
#define FATFS_COMMAND_FLUSH_ALL 25
#define FATFS_COMMAND_UNMOUNT_ALL 26
#define FATFS_COMMAND_MOUNT_SPECIAL 27
#define FATFS_COMMAND_SET_NDMA_PARAMETERS 28
#define FATFS_COMMAND_SET_SEEK_CACHE 29
#define FATFS_COMMAND_FORMAT_SPECIAL 30
#define FATFS_COMMAND_SET_LATENCY_EMULATION 31
#define FATFS_COMMAND_SEARCH_WILDCARD 32
#define FATFS_COMMAND_MAX 33

typedef u32 FATFSMediaType;
#define FATFS_MEDIA_TYPE_NAND 0
#define FATFS_MEDIA_TYPE_SD 1
#define FATFS_MEDIA_TYPE_SUBNAND 2

typedef u32 FATFSSeekMode;
#define FATFS_SEEK_SET 0
#define FATFS_SEEK_CUR 1
#define FATFS_SEEK_END 2

#define FATFS_PATH_MAX 260
#define FATFS_DRIVE_NAME_MAX 15 // Consider future extensions.
#define FATFS_PERMIT_STRING_MAX 15
#define FATFS_ACCESSMODE_STRING_MAX 15
#define FATFS_ARCHIVE_FULLPATH_MAX                                             \
  (FATFS_DRIVE_NAME_MAX + 1 + 1 + FATFS_PATH_MAX)
#define FATFS_SHORTNAME_FILENAME_MAX 8
#define FATFS_SHORTNAME_PREFIX_MAX 5
#define FATFS_SHORTNAME_SUFFIX_MAX 3

#define FATFS_ATTRIBUTE_IS_PROTECTED 0x00000200UL
#define FATFS_ATTRIBUTE_DOS_MASK 0x000000FFUL
#define FATFS_ATTRIBUTE_DOS_READONLY 0x00000001UL
#define FATFS_ATTRIBUTE_DOS_HIDDEN 0x00000002UL
#define FATFS_ATTRIBUTE_DOS_SYSTEM 0x00000004UL
#define FATFS_ATTRIBUTE_DOS_VOLUME 0x00000008UL
#define FATFS_ATTRIBUTE_DOS_DIRECTORY 0x00000010UL
#define FATFS_ATTRIBUTE_DOS_ARCHIVE 0x00000020UL

#define FATFS_PROPERTY_CTRL_SET_MTIME 0x01000000UL
#define FATFS_PROPERTY_CTRL_MASK 0x01000000UL

#define FATFS_DOSTIME_TO_SEC(dostime) (u32)((((dostime) >> 0) & 0x1F) * 2)
#define FATFS_DOSTIME_TO_MIN(dostime) (u32)((((dostime) >> 5) & 0x3F) + 0)
#define FATFS_DOSTIME_TO_HOUR(dostime) (u32)((((dostime) >> 11) & 0x1F) + 0)
#define FATFS_DOSTIME_TO_MDAY(dostime) (u32)((((dostime) >> 16) & 0x1F) + 0)
#define FATFS_DOSTIME_TO_MON(dostime) (u32)((((dostime) >> 21) & 0x0F) + 0)
#define FATFS_DOSTIME_TO_YEAR(dostime) (u32)((((dostime) >> 25) & 0x7F) + 1980)

#define FATFS_DATETIME_TO_DOSTIME(year, mon, mday, hour, min, sec)             \
  (u32)((((sec / 2) & 0x1F) << 0) | (((min - 0) & 0x3F) << 5) |                \
        (((hour - 0) & 0x1F) << 11) | (((mday - 0) & 0x1F) << 16) |            \
        (((mon - 0) & 0x0F) << 21) | (((year - 1980) & 0x7F) << 25))

#define FATFS_EVENT_SD_INSERTED 0x00010001
#define FATFS_EVENT_SD_REMOVED 0x00010002

typedef u32 FATFSFileHandle;
typedef u32 FATFSDirectoryHandle;

typedef struct FATFSFileInfo {
  char shortname[16];
  char longname[FATFS_PATH_MAX + 1];
  char padding[3];
  u32 length;
  u32 dos_atime;
  u32 dos_mtime;
  u32 dos_ctime;
  u32 attributes;
} FATFSFileInfo;

typedef struct FATFSDiskInfo {
  u32 n_user_files;
  u32 n_hidden_files;
  u32 n_user_directories;
  u32 n_free_clusters;
  u32 n_bad_clusters;
  u32 n_file_clusters;
  u32 n_hidden_clusters;
  u32 n_dir_clusters;
  u32 n_crossed_points;
  u32 n_lost_chains;
  u32 n_lost_clusters;
  u32 n_bad_lfns;
} FATFSDiskInfo;

typedef struct FATFSDriveResource {
  u64 totalSize;
  u64 availableSize;
  u32 maxFileHandles;
  u32 currentFileHandles;
  u32 maxDirectoryHandles;
  u32 currentDirectoryHandles;

  u32 bytesPerSector;
  u32 sectorsPerCluster;
  u32 totalClusters;
  u32 availableClusters;
  u32 fatBits;
} FATFSDriveResource;

typedef struct FATFSCommandHeader {
  FATFSCommandID command;
  FATFSResult result;
  u32 flags;
  struct FATFSCommandHeader *next;
} FATFSCommandHeader;

typedef struct FATFSCommandMountDrive {
  FATFSCommandHeader header;
  FATFSMediaType media;
  int partition;
  char name[FATFS_DRIVE_NAME_MAX + 1];
} FATFSCommandMountDrive;

typedef struct FATFSCommandUnmountDrive {
  FATFSCommandHeader header;
  char name[FATFS_DRIVE_NAME_MAX + 1];
} FATFSCommandUnmountDrive;

typedef struct FATFSCommandFormatDrive {
  FATFSCommandHeader header;
  char name[FATFS_DRIVE_NAME_MAX + 1];
  u32 formatMedia;
} FATFSCommandFormatDrive;

typedef struct FATFSCommandCheckDisk {
  FATFSCommandHeader header;
  char name[FATFS_DRIVE_NAME_MAX + 1];
  u32 verbose;
  u32 fix_problems;
  u32 write_chains;
  FATFSDiskInfo info;
} FATFSCommandCheckDisk;

typedef struct FATFSCommandGetDriveResource {
  FATFSCommandHeader header;
  char name[FATFS_DRIVE_NAME_MAX + 1];
  FATFSDriveResource resource[1];
} FATFSCommandGetDriveResource;

typedef struct FATFSCommandSetDefaultDrive {
  FATFSCommandHeader header;
  char name[FATFS_DRIVE_NAME_MAX + 1];
} FATFSCommandSetDefaultDrive;

typedef struct FATFSCommandGetFileInfo {
  FATFSCommandHeader header;
  char path[FATFS_ARCHIVE_FULLPATH_MAX + 1];
  u8 padding[2];
  FATFSFileInfo info;
} FATFSCommandGetFileInfo;

typedef struct FATFSCommandSetFileInfo {
  FATFSCommandHeader header;
  char path[FATFS_ARCHIVE_FULLPATH_MAX + 1];
  u8 padding[2];
  FATFSFileInfo info;
} FATFSCommandSetFileInfo;

typedef struct FATFSCommandCreateFile {
  FATFSCommandHeader header;
  BOOL trunc;
  char permit[FATFS_PERMIT_STRING_MAX + 1];
  char path[FATFS_ARCHIVE_FULLPATH_MAX + 1];
  u8 padding[2];
} FATFSCommandCreateFile;

typedef struct FATFSCommandDeleteFile {
  FATFSCommandHeader header;
  char path[FATFS_ARCHIVE_FULLPATH_MAX + 1];
  u8 padding[2];
} FATFSCommandDeleteFile;

typedef struct FATFSCommandRenameFile {
  FATFSCommandHeader header;
  char path[FATFS_ARCHIVE_FULLPATH_MAX + 1];
  char newpath[FATFS_ARCHIVE_FULLPATH_MAX + 1];
  u8 padding[4];
} FATFSCommandRenameFile;

typedef struct FATFSCommandCreateDirectory {
  FATFSCommandHeader header;
  char permit[FATFS_PERMIT_STRING_MAX + 1];
  char path[FATFS_ARCHIVE_FULLPATH_MAX + 1];
  u8 padding[2];
} FATFSCommandCreateDirectory;

typedef struct FATFSCommandDeleteDirectory {
  FATFSCommandHeader header;
  char path[FATFS_ARCHIVE_FULLPATH_MAX + 1];
  u8 padding[2];
} FATFSCommandDeleteDirectory;

typedef struct FATFSCommandRenameDirectory {
  FATFSCommandHeader header;
  char path[FATFS_ARCHIVE_FULLPATH_MAX + 1];
  char newpath[FATFS_ARCHIVE_FULLPATH_MAX + 1];
  u8 padding[4];
} FATFSCommandRenameDirectory;

typedef struct FATFSCommandOpenFile {
  FATFSCommandHeader header;
  FATFSFileHandle handle;
  char mode[FATFS_ACCESSMODE_STRING_MAX + 1];
  char path[FATFS_ARCHIVE_FULLPATH_MAX + 1];
  u8 padding[2];
} FATFSCommandOpenFile;

typedef struct FATFSCommandCloseFile {
  FATFSCommandHeader header;
  FATFSFileHandle handle;
} FATFSCommandCloseFile;

typedef struct FATFSCommandReadFile {
  FATFSCommandHeader header;
  FATFSFileHandle handle;
  void *buffer;
  int length;
} FATFSCommandReadFile;

typedef struct FATFSCommandWriteFile {
  FATFSCommandHeader header;
  FATFSFileHandle handle;
  const void *buffer;
  int length;
} FATFSCommandWriteFile;

typedef struct FATFSCommandSetSeekCache {
  FATFSCommandHeader header;
  FATFSFileHandle handle;
  void *buf;
  u32 buf_size;
} FATFSCommandSetSeekCache;

typedef struct FATFSCommandSeekFile {
  FATFSCommandHeader header;
  FATFSFileHandle handle;
  int offset;
  FATFSSeekMode origin;
} FATFSCommandSeekFile;

typedef struct FATFSCommandFlushFile {
  FATFSCommandHeader header;
  FATFSFileHandle handle;
} FATFSCommandFlushFile;

typedef struct FATFSCommandGetFileLength {
  FATFSCommandHeader header;
  FATFSFileHandle handle;
  u32 length;
} FATFSCommandGetFileLength;

typedef struct FATFSCommandSetFileLength {
  FATFSCommandHeader header;
  FATFSFileHandle handle;
  u32 length;
} FATFSCommandSetFileLength;

typedef struct FATFSCommandOpenDirectory {
  FATFSCommandHeader header;
  FATFSDirectoryHandle handle;
  char mode[FATFS_ACCESSMODE_STRING_MAX + 1];
  char path[FATFS_ARCHIVE_FULLPATH_MAX + 1];
  u8 padding[2];
} FATFSCommandOpenDirectory;

typedef struct FATFSCommandCloseDirectory {
  FATFSCommandHeader header;
  FATFSDirectoryHandle handle;
} FATFSCommandCloseDirectory;

typedef struct FATFSCommandReadDirectory {
  FATFSCommandHeader header;
  FATFSDirectoryHandle handle;
  FATFSFileInfo info;
} FATFSCommandReadDirectory;

typedef struct FATFSCommandFlushAll {
  FATFSCommandHeader header;
} FATFSCommandFlushAll;

typedef struct FATFSCommandUnmountAll {
  FATFSCommandHeader header;
} FATFSCommandUnmountAll;

typedef struct FATFSCommandMountSpecial {
  FATFSCommandHeader header;
  u64 param;
  int slot;
  char arcname[OS_MOUNT_ARCHIVE_NAME_LEN];
} FATFSCommandMountSpecial;

typedef struct FATFSCommandSetNdmaParameters {
  FATFSCommandHeader header;
  u32 ndmaNo;
  u32 blockWord;
  u32 intervalTimer;
  u32 prescaler;
} FATFSCommandSetNdmaParameters;

typedef struct FATFSCommandFormatSpecial {
  FATFSCommandHeader header;
  char path[FATFS_ARCHIVE_FULLPATH_MAX + 1];
  u8 padding[6];
  u64 data;
} FATFSCommandFormatSpecial;

typedef struct FATFSCommandSetLatencyEmulation {
  FATFSCommandHeader header;
  u32 enable;
} FATFSCommandSetLatencyEmulation;

typedef struct FATFSCommandSearchWildcard {
  FATFSCommandHeader header;
  char directory[FATFS_SHORTNAME_FILENAME_MAX + 1];
  char prefix[FATFS_SHORTNAME_PREFIX_MAX + 1];
  char suffix[FATFS_SHORTNAME_SUFFIX_MAX + 1];
  u8 buffer[10000 / 8 + 1];
  u8 padding[2];
} FATFSCommandSearchWildcard;

SDK_COMPILER_ASSERT(sizeof(FATFSCommandHeader) <= FATFS_COMMAND_BUFFER_SIZE);
SDK_COMPILER_ASSERT(sizeof(FATFSCommandMountDrive) <=
                    FATFS_COMMAND_BUFFER_SIZE);
SDK_COMPILER_ASSERT(sizeof(FATFSCommandUnmountDrive) <=
                    FATFS_COMMAND_BUFFER_SIZE);
SDK_COMPILER_ASSERT(sizeof(FATFSCommandSetDefaultDrive) <=
                    FATFS_COMMAND_BUFFER_SIZE);
SDK_COMPILER_ASSERT(sizeof(FATFSCommandFormatDrive) <=
                    FATFS_COMMAND_BUFFER_SIZE);
SDK_COMPILER_ASSERT(sizeof(FATFSCommandCheckDisk) <= FATFS_COMMAND_BUFFER_SIZE);
SDK_COMPILER_ASSERT(sizeof(FATFSCommandGetDriveResource) <=
                    FATFS_COMMAND_BUFFER_SIZE);
SDK_COMPILER_ASSERT(sizeof(FATFSCommandGetFileInfo) <=
                    FATFS_COMMAND_BUFFER_SIZE);
SDK_COMPILER_ASSERT(sizeof(FATFSCommandCreateFile) <=
                    FATFS_COMMAND_BUFFER_SIZE);
SDK_COMPILER_ASSERT(sizeof(FATFSCommandDeleteFile) <=
                    FATFS_COMMAND_BUFFER_SIZE);
SDK_COMPILER_ASSERT(sizeof(FATFSCommandRenameFile) <=
                    FATFS_COMMAND_BUFFER_SIZE);

SDK_COMPILER_ASSERT(sizeof(FATFSCommandOpenFile) <= FATFS_COMMAND_BUFFER_SIZE);
SDK_COMPILER_ASSERT(sizeof(FATFSCommandCloseFile) <= FATFS_COMMAND_BUFFER_SIZE);
SDK_COMPILER_ASSERT(sizeof(FATFSCommandReadFile) <= FATFS_COMMAND_BUFFER_SIZE);
SDK_COMPILER_ASSERT(sizeof(FATFSCommandWriteFile) <= FATFS_COMMAND_BUFFER_SIZE);
SDK_COMPILER_ASSERT(sizeof(FATFSCommandSeekFile) <= FATFS_COMMAND_BUFFER_SIZE);
SDK_COMPILER_ASSERT(sizeof(FATFSCommandFlushFile) <= FATFS_COMMAND_BUFFER_SIZE);
SDK_COMPILER_ASSERT(sizeof(FATFSCommandGetFileLength) <=
                    FATFS_COMMAND_BUFFER_SIZE);
SDK_COMPILER_ASSERT(sizeof(FATFSCommandSetFileLength) <=
                    FATFS_COMMAND_BUFFER_SIZE);

SDK_COMPILER_ASSERT(sizeof(FATFSCommandOpenDirectory) <=
                    FATFS_COMMAND_BUFFER_SIZE);
SDK_COMPILER_ASSERT(sizeof(FATFSCommandCloseDirectory) <=
                    FATFS_COMMAND_BUFFER_SIZE);
SDK_COMPILER_ASSERT(sizeof(FATFSCommandReadDirectory) <=
                    FATFS_COMMAND_BUFFER_SIZE);

SDK_COMPILER_ASSERT(sizeof(FATFSCommandFlushAll) <= FATFS_COMMAND_BUFFER_SIZE);
SDK_COMPILER_ASSERT(sizeof(FATFSCommandUnmountAll) <=
                    FATFS_COMMAND_BUFFER_SIZE);
SDK_COMPILER_ASSERT(sizeof(FATFSCommandMountSpecial) <=
                    FATFS_COMMAND_BUFFER_SIZE);
SDK_COMPILER_ASSERT(sizeof(FATFSCommandSetNdmaParameters) <=
                    FATFS_COMMAND_BUFFER_SIZE);
SDK_COMPILER_ASSERT(sizeof(FATFSCommandFormatSpecial) <=
                    FATFS_COMMAND_BUFFER_SIZE);
SDK_COMPILER_ASSERT(sizeof(FATFSCommandSetLatencyEmulation) <=
                    FATFS_COMMAND_BUFFER_SIZE);
SDK_COMPILER_ASSERT(sizeof(FATFSCommandSearchWildcard) <=
                    FATFS_COMMAND_BUFFER_SIZE);

typedef struct FATFSRequestBuffer {
  union {
    FATFSCommandHeader header;
    u8 buffer[FATFS_COMMAND_BUFFER_SIZE];
  };
  OSThread *thread;
  void (*callback)(struct FATFSRequestBuffer *);
  void *userdata;
  struct FATFSRequestBuffer *next;
  u8 padding[16];
} FATFSRequestBuffer;

typedef struct FATFSResultBuffer {
  struct FATFSResultBuffer *next;
  OSThread *thread;
  FATFSCommandHeader *command;
  u32 result;
  u8 reserved[16];
} FATFSResultBuffer;

typedef struct FATFSFileInfoW {
  char shortname[16];
  u16 longname[FATFS_PATH_MAX + 1];
  u16 padding;
  u32 length;
  u32 dos_atime;
  u32 dos_mtime;
  u32 dos_ctime;
  u32 attributes;
} FATFSFileInfoW;

SDK_COMPILER_ASSERT(sizeof(FATFSFileInfoW) <= FATFS_COMMAND_BUFFER_SIZE);

typedef struct FATFSCommandGetFileInfoW {
  FATFSCommandHeader header;
  u16 path[FATFS_ARCHIVE_FULLPATH_MAX + 1];
  u16 padding[2];
  FATFSFileInfoW info;
} FATFSCommandGetFileInfoW;

typedef struct FATFSCommandSetFileInfoW {
  FATFSCommandHeader header;
  u16 path[FATFS_ARCHIVE_FULLPATH_MAX + 1];
  u16 padding[2];
  FATFSFileInfoW info;
} FATFSCommandSetFileInfoW;

typedef struct FATFSCommandCreateFileW {
  FATFSCommandHeader header;
  BOOL trunc;
  char permit[FATFS_PERMIT_STRING_MAX + 1];
  u16 path[FATFS_ARCHIVE_FULLPATH_MAX + 1];
  u16 padding[2];
} FATFSCommandCreateFileW;

typedef struct FATFSCommandDeleteFileW {
  FATFSCommandHeader header;
  u16 path[FATFS_ARCHIVE_FULLPATH_MAX + 1];
  u16 padding[2];
} FATFSCommandDeleteFileW;

typedef struct FATFSCommandRenameFileW {
  FATFSCommandHeader header;
  u16 path[FATFS_ARCHIVE_FULLPATH_MAX + 1];
  u16 newpath[FATFS_ARCHIVE_FULLPATH_MAX + 1];
} FATFSCommandRenameFileW;

typedef struct FATFSCommandCreateDirectoryW {
  FATFSCommandHeader header;
  char permit[FATFS_PERMIT_STRING_MAX + 1];
  u16 path[FATFS_ARCHIVE_FULLPATH_MAX + 1];
  u16 padding[2];
} FATFSCommandCreateDirectoryW;

typedef struct FATFSCommandDeleteDirectoryW {
  FATFSCommandHeader header;
  u16 path[FATFS_ARCHIVE_FULLPATH_MAX + 1];
  u16 padding[2];
} FATFSCommandDeleteDirectoryW;

typedef struct FATFSCommandRenameDirectoryW {
  FATFSCommandHeader header;
  u16 path[FATFS_ARCHIVE_FULLPATH_MAX + 1];
  u16 newpath[FATFS_ARCHIVE_FULLPATH_MAX + 1];
} FATFSCommandRenameDirectoryW;

typedef struct FATFSCommandOpenFileW {
  FATFSCommandHeader header;
  FATFSFileHandle handle;
  char mode[FATFS_ACCESSMODE_STRING_MAX + 1];
  u16 path[FATFS_ARCHIVE_FULLPATH_MAX + 1];
  u16 padding[2];
} FATFSCommandOpenFileW;

typedef struct FATFSCommandOpenDirectoryW {
  FATFSCommandHeader header;
  FATFSDirectoryHandle handle;
  char mode[FATFS_ACCESSMODE_STRING_MAX + 1];
  u16 path[FATFS_ARCHIVE_FULLPATH_MAX + 1];
  u16 padding[2];
} FATFSCommandOpenDirectoryW;

typedef struct FATFSCommandReadDirectoryW {
  FATFSCommandHeader header;
  FATFSDirectoryHandle handle;
  FATFSFileInfoW info;
} FATFSCommandReadDirectoryW;

SDK_COMPILER_ASSERT(sizeof(FATFSCommandGetFileInfoW) <=
                    FATFS_COMMAND_BUFFER_SIZE);
SDK_COMPILER_ASSERT(sizeof(FATFSCommandSetFileInfoW) <=
                    FATFS_COMMAND_BUFFER_SIZE);
SDK_COMPILER_ASSERT(sizeof(FATFSCommandCreateFileW) <=
                    FATFS_COMMAND_BUFFER_SIZE);
SDK_COMPILER_ASSERT(sizeof(FATFSCommandDeleteFileW) <=
                    FATFS_COMMAND_BUFFER_SIZE);
SDK_COMPILER_ASSERT(sizeof(FATFSCommandRenameFileW) <=
                    FATFS_COMMAND_BUFFER_SIZE);
SDK_COMPILER_ASSERT(sizeof(FATFSCommandCreateDirectoryW) <=
                    FATFS_COMMAND_BUFFER_SIZE);
SDK_COMPILER_ASSERT(sizeof(FATFSCommandDeleteDirectoryW) <=
                    FATFS_COMMAND_BUFFER_SIZE);
SDK_COMPILER_ASSERT(sizeof(FATFSCommandRenameDirectoryW) <=
                    FATFS_COMMAND_BUFFER_SIZE);
SDK_COMPILER_ASSERT(sizeof(FATFSCommandOpenFileW) <= FATFS_COMMAND_BUFFER_SIZE);
SDK_COMPILER_ASSERT(sizeof(FATFSCommandOpenDirectoryW) <=
                    FATFS_COMMAND_BUFFER_SIZE);
SDK_COMPILER_ASSERT(sizeof(FATFSCommandReadDirectoryW) <=
                    FATFS_COMMAND_BUFFER_SIZE);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

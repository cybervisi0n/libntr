#if !defined(NITRO_FS_COMMAND_H_)
#define NITRO_FS_COMMAND_H_

#include <nitro/misc.h>
#include <nitro/types.h>
#include <nitro/fs/file.h>
#if SDK_VERSION_MAJOR == 5
#include <nitro/fs/archive.h>
#include <nitro/fs/romfat.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SDK_VERSION_MAJOR == 4
extern FSResult (*const (fsi_default_command[]))(FSFile *);
extern FSDirPos current_dir_pos;

int FSi_StrNICmp(const char * str1, const char * str2, u32 len);
BOOL FSi_SendCommand(FSFile * p_file, FSCommandType command);
FSResult FSi_TranslateCommand(FSFile * p_file, FSCommandType command);
void FSi_ReleaseCommand(FSFile * p_file, FSResult ret);
FSFile * FSi_NextCommand(FSArchive * p_arc);
void FSi_ExecuteAsyncCommand(FSFile * p_file);
BOOL FSi_ExecuteSyncCommand(FSFile * p_file);
#elif (SDK_VERSION_MAJOR == 5)
typedef struct FSArgumentForReadFile {
  void *buffer;
  u32 length;
} FSArgumentForReadFile;

typedef struct FSArgumentForWriteFile {
  const void *buffer;
  u32 length;
} FSArgumentForWriteFile;

typedef struct FSArgumentForSeekDirectory {
  u32 id;
  u32 position;
} FSArgumentForSeekDirectory;

typedef struct FSArgumentForReadDirectory {
  FSDirectoryEntryInfo *info;
} FSArgumentForReadDirectory;

typedef struct FSArgumentForFindPath {
  u32 baseid;
  const char *relpath;
  u32 target_id;
  BOOL target_is_directory;
} FSArgumentForFindPath;

typedef struct FSArgumentForGetPath {
  BOOL is_directory;
  char *buffer;
  u32 length;
} FSArgumentForGetPath;

typedef struct FSArgumentForOpenFileFast {
  u32 id;
  u32 mode;
} FSArgumentForOpenFileFast;

typedef struct FSArgumentForOpenFileDirect {
  u32 id; // in : requested-id
  u32 top;
  u32 bottom;
  u32 mode;
} FSArgumentForOpenFileDirect;

typedef void FSArgumentForCloseFile;
typedef void FSArgumentForActivate;
typedef void FSArgumentForIdle;
typedef void FSArgumentForSuspend;
typedef void FSArgumentForResume;

typedef struct FSArgumentForOpenFile {
  u32 baseid;
  const char *relpath;
  u32 mode;
} FSArgumentForOpenFile;

typedef struct FSArgumentForSetSeekCache {
  void *buf;
  u32 buf_size;
} FSArgumentForSetSeekCache;

typedef struct FSArgumentForSeekFile {
  int offset;
  FSSeekFileMode from;
} FSArgumentForSeekFile;

typedef struct FSArgumentForGetFileLength {
  u32 length;
} FSArgumentForGetFileLength;

typedef struct FSArgumentForGetFilePosition {
  u32 position;
} FSArgumentForGetFilePosition;

typedef struct FSArgumentForGetArchiveCaps {
  u32 caps;
} FSArgumentForGetArchiveCaps;

typedef void FSArgumentForMount;
typedef void FSArgumentForUnmount;

typedef struct FSArgumentForCreateFile {
  u32 baseid;
  const char *relpath;
  u32 permit;
} FSArgumentForCreateFile;

typedef struct FSArgumentForDeleteFile {
  u32 baseid;
  const char *relpath;
} FSArgumentForDeleteFile;

typedef struct FSArgumentForRenameFile {
  u32 baseid_src;
  const char *relpath_src;
  u32 baseid_dst;
  const char *relpath_dst;
} FSArgumentForRenameFile;

typedef struct FSArgumentForGetPathInfo {
  u32 baseid;
  const char *relpath;
  FSPathInfo *info;
} FSArgumentForGetPathInfo;

typedef struct FSArgumentForSetPathInfo {
  u32 baseid;
  const char *relpath;
  FSPathInfo *info;
} FSArgumentForSetPathInfo;

typedef FSArgumentForCreateFile FSArgumentForCreateDirectory;
typedef FSArgumentForDeleteFile FSArgumentForDeleteDirectory;
typedef FSArgumentForRenameFile FSArgumentForRenameDirectory;

typedef struct FSArgumentForGetArchiveResource {
  FSArchiveResource *resource;
} FSArgumentForGetArchiveResource;

typedef void FSArgumentForFlushFile;

typedef struct FSArgumentForSetFileLength {
  u32 length;
} FSArgumentForSetFileLength;

typedef FSArgumentForOpenFile FSArgumentForOpenDirectory;
typedef FSArgumentForCloseFile FSArgumentForCloseDirectory;

BOOL FSi_SendCommand(FSFile *p_file, FSCommandType command, BOOL blocking);

SDK_INLINE FSCommandType FSi_GetCurrentCommand(const FSFile *file) {
  return (FSCommandType)((file->stat >> FS_FILE_STATUS_CMD_SHIFT) &
                         FS_FILE_STATUS_CMD_MASK);
}

FSResult FSi_WaitForArchiveCompletion(FSFile *file, FSResult result);

FSArchive *FSi_GetArchiveChain(void);

BOOL FSi_IsUnreadableRomOffset(FSArchive *arc, u32 offset);
#endif

#ifdef __cplusplus
}
#endif

#endif

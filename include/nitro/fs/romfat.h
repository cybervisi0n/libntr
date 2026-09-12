#ifndef NITRO_FS_RFAT_H_
#define NITRO_FS_RFAT_H_

#include <nitro/fs/types.h>

#if SDK_VERSION_MAJOR < 5
#error This file is for SDK version 5 only!
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define FS_COMMAND_ASYNC_BEGIN (FSCommandType)(FS_COMMAND_READFILE)
#define FS_COMMAND_ASYNC_END (FSCommandType)(FS_COMMAND_WRITEFILE + 1)
#define FS_COMMAND_SYNC_BEGIN (FSCommandType)(FS_COMMAND_SEEKDIR)
#define FS_COMMAND_SYNC_END (FSCommandType)(FS_COMMAND_CLOSEFILE + 1)
#define FS_COMMAND_STATUS_BEGIN (FSCommandType)(FS_COMMAND_ACTIVATE)
#define FS_COMMAND_STATUS_END (FSCommandType)(FS_COMMAND_RESUME + 1)

#define FS_ARCHIVE_FLAG_IS_SYNC 0x00000100
#define FS_ARCHIVE_FLAG_TABLE_LOAD 0x00000004
#define FS_ARCHIVE_FLAG_OBSOLETE 0x00000104

#define FS_ARCHIVE_PROC_READFILE (1 << FS_COMMAND_READFILE)
#define FS_ARCHIVE_PROC_WRITEFILE (1 << FS_COMMAND_WRITEFILE)

#define FS_ARCHIVE_PROC_ASYNC                                                  \
  (FS_ARCHIVE_PROC_READFILE | FS_ARCHIVE_PROC_WRITEFILE)

#define FS_ARCHIVE_PROC_SEEKDIR (1 << FS_COMMAND_SEEKDIR)
#define FS_ARCHIVE_PROC_READDIR (1 << FS_COMMAND_READDIR)
#define FS_ARCHIVE_PROC_FINDPATH (1 << FS_COMMAND_FINDPATH)
#define FS_ARCHIVE_PROC_GETPATH (1 << FS_COMMAND_GETPATH)
#define FS_ARCHIVE_PROC_OPENFILEFAST (1 << FS_COMMAND_OPENFILEFAST)
#define FS_ARCHIVE_PROC_OPENFILEDIRECT (1 << FS_COMMAND_OPENFILEDIRECT)
#define FS_ARCHIVE_PROC_CLOSEFILE (1 << FS_COMMAND_CLOSEFILE)

#define FS_ARCHIVE_PROC_SYNC                                                   \
  (FS_ARCHIVE_PROC_SEEKDIR | FS_ARCHIVE_PROC_READDIR |                         \
   FS_ARCHIVE_PROC_FINDPATH | FS_ARCHIVE_PROC_GETPATH |                        \
   FS_ARCHIVE_PROC_OPENFILEFAST | FS_ARCHIVE_PROC_OPENFILEDIRECT |             \
   FS_ARCHIVE_PROC_CLOSEFILE)

#define FS_ARCHIVE_PROC_ACTIVATE (1 << FS_COMMAND_ACTIVATE)
#define FS_ARCHIVE_PROC_IDLE (1 << FS_COMMAND_IDLE)
#define FS_ARCHIVE_PROC_SUSPENDING (1 << FS_COMMAND_SUSPEND)
#define FS_ARCHIVE_PROC_RESUME (1 << FS_COMMAND_RESUME)

#define FS_ARCHIVE_PROC_STATUS                                                 \
  (FS_ARCHIVE_PROC_ACTIVATE | FS_ARCHIVE_PROC_IDLE |                           \
   FS_ARCHIVE_PROC_SUSPENDING | FS_ARCHIVE_PROC_RESUME)

#define FS_ARCHIVE_PROC_ALL (~0)

typedef FSResult (*FS_ARCHIVE_PROC_FUNC)(struct FSFile *, FSCommandType);
typedef FSResult (*FS_ARCHIVE_READ_FUNC)(struct FSArchive *p, void *dst,
                                         u32 pos, u32 size);
typedef FSResult (*FS_ARCHIVE_WRITE_FUNC)(struct FSArchive *p, const void *src,
                                          u32 pos, u32 size);

typedef struct FSArchiveFAT {
  u32 top;
  u32 bottom;
} FSArchiveFAT;

typedef struct FSArchiveFNT {
  u32 start;
  u16 index;
  u16 parent;
} FSArchiveFNT;

#define FS_ROMFAT_CONTEXT_DEFINITION()                                         \
  {                                                                            \
    u32 base;                                                                  \
    u32 fat;                                                                   \
    u32 fat_size;                                                              \
    u32 fnt;                                                                   \
    u32 fnt_size;                                                              \
    u32 fat_bak;                                                               \
    u32 fnt_bak;                                                               \
    void *load_mem;                                                            \
    FS_ARCHIVE_READ_FUNC read_func;                                            \
    FS_ARCHIVE_WRITE_FUNC write_func;                                          \
    u8 reserved3[4];                                                           \
    FS_ARCHIVE_PROC_FUNC proc;                                                 \
    u32 proc_flag;                                                             \
  }

typedef struct FSROMFATArchiveContext
FS_ROMFAT_CONTEXT_DEFINITION() FSROMFATArchiveContext;

#ifdef FS_DISABLE_OLDPROTOTYPES
#undef FS_ROMFAT_CONTEXT_DEFINITION
#define FS_ROMFAT_CONTEXT_DEFINITION()                                         \
  {                                                                            \
    u8 reserved[52];                                                           \
  }                                                                            \
  obsolete
#endif // FS_DISABLE_OLDPROTOTYPES

typedef struct {
  FSDirPos pos;
} FSSeekDirInfo;

typedef struct {
  FSDirEntry *p_entry;
  BOOL skip_string;
} FSReadDirInfo;

typedef struct {
  FSDirPos pos;
  const char *path;
  BOOL find_directory;
  union {
    FSFileID *file;
    FSDirPos *dir;
  } result;
} FSFindPathInfo;

typedef struct {
  u8 *buf;
  u32 buf_len;
  u16 total_len;
  u16 dir_id;
} FSGetPathInfo;

typedef struct {
  FSFileID id;
} FSOpenFileFastInfo;

typedef struct {
  u32 top;
  u32 bottom;
  u32 index;
} FSOpenFileDirectInfo;

typedef struct {
  u32 reserved;
} FSCloseFileInfo;

typedef struct {
  void *dst;
  u32 len_org;
  u32 len;
} FSReadFileInfo;

typedef struct {
  const void *src;
  u32 len_org;
  u32 len;
} FSWriteFileInfo;

typedef union FSROMFATCommandInfo {
  FSReadFileInfo readfile;
  FSWriteFileInfo writefile;
  FSSeekDirInfo seekdir;
  FSReadDirInfo readdir;
  FSFindPathInfo findpath;
  FSGetPathInfo getpath;
  FSOpenFileFastInfo openfilefast;
  FSOpenFileDirectInfo openfiledirect;
  FSCloseFileInfo closefile;
} FSROMFATCommandInfo;

typedef struct FSROMFATFileProperty {
  u32 own_id;
  u32 top;
  u32 bottom;
  u32 pos;
} FSROMFATFileProperty;

typedef struct FSROMFATDirProperty {
  FSDirPos pos;
  u32 parent;
} FSROMFATDirProperty;

typedef union FSROMFATProperty {
  FSROMFATFileProperty file;
  FSROMFATDirProperty dir;
} FSROMFATProperty;

u32 FS_GetArchiveBase(const struct FSArchive *arc);

u32 FS_GetArchiveFAT(const struct FSArchive *arc);

u32 FS_GetArchiveFNT(const struct FSArchive *arc);

u32 FS_GetArchiveOffset(const struct FSArchive *arc, u32 pos);

BOOL FS_IsArchiveTableLoaded(volatile const struct FSArchive *arc);

BOOL FS_LoadArchive(struct FSArchive *arc, u32 base, u32 fat, u32 fat_size,
                    u32 fnt, u32 fnt_size, FS_ARCHIVE_READ_FUNC read_func,
                    FS_ARCHIVE_WRITE_FUNC write_func);

BOOL FS_UnloadArchive(struct FSArchive *arc);

u32 FS_LoadArchiveTables(struct FSArchive *arc, void *mem, u32 size);

void *FS_UnloadArchiveTables(struct FSArchive *arc);

void FS_SetArchiveProc(struct FSArchive *arc, FS_ARCHIVE_PROC_FUNC proc,
                       u32 flags);

u32 FS_GetFileImageTop(const struct FSFile *file);

u32 FS_GetFileImageBottom(const struct FSFile *file);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NITRO_FS_RFAT_H_ */

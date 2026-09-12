#if !defined(NITRO_FS_ARCHIVE_H_)
#define NITRO_FS_ARCHIVE_H_

#if SDK_VERSION_MAJOR == 4
#include <nitro/misc.h>
#include <nitro/types.h>
#elif SDK_VERSION_MAJOR == 5
#include <nitro/fs/types.h>
#include <nitro/fs/romfat.h>
#endif
#include <nitro/os/common/thread.h>

#ifdef __cplusplus
extern "C" {
#endif

#if SDK_VERSION_MAJOR == 4
enum {
	FS_ARCHIVE_NAME_LEN_MAX = 3
};

#define FS_ARCHIVE_FLAG_REGISTER              0x00000001
#define FS_ARCHIVE_FLAG_LOADED                0x00000002
#define FS_ARCHIVE_FLAG_TABLE_LOAD            0x00000004
#define FS_ARCHIVE_FLAG_SUSPEND               0x00000008
#define FS_ARCHIVE_FLAG_RUNNING               0x00000010
#define FS_ARCHIVE_FLAG_CANCELING             0x00000020
#define FS_ARCHIVE_FLAG_SUSPENDING            0x00000040
#define FS_ARCHIVE_FLAG_UNLOADING             0x00000080
#define FS_ARCHIVE_FLAG_IS_ASYNC              0x00000100
#define FS_ARCHIVE_FLAG_IS_SYNC               0x00000200

#define FS_ARCHIVE_FLAG_USER_RESERVED_BIT     0x00010000
#define FS_ARCHIVE_FLAG_USER_RESERVED_MASK    0xFFFF0000

typedef enum {
	FS_COMMAND_ASYNC_BEGIN = 0,
	FS_COMMAND_READFILE = FS_COMMAND_ASYNC_BEGIN,
	FS_COMMAND_WRITEFILE,
	FS_COMMAND_ASYNC_END,
	FS_COMMAND_SYNC_BEGIN = FS_COMMAND_ASYNC_END,
	FS_COMMAND_SEEKDIR = FS_COMMAND_SYNC_BEGIN,
	FS_COMMAND_READDIR,
	FS_COMMAND_FINDPATH,
	FS_COMMAND_GETPATH,
	FS_COMMAND_OPENFILEFAST,
	FS_COMMAND_OPENFILEDIRECT,
	FS_COMMAND_CLOSEFILE,
	FS_COMMAND_SYNC_END,
	FS_COMMAND_STATUS_BEGIN = FS_COMMAND_SYNC_END,
	FS_COMMAND_ACTIVATE = FS_COMMAND_STATUS_BEGIN,
	FS_COMMAND_IDLE,
	FS_COMMAND_SUSPEND,
	FS_COMMAND_RESUME,
	FS_COMMAND_STATUS_END,
	FS_COMMAND_INVALID
} FSCommandType;

#define FS_ARCHIVE_PROC_READFILE        (1 << FS_COMMAND_READFILE)
#define FS_ARCHIVE_PROC_WRITEFILE       (1 << FS_COMMAND_WRITEFILE)

#define FS_ARCHIVE_PROC_ASYNC   \
	(FS_ARCHIVE_PROC_READFILE | FS_ARCHIVE_PROC_WRITEFILE)

#define FS_ARCHIVE_PROC_SEEKDIR         (1 << FS_COMMAND_SEEKDIR)
#define FS_ARCHIVE_PROC_READDIR         (1 << FS_COMMAND_READDIR)
#define FS_ARCHIVE_PROC_FINDPATH        (1 << FS_COMMAND_FINDPATH)
#define FS_ARCHIVE_PROC_GETPATH         (1 << FS_COMMAND_GETPATH)
#define FS_ARCHIVE_PROC_OPENFILEFAST    (1 << FS_COMMAND_OPENFILEFAST)
#define FS_ARCHIVE_PROC_OPENFILEDIRECT  (1 << FS_COMMAND_OPENFILEDIRECT)
#define FS_ARCHIVE_PROC_CLOSEFILE       (1 << FS_COMMAND_CLOSEFILE)

#define FS_ARCHIVE_PROC_SYNC                                                                      \
	(FS_ARCHIVE_PROC_SEEKDIR | FS_ARCHIVE_PROC_READDIR |                                          \
	 FS_ARCHIVE_PROC_FINDPATH | FS_ARCHIVE_PROC_GETPATH |                                         \
	 FS_ARCHIVE_PROC_OPENFILEFAST | FS_ARCHIVE_PROC_OPENFILEDIRECT | FS_ARCHIVE_PROC_CLOSEFILE)

#define FS_ARCHIVE_PROC_ACTIVATE        (1 << FS_COMMAND_ACTIVATE)
#define FS_ARCHIVE_PROC_IDLE            (1 << FS_COMMAND_IDLE)
#define FS_ARCHIVE_PROC_SUSPENDING      (1 << FS_COMMAND_SUSPEND)
#define FS_ARCHIVE_PROC_RESUME          (1 << FS_COMMAND_RESUME)

#define FS_ARCHIVE_PROC_STATUS                              \
	(FS_ARCHIVE_PROC_ACTIVATE | FS_ARCHIVE_PROC_IDLE |      \
	 FS_ARCHIVE_PROC_SUSPENDING | FS_ARCHIVE_PROC_RESUME)

#define FS_ARCHIVE_PROC_ALL (~0)

typedef enum {
	FS_RESULT_SUCCESS = 0,
	FS_RESULT_FAILURE,
	FS_RESULT_BUSY,
	FS_RESULT_CANCELED,
	FS_RESULT_CANCELLED = FS_RESULT_CANCELED,
	FS_RESULT_UNSUPPORTED,
	FS_RESULT_ERROR,
	FS_RESULT_PROC_ASYNC,
	FS_RESULT_PROC_DEFAULT,
	FS_RESULT_PROC_UNKNOWN
} FSResult;

struct FSFile;
struct FSFileLink;
struct FSArchiveFAT;
struct FSArchiveFNT;
struct FSArchive;

typedef FSResult (*FS_ARCHIVE_PROC_FUNC) (struct FSFile *, FSCommandType);
typedef FSResult (*FS_ARCHIVE_READ_FUNC) (struct FSArchive * p, void * dst, u32 pos, u32 size);
typedef FSResult (*FS_ARCHIVE_WRITE_FUNC) (struct FSArchive * p, const void * src, u32 pos, u32 size);

typedef struct FSFileLink {
	struct FSFile * prev;
	struct FSFile * next;
} FSFileLink;

typedef struct FSArchiveFAT {
	u32 top;
	u32 bottom;
} FSArchiveFAT;

typedef struct FSArchiveFNT {
	u32 start;
	u16 index;
	u16 parent;
} FSArchiveFNT;

#elif SDK_VERSION_MAJOR == 5
typedef struct FSArchiveInterface {

  FSResult (*ReadFile)(struct FSArchive *, struct FSFile *, void *buffer,
                       u32 *length);
  FSResult (*WriteFile)(struct FSArchive *, struct FSFile *, const void *buffer,
                        u32 *length);
  FSResult (*SeekDirectory)(struct FSArchive *, struct FSFile *, u32 id,
                            u32 position);
  FSResult (*ReadDirectory)(struct FSArchive *, struct FSFile *,
                            FSDirectoryEntryInfo *info);
  FSResult (*FindPath)(struct FSArchive *, u32 base_dir_id, const char *path,
                       u32 *target_id, BOOL target_is_directory);
  FSResult (*GetPath)(struct FSArchive *, struct FSFile *, BOOL is_directory,
                      char *buffer, u32 *length);
  FSResult (*OpenFileFast)(struct FSArchive *, struct FSFile *, u32 id,
                           u32 mode);
  FSResult (*OpenFileDirect)(struct FSArchive *, struct FSFile *, u32 top,
                             u32 bottom, u32 *id);
  FSResult (*CloseFile)(struct FSArchive *, struct FSFile *);
  void (*Activate)(struct FSArchive *);
  void (*Idle)(struct FSArchive *);
  void (*Suspend)(struct FSArchive *);
  void (*Resume)(struct FSArchive *);

  FSResult (*OpenFile)(struct FSArchive *, struct FSFile *, u32 base_dir_id,
                       const char *path, u32 mode);
  FSResult (*SeekFile)(struct FSArchive *, struct FSFile *, int *offset,
                       FSSeekFileMode from);
  FSResult (*GetFileLength)(struct FSArchive *, struct FSFile *, u32 *length);
  FSResult (*GetFilePosition)(struct FSArchive *, struct FSFile *,
                              u32 *position);

  void (*Mount)(struct FSArchive *);
  void (*Unmount)(struct FSArchive *);
  FSResult (*GetArchiveCaps)(struct FSArchive *, u32 *caps);
  FSResult (*CreateFile)(struct FSArchive *, u32 baseid, const char *relpath,
                         u32 permit);
  FSResult (*DeleteFile)(struct FSArchive *, u32 baseid, const char *relpath);
  FSResult (*RenameFile)(struct FSArchive *, u32 baseid_src,
                         const char *relpath_src, u32 baseid_dst,
                         const char *relpath_dst);
  FSResult (*GetPathInfo)(struct FSArchive *, u32 baseid, const char *relpath,
                          FSPathInfo *info);
  FSResult (*SetPathInfo)(struct FSArchive *, u32 baseid, const char *relpath,
                          FSPathInfo *info);
  FSResult (*CreateDirectory)(struct FSArchive *, u32 baseid,
                              const char *relpath, u32 permit);
  FSResult (*DeleteDirectory)(struct FSArchive *, u32 baseid,
                              const char *relpath);
  FSResult (*RenameDirectory)(struct FSArchive *, u32 baseid,
                              const char *relpath_src, u32 baseid_dst,
                              const char *relpath_dst);
  FSResult (*GetArchiveResource)(struct FSArchive *,
                                 FSArchiveResource *resource);
  void *unused_29;
  FSResult (*FlushFile)(struct FSArchive *, struct FSFile *);
  FSResult (*SetFileLength)(struct FSArchive *, struct FSFile *, u32 length);
  FSResult (*OpenDirectory)(struct FSArchive *, struct FSFile *,
                            u32 base_dir_id, const char *path, u32 mode);
  FSResult (*CloseDirectory)(struct FSArchive *, struct FSFile *);
  FSResult (*SetSeekCache)(struct FSArchive *, struct FSFile *, void *buf,
                           u32 buf_size);

  u8 reserved[116];
} FSArchiveInterface;

SDK_COMPILER_ASSERT(sizeof(FSArchiveInterface) == 256);
#endif

typedef struct FSArchive {
	union {
		char ptr[FS_ARCHIVE_NAME_LEN_MAX + 1];
		u32 pack;
	} name;
	struct FSArchive * next;
	#if SDK_VERSION_MAJOR == 4
	struct FSArchive * prev;
	OSThreadQueue sync_q;
	OSThreadQueue stat_q;
	u32 flag;
	FSFileLink list;
	u32 base;
	u32 fat;
	u32 fat_size;
	u32 fnt;
	u32 fnt_size;
	u32 fat_bak;
	u32 fnt_bak;
	void * load_mem;
	FS_ARCHIVE_READ_FUNC read_func;
	FS_ARCHIVE_WRITE_FUNC write_func;
	FS_ARCHIVE_READ_FUNC table_func;
	FS_ARCHIVE_PROC_FUNC proc;
	u32 proc_flag;
	#elif SDK_VERSION_MAJOR == 5
  	struct FSFile *list;            // Process wait command list
  	OSThreadQueue queue;            // General-purpose queue to wait for events
  	u32 flag;                       // Internal status flags (FS_ARCHIVE_FLAG_*)
  	FSCommandType command;          // The most recent command
  	FSResult result;                // The most recent processing result
  	void *userdata;                 // User-defined pointer
  	const FSArchiveInterface *vtbl; // Command interface

  	union {

  	  u8 reserved2[52];

  	  struct FS_ROMFAT_CONTEXT_DEFINITION();
  	};
	#endif
} FSArchive;

#if SDK_VERSION_MAJOR == 5
SDK_COMPILER_ASSERT(sizeof(FSArchive) == 92);

FSArchive *FS_NormalizePath(const char *path, u32 *baseid, char *relpath);
const char *FS_GetCurrentDirectory(void);
FSResult FS_GetArchiveResultCode(const void *path_or_archive);

SDK_INLINE FSCommandType FS_GetLastArchiveCommand(const FSArchive *arc) {
  return arc->command;
}
#endif

void FS_InitArchive(FSArchive * p_arc);

#if SDK_VERSION_MAJOR == 4
static inline const char * FS_GetArchiveName (const FSArchive * p_arc)
{
	return p_arc->name.ptr;
}
#elif SDK_VERSION_MAJOR == 5
const char *FS_GetArchiveName(const FSArchive *arc);
#endif

#if SDK_VERSION_MAJOR == 4
static inline u32 FS_GetArchiveBase (const FSArchive * p_arc)
{
	return p_arc->base;
}

static inline u32 FS_GetArchiveFAT (const FSArchive * p_arc)
{
	return p_arc->fat;
}

static inline u32 FS_GetArchiveFNT (const FSArchive * p_arc)
{
	return p_arc->fnt;
}

static inline u32 FS_GetArchiveOffset (const FSArchive * p_arc, u32 pos)
{
	return p_arc->base + pos;
}
#endif

static inline BOOL FS_IsArchiveLoaded (volatile const FSArchive * p_arc)
{
	return (p_arc->flag & FS_ARCHIVE_FLAG_LOADED) ? TRUE : FALSE;
}

#if SDK_VERSION_MAJOR == 4
static inline BOOL FS_IsArchiveTableLoaded (volatile const FSArchive * p_arc)
{
	return (p_arc->flag & FS_ARCHIVE_FLAG_TABLE_LOAD) ? TRUE : FALSE;
}
#endif

static inline BOOL FS_IsArchiveSuspended (volatile const FSArchive * p_arc)
{
	return (p_arc->flag & FS_ARCHIVE_FLAG_SUSPEND) ? TRUE : FALSE;
}

FSArchive * FS_FindArchive(const char * name, int name_len);

BOOL FS_RegisterArchiveName(FSArchive * p_arc, const char * name, u32 name_len);
void FS_ReleaseArchiveName(FSArchive * p_arc);

#if SDK_VERSION_MAJOR == 4
BOOL FS_LoadArchive(FSArchive * p_arc, u32 base,
                    u32 fat, u32 fat_size, u32 fnt, u32 fnt_size,
                    FS_ARCHIVE_READ_FUNC read_func, FS_ARCHIVE_WRITE_FUNC write_func);

BOOL FS_UnloadArchive(FSArchive * p_arc);
u32 FS_LoadArchiveTables(FSArchive * p_arc, void * p_mem, u32 max_size);
void * FS_UnloadArchiveTables(FSArchive * p_arc);
#endif
BOOL FS_SuspendArchive(FSArchive * p_arc);
BOOL FS_ResumeArchive(FSArchive * p_arc);
#if SDK_VERSION_MAJOR == 4
void FS_SetArchiveProc(struct FSArchive * p_arc, FS_ARCHIVE_PROC_FUNC proc, u32 flags);
#endif
void FS_NotifyArchiveAsyncEnd(FSArchive * p_arc, FSResult ret);
void FSi_EndArchive(void);

#ifdef __cplusplus
}
#endif

#endif

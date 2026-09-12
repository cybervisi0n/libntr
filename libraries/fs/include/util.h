#if !defined(NITRO_FS_UTIL_H_)
#define NITRO_FS_UTIL_H_

#include <nitro/misc.h>
#include <nitro/types.h>
#if SDK_VERSION_MAJOR == 4
#include <nitro/fs/file.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SDK_VERSION_MAJOR == 4
#if !defined(SDK_ARM7) || defined(SDK_ARM7FS)
    #define FS_IMPLEMENT
#endif

#define BIT_MASK(n) ((1 << (n)) - 1)
#define ALIGN_MASK(a)   ((a) - 1)
#define ALIGN_BYTE(n, a)    (((u32)(n) + ALIGN_MASK(a)) & ~ALIGN_MASK(a))

#if !defined(SDK_FINALROM)
    extern const char * fsi_assert_fs_format;
    extern const char * fsi_assert_is_init;
    extern const char * fsi_assert_is_valid_arg;
    extern const char * fsi_assert_is_valid_dma_channel;
    extern const char * fsi_assert_is_empty;
    extern const char * fsi_assert_is_handle;
    extern const char * fsi_assert_is_file;
    extern const char * fsi_assert_is_dir;
    extern const char * fsi_assert_is_idle;
    extern const char * fsi_assert_irq_enabled;

    extern const char * fsi_assert_arc_free;
    extern const char * fsi_assert_arc_unloaded;
    extern const char * fsi_assert_arc_not_rom;

    #define FS_ASSERT_INIT(ret) \
        if(!FS_IsAvailable())   OS_TPanic(fsi_assert_fs_format, fsi_assert_is_init)

    #define FS_ASSERT_ARG(e, ret)   \
        if(!(e))    OS_TPanic(fsi_assert_fs_format, fsi_assert_is_valid_arg)

    #define FS_ASSERT_DMA_CHANNEL(dma, ret) \
        if(dma == 0)    OS_TPanic(fsi_assert_fs_format, fsi_assert_is_valid_dma_channel)

    #define FS_ASSERT_EMPTY(p_file, ret)    \
        if(FS_IsFile(p_file))   OS_TPanic(fsi_assert_fs_format, fsi_assert_is_empty)

    #define FS_ASSERT_HANDLE(p_file, ret)   \
        if(!FS_IsFile(p_file) && !FS_IsDir(p_file)) OS_TPanic(fsi_assert_fs_format, fsi_assert_is_handle)

    #define FS_ASSERT_FILE(p_file, ret) \
        if(!FS_IsFile(p_file))  OS_TPanic(fsi_assert_fs_format, fsi_assert_is_file)

    #define FS_ASSERT_DIR(p_file, ret)  \
        if(!FS_IsDir(p_file))   OS_TPanic(fsi_assert_fs_format, fsi_assert_is_dir)

    #define FS_ASSERT_IDLE(p_file, ret) \
        if(FS_IsBusy(p_file))   OS_TPanic(fsi_assert_fs_format, fsi_assert_is_idle)

    #define FS_ASSERT_IRQ_ENABLED(ret)  \
        if(OS_GetProcMode() == OS_PROCMODE_IRQ) \
        OS_TPanic(fsi_assert_fs_format, fsi_assert_irq_enabled)

    #define FS_ASSERT_ARC_FREE(p_arc, ret)  \
        if(p_arc->name.pack)    OS_TPanic(fsi_assert_fs_format, fsi_assert_arc_free)

    #define FS_ASSERT_ARC_UNLOADED(p_arc, ret)  \
        if(FS_IsArchiveLoaded(p_arc))   OS_TPanic(fsi_assert_fs_format, fsi_assert_arc_unloaded)

    #define FS_ASSERT_ARC_NOT_ROM(p_arc, ret)   \
        if(p_arc == arc_list)   OS_TPanic(fsi_assert_fs_format, fsi_assert_arc_not_rom)
#else
    #define FS_ASSERT_INIT(ret)
    #define FS_ASSERT_ARG(e, ret)
    #define FS_ASSERT_DMA_CHANNEL(dma, ret)

    #define FS_ASSERT_EMPTY(p_file, ret)
    #define FS_ASSERT_HANDLE(p_file, ret)
    #define FS_ASSERT_FILE(p_file, ret)
    #define FS_ASSERT_DIR(p_file, ret)
    #define FS_ASSERT_IDLE(p_file, ret)
    #define FS_ASSERT_IRQ_ENABLED(ret)

    #define FS_ASSERT_ARC_FREE(p_file, ret)
    #define FS_ASSERT_ARC_UNLOADED(p_file, ret)
    #define FS_ASSERT_ARC_NOT_ROM(p_file, ret)
#endif

#if defined(FS_IMPLEMENT)
    static inline void FSi_CutFromListCore (FSFileLink * trg)
    {
        FSFile * const pr = trg->prev;
        FSFile * const nx = trg->next;
        if (pr)
            pr->link.next = nx;
        if (nx)
            nx->link.prev = pr;
    }

    static inline void FSi_CutFromList (FSFile * elem)
    {
        FSFileLink * const trg = &elem->link;
        FSi_CutFromListCore(trg);
        trg->next = trg->prev = NULL;
    }

    static inline void FSi_AppendToList (FSFile * elem, FSFile * list)
    {
        FSFileLink * const trg = &elem->link;
        FSi_CutFromListCore(trg);
        {
            while (list->link.next)
                list = list->link.next;
            list->link.next = elem;
            trg->prev = list;
            trg->next = NULL;
        }
    }
#endif

static inline BOOL FSi_IsSlash (u32 c)
{
	return (c == '/') || (c == '\\');
}

static inline BOOL FSi_IsArchiveRunning (const volatile FSArchive * p_arc)
{
	return ((p_arc->flag & FS_ARCHIVE_FLAG_RUNNING) != 0);
}

static inline BOOL FSi_IsArchiveCanceling (const volatile FSArchive * p_arc)
{
	return ((p_arc->flag & FS_ARCHIVE_FLAG_CANCELING) != 0);
}

static inline BOOL FSi_IsArchiveSuspending (const volatile FSArchive * p_arc)
{
	return ((p_arc->flag & FS_ARCHIVE_FLAG_SUSPENDING) != 0);
}

static inline BOOL FSi_IsArchiveAsync (const volatile FSArchive * p_arc)
{
	return ((p_arc->flag & FS_ARCHIVE_FLAG_IS_ASYNC) != 0);
}

static inline BOOL FSi_IsArchiveSync (const volatile FSArchive * p_arc)
{
	return ((p_arc->flag & FS_ARCHIVE_FLAG_IS_SYNC) != 0);
}
#elif SDK_VERSION_MAJOR == 5

#if !defined(SDK_ARM7) || defined(SDK_ARM7FS)
#define FS_IMPLEMENT
#endif

SDK_INLINE BOOL FSi_IsSlash(u32 c) { return (c == '/') || (c == '\\'); }

SDK_INLINE int FSi_IncrementSjisPosition(const char *str, int pos) {
  return pos + 1 + STD_IsSjisLeadByte(str[pos]);
}

int FSi_DecrementSjisPosition(const char *str, int pos);

int FSi_IncrementSjisPositionToSlash(const char *str, int pos);

int FSi_DecrementSjisPositionToSlash(const char *str, int pos);

int FSi_TrimSjisTrailingSlash(char *str);

SDK_INLINE int FSi_StrNICmp(const char *str1, const char *str2, u32 len) {
  int retval = 0;
  int i;
  for (i = 0; i < len; ++i) {
    u32 c = (u8)(str1[i] - 'A');
    u32 d = (u8)(str2[i] - 'A');
    if (c <= 'Z' - 'A') {
      c += 'a' - 'A';
    }
    if (d <= 'Z' - 'A') {
      d += 'a' - 'A';
    }
    retval = (int)(c - d);
    if (retval != 0) {
      break;
    }
  }
  return retval;
}

SDK_INLINE BOOL FSi_IsUnicodeSlash(u16 c) {
  return (c == L'/') || (c == L'\\');
}

int FSi_DecrementUnicodePosition(const u16 *str, int pos);

int FSi_DecrementUnicodePositionToSlash(const u16 *str, int pos);

SDK_INLINE void FSi_WaitConditionChange(u32 *flags, u32 on, u32 off,
                                        OSThreadQueue *queue) {
  OSIntrMode bak = OS_DisableInterrupts();
  while ((!on || ((*flags & on) == 0)) && (!off || ((*flags & off) != 0))) {
    OS_SleepThread(queue);
  }
  (void)OS_RestoreInterrupts(bak);
}

SDK_INLINE void FSi_WaitConditionOn(u32 *flags, u32 bits,
                                    OSThreadQueue *queue) {
  FSi_WaitConditionChange(flags, bits, 0, queue);
}

SDK_INLINE void FSi_WaitConditionOff(u32 *flags, u32 bits,
                                     OSThreadQueue *queue) {
  FSi_WaitConditionChange(flags, 0, bits, queue);
}

BOOL FSi_GetFileLengthIfProc(FSFile *file, u32 *length);

BOOL FSi_GetFilePositionIfProc(FSFile *file, u32 *length);

BOOL FSi_SeekFileIfProc(FSFile *file, s32 offset, FSSeekFileMode from);
#endif

#ifdef __cplusplus
}
#endif

#endif

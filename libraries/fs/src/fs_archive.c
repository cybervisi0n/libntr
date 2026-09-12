#if SDK_VERSION_MAJOR == 4
#include <nitro/mi.h>
#include <nitro/os.h>
#endif
#include <nitro/fs.h>

#if SDK_VERSION_MAJOR == 5
#include <nitro/mi/memory.h>
#include <nitro/std.h>
#endif

#include "../include/util.h"
#include "../include/command.h"

#if defined(FS_IMPLEMENT)

static FSArchive *arc_list = NULL;

FSDirPos current_dir_pos;
#if SDK_VERSION_MAJOR == 5
static char current_dir_path[FS_ENTRY_LONGNAME_MAX];


#if defined(FS_SUPPORT_LONG_ARCNAME)

#define FS_LONG_ARCNAME_LENGTH_MAX 15
#define FS_LONG_ARCNAME_TABLE_MAX 16
static char FSiLongNameTable[FS_LONG_ARCNAME_TABLE_MAX]
                            [FS_LONG_ARCNAME_LENGTH_MAX + 1];
#endif

FSArchive *FSi_GetArchiveChain(void) { return arc_list; }

static BOOL FSi_IsEventCommand(FSCommandType command) {
  return ((command == FS_COMMAND_ACTIVATE) || (command == FS_COMMAND_IDLE) ||
          (command == FS_COMMAND_SUSPEND) || (command == FS_COMMAND_RESUME) ||
          (command == FS_COMMAND_MOUNT) || (command == FS_COMMAND_UNMOUNT) ||
          (command == FS_COMMAND_INVALID));
}

static void FSi_EndCommand(FSFile *file, FSResult ret) {
  OSIntrMode bak_psr = OS_DisableInterrupts();

  FSArchive *const arc = file->arc;
  if (arc) {
    FSFile **pp = &arc->list;
    for (; *pp; pp = &(*pp)->next) {
      if (*pp == file) {
        *pp = file->next;
        break;
      }
    }
    file->next = NULL;
  }

  {
    FSCommandType command = FSi_GetCurrentCommand(file);
    if (!FSi_IsEventCommand(command) && (arc != NULL)) {
      arc->command = command;
      arc->result = ret;
    }
    file->error = ret;
    file->stat &= ~(FS_FILE_STATUS_CANCEL | FS_FILE_STATUS_BUSY |
                    FS_FILE_STATUS_BLOCKING | FS_FILE_STATUS_OPERATING |
                    FS_FILE_STATUS_ASYNC_DONE | FS_FILE_STATUS_UNICODE_MODE);
  }

  OS_WakeupThread(file->queue);
  (void)OS_RestoreInterrupts(bak_psr);
}

FSResult FSi_WaitForArchiveCompletion(FSFile *file, FSResult result) {
  if (result == FS_RESULT_PROC_ASYNC) {
    FSi_WaitConditionOn(&file->stat, FS_FILE_STATUS_ASYNC_DONE, file->queue);
    file->stat &= ~FS_FILE_STATUS_ASYNC_DONE;
    result = file->error;
  }
  return result;
}

static FSResult FSi_InvokeCommand(FSFile *file, FSCommandType command) {
  FSResult result = FS_RESULT_UNSUPPORTED;
  FSArchive *const arc = file->arc;

  {
    const void *(*table) = (const void *)arc->vtbl;

    if ((command < 0) || (command >= FS_COMMAND_MAX)) {
      OS_TWarning("undefined command (%d)\n", command);
      result = FS_RESULT_UNSUPPORTED;
    }

    else if (table[command] == NULL) {
      result = FS_RESULT_UNSUPPORTED;
    } else {
#define FS_DECLARE_ARGUMENT_(type)                                             \
  type *arg = (type *)file->argument;                                          \
  (void)arg

#define FS_INVOKE_METHOD_(command, ...)                                        \
  do {                                                                         \
    FS_DECLARE_ARGUMENT_(FSArgumentFor##command);                              \
    result = arc->vtbl->command(__VA_ARGS__);                                  \
  } while (0)
#define FS_NOTIFY_EVENT_(command, ...)                                         \
  do {                                                                         \
    FS_DECLARE_ARGUMENT_(FSArgumentFor##command);                              \
    (void)arc->vtbl->command(__VA_ARGS__);                                     \
    return FS_RESULT_SUCCESS;                                                  \
  } while (0)
      switch (command) {
      case FS_COMMAND_READFILE:
        FS_INVOKE_METHOD_(ReadFile, arc, file, arg->buffer, &arg->length);
        break;
      case FS_COMMAND_WRITEFILE:
        FS_INVOKE_METHOD_(WriteFile, arc, file, arg->buffer, &arg->length);
        break;
      case FS_COMMAND_SEEKDIR:
        FS_INVOKE_METHOD_(SeekDirectory, arc, file, arg->id, arg->position);
        break;
      case FS_COMMAND_READDIR:
        FS_INVOKE_METHOD_(ReadDirectory, arc, file, arg->info);
        break;
      case FS_COMMAND_FINDPATH:
        FS_INVOKE_METHOD_(FindPath, arc, arg->baseid, arg->relpath,
                          &arg->target_id, arg->target_is_directory);
        break;
      case FS_COMMAND_GETPATH:
        FS_INVOKE_METHOD_(GetPath, arc, file, arg->is_directory, arg->buffer,
                          &arg->length);
        break;
      case FS_COMMAND_OPENFILEFAST:
        FS_INVOKE_METHOD_(OpenFileFast, arc, file, arg->id, arg->mode);
        break;
      case FS_COMMAND_OPENFILEDIRECT:
        FS_INVOKE_METHOD_(OpenFileDirect, arc, file, arg->top, arg->bottom,
                          &arg->id);
        break;
      case FS_COMMAND_CLOSEFILE:
        FS_INVOKE_METHOD_(CloseFile, arc, file);
        break;
      case FS_COMMAND_ACTIVATE:
        FS_NOTIFY_EVENT_(Activate, arc);
        break;
      case FS_COMMAND_IDLE:
        FS_NOTIFY_EVENT_(Idle, arc);
        break;
      case FS_COMMAND_SUSPEND:
        FS_NOTIFY_EVENT_(Suspend, arc);
        break;
      case FS_COMMAND_RESUME:
        FS_NOTIFY_EVENT_(Resume, arc);
        break;
      case FS_COMMAND_OPENFILE:
        FS_INVOKE_METHOD_(OpenFile, arc, file, arg->baseid, arg->relpath,
                          arg->mode);
        break;
      case FS_COMMAND_SEEKFILE:
        FS_INVOKE_METHOD_(SeekFile, arc, file, &arg->offset, arg->from);
        break;
      case FS_COMMAND_GETFILELENGTH:
        FS_INVOKE_METHOD_(GetFileLength, arc, file, &arg->length);
        break;
      case FS_COMMAND_GETFILEPOSITION:
        FS_INVOKE_METHOD_(GetFilePosition, arc, file, &arg->position);
        break;

      case FS_COMMAND_MOUNT:
        FS_NOTIFY_EVENT_(Mount, arc);
        break;
      case FS_COMMAND_UNMOUNT:
        FS_NOTIFY_EVENT_(Unmount, arc);
        break;
      case FS_COMMAND_GETARCHIVECAPS:
        FS_INVOKE_METHOD_(GetArchiveCaps, arc, &arg->caps);
        break;
      case FS_COMMAND_CREATEFILE:
        FS_INVOKE_METHOD_(CreateFile, arc, arg->baseid, arg->relpath,
                          arg->permit);
        break;
      case FS_COMMAND_DELETEFILE:
        FS_INVOKE_METHOD_(DeleteFile, arc, arg->baseid, arg->relpath);
        break;
      case FS_COMMAND_RENAMEFILE:
        FS_INVOKE_METHOD_(RenameFile, arc, arg->baseid_src, arg->relpath_src,
                          arg->baseid_dst, arg->relpath_dst);
        break;
      case FS_COMMAND_GETPATHINFO:
        FS_INVOKE_METHOD_(GetPathInfo, arc, arg->baseid, arg->relpath,
                          arg->info);
        break;
      case FS_COMMAND_SETPATHINFO:
        FS_INVOKE_METHOD_(SetPathInfo, arc, arg->baseid, arg->relpath,
                          arg->info);
        break;
      case FS_COMMAND_CREATEDIRECTORY:
        FS_INVOKE_METHOD_(CreateDirectory, arc, arg->baseid, arg->relpath,
                          arg->permit);
        break;
      case FS_COMMAND_DELETEDIRECTORY:
        FS_INVOKE_METHOD_(DeleteDirectory, arc, arg->baseid, arg->relpath);
        break;
      case FS_COMMAND_RENAMEDIRECTORY:
        FS_INVOKE_METHOD_(RenameDirectory, arc, arg->baseid_src,
                          arg->relpath_src, arg->baseid_dst, arg->relpath_dst);
        break;
      case FS_COMMAND_GETARCHIVERESOURCE:
        FS_INVOKE_METHOD_(GetArchiveResource, arc, arg->resource);
        break;
      case FS_COMMAND_FLUSHFILE:
        FS_INVOKE_METHOD_(FlushFile, arc, file);
        break;
      case FS_COMMAND_SETFILELENGTH:
        FS_INVOKE_METHOD_(SetFileLength, arc, file, arg->length);
        break;
      case FS_COMMAND_OPENDIRECTORY:
        FS_INVOKE_METHOD_(OpenDirectory, arc, file, arg->baseid, arg->relpath,
                          arg->mode);
        break;
      case FS_COMMAND_CLOSEDIRECTORY:
        FS_INVOKE_METHOD_(CloseDirectory, arc, file);
        break;
      case FS_COMMAND_SETSEEKCACHE:
        FS_INVOKE_METHOD_(SetSeekCache, arc, file, arg->buf, arg->buf_size);
        break;
      default:
        result = FS_RESULT_UNSUPPORTED;
      }
#undef FS_DECLARE_ARGUMENT_
#undef FS_INVOKE_METHOD_
#undef FS_NOTIFY_EVENT_
    }
  }

  if (!FSi_IsEventCommand(command)) {

    if (result == FS_RESULT_UNSUPPORTED) {
      OS_TWarning("archive \"%s:\" cannot support command %d.\n",
                  FS_GetArchiveName(arc), command);
    }

    if ((file->stat & FS_FILE_STATUS_BLOCKING) != 0) {
      result = FSi_WaitForArchiveCompletion(file, result);
    }

    else if (result != FS_RESULT_PROC_ASYNC) {
      FSi_EndCommand(file, result);
    }
  }
  return result;
}
#endif

#if SDK_VERSION_MAJOR == 4
static u32 FSi_GetPackedName (const char *name, int name_len)
{
	u32 ret = 0;

	if (name_len <= FS_ARCHIVE_NAME_LEN_MAX) {
		int i = 0;
		for (; i < name_len; ++i) {
			u32 c = (u32)MI_ReadByte(name + i);
			if (!c)
				break;
			c = (u32)(c - 'A');
			if (c <= (u32)('Z' - 'A'))
				c = (u32)(c + 'a');
			else
				c = (u32)(c + 'A');
			ret |= (u32)(c << (i * 8));
		}
	}

	return ret;
}

static FSResult FSi_ReadMemCallback (FSArchive *p_arc, void *dst, u32 pos, u32 size)
{
	MI_CpuCopy8((const void *)FS_GetArchiveOffset(p_arc, pos), dst, size);
	return FS_RESULT_SUCCESS;
}

static FSResult FSi_WriteMemCallback (FSArchive *p_arc, const void *src, u32 pos, u32 size)
{
	MI_CpuCopy8(src, (void *)FS_GetArchiveOffset(p_arc, pos), size);
	return FS_RESULT_SUCCESS;
}

static FSResult FSi_ReadMemoryCore (FSArchive *p_arc, void *dst, u32 pos, u32 size)
{
	(void)p_arc;
	MI_CpuCopy8((const void *)pos, dst, size);

	return FS_RESULT_SUCCESS;
}
#endif

FSFile *FSi_NextCommand (
	FSArchive *p_arc
	#if SDK_VERSION_MAJOR == 5
	, BOOL owner
	#endif
)
{
	#if SDK_VERSION_MAJOR == 4
	OSIntrMode bak_psr = OS_DisableInterrupts();

	if (FSi_IsArchiveCanceling(p_arc)) {
		FSFile *p, *q;
		p_arc->flag &= ~FS_ARCHIVE_FLAG_CANCELING;
		for (p = p_arc->list.next; p; p = q) {
			q = p->link.next;
			if (FS_IsCanceling(p)) {
				if (p_arc->list.next == p)
					p_arc->list.next = q;
				FSi_ReleaseCommand(p, FS_RESULT_CANCELED);
				if (!q)
					q = p_arc->list.next;
			}
		}
	}

	if (!FSi_IsArchiveSuspending(p_arc) && !FS_IsArchiveSuspended(p_arc) && p_arc->list.next) {
		FSFile *p_file = p_arc->list.next;
		const BOOL is_start = !FSi_IsArchiveRunning(p_arc);

		if (is_start)
			p_arc->flag |= FS_ARCHIVE_FLAG_RUNNING;

		(void)OS_RestoreInterrupts(bak_psr);

		if (is_start) {
			if ((p_arc->proc_flag & FS_ARCHIVE_PROC_ACTIVATE) != 0)
				(void)(*p_arc->proc)(p_file, FS_COMMAND_ACTIVATE);
		}

		bak_psr = OS_DisableInterrupts();
		p_file->stat |= FS_FILE_STATUS_OPERATING;

		if (FS_IsFileSyncMode(p_file)) {
			OS_WakeupThread(p_file->queue);
			(void)OS_RestoreInterrupts(bak_psr);
			return NULL;
		} else {
			(void)OS_RestoreInterrupts(bak_psr);
			return p_file;
		}
	}

	if (FSi_IsArchiveRunning(p_arc)) {
		p_arc->flag &= ~FS_ARCHIVE_FLAG_RUNNING;

		if ((p_arc->proc_flag & FS_ARCHIVE_PROC_IDLE) != 0) {
			FSFile tmp;
			FS_InitFile(&tmp);
			tmp.arc = p_arc;
			(void)(*p_arc->proc)(&tmp, FS_COMMAND_IDLE);
		}
	}

	if (FSi_IsArchiveSuspending(p_arc)) {
		p_arc->flag &= ~FS_ARCHIVE_FLAG_SUSPENDING;
		p_arc->flag |= FS_ARCHIVE_FLAG_SUSPEND;
		OS_WakeupThread(&p_arc->stat_q);
	}

	(void)OS_RestoreInterrupts(bak_psr);

	return NULL;
	#elif SDK_VERSION_MAJOR == 5
  FSFile *next = NULL;

  {
    OSIntrMode bak_psr = OS_DisableInterrupts();
    if ((arc->flag & FS_ARCHIVE_FLAG_CANCELING) != 0) {
      FSFile *p = arc->list;
      arc->flag &= ~FS_ARCHIVE_FLAG_CANCELING;
      while (p != NULL) {
        FSFile *q = p->next;

        if (FS_IsCanceling(p) && ((p->stat & FS_FILE_STATUS_OPERATING) == 0)) {
          FSi_EndCommand(p, FS_RESULT_CANCELED);
          if (!q) {
            q = arc->list;
          }
        }
        p = q;
      }
    }
    (void)OS_RestoreInterrupts(bak_psr);
  }

  {
    OSIntrMode bak_psr = OS_DisableInterrupts();
    if (((arc->flag & FS_ARCHIVE_FLAG_SUSPENDING) == 0) &&
        ((arc->flag & FS_ARCHIVE_FLAG_SUSPEND) == 0) && arc->list) {

      const BOOL is_started =
          owner && ((arc->flag & FS_ARCHIVE_FLAG_RUNNING) == 0);
      if (is_started) {
        arc->flag |= FS_ARCHIVE_FLAG_RUNNING;
      }
      (void)OS_RestoreInterrupts(bak_psr);
      if (is_started) {
        (void)FSi_InvokeCommand(arc->list, FS_COMMAND_ACTIVATE);
      }
      bak_psr = OS_DisableInterrupts();

      if (owner || is_started) {
        next = arc->list;
        next->stat |= FS_FILE_STATUS_OPERATING;
      }

      if (owner && ((next->stat & FS_FILE_STATUS_BLOCKING) != 0)) {
        OS_WakeupThread(next->queue);
        next = NULL;
      }
      (void)OS_RestoreInterrupts(bak_psr);
    }

    else {

      if (owner) {
        if ((arc->flag & FS_ARCHIVE_FLAG_RUNNING) != 0) {
          FSFile tmp;
          FS_InitFile(&tmp);
          tmp.arc = arc;
          arc->flag &= ~FS_ARCHIVE_FLAG_RUNNING;
          (void)FSi_InvokeCommand(&tmp, FS_COMMAND_IDLE);
        }

        if ((arc->flag & FS_ARCHIVE_FLAG_SUSPENDING) != 0) {
          arc->flag &= ~FS_ARCHIVE_FLAG_SUSPENDING;
          arc->flag |= FS_ARCHIVE_FLAG_SUSPEND;
          OS_WakeupThread(&arc->queue);
        }
      }
      (void)OS_RestoreInterrupts(bak_psr);
    }
  }
  return next;
	#endif
}

void FSi_ExecuteAsyncCommand (FSFile *p_file)
{
	FSArchive *const p_arc = p_file->arc;
	while (p_file) {
		OSIntrMode bak_psr = OS_DisableInterrupts();
		p_file->stat |= FS_FILE_STATUS_OPERATING;

		#if SDK_VERSION_MAJOR == 4
		if (FS_IsFileSyncMode(p_file)) {
			OS_WakeupThread(p_file->queue);
			(void)OS_RestoreInterrupts(bak_psr);
			break;
		} else {
			p_file->stat |= FS_FILE_STATUS_ASYNC;
		}

		(void)OS_RestoreInterrupts(bak_psr);

		if (FSi_TranslateCommand(p_file, p_file->command) == FS_RESULT_PROC_ASYNC)
			break;

		p_file = FSi_NextCommand(p_arc);
		#elif SDK_VERSION_MAJOR == 5
    	  if ((p_file->stat & FS_FILE_STATUS_BLOCKING) != 0) {
    	    OS_WakeupThread(p_file->queue);
    	    p_file = NULL;
    	  }
    	  (void)OS_RestoreInterrupts(bak_psr);

    	if (!p_file) {
    	  break;
    	}

    	else if (FSi_InvokeCommand(p_file, FSi_GetCurrentCommand(p_file)) ==
    	         FS_RESULT_PROC_ASYNC) {
    	  break;
    	}

    	else {
    	  p_file = FSi_NextCommand(arc, TRUE);
    	}
		#endif
	}
}

BOOL FSi_ExecuteSyncCommand (FSFile *p_file)
{
	#if SDK_VERSION_MAJOR == 4
	FSFile *p_target;
	FSResult ret = FSi_TranslateCommand(p_file, p_file->command);

	FSi_ReleaseCommand(p_file, ret);
	p_target = FSi_NextCommand(p_file->arc);

	if (p_target)
		FSi_ExecuteAsyncCommand(p_target);

	return FS_IsSucceeded(p_file);
	#elif SDK_VERSION_MAJOR == 5
  FSi_WaitConditionChange(&file->stat, FS_FILE_STATUS_OPERATING,
                          FS_FILE_STATUS_BUSY, file->queue);

  if ((file->stat & FS_FILE_STATUS_OPERATING) != 0) {
    FSArchive *const arc = file->arc;
    FSResult result;
    result = FSi_InvokeCommand(file, FSi_GetCurrentCommand(file));
    FSi_EndCommand(file, result);

    file = FSi_NextCommand(arc, TRUE);
    if (file) {
      FSi_ExecuteAsyncCommand(file);
    }
  }
	#endif
}

BOOL FSi_SendCommand (
	FSFile *p_file, 
	FSCommandType command
	#if SDK_VERSION_MAJOR == 5
	, BOOL blocking
	#endif
)
{
	#if SDK_VERSION_MAJOR == 4
	FSArchive *const p_arc = p_file->arc;
	const int bit = (1 << command);

	p_file->command = command;
	p_file->error = FS_RESULT_BUSY;
	p_file->stat |= FS_FILE_STATUS_BUSY;

	{
		OSIntrMode bak_psr = OS_DisableInterrupts();

		if (p_arc->flag & FS_ARCHIVE_FLAG_UNLOADING) {
			FSi_ReleaseCommand(p_file, FS_RESULT_CANCELED);
			(void)OS_RestoreInterrupts(bak_psr);
			return FALSE;
		}

		if ((bit & FS_ARCHIVE_PROC_SYNC) != 0)
			p_file->stat |= FS_FILE_STATUS_SYNC;

		FSi_AppendToList(p_file, (FSFile *)&p_arc->list);

		if (!FS_IsArchiveSuspended(p_arc) && !FSi_IsArchiveRunning(p_arc)) {
			p_arc->flag |= FS_ARCHIVE_FLAG_RUNNING;
			(void)OS_RestoreInterrupts(bak_psr);

			if ((p_arc->proc_flag & FS_ARCHIVE_PROC_ACTIVATE) != 0)
				(void)(*p_arc->proc)(p_file, FS_COMMAND_ACTIVATE);

			bak_psr = OS_DisableInterrupts();
			p_file->stat |= FS_FILE_STATUS_OPERATING;

			if (!FS_IsFileSyncMode(p_file)) {
				(void)OS_RestoreInterrupts(bak_psr);
				FSi_ExecuteAsyncCommand(p_file);
				return TRUE;
			}

			(void)OS_RestoreInterrupts(bak_psr);
		} else if (!FS_IsFileSyncMode(p_file))   {
			(void)OS_RestoreInterrupts(bak_psr);
			return TRUE;
		} else {
			do {
				OS_SleepThread(p_file->queue);
			} while (!(p_file->stat & FS_FILE_STATUS_OPERATING));
			(void)OS_RestoreInterrupts(bak_psr);
		}

	}

	return FSi_ExecuteSyncCommand(p_file);
	#elif SDK_VERSION_MAJOR == 5
  BOOL retval = FALSE;
  FSArchive *const arc = p_file->arc;
  BOOL owner = FALSE;

  if (FS_IsBusy(p_file)) {
    OS_TPanic("specified file is now still proceccing previous command!");
  }
  if (!arc) {
    OS_TWarning("specified handle is not related by any archive\n");
    p_file->error = FS_RESULT_INVALID_PARAMETER;
    return FALSE;
  }

  p_file->error = FS_RESULT_BUSY;
  p_file->stat &= ~(FS_FILE_STATUS_CMD_MASK << FS_FILE_STATUS_CMD_SHIFT);
  p_file->stat |= (command << FS_FILE_STATUS_CMD_SHIFT);
  p_file->stat |= FS_FILE_STATUS_BUSY;
  p_file->next = NULL;
  if (blocking) {
    p_file->stat |= FS_FILE_STATUS_BLOCKING;
  }

  {
    OSIntrMode bak_psr = OS_DisableInterrupts();
    if ((arc->flag & FS_ARCHIVE_FLAG_UNLOADING) != 0) {
      FSi_EndCommand(p_file, FS_RESULT_CANCELED);
    } else {
      FSFile **pp;
      for (pp = &arc->list; *pp; pp = &(*pp)->next) {
      }
      *pp = p_file;
    }
    owner = (arc->list == p_file) && ((arc->flag & FS_ARCHIVE_FLAG_RUNNING) == 0);
    (void)OS_RestoreInterrupts(bak_psr);
  }

  if (p_file->error != FS_RESULT_CANCELED) {

    FSFile *next = FSi_NextCommand(arc, owner);

    if (blocking) {
      FSi_ExecuteSyncCommand(p_file);
      retval = FS_IsSucceeded(p_file);
    }

    else {
      if (next != NULL) {
        FSi_ExecuteAsyncCommand(next);
      }
      retval = TRUE;
    }
  }

  return retval;
	#endif
}

void FSi_EndArchive (void)
{
	OSIntrMode bak_psr = OS_DisableInterrupts();
	#if SDK_VERSION_MAJOR == 4
	FSArchive *volatile *p_list = &arc_list;

	for (;;) {
		FSArchive *p_arc = arc_list;

		if (!p_arc)
			break;

		arc_list = arc_list->next;
		(void)FS_UnloadArchive(p_arc);
		FS_ReleaseArchiveName(p_arc);
	}
	#elif SDK_VERSION_MAJOR == 5
  while (arc_list) {
    FSArchive *p_arc = arc_list;
    arc_list = arc_list->next;
    (void)FS_UnloadArchive(p_arc);
    FS_ReleaseArchiveName(p_arc);
  }
	#endif

	(void)OS_RestoreInterrupts(bak_psr);
}

void FS_InitArchive (FSArchive *p_arc)
{
	FS_ASSERT_ARG(p_arc, FALSE);
	MI_CpuClear8(p_arc, sizeof(FSArchive));
	#if SDK_VERSION_MAJOR == 4
#if !defined(SDK_NO_THREAD)
	OS_InitThreadQueue(&p_arc->sync_q);
	OS_InitThreadQueue(&p_arc->stat_q);
#endif
	#elif SDK_VERSION_MAJOR == 5
	OS_InitThreadQueue(&p_arc->queue);
	#endif
}

FSArchive *FS_FindArchive (const char *name, int name_len)
{
	#if SDK_VERSION_MAJOR == 4
	u32 pack = FSi_GetPackedName(name, name_len);
	OSIntrMode bak_psr = OS_DisableInterrupts();
	FSArchive *p_arc = arc_list;

	while (p_arc && (p_arc->name.pack != pack))
		p_arc = p_arc->next;

	(void)OS_RestoreInterrupts(bak_psr);

	return p_arc;
	#elif SDK_VERSION_MAJOR == 5
  OSIntrMode bak_psr = OS_DisableInterrupts();
  FSArchive *arc = arc_list;
  for (; arc; arc = arc->next) {
    if (FS_IsArchiveLoaded(arc)) {
      const char *arcname = FS_GetArchiveName(arc);
      if ((STD_CompareNString(arcname, name, name_len) == 0) &&
          (arcname[name_len] == '\0')) {
        break;
      }
    }
  }
  (void)OS_RestoreInterrupts(bak_psr);
  return arc;
	#endif
}

#if SDK_VERSION_MAJOR == 5
FSResult FS_GetArchiveResultCode(const void *path_or_archive) {
  OSIntrMode bak_psr = OS_DisableInterrupts();
  FSArchive *arc = arc_list;
  while (arc && (arc != (const FSArchive *)path_or_archive)) {
    arc = arc->next;
  }
  if (!arc) {
    arc = FS_NormalizePath((const char *)path_or_archive, NULL, NULL);
  }
  (void)OS_RestoreInterrupts(bak_psr);
  return arc ? arc->result : FS_RESULT_ERROR;
}

const char *FS_GetCurrentDirectory(void) { return current_dir_path; }

BOOL FS_SetCurrentDirectory(const char *path) {
  BOOL retval = FALSE;
  FSArchive *arc = NULL;
  u32 baseid = 0;
  char relpath[FS_ENTRY_LONGNAME_MAX];

  SDK_NULL_ASSERT(path);
  SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);

  arc = FS_NormalizePath(path, &baseid, relpath);
  if (arc) {

    current_dir_pos.arc = arc;
    current_dir_pos.own_id = 0;
    current_dir_pos.index = 0;
    current_dir_pos.pos = 0;
    (void)STD_CopyLString(current_dir_path, relpath, sizeof(current_dir_path));

    if (arc->vtbl->FindPath != NULL) {
      FSFile dir[1];
      FSArgumentForFindPath arg[1];
      FS_InitFile(dir);
      dir->arc = arc;
      dir->argument = arg;
      arg->baseid = baseid;
      arg->relpath = relpath;
      arg->target_is_directory = TRUE;
      if (FSi_SendCommand(dir, FS_COMMAND_FINDPATH, TRUE)) {
        current_dir_pos.own_id = (u16)arg->target_id;
        (void)STD_CopyLString(current_dir_path, relpath,
                              sizeof(current_dir_path));
      }
    }
    retval = TRUE;
  }
  return retval;
}

static int FSi_CopySafeString(char *dst, int dstlen, const char *src,
                              int srclen, BOOL *stickyFailure) {
  int i;
  int n = (dstlen - 1 < srclen) ? (dstlen - 1) : srclen;
  for (i = 0; (i < n) && src[i]; ++i) {
    dst[i] = src[i];
  }
  if ((i < srclen) && src[i]) {
    *stickyFailure = TRUE;
  }
  dst[i] = '\0';
  return i;
}

FSArchive *FS_NormalizePath(const char *path, u32 *baseid, char *relpath) {
  FSArchive *arc = NULL;
  int pathlen = 0;
  int pathmax = FS_ENTRY_LONGNAME_MAX;
  BOOL stickyFailure = FALSE;

  if (current_dir_pos.arc == NULL) {
    current_dir_pos.arc = arc_list;
    current_dir_pos.own_id = 0;
    current_dir_pos.pos = 0;
    current_dir_pos.index = 0;
    current_dir_path[0] = '\0';
  }

  if (FSi_IsSlash((u8)*path)) {
    arc = current_dir_pos.arc;
    ++path;
    if (baseid) {
      *baseid = 0;
    }
  } else {
    int i;
    for (i = 0;; i = FSi_IncrementSjisPosition(path, i)) {
      u32 c = (u8)path[i];

      if (!c || FSi_IsSlash(c)) {
        arc = current_dir_pos.arc;
        if (baseid) {
          *baseid = current_dir_pos.own_id;
        }
        if (relpath) {

          if ((current_dir_pos.own_id == 0) && (current_dir_path[0] != '\0')) {
            pathlen += FSi_CopySafeString(
                &relpath[pathlen], pathmax - pathlen, current_dir_path,
                FS_ENTRY_LONGNAME_MAX, &stickyFailure);
            pathlen += FSi_CopySafeString(&relpath[pathlen], pathmax - pathlen,
                                          "/", 1, &stickyFailure);
          }
        }
        break;
      }

      else if (c == ':') {
        arc = FS_FindArchive(path, i);
        if (!arc) {
          OS_TWarning("archive \"%*s\" is not found.", i, path);
        }
        path += i + 1;
        if (FSi_IsSlash((u8)*path)) {
          ++path;
        }
        if (baseid) {
          *baseid = 0;
        }
        break;
      }
    }
  }
  if (relpath) {

    int curlen = 0;
    while (!stickyFailure) {
      char c = path[curlen];
      if ((c != '\0') && !FSi_IsSlash((u8)c)) {
        curlen += STD_IsSjisCharacter(&path[curlen]) ? 2 : 1;
      } else {

        if (curlen == 0) {
        }

        else if ((curlen == 1) && (path[0] == '.')) {
        }

        else if ((curlen == 2) && (path[0] == '.') && (path[1] == '.')) {
          if (pathlen > 0) {
            --pathlen;
          }
          pathlen = FSi_DecrementSjisPositionToSlash(relpath, pathlen) + 1;
        }

        else {
          pathlen += FSi_CopySafeString(&relpath[pathlen], pathmax - pathlen,
                                        path, curlen, &stickyFailure);
          if (c != '\0') {
            pathlen += FSi_CopySafeString(&relpath[pathlen], pathmax - pathlen,
                                          "/", 1, &stickyFailure);
          }
        }
        if (c == '\0') {
          break;
        }
        path += curlen + 1;
        curlen = 0;
      }
    }
    relpath[pathlen] = '\0';
    pathlen = FSi_TrimSjisTrailingSlash(relpath);
  }
  return stickyFailure ? NULL : arc;
}
#endif

BOOL FS_RegisterArchiveName (FSArchive *p_arc, const char *name, u32 name_len)
{
	#if SDK_VERSION_MAJOR == 4
	FS_ASSERT_INIT(FALSE);
	FS_ASSERT_ARG(p_arc && name, FALSE);
	FS_ASSERT_ARC_FREE(p_arc, FALSE);

	{
		BOOL ret = FALSE;
		OSIntrMode bak_intr = OS_DisableInterrupts();

		if (!FS_FindArchive(name, (s32)name_len)) {
			FSArchive *p_tail = arc_list;
			if (!p_tail) {
				arc_list = p_arc;
				current_dir_pos.arc = p_arc;
				current_dir_pos.pos = 0;
				current_dir_pos.index = 0;
				current_dir_pos.own_id = 0;
			} else {
				while (p_tail->next)
					p_tail = p_tail->next;

				p_tail->next = p_arc;
				p_arc->prev = p_tail;
			}

			p_arc->name.pack = FSi_GetPackedName(name, (s32)name_len);
			p_arc->flag |= FS_ARCHIVE_FLAG_REGISTER;

			ret = TRUE;
		}

		(void)OS_RestoreInterrupts(bak_intr);
		return ret;
	}
	#elif SDK_VERSION_MAJOR == 5
  BOOL retval = FALSE;

  SDK_ASSERT(FS_IsAvailable());
  SDK_NULL_ASSERT(p_arc);
  SDK_NULL_ASSERT(name);

  {
    OSIntrMode bak_intr = OS_DisableInterrupts();
    if (!FS_FindArchive(name, (s32)name_len)) {

      FSArchive **pp;
      for (pp = &arc_list; *pp; pp = &(*pp)->next) {
      }
      *pp = p_arc;

      if (name_len <= FS_ARCHIVE_NAME_LEN_MAX) {
        p_arc->name.pack = 0;
        (void)STD_CopyLString(p_arc->name.ptr, name, (int)(name_len + 1));
      } else {
#if defined(FS_SUPPORT_LONG_ARCNAME)

        if (name_len <= FS_LONG_ARCNAME_LENGTH_MAX) {
          int i;
          for (i = 0;; ++i) {
            if (i >= FS_LONG_ARCNAME_TABLE_MAX) {
              OS_TPanic(
                  "failed to allocate memory for long archive-name(%.*s)!",
                  name_len, name);
            } else if (FSiLongNameTable[i][0] == '\0') {
              (void)STD_CopyLString(FSiLongNameTable[i], name,
                                    (int)(name_len + 1));
              p_arc->name.pack = (u32)FSiLongNameTable[i];
              break;
            }
          }
        }
#endif

        else {
          OS_TPanic("too long archive-name(%.*s)!", name_len, name);
        }
      }
      p_arc->flag |= FS_ARCHIVE_FLAG_REGISTER;
      retval = TRUE;
    }
    (void)OS_RestoreInterrupts(bak_intr);
  }
  return retval;
	#endif
}

void FS_ReleaseArchiveName (FSArchive *p_arc)
{
	#if SDK_VERSION_MAJOR == 4
	FS_ASSERT_INIT(void);
	FS_ASSERT_ARG(p_arc, void);
	FS_ASSERT_ARC_NOT_ROM(p_arc, void);

	if (p_arc->name.pack) {
		OSIntrMode bak_psr = OS_DisableInterrupts();

		if (p_arc->next)
			p_arc->next->prev = p_arc->prev;

		if (p_arc->prev)
			p_arc->prev->next = p_arc->next;

		p_arc->name.pack = 0;
		p_arc->next = p_arc->prev = NULL;
		p_arc->flag &= ~FS_ARCHIVE_FLAG_REGISTER;

		if (current_dir_pos.arc == p_arc) {
			current_dir_pos.arc = arc_list;
			current_dir_pos.pos = 0;
			current_dir_pos.index = 0;
			current_dir_pos.own_id = 0;
		}

		(void)OS_RestoreInterrupts(bak_psr);
	}
	#elif SDK_VERSION_MAJOR == 5
  SDK_ASSERT(FS_IsAvailable());
  SDK_NULL_ASSERT(p_arc);

  if (p_arc == arc_list) {
    OS_TPanic("[file-system] cannot modify \"rom\" archive.\n");
  }

  if (p_arc->name.pack) {
    OSIntrMode bak_psr = OS_DisableInterrupts();

    FSArchive **pp;
    for (pp = &arc_list; *pp; pp = &(*pp)->next) {
      if (*pp == p_arc) {
        *pp = (*pp)->next;
        break;
      }
    }
#if defined(FS_SUPPORT_LONG_ARCNAME)

    if (p_arc->name.ptr[3] != '\0') {
      ((char *)p_arc->name.pack)[0] = '\0';
    }
#endif
    p_arc->name.pack = 0;
    p_arc->next = NULL;
    p_arc->flag &= ~FS_ARCHIVE_FLAG_REGISTER;

    if (current_dir_pos.arc == p_arc) {
      current_dir_pos.arc = NULL;
    }
    (void)OS_RestoreInterrupts(bak_psr);
  }
	#endif
}

#if SDK_VERSION_MAJOR == 5
const char *FS_GetArchiveName(const FSArchive *arc) {
#if defined(FS_SUPPORT_LONG_ARCNAME)
  return (arc->name.ptr[3] != '\0') ? (const char *)arc->name.pack
                                    : arc->name.ptr;
#else
  return arc->name.ptr;
#endif
}

BOOL FS_MountArchive(FSArchive *arc, void *userdata,
                     const FSArchiveInterface *vtbl, u32 reserved) {
  (void)reserved;
  SDK_ASSERT(FS_IsAvailable());
  SDK_NULL_ASSERT(arc);
  SDK_ASSERT(!FS_IsArchiveLoaded(arc));

  arc->userdata = userdata;
  arc->vtbl = vtbl;

  {
    FSFile tmp[1];
    FS_InitFile(tmp);
    tmp->arc = arc;
    (void)FSi_InvokeCommand(tmp, FS_COMMAND_MOUNT);
  }
  arc->flag |= FS_ARCHIVE_FLAG_LOADED;
  return TRUE;
}

BOOL FS_UnmountArchive(FSArchive *arc) {
  SDK_ASSERT(FS_IsAvailable());
  SDK_NULL_ASSERT(arc);

  {
    OSIntrMode bak_psr = OS_DisableInterrupts();

    if (FS_IsArchiveLoaded(arc)) {

      {
        BOOL bak_state = FS_SuspendArchive(arc);
        FSFile *file = arc->list;
        arc->flag |= FS_ARCHIVE_FLAG_UNLOADING;
        while (file) {
          FSFile *next = file->next;
          FSi_EndCommand(file, FS_RESULT_CANCELED);
          file = next;
        }
        arc->list = NULL;
        if (bak_state) {
          (void)FS_ResumeArchive(arc);
        }
      }

      {
        FSFile tmp[1];
        FS_InitFile(tmp);
        tmp->arc = arc;
        (void)FSi_InvokeCommand(tmp, FS_COMMAND_UNMOUNT);
      }
      arc->flag &= ~(FS_ARCHIVE_FLAG_CANCELING | FS_ARCHIVE_FLAG_LOADED |
                     FS_ARCHIVE_FLAG_UNLOADING);
    }
    (void)OS_RestoreInterrupts(bak_psr);
  }
  return TRUE;
}
#endif

#if SDK_VERSION_MAJOR == 4
BOOL FS_LoadArchive (FSArchive *p_arc, u32 base,
                     u32 fat, u32 fat_size,
                     u32 fnt, u32 fnt_size,
                     FS_ARCHIVE_READ_FUNC read_func, FS_ARCHIVE_WRITE_FUNC write_func)
{
	FS_ASSERT_INIT(FALSE);
	FS_ASSERT_ARG(p_arc, FALSE);
	FS_ASSERT_ARC_UNLOADED(p_arc, FALSE);

	p_arc->base = base;
	p_arc->fat_size = fat_size;
	p_arc->fat = p_arc->fat_bak = fat;
	p_arc->fnt_size = fnt_size;
	p_arc->fnt = p_arc->fnt_bak = fnt;

	p_arc->read_func = read_func ? read_func : FSi_ReadMemCallback;
	p_arc->write_func = write_func ? write_func : FSi_WriteMemCallback;

	p_arc->table_func = p_arc->read_func;
	p_arc->load_mem = NULL;

	p_arc->flag |= FS_ARCHIVE_FLAG_LOADED;
	return TRUE;
}

BOOL FS_UnloadArchive (FSArchive *p_arc)
{
	FS_ASSERT_INIT(FALSE);
	FS_ASSERT_ARG(p_arc, FALSE);

	{
		OSIntrMode bak_psr = OS_DisableInterrupts();

		if (FS_IsArchiveLoaded(p_arc)) {

			if (FS_IsArchiveTableLoaded(p_arc)) {
				OS_TWarning("memory may leak. preloaded-table of archive \"%s\" (0x%08X)",
				            p_arc->name.ptr, p_arc->load_mem);
			}
			{
				FSFile *p, *q;
				BOOL bak_state = FS_SuspendArchive(p_arc);

				p_arc->flag |= FS_ARCHIVE_FLAG_UNLOADING;

				for (p = p_arc->list.next; p; p = q) {
					q = p->link.next;
					FSi_ReleaseCommand(p, FS_RESULT_CANCELED);
				}

				p_arc->list.next = NULL;

				if (bak_state)
					(void)FS_ResumeArchive(p_arc);
			}

			p_arc->base = 0;
			p_arc->fat = 0;
			p_arc->fat_size = 0;
			p_arc->fnt = 0;
			p_arc->fnt_size = 0;
			p_arc->fat_bak = p_arc->fnt_bak = 0;
			p_arc->flag &= ~(FS_ARCHIVE_FLAG_CANCELING | FS_ARCHIVE_FLAG_LOADED | FS_ARCHIVE_FLAG_UNLOADING);
		}

		(void)OS_RestoreInterrupts(bak_psr);
	}

	return TRUE;
}

u32 FS_LoadArchiveTables (FSArchive *p_arc, void *p_mem, u32 max_size)
{
	FS_ASSERT_INIT(0);
	FS_ASSERT_ARG(p_arc, 0);

	{
		u32 total_size = ALIGN_BYTE(p_arc->fat_size + p_arc->fnt_size + 32, 32);

		if (total_size <= max_size) {
            #ifdef SDK_PORT
            u8 *p_cache = (u8*)p_mem;
            #else
			u8 *p_cache = (u8 *)ALIGN_BYTE((u32)p_mem, 32);
            #endif
			FSFile tmp;
			FS_InitFile(&tmp);

			if (FS_OpenFileDirect(&tmp, p_arc, p_arc->fat, p_arc->fat + p_arc->fat_size, (u32) ~0)) {
				if (FS_ReadFile(&tmp, p_cache, (s32)p_arc->fat_size) < 0) {
					MI_CpuFill8(p_cache, 0x00, p_arc->fat_size);
				}
				(void)FS_CloseFile(&tmp);
			}

			p_arc->fat = (u32)p_cache;
			p_cache += p_arc->fat_size;

			if (FS_OpenFileDirect(&tmp, p_arc, p_arc->fnt, p_arc->fnt + p_arc->fnt_size, (u32) ~0)) {
				if (FS_ReadFile(&tmp, p_cache, (s32)p_arc->fnt_size) < 0) {
					MI_CpuFill8(p_cache, 0x00, p_arc->fnt_size);
				}
				(void)FS_CloseFile(&tmp);
			}

			p_arc->fnt = (u32)p_cache;
			p_arc->load_mem = p_mem;
			p_arc->table_func = FSi_ReadMemoryCore;
			p_arc->flag |= FS_ARCHIVE_FLAG_TABLE_LOAD;
		}

		return total_size;
	}
}

void *FS_UnloadArchiveTables (FSArchive *p_arc)
{
	FS_ASSERT_INIT(0);
	FS_ASSERT_ARG(p_arc, 0);

	{
		void *ret = NULL;

		if (FS_IsArchiveLoaded(p_arc)) {
			BOOL bak_stat = FS_SuspendArchive(p_arc);

			if (FS_IsArchiveTableLoaded(p_arc)) {
				p_arc->flag &= ~FS_ARCHIVE_FLAG_TABLE_LOAD;
				ret = p_arc->load_mem;
				p_arc->load_mem = NULL;
				p_arc->fat = p_arc->fat_bak;
				p_arc->fnt = p_arc->fnt_bak;
				p_arc->table_func = p_arc->read_func;
			}

			if (bak_stat)
				(void)FS_ResumeArchive(p_arc);
		}

		return ret;
	}
}
#endif

BOOL FS_SuspendArchive (FSArchive *p_arc)
{
	FS_ASSERT_INIT(0);
	FS_ASSERT_ARG(p_arc, 0);

	{
		OSIntrMode bak_psr = OS_DisableInterrupts();
		const BOOL bak_stat = !FS_IsArchiveSuspended(p_arc);

		if (bak_stat) {
			#if SDK_VERSION_MAJOR == 4
			if (FSi_IsArchiveRunning(p_arc)) {
				p_arc->flag |= FS_ARCHIVE_FLAG_SUSPENDING;

				do
					OS_SleepThread(&p_arc->stat_q);
				while (FSi_IsArchiveSuspending(p_arc));
			} else {
				p_arc->flag |= FS_ARCHIVE_FLAG_SUSPEND;
			}
			#elif SDK_VERSION_MAJOR == 5
      			if ((p_arc->flag & FS_ARCHIVE_FLAG_RUNNING) == 0) {
      			  p_arc->flag |= FS_ARCHIVE_FLAG_SUSPEND;
      			} else {
      			  p_arc->flag |= FS_ARCHIVE_FLAG_SUSPENDING;
      			  FSi_WaitConditionOff(&p_arc->flag, FS_ARCHIVE_FLAG_SUSPENDING,
      			                       &p_arc->queue);
      			}
			#endif
		}
		(void)OS_RestoreInterrupts(bak_psr);
		return bak_stat;
	}
}

BOOL FS_ResumeArchive (FSArchive *p_arc)
{
	#if SDK_VERSION_MAJOR == 4
	FS_ASSERT_INIT(0);
	FS_ASSERT_ARG(p_arc, 0);
	{
		FSFile *p_target = NULL;

		OSIntrMode bak_irq = OS_DisableInterrupts();
		const BOOL bak_stat = !FS_IsArchiveSuspended(p_arc);

		if (!bak_stat) {
			p_arc->flag &= ~FS_ARCHIVE_FLAG_SUSPEND;
			p_target = FSi_NextCommand(p_arc);
		}

		(void)OS_RestoreInterrupts(bak_irq);

		if (p_target)
			FSi_ExecuteAsyncCommand(p_target);

		return bak_stat;
	}
	#elif SDK_VERSION_MAJOR == 5
  	BOOL retval;
  	SDK_ASSERT(FS_IsAvailable());
  	SDK_NULL_ASSERT(p_arc);
  	{
  	  OSIntrMode bak_irq = OS_DisableInterrupts();
  	  retval = !FS_IsArchiveSuspended(p_arc);
  	  if (!retval) {
  	    p_arc->flag &= ~FS_ARCHIVE_FLAG_SUSPEND;
  	  }
  	  (void)OS_RestoreInterrupts(bak_irq);
  	}
  	{
  	  FSFile *file = NULL;
  	  file = FSi_NextCommand(p_arc, TRUE);
  	  if (file) {
  	    FSi_ExecuteAsyncCommand(file);
  	  }
  	}
  	return retval;
	#endif
}

void FS_SetArchiveProc (struct FSArchive *p_arc, FS_ARCHIVE_PROC_FUNC proc, u32 flags)
{
	if (!flags)
		proc = NULL;

	else if (!proc)
		flags = 0;

	p_arc->proc = proc;
	p_arc->proc_flag = flags;
}

void FS_NotifyArchiveAsyncEnd (FSArchive *p_arc, FSResult ret)
{
	#if SDK_VERSION_MAJOR == 4
	if (FSi_IsArchiveAsync(p_arc)) {
		FSFile *p_file = p_arc->list.next;
		p_arc->flag &= ~FS_ARCHIVE_FLAG_IS_ASYNC;
		FSi_ReleaseCommand(p_file, ret);

		p_file = FSi_NextCommand(p_arc);

		if (p_file)
			FSi_ExecuteAsyncCommand(p_file);
	} else {
		FSFile *p_file = p_arc->list.next;
		OSIntrMode bak_psr = OS_DisableInterrupts();
		p_file->error = ret;
		p_arc->flag &= ~FS_ARCHIVE_FLAG_IS_SYNC;
#if !defined(SDK_NO_THREAD)
		OS_WakeupThread(&p_arc->sync_q);
#endif
		(void)OS_RestoreInterrupts(bak_psr);
	}
	#elif SDK_VERSION_MAJOR == 5
  FSFile *file = arc->list;
  if ((file->stat & FS_FILE_STATUS_BLOCKING) != 0) {
    OSIntrMode bak_psr = OS_DisableInterrupts();
    file->stat |= FS_FILE_STATUS_ASYNC_DONE;
    file->error = ret;
    OS_WakeupThread(file->queue);
    (void)OS_RestoreInterrupts(bak_psr);
  } else {
    FSi_EndCommand(file, ret);
    file = FSi_NextCommand(arc, TRUE);
    if (file) {
      FSi_ExecuteAsyncCommand(file);
    }
  }
	#endif
}

#if SDK_VERSION_MAJOR == 5
BOOL FS_WaitAsync(FSFile *file) {
  SDK_NULL_ASSERT(file);
  SDK_ASSERT(FS_IsAvailable());
  SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);

  {
    BOOL is_owner = FALSE;
    OSIntrMode bak_psr = OS_DisableInterrupts();
    if (FS_IsBusy(file)) {

      is_owner =
          !(file->stat & (FS_FILE_STATUS_BLOCKING | FS_FILE_STATUS_OPERATING));
      if (is_owner) {
        file->stat |= FS_FILE_STATUS_BLOCKING;
      }
    }
    (void)OS_RestoreInterrupts(bak_psr);
    if (is_owner) {
      FSi_ExecuteSyncCommand(file);
    } else {
      FSi_WaitConditionOff(&file->stat, FS_FILE_STATUS_BUSY, file->queue);
    }
  }

  return FS_IsSucceeded(file);
}
#endif /* SDK_VERSION_MAJOR */

#endif

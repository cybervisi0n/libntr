#if SDK_VERSION_MAJOR == 5
#include <nitro/types.h>
#include <nitro/misc.h>
#endif
#include <nitro/mi.h>
#include <nitro/os.h>
#include <nitro/pxi.h>
#if SDK_VERSION_MAJOR == 5
#include <nitro/std/string.h>
#include <nitro/std/unicode.h>
#include <nitro/math/math.h>
#endif
#include <nitro/fs.h>

#include "../include/rom.h"
#include "../include/util.h"
#include "../include/command.h"

#ifdef SDK_PORT
#include <string.h>
#include <SDL2/SDL.h>
#endif

#if SDK_VERSION_MAJOR == 5
#define FS_DEBUG_TRACE(...) (void)0
#endif

#if SDK_VERSION_MAJOR == 4
#if !defined(SDK_FINALROM)
const char *fsi_assert_fs_format = "[file-system] %s.\n";
const char *fsi_assert_is_init = "not initialized";
const char *fsi_assert_is_file = "invalid file-handle";
const char *fsi_assert_is_dir = "invalid directory-handle";
const char *fsi_assert_is_handle = "neither file-handle nor directory-handle";
const char *fsi_assert_is_idle = "specified file-handle is busy";
const char *fsi_assert_is_empty = "specified file-handle is already opened";
const char *fsi_assert_is_valid_arg = "invalid argument parameter";
const char *fsi_assert_is_valid_dma_channel = "cannot specify DMA channel 0";
const char *fsi_assert_irq_enabled = "not IRQ-enabled";
const char *fsi_assert_arc_free = "archive is still now registered";
const char *fsi_assert_arc_unloaded = "archive is still now loaded";
const char *fsi_assert_arc_not_rom = "cannot modify \"rom\" archive";
#endif

static BOOL is_init = FALSE;

void FS_Init (u32 default_dma_no)
{
	#if defined(FS_IMPLEMENT)
	FS_ASSERT_DMA_CHANNEL(default_dma_no, void);
	if (!is_init) {
		is_init = TRUE;
		FSi_InitRom(default_dma_no);
	}

	#else
	#pragma unused(default_dma_no)
	CARD_Init();
	#endif
}

BOOL FS_IsAvailable (void)
{
	return is_init;
}

void FS_End (void)
{
	OSIntrMode bak_psr = OS_DisableInterrupts();

	if (is_init) {
		#if defined(FS_IMPLEMENT)
		FSi_EndArchive();
		OS_ReleaseLockID((u16)fsi_card_lock_id);
		#endif
		is_init = FALSE;
	}

	(void)OS_RestoreInterrupts(bak_psr);
}
#endif

#if SDK_VERSION_MAJOR == 5
static BOOL FSi_IsValidTransferRegion(const void *buffer, s32 length) {
  BOOL retval = FALSE;
  if (buffer == NULL) {
    OS_TWarning("specified transfer buffer is NULL.\n");
  } else if (((u32)buffer >= HW_IOREG) && ((u32)buffer < HW_IOREG_END)) {
    OS_TWarning("specified transfer buffer is in I/O register %08X. (seems to "
                "be dangerous)\n",
                buffer);
  } else if (length < 0) {
    OS_TWarning("specified transfer size is minus. (%d)\n", length);
  } else {
#if !defined(SDK_TWL)
    s32 mainmem_size = HW_MAIN_MEM_EX_SIZE;
#else
    s32 mainmem_size =
        OS_IsRunOnTwl() ? HW_TWL_MAIN_MEM_EX_SIZE : HW_MAIN_MEM_EX_SIZE;
#endif
    if (length > mainmem_size) {
      OS_TWarning("specified transfer size is over mainmemory-size. (%d)\n",
                  length);
    } else {
      retval = TRUE;
    }
  }
  return retval;
}

int FSi_DecrementSjisPosition(const char *str, int pos) {

  int prev = --pos;

  for (; (prev > 0) && STD_IsSjisLeadByte(str[prev - 1]); --prev) {
  }

  return pos - ((pos - prev) & 1);
}

int FSi_IncrementSjisPositionToSlash(const char *str, int pos) {
  while (str[pos] && !FSi_IsSlash((u8)str[pos])) {
    pos = FSi_IncrementSjisPosition(str, pos);
  }
  return pos;
}

int FSi_DecrementSjisPositionToSlash(const char *str, int pos) {
  for (;;) {
    pos = FSi_DecrementSjisPosition(str, pos);
    if ((pos < 0) || FSi_IsSlash((u8)str[pos])) {
      break;
    }
  }
  return pos;
}

int FSi_TrimSjisTrailingSlash(char *str) {
  int length = STD_GetStringLength(str);
  int lastpos = FSi_DecrementSjisPosition(str, length);
  if ((lastpos >= 0) && FSi_IsSlash((u8)str[lastpos])) {
    length = lastpos;
    str[length] = '\0';
  }
  return length;
}

int FSi_DecrementUnicodePosition(const u16 *str, int pos) {

  int prev = --pos;

  if ((pos > 0) && ((str[pos - 1] >= 0xD800) && (str[pos - 1] <= 0xDC00)) &&
      ((str[pos - 0] >= 0xDC00) && (str[pos - 0] <= 0xE000))) {
    --pos;
  }
  return pos;
}

int FSi_DecrementUnicodePositionToSlash(const u16 *str, int pos) {
  for (;;) {
    pos = FSi_DecrementUnicodePosition(str, pos);
    if ((pos < 0) || FSi_IsUnicodeSlash(str[pos])) {
      break;
    }
  }
  return pos;
}
#endif

#if defined(FS_IMPLEMENT)
void FS_InitFile (FSFile *p_file)
{
	FS_ASSERT_ARG(p_file, void);

	#if SDK_VERSION_MAJOR == 4
	p_file->link.next = p_file->link.prev = NULL;
	p_file->command = FS_COMMAND_INVALID;
	#endif
	#if !defined(SDK_NO_THREAD)
	OS_InitThreadQueue(p_file->queue);
	#endif
	p_file->arc = NULL;
	p_file->stat = 0;
	#if SDK_VERSION_MAJOR == 5
    file->userdata = NULL;
    file->next = NULL;
    file->stat |= (FS_COMMAND_INVALID << FS_FILE_STATUS_CMD_SHIFT);
    file->argument = NULL;
    file->error = FS_RESULT_SUCCESS;
	#endif
}

#if SDK_VERSION_MAJOR == 4
static BOOL FSi_FindPath (FSFile *p_dir, const char *path, FSFileID *p_file_id, FSDirPos *p_dir_pos)
{
	FSDirPos pos;

	FS_ASSERT_ARG(p_dir && path, FALSE);
	FS_ASSERT_IRQ_ENABLED(-1);

	if (FSi_IsSlash(MI_ReadByte(path))) {

		pos.arc = current_dir_pos.arc;
		pos.own_id = 0;
		pos.pos = 0;
		pos.index = 0;
		++path;
	} else {
		int i;

		pos = current_dir_pos;
		for (i = 0; i <= FS_ARCHIVE_NAME_LEN_MAX; ++i) {
			u32 c = MI_ReadByte(path + i);
			if (!c || FSi_IsSlash(c))
				break;
			else if (c == ':') {
				FSArchive *const p_arc = FS_FindArchive(path, i);
				if (!p_arc) {
					OS_Warning("[file-system] " "archive \"%*s\" is not found.", i, path);
					return FALSE;
				} else if (!FS_IsArchiveLoaded(p_arc))   {
					OS_Warning("[file-system] "
					"archive \"%*s\" is registered, but not loaded yet.", i, path);
					return FALSE;
				}

				pos.arc = p_arc;
				pos.pos = 0;
				pos.index = 0;
				pos.own_id = 0;
				path += i + 1;

				if (FSi_IsSlash(MI_ReadByte(path)))
					++path;
				break;
			}
		}
	}

	p_dir->arc = pos.arc;
	p_dir->arg.findpath.path = path;
	p_dir->arg.findpath.pos = pos;

	if (p_dir_pos) {
		p_dir->arg.findpath.find_directory = TRUE;
		p_dir->arg.findpath.result.dir = p_dir_pos;
	} else {
		p_dir->arg.findpath.find_directory = FALSE;
		p_dir->arg.findpath.result.file = p_file_id;
	}

	return FSi_SendCommand(p_dir, FS_COMMAND_FINDPATH);
}

static s32 FSi_ReadFileCore (FSFile *p_file, void *dst, s32 len, BOOL async)
{
	FS_ASSERT_INIT(-1);
	FS_ASSERT_ARG(p_file && dst && (len >= 0), -1);
	FS_ASSERT_FILE(p_file, -1);
	FS_ASSERT_IDLE(p_file, -1);

	{
		#ifdef SDK_PORT
		s32 old_len;
		old_len = len;
		#endif
		const s32 pos = (s32)p_file->prop.file.pos;
		const s32 rest = (s32)p_file->prop.file.bottom - pos;
		const u32 org = (u32)len;

		if (len > rest)
			len = rest;
		if (len < 0)
			len = 0;

		p_file->arg.readfile.dst = dst;
		p_file->arg.readfile.len_org = org;
		p_file->arg.readfile.len = (u32)(len);
		#ifdef SDK_PORT
		if( p_file->pcFilePtr != NULL ){
			return fread( dst, 1, old_len, p_file->pcFilePtr );
		}
		#endif

		if (!async)
			p_file->stat |= FS_FILE_STATUS_SYNC;

		#ifdef SDK_BUILD_ARM
		(void)FSi_SendCommand(p_file, FS_COMMAND_READFILE);
		#endif

		if (!async) {
			if (FS_WaitAsync(p_file))
				len = (s32)p_file->prop.file.pos - pos;
			else
				len = -1;
		}
	}
	return len;
}

static s32 FSi_WriteFileCore (FSFile *p_file, const void *src, s32 len, BOOL async)
{
	FS_ASSERT_INIT(-1);
	FS_ASSERT_ARG(p_file && src && (len >= 0), -1);
	FS_ASSERT_FILE(p_file, -1);
	FS_ASSERT_IDLE(p_file, -1);

	{
		const s32 pos = (s32)p_file->prop.file.pos;
		const s32 rest = (s32)p_file->prop.file.bottom - pos;
		const u32 org = (u32)len;

		if (len > rest)
			len = rest;
		if (len < 0)
			len = 0;

		p_file->arg.writefile.src = src;
		p_file->arg.writefile.len_org = org;
		p_file->arg.writefile.len = (u32)(len);
		#ifdef SDK_PORT
		if( p_file->pcFilePtr != NULL ){
			return fwrite( src, 1, len, p_file->pcFilePtr );
		}
		#endif

		if (!async)
			p_file->stat |= FS_FILE_STATUS_SYNC;

		(void)FSi_SendCommand(p_file, FS_COMMAND_WRITEFILE);

		if (!async) {
			if (FS_WaitAsync(p_file))
				len = (s32)p_file->prop.file.pos - pos;
			else
				len = -1;
		}
	}

	return len;
}
#endif

BOOL FS_ConvertPathToFileID (FSFileID *p_file_id, const char *path)
{
	#if SDK_VERSION_MAJOR == 4
	FS_ASSERT_INIT(FALSE);
	FS_ASSERT_ARG(p_file_id && path, FALSE);
	FS_ASSERT_IRQ_ENABLED(FALSE);

	{
		FSFile dir;
		FS_InitFile(&dir);

		if (!FSi_FindPath(&dir, path, p_file_id, NULL))
			return FALSE;
	}

	return TRUE;
	#elif SDK_VERSION_MAJOR == 5
  	BOOL retval = FALSE;
  	SDK_NULL_ASSERT(p_fileid);
  	SDK_NULL_ASSERT(path);
  	SDK_ASSERT(FS_IsAvailable());
  	SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
  	{
  	  char relpath[FS_ARCHIVE_FULLPATH_MAX + 1];
  	  u32 baseid = 0;
  	  FSArchive *arc = FS_NormalizePath(path, &baseid, relpath);
  	  if (arc) {
  	    FSFile file[1];
  	    FSArgumentForFindPath arg[1];
  	    FS_InitFile(file);
  	    file->arc = arc;
  	    file->argument = arg;
  	    arg->baseid = baseid;
  	    arg->relpath = relpath;
  	    arg->target_is_directory = FALSE;
  	    if (FSi_SendCommand(file, FS_COMMAND_FINDPATH, TRUE)) {
  	      p_fileid->arc = arc;
  	      p_fileid->file_id = arg->target_id;
  	      retval = TRUE;
  	    }
  	  }
  	}
  	return retval;
	#endif
}

BOOL FS_OpenFileDirect (FSFile *p_file, FSArchive *p_arc, u32 image_top, u32 image_bottom, u32 file_index)
{
	#if SDK_VERSION_MAJOR == 4
	FS_ASSERT_INIT(FALSE);
	FS_ASSERT_ARG(p_file && p_arc, FALSE);
	FS_ASSERT_EMPTY(p_file, FALSE);
	FS_ASSERT_IRQ_ENABLED(FALSE);

	{
		p_file->arc = p_arc;
		p_file->arg.openfiledirect.index = file_index;
		p_file->arg.openfiledirect.top = image_top;
		p_file->arg.openfiledirect.bottom = image_bottom;

		#ifdef SDK_BUILD_ARM
		if (!FSi_SendCommand(p_file, FS_COMMAND_OPENFILEDIRECT))
			return FALSE;
		#endif

		p_file->stat |= FS_FILE_STATUS_IS_FILE;
		p_file->stat &= ~FS_FILE_STATUS_IS_DIR;
	}
	#elif SDK_VERSION_MAJOR == 5
  	BOOL retval = FALSE;
  	SDK_NULL_ASSERT(file);
  	SDK_NULL_ASSERT(arc);
  	SDK_ASSERT(FS_IsAvailable());
  	SDK_ASSERT(!FS_IsFile(file));
  	SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
  	{
  	  FSArgumentForOpenFileDirect arg[1];
  	  file->arc = arc;
  	  file->argument = arg;
  	  arg->id = id;
  	  arg->top = image_top;
  	  arg->bottom = image_bottom;
  	  arg->mode = 0;
  	  retval = FSi_SendCommand(file, FS_COMMAND_OPENFILEDIRECT, TRUE);
  	}
	#endif

	#ifdef SDK_PORT
	const char* romName = "rom";

	p_file->pcFilePtr = NULL;
	if( strcmp( romName, p_file->arc->name.ptr) == 0 )
	{
		p_file->pcFilePtr = fopen("header.bin", "rb");
		if( p_file->pcFilePtr == NULL )
		{
			return FALSE;
		}
	}
	#endif

	#if SDK_VERSION_MAJOR == 4
	return TRUE;
	#elif SDK_VERSION_MAJOR == 5
	return retVal;
	#endif
}

BOOL FS_OpenFileFast (FSFile *p_file, FSFileID file_id)
{
	#if SDK_VERSION_MAJOR == 4
	FS_ASSERT_INIT(FALSE);
	FS_ASSERT_ARG(p_file, FALSE);
	FS_ASSERT_EMPTY(p_file, FALSE);
	FS_ASSERT_IRQ_ENABLED(FALSE);

	{
		if (!file_id.arc)
			return FALSE;

		p_file->arc = file_id.arc;
		p_file->arg.openfilefast.id = file_id;

		if (!FSi_SendCommand(p_file, FS_COMMAND_OPENFILEFAST))
			return FALSE;

		p_file->stat |= FS_FILE_STATUS_IS_FILE;
		p_file->stat &= ~FS_FILE_STATUS_IS_DIR;
	}

	return TRUE;
	#elif SDK_VERSION_MAJOR == 5
  	BOOL retval = FALSE;
  	SDK_NULL_ASSERT(file);
  	SDK_ASSERT(FS_IsAvailable());
  	SDK_ASSERT(!FS_IsFile(file));
  	SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
  	if (id.arc) {
  	  FSArgumentForOpenFileFast arg[1];
  	  file->arc = id.arc;
  	  file->argument = arg;
  	  arg->id = id.file_id;
  	  arg->mode = 0;
  	  retval = FSi_SendCommand(file, FS_COMMAND_OPENFILEFAST, TRUE);
  	}
  	return retval;
	#endif
}

BOOL FS_OpenFile (FSFile *p_file, const char *path)
{
	#if SDK_VERSION_MAJOR == 5
	return FS_OpenFileEx(file, path, FS_FILEMODE_R);
	#endif
	FSFileID id;
	#ifdef SDK_PORT
	if(path[0] == 'r'
		&& path[1] == 'o'
	&& path[2] == 'm'
	&& path[3] == ':'
	&& strlen(path) > 4) {
		path = path+4;
	}
	if( path[0] == '/')
	{
		path = path + 1;
	}
	p_file->pcFilePtr = NULL;
	p_file->pcFilePtr = fopen(path, "rb");
	if(p_file->pcFilePtr == NULL) {
		// Try opening with lowercase path
		char * lowercasePath = malloc(sizeof(char) * strlen(path) + 1);
		memset(lowercasePath, 0, sizeof(char) * strlen(path) + 1);
		strcpy(lowercasePath, path);
		for(int i=0; i < strlen(path); i++) {
			lowercasePath[i] = tolower(path[i]);
		}
		p_file->pcFilePtr = fopen(lowercasePath, "rb");
		free(lowercasePath);
	}
	if( p_file->pcFilePtr != NULL )
	{
		p_file->stat |= FS_FILE_STATUS_IS_FILE;
		p_file->stat &= ~FS_FILE_STATUS_IS_DIR;
		u32 fileSize;
		fseek(p_file->pcFilePtr, 0, SEEK_END);
		fileSize = ftell(p_file->pcFilePtr);
		fseek(p_file->pcFilePtr, 0, SEEK_SET);
		p_file->prop.file.top = 0;
		p_file->prop.file.bottom = fileSize;
		return TRUE;
	}
	else
	{
        char messageBuf[200] = {0};
        snprintf(messageBuf, 199, "Failed to open file '%s'. Please extract your ROM into the same directory as the executable.", path);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "FS Error", messageBuf, NULL);
        return FALSE;
	}
	#else
	return (FS_ConvertPathToFileID(&id, path) && FS_OpenFileFast(p_file, id));
	#endif
}

#if SDK_VERSION_MAJOR == 5
BOOL FS_OpenFileEx(FSFile *file, const char *path, u32 mode) {
  BOOL retval = FALSE;
  FS_DEBUG_TRACE("%s\n", __FUNCTION__);
  SDK_NULL_ASSERT(file);
  SDK_NULL_ASSERT(path);
  SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);

  if (((mode & FS_FILEMODE_L) != 0) &&
      ((mode & FS_FILEMODE_RW) == FS_FILEMODE_W)) {
    OS_TWarning(
        "\"FS_FILEMODE_WL\" seems useless.\n"
        "(this means creating empty file and prohibiting any modifications)");
  }
  {
    char relpath[FS_ARCHIVE_FULLPATH_MAX + 1];
    u32 baseid = 0;
    FSArchive *arc = FS_NormalizePath(path, &baseid, relpath);
    if (arc) {
      FSArgumentForOpenFile arg[1];
      FS_InitFile(file);
      file->arc = arc;
      file->argument = arg;
      arg->baseid = baseid;
      arg->relpath = relpath;
      arg->mode = mode;
      if (FSi_SendCommand(file, FS_COMMAND_OPENFILE, TRUE)) {
        retval = TRUE;
      } else {
        file->arc = NULL;
      }
    }
  }
  return retval;
}
#endif

BOOL FS_CloseFile (FSFile *p_file)
{
	#if SDK_VERSION_MAJOR == 4
	FS_ASSERT_INIT(FALSE);
	FS_ASSERT_ARG(p_file, FALSE);
	FS_ASSERT_FILE(p_file, FALSE);
	FS_ASSERT_IRQ_ENABLED(FALSE);
	#elif SDK_VERSION_MAJOR == 5
  	BOOL retval = FALSE;
  	FS_DEBUG_TRACE("%s\n", __FUNCTION__);
  	SDK_NULL_ASSERT(file);
  	SDK_ASSERT(FS_IsAvailable());
  	SDK_ASSERT(FS_IsFile(file));
  	SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
	#endif

	#ifdef SDK_PORT
	p_file->arc = NULL;
	p_file->command = FS_COMMAND_INVALID;
	p_file->stat &= ~(FS_FILE_STATUS_IS_FILE | FS_FILE_STATUS_IS_DIR);
	if( p_file->pcFilePtr != NULL )
	{
		if( fclose( p_file->pcFilePtr ) == 0 )
		{
			return TRUE;
		}else{
			return FALSE;
		}
	} else {
		return TRUE;
	}
	#endif

	#if SDK_VERSION_MAJOR == 4
	if (!FSi_SendCommand(p_file, FS_COMMAND_CLOSEFILE))
		return FALSE;

	p_file->arc = NULL;
	p_file->command = FS_COMMAND_INVALID;
	p_file->stat &= ~(FS_FILE_STATUS_IS_FILE | FS_FILE_STATUS_IS_DIR);

	return TRUE;
	#elif SDK_VERSION_MAJOR == 5
  	{
  	  	retval = FSi_SendCommand(file, FS_COMMAND_CLOSEFILE, TRUE);
  	}
  	return retval;
	#endif
}

#if SDK_VERSION_MAJOR == 5
u32 FS_GetSeekCacheSize(const char *path) {
  u32 retval = 0;

  FSPathInfo info;
  if (FS_GetPathInfo(path, &info) &&
      ((info.attributes & FS_ATTRIBUTE_IS_DIRECTORY) == 0)) {

    FSArchiveResource resource;
    if (FS_GetArchiveResource(path, &resource)) {

      u32 bytesPerCluster =
          resource.sectorsPerCluster * resource.bytesPerSector;
      if (bytesPerCluster != 0) {
        static const u32 fatBits = 32;
        retval =
            (u32)((info.filesize + bytesPerCluster - 1) / bytesPerCluster) *
            ((fatBits + 4) / 8);

        retval += (u32)(HW_CACHE_LINE_SIZE * 2);
      }
    }
  }
  return retval;
}

BOOL FS_SetSeekCache(FSFile *file, void *buf, u32 buf_size) {
  FSArgumentForSetSeekCache arg[1];
  BOOL retval = FALSE;
  SDK_ASSERT(FS_IsAvailable());
  SDK_ASSERT(FS_IsFile(file));

  file->argument = arg;
  arg->buf = buf;
  arg->buf_size = buf_size;
  retval = FSi_SendCommand(file, FS_COMMAND_SETSEEKCACHE, TRUE);

  return retval;
}

u32 FS_GetFileLength(FSFile *file) {
  u32 retval = 0;
  SDK_ASSERT(FS_IsAvailable());
  SDK_ASSERT(FS_IsFile(file));

  if (!FSi_GetFileLengthIfProc(file, &retval)) {
    FSArgumentForGetFileLength arg[1];
    file->argument = arg;
    arg->length = 0;
    if (FSi_SendCommand(file, FS_COMMAND_GETFILELENGTH, TRUE)) {
      retval = arg->length;
    }
  }
  return retval;
}

u32 FS_GetFilePosition(FSFile *file) {
  u32 retval = 0;
  SDK_ASSERT(FS_IsAvailable());
  SDK_ASSERT(FS_IsFile(file));

  if (!FSi_GetFilePositionIfProc(file, &retval)) {
    FSArgumentForGetFilePosition arg[1];
    file->argument = arg;
    arg->position = 0;
    if (FSi_SendCommand(file, FS_COMMAND_GETFILEPOSITION, TRUE)) {
      retval = arg->position;
    }
  }
  return retval;
}
#endif

BOOL FS_GetPathName (FSFile *p_file, char *buf, u32 len)
{
	#if SDK_VERSION_MAJOR == 4
	FS_ASSERT_INIT(FALSE);
	FS_ASSERT_HANDLE(p_file, FALSE);
	FS_ASSERT_IRQ_ENABLED(FALSE);
	#elif SDK_VERSION_MAJOR == 5
	BOOL retval = FALSE;
  	SDK_ASSERT(FS_IsAvailable());
  	SDK_ASSERT(FS_IsFile(file) || FS_IsDir(file));
  	SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
	#endif

	#if SDK_VERSION_MAJOR == 4
	if (p_file->command != FS_COMMAND_GETPATH) {
		p_file->arg.getpath.total_len = 0;
		p_file->arg.getpath.dir_id = 0;
	}

	p_file->arg.getpath.buf = (u8 *)buf;
	p_file->arg.getpath.buf_len = len;

	return FSi_SendCommand(p_file, FS_COMMAND_GETPATH);
	#elif SDK_VERSION_MAJOR == 5
  	{
  	  FSArgumentForGetPath arg[1];
  	  file->argument = arg;
  	  arg->is_directory = FS_IsDir(file);
  	  arg->buffer = buffer;
  	  arg->length = length;
  	  retval = FSi_SendCommand(file, FS_COMMAND_GETPATH, TRUE);
  	}
  	return retval;
	#endif
}

s32 FS_GetPathLength (FSFile *p_file)
{
	return FS_GetPathName(p_file, NULL, 0) ? p_file->arg.getpath.total_len : -1;
}

BOOL FS_WaitAsync (FSFile *p_file)
{
	FS_ASSERT_INIT(FALSE);
	FS_ASSERT_ARG(p_file, FALSE);
	FS_ASSERT_IRQ_ENABLED(-1);

	#if !defined(SDK_NO_THREAD)
	{
		BOOL is_owner = FALSE;
		OSIntrMode bak_psr = OS_DisableInterrupts();

		if (FS_IsBusy(p_file)) {
			is_owner = !(p_file->stat & (FS_FILE_STATUS_SYNC | FS_FILE_STATUS_OPERATING));
			if (is_owner) {
				p_file->stat |= FS_FILE_STATUS_SYNC;
				do {
					OS_SleepThread(p_file->queue);
				} while (!(p_file->stat & FS_FILE_STATUS_OPERATING));
			} else {
				do {
					OS_SleepThread(p_file->queue);
				} while (FS_IsBusy(p_file));
			}
		}
		(void)OS_RestoreInterrupts(bak_psr);

		if (is_owner) {
			return FSi_ExecuteSyncCommand(p_file);
		}
	}
	#else
	while (FS_IsBusy(p_file))
		;
	#endif

	return FS_IsSucceeded(p_file);
}

void FS_CancelFile (FSFile *p_file)
{
	FS_ASSERT_INIT(FALSE);
	FS_ASSERT_ARG(p_file, FALSE);

	{
		OSIntrMode bak_psr = OS_DisableInterrupts();

		if (FS_IsBusy(p_file)) {
			p_file->stat |= FS_FILE_STATUS_CANCEL;
			p_file->arc->flag |= FS_ARCHIVE_FLAG_CANCELING;
		}

		(void)OS_RestoreInterrupts(bak_psr);
	}
}

#if SDK_VERSION_MAJOR == 5
BOOL FS_CreateFile(const char *path, u32 permit) {
  BOOL retval = FALSE;
  FS_DEBUG_TRACE("%s(%s)\n", __FUNCTION__, path);
  SDK_NULL_ASSERT(path);
  SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
  {
    char relpath[FS_ARCHIVE_FULLPATH_MAX + 1];
    u32 baseid = 0;
    FSArchive *arc = FS_NormalizePath(path, &baseid, relpath);
    if (arc) {
      FSFile file[1];
      FSArgumentForCreateFile arg[1];
      FS_InitFile(file);
      file->arc = arc;
      file->argument = arg;
      arg->baseid = baseid;
      arg->relpath = relpath;
      arg->permit = permit;
      retval = FSi_SendCommand(file, FS_COMMAND_CREATEFILE, TRUE);
    }
  }
  return retval;
}

BOOL FS_DeleteFile(const char *path) {
  BOOL retval = FALSE;
  FS_DEBUG_TRACE("%s(%s)\n", __FUNCTION__, path);
  SDK_NULL_ASSERT(path);
  SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
  {
    char relpath[FS_ARCHIVE_FULLPATH_MAX + 1];
    u32 baseid = 0;
    FSArchive *arc = FS_NormalizePath(path, &baseid, relpath);
    if (arc) {
      FSFile file[1];
      FSArgumentForDeleteFile arg[1];
      FS_InitFile(file);
      file->arc = arc;
      file->argument = arg;
      arg->baseid = baseid;
      arg->relpath = relpath;
      retval = FSi_SendCommand(file, FS_COMMAND_DELETEFILE, TRUE);
    }
  }
  return retval;
}

BOOL FS_RenameFile(const char *src, const char *dst) {
  BOOL retval = FALSE;
  FS_DEBUG_TRACE("%s(%s->%s)\n", __FUNCTION__, src, dst);
  SDK_NULL_ASSERT(src);
  SDK_NULL_ASSERT(dst);
  SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
  {
    char relpath_src[FS_ARCHIVE_FULLPATH_MAX + 1];
    char relpath_dst[FS_ARCHIVE_FULLPATH_MAX + 1];
    u32 baseid_src = 0;
    u32 baseid_dst = 0;
    FSArchive *arc_src = FS_NormalizePath(src, &baseid_src, relpath_src);
    FSArchive *arc_dst = FS_NormalizePath(dst, &baseid_dst, relpath_dst);
    if (arc_src != arc_dst) {
      OS_TWarning("cannot rename between defferent archives.\n");
    } else {
      FSFile file[1];
      FSArgumentForRenameFile arg[1];
      FS_InitFile(file);
      file->arc = arc_src;
      file->argument = arg;
      arg->baseid_src = baseid_src;
      arg->relpath_src = relpath_src;
      arg->baseid_dst = baseid_dst;
      arg->relpath_dst = relpath_dst;
      retval = FSi_SendCommand(file, FS_COMMAND_RENAMEFILE, TRUE);
    }
  }
  return retval;
}

BOOL FS_GetPathInfo(const char *path, FSPathInfo *info) {
  BOOL retval = FALSE;
  FS_DEBUG_TRACE("%s\n", __FUNCTION__);
  SDK_NULL_ASSERT(path);
  SDK_NULL_ASSERT(info);
  SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
  {
    char relpath[FS_ARCHIVE_FULLPATH_MAX + 1];
    u32 baseid = 0;
    FSArchive *arc = FS_NormalizePath(path, &baseid, relpath);
    if (arc) {
      FSFile file[1];
      FSArgumentForGetPathInfo arg[1];
      FS_InitFile(file);
      file->arc = arc;
      file->argument = arg;
      arg->baseid = baseid;
      arg->relpath = relpath;
      arg->info = info;
      retval = FSi_SendCommand(file, FS_COMMAND_GETPATHINFO, TRUE);
    }
  }
  return retval;
}

BOOL FS_SetPathInfo(const char *path, const FSPathInfo *info) {
  BOOL retval = FALSE;
  FS_DEBUG_TRACE("%s\n", __FUNCTION__);
  SDK_NULL_ASSERT(path);
  SDK_NULL_ASSERT(info);
  SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
  {
    char relpath[FS_ARCHIVE_FULLPATH_MAX + 1];
    u32 baseid = 0;
    FSArchive *arc = FS_NormalizePath(path, &baseid, relpath);
    if (arc) {
      FSFile file[1];
      FSArgumentForSetPathInfo arg[1];
      FS_InitFile(file);
      file->arc = arc;
      file->argument = arg;
      arg->baseid = baseid;
      arg->relpath = relpath;
      arg->info = (FSPathInfo *)
          info; // To clear FATFS_PROPERTY_CTRL_MASK in info->attributes
      retval = FSi_SendCommand(file, FS_COMMAND_SETPATHINFO, TRUE);
    }
  }
  return retval;
}

BOOL FS_CreateDirectory(const char *path, u32 permit) {
  BOOL retval = FALSE;
  FS_DEBUG_TRACE("%s(%s)\n", __FUNCTION__, path);
  SDK_NULL_ASSERT(path);
  SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
  {
    char relpath[FS_ARCHIVE_FULLPATH_MAX + 1];
    u32 baseid = 0;
    FSArchive *arc = FS_NormalizePath(path, &baseid, relpath);
    if (arc) {
      FSFile file[1];
      FSArgumentForCreateDirectory arg[1];
      FS_InitFile(file);
      file->arc = arc;
      file->argument = arg;
      arg->baseid = baseid;
      arg->relpath = relpath;
      arg->permit = permit;
      retval = FSi_SendCommand(file, FS_COMMAND_CREATEDIRECTORY, TRUE);
    }
  }
  return retval;
}

BOOL FS_DeleteDirectory(const char *path) {
  BOOL retval = FALSE;
  FS_DEBUG_TRACE("%s(%s)\n", __FUNCTION__, path);
  SDK_NULL_ASSERT(path);
  SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
  {
    char relpath[FS_ARCHIVE_FULLPATH_MAX + 1];
    u32 baseid = 0;
    FSArchive *arc = FS_NormalizePath(path, &baseid, relpath);
    if (arc) {
      FSFile file[1];
      FSArgumentForDeleteDirectory arg[1];
      FS_InitFile(file);
      file->arc = arc;
      file->argument = arg;
      arg->baseid = baseid;
      arg->relpath = relpath;
      retval = FSi_SendCommand(file, FS_COMMAND_DELETEDIRECTORY, TRUE);
    }
  }
  return retval;
}

BOOL FS_RenameDirectory(const char *src, const char *dst) {
  BOOL retval = FALSE;
  FS_DEBUG_TRACE("%s(%s->%s)\n", __FUNCTION__, src, dst);
  SDK_NULL_ASSERT(src);
  SDK_NULL_ASSERT(dst);
  SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
  {
    char relpath_src[FS_ARCHIVE_FULLPATH_MAX + 1];
    char relpath_dst[FS_ARCHIVE_FULLPATH_MAX + 1];
    u32 baseid_src = 0;
    u32 baseid_dst = 0;
    FSArchive *arc_src = FS_NormalizePath(src, &baseid_src, relpath_src);
    FSArchive *arc_dst = FS_NormalizePath(dst, &baseid_dst, relpath_dst);
    if (arc_src != arc_dst) {
      OS_TWarning("cannot rename between defferent archives.\n");
    } else {
      FSFile file[1];
      FSArgumentForRenameDirectory arg[1];
      FS_InitFile(file);
      file->arc = arc_src;
      file->argument = arg;
      arg->baseid_src = baseid_src;
      arg->relpath_src = relpath_src;
      arg->baseid_dst = baseid_dst;
      arg->relpath_dst = relpath_dst;
      retval = FSi_SendCommand(file, FS_COMMAND_RENAMEDIRECTORY, TRUE);
    }
  }
  return retval;
}

static BOOL FSi_GetFullPath(char *dst, const char *path) {
  FSArchive *arc = FS_NormalizePath(path, NULL, dst);
  if (arc) {
    const char *arcname = FS_GetArchiveName(arc);
    int m = STD_GetStringLength(arcname);
    int n = STD_GetStringLength(dst);
    (void)STD_MoveMemory(&dst[m + 2], &dst[0], (u32)n + 1);
    (void)STD_MoveMemory(&dst[0], arcname, (u32)m);
    dst[m + 0] = ':';
    dst[m + 1] = '/';
  }
  return (arc != NULL);
}

static BOOL FSi_ComplementDirectory(const char *path, char *autogen) {
  BOOL retval = FALSE;
  int root = 0;

  char *tmppath = autogen;
  if (FSi_GetFullPath(tmppath, path)) {
    int length = STD_GetStringLength(tmppath);
    if (length > 0) {
      int pos = 0;
      FS_DEBUG_TRACE("  trying to complete \"%s\"\n", tmppath);

      length = FSi_TrimSjisTrailingSlash(tmppath);

      length = FSi_DecrementSjisPositionToSlash(tmppath, length);

      for (pos = length; pos >= 0;) {
        FSPathInfo info[1];
        BOOL exists;
        tmppath[pos] = '\0';
        exists = FS_GetPathInfo(tmppath, info);
        FS_DEBUG_TRACE("    - \"%s\" is%s existent (result:%d)\n", tmppath,
                       exists ? "" : " not", FS_GetArchiveResultCode(tmppath));
        tmppath[pos] = '/';

        if (!exists) {
          pos = FSi_DecrementSjisPositionToSlash(tmppath, pos);
        }

        else {

          if ((info->attributes & FS_ATTRIBUTE_IS_DIRECTORY) == 0) {
            pos = -1;
          }

          else {
            ++pos;
          }
          break;
        }
      }

      if (pos >= 0) {
        for (;;) {

          if (pos >= length) {
            retval = TRUE;
            break;
          } else {
            pos = FSi_IncrementSjisPositionToSlash(tmppath, pos);
            tmppath[pos] = '\0';
            if (!FS_CreateDirectory(tmppath, FS_PERMIT_R | FS_PERMIT_W)) {
              break;
            } else {

              if (root == 0) {
                FS_DEBUG_TRACE("    - we have created \"%s\" as root\n",
                               tmppath);
                root = pos;
              }
              tmppath[pos++] = '/';
            }
          }
        }
      }
    }
  }

  autogen[root] = '\0';
  return retval;
}

BOOL FS_CreateFileAuto(const char *path, u32 permit) {
  BOOL result = FALSE;
  char autogen[FS_ARCHIVE_FULLPATH_MAX + 1];
  FS_DEBUG_TRACE("%s(%s)\n", __FUNCTION__, path);
  if (FSi_ComplementDirectory(path, autogen)) {
    result = FS_CreateFile(path, permit);
    if (!result) {
      (void)FS_DeleteDirectoryAuto(autogen);
    }
  }
  return result;
}

BOOL FS_DeleteFileAuto(const char *path) {
  FS_DEBUG_TRACE("%s(%s)\n", __FUNCTION__, path);

  return FS_DeleteFile(path);
}

BOOL FS_RenameFileAuto(const char *src, const char *dst) {
  BOOL result = FALSE;
  char autogen[FS_ARCHIVE_FULLPATH_MAX + 1];
  FS_DEBUG_TRACE("%s(%s->%s)\n", __FUNCTION__);
  if (FSi_ComplementDirectory(dst, autogen)) {
    result = FS_RenameFile(src, dst);
    if (!result) {
      (void)FS_DeleteDirectoryAuto(autogen);
    }
  }
  return result;
}

BOOL FS_CreateDirectoryAuto(const char *path, u32 permit) {
  BOOL result = FALSE;
  char autogen[FS_ARCHIVE_FULLPATH_MAX + 1];
  FS_DEBUG_TRACE("%s(%s)\n", __FUNCTION__, path);
  if (FSi_ComplementDirectory(path, autogen)) {
    result = FS_CreateDirectory(path, permit);
    if (!result) {
      (void)FS_DeleteDirectoryAuto(autogen);
    }
  }
  return result;
}

BOOL FS_DeleteDirectoryAuto(const char *path) {
  BOOL retval = FALSE;
  FS_DEBUG_TRACE("%s(%s)\n", __FUNCTION__, path);
  if (path && *path) {
    char tmppath[FS_ARCHIVE_FULLPATH_MAX + 1];
    if (FSi_GetFullPath(tmppath, path)) {
      int pos;
      BOOL mayBeEmpty;
      int length = FSi_TrimSjisTrailingSlash(tmppath);
      FS_DEBUG_TRACE("  trying to force-delete \"%s\"\n", tmppath);
      mayBeEmpty = TRUE;
      for (pos = 0; pos >= 0;) {
        BOOL failure = FALSE;

        tmppath[length + pos] = '\0';
        if (mayBeEmpty &&
            (FS_DeleteDirectory(tmppath) ||
             (FS_GetArchiveResultCode(tmppath) == FS_RESULT_ALREADY_DONE))) {
          FS_DEBUG_TRACE("  -> succeeded to delete \"%s\"\n", tmppath);
          pos = FSi_DecrementSjisPositionToSlash(&tmppath[length], pos);
        } else {

          FSFile dir[1];
          FS_InitFile(dir);
          if (!FS_OpenDirectory(dir, tmppath, FS_FILEMODE_R)) {
            FS_DEBUG_TRACE("  -> failed to delete & open \"%s\"\n", tmppath);
            failure = TRUE;
          } else {
            FSDirectoryEntryInfo info[1];
            tmppath[length + pos] = '/';
            mayBeEmpty = TRUE;
            while (FS_ReadDirectory(dir, info)) {
              (void)STD_CopyString(&tmppath[length + pos + 1], info->longname);

              if ((info->attributes & FS_ATTRIBUTE_IS_DIRECTORY) == 0) {
                if (!FS_DeleteFile(tmppath)) {
                  FS_DEBUG_TRACE("  -> failed to delete file \"%s\"\n",
                                 tmppath);
                  failure = TRUE;
                  break;
                }
                FS_DEBUG_TRACE("  -> succeeded to delete \"%s\"\n", tmppath);
              }

              else if ((STD_CompareString(info->longname, ".") == 0) ||
                       (STD_CompareString(info->longname, "..") == 0)) {
              }

              else if (!FS_DeleteDirectory(tmppath)) {
                pos += 1 + STD_GetStringLength(info->longname);
                mayBeEmpty = FALSE;
                break;
              }
            }
            (void)FS_CloseDirectory(dir);
          }
        }

        if (failure) {
          break;
        }
      }
      retval = (pos < 0);
    }
  }
  return retval;
}

BOOL FS_RenameDirectoryAuto(const char *src, const char *dst) {
  BOOL result = FALSE;
  char autogen[FS_ARCHIVE_FULLPATH_MAX + 1];
  FS_DEBUG_TRACE("%s(%s->%s)\n", __FUNCTION__, src, dst);
  if (FSi_ComplementDirectory(dst, autogen)) {
    result = FS_RenameDirectory(src, dst);
    if (!result) {
      (void)FS_DeleteDirectoryAuto(autogen);
    }
  }
  return result;
}

BOOL FS_GetArchiveResource(const char *path, FSArchiveResource *resource) {
  BOOL retval = FALSE;
  SDK_NULL_ASSERT(path);
  SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
  {
    FSArchive *arc = FS_NormalizePath(path, NULL, NULL);
    if (arc) {
      FSFile file[1];
      FSArgumentForGetArchiveResource arg[1];
      FS_InitFile(file);
      file->arc = arc;
      file->argument = arg;
      arg->resource = resource;
      retval = FSi_SendCommand(file, FS_COMMAND_GETARCHIVERESOURCE, TRUE);
    }
  }
  return retval;
}

u32 FSi_GetSpaceToCreateDirectoryEntries(const char *path,
                                         u32 bytesPerCluster) {
  static const u32 bytesPerEntry = 32UL;
  static const u32 longnamePerEntry = 13UL;

  const char *root = STD_SearchString(path, ":");
  const char *current = (root != NULL) ? (root + 1) : path;
  u32 totalBytes = 0;
  u32 restBytesInCluster = 0;
  current += (*current == '/');
  while (*current) {
    BOOL isShortName = FALSE;
    u32 entries = 0;

    u32 len = (u32)FSi_IncrementSjisPositionToSlash(current, 0);

#if 0


        {
            static const char  *alnum = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
            static const char  *special = "!#$%&'()*+-<>?@^_`{}~";
            if ((len <= 8 + 1 + 3) && STD_SearchChar(alnum, current[0]))
            {
                u32     namelen = 0;
                u32     extlen = 0;
                u32     scanned = 0;
                for (; namelen < len; ++namelen)
                {
                    char    c = current[scanned + namelen];
                    if (!STD_SearchChar(alnum, c) && !STD_SearchChar(special, c))
                    {
                        break;
                    }
                }
                scanned += namelen;
                if ((scanned < len) && (current[scanned] == '.'))
                {
                    ++scanned;
                    for (; scanned + extlen < len; ++extlen)
                    {
                        char    c = current[scanned + extlen];
                        if (!STD_SearchChar(alnum, c) && !STD_SearchChar(special, c))
                        {
                            break;
                        }
                    }
                    scanned += extlen;
                }
                if ((scanned == len) && (namelen <= 8) && (extlen <= 3))
                {
                    isShortName = TRUE;
                }
            }
        }
#endif

    if (!isShortName) {
      entries += ((len + longnamePerEntry - 1UL) / longnamePerEntry);
    }

    entries += 1;
    current += len;

    {
      int over = (int)(entries * bytesPerEntry - restBytesInCluster);
      if (over > 0) {
        totalBytes += MATH_ROUNDUP(over, bytesPerCluster);
      }
    }

    if (*current != '\0') {
      current += 1;
      totalBytes += bytesPerCluster;
      restBytesInCluster = bytesPerCluster - (2 * bytesPerEntry);
    }
  }
  return totalBytes;
}

BOOL FS_HasEnoughSpaceToCreateFile(FSArchiveResource *resource,
                                   const char *path, u32 size) {
  BOOL retval = FALSE;
  u32 bytesPerCluster = resource->bytesPerSector * resource->sectorsPerCluster;
  if (bytesPerCluster != 0) {
    u32 needbytes =
        (FSi_GetSpaceToCreateDirectoryEntries(path, bytesPerCluster) +
         MATH_ROUNDUP(size, bytesPerCluster));
    u32 needclusters = needbytes / bytesPerCluster;
    if (needclusters <= resource->availableClusters) {
      resource->availableClusters -= needclusters;
      resource->availableSize -= needbytes;
      retval = TRUE;
    }
  }
  return retval;
}

BOOL FS_IsArchiveReady(const char *path) {
  FSArchiveResource resource[1];
  return FS_GetArchiveResource(path, resource);
}

FSResult FS_FlushFile(FSFile *file) {
  FSResult retval = FS_RESULT_ERROR;
  SDK_NULL_ASSERT(file);
  SDK_ASSERT(FS_IsFile(file));
  SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
  {
    (void)FSi_SendCommand(file, FS_COMMAND_FLUSHFILE, TRUE);
    retval = FS_GetResultCode(file);
  }
  return retval;
}

FSResult FS_SetFileLength(FSFile *file, u32 length) {
  FSResult retval = FS_RESULT_ERROR;
  SDK_NULL_ASSERT(file);
  SDK_ASSERT(FS_IsFile(file));
  SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
  {
    FSArgumentForSetFileLength arg[1];
    file->argument = arg;
    arg->length = length;
    (void)FSi_SendCommand(file, FS_COMMAND_SETFILELENGTH, TRUE);
    retval = FS_GetResultCode(file);
  }
  return retval;
}
#endif

s32 FS_ReadFileAsync (FSFile *p_file, void *dst, s32 len)
{
	#if SDK_VERSION_MAJOR == 4
	return FSi_ReadFileCore(p_file, dst, len, TRUE);
	#elif SDK_VERSION_MAJOR == 5
  	FS_DEBUG_TRACE("%s\n", __FUNCTION__);
  	SDK_NULL_ASSERT(p_file);
  	SDK_ASSERT(FSi_IsValidTransferRegion(dst, len));
  	SDK_ASSERT(FS_IsAvailable());
  	SDK_ASSERT(FS_IsFile(p_file) && !FS_IsBusy(p_file));

  	{
  	  u32 end, pos;
  	  if (FSi_GetFilePositionIfProc(p_file, &pos) &&
  	      FSi_GetFileLengthIfProc(p_file, &end) && (pos + len > end)) {
  	    len = (s32)(end - pos);
  	  }
  	}
  	{
  	  FSArgumentForReadFile *arg = (FSArgumentForReadFile *)p_file->reserved2;
  	  p_file->argument = arg;
  	  arg->buffer = dst;
  	  arg->length = (u32)len;
  	  (void)FSi_SendCommand(p_file, FS_COMMAND_READFILE, FALSE);
  	}
  	return len;
	#endif
}

s32 FS_ReadFile (FSFile *p_file, void *dst, s32 len)
{
	#if SDK_VERSION_MAJOR == 4
	FS_ASSERT_IRQ_ENABLED(-1);
	return FSi_ReadFileCore(p_file, dst, len, FALSE);
	#elif SDK_VERSION_MAJOR == 5
  	FS_DEBUG_TRACE("%s\n", __FUNCTION__);
  	SDK_NULL_ASSERT(file);
  	SDK_ASSERT(FSi_IsValidTransferRegion(buffer, length));
  	SDK_ASSERT(FS_IsAvailable());
  	SDK_ASSERT(FS_IsFile(file) && !FS_IsBusy(file));
  	SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
  	{
  	  FSArgumentForReadFile arg[1];
  	  p_file->argument = arg;
  	  arg->buffer = dst;
  	  arg->length = (u32)len;
  	  if (FSi_SendCommand(file, FS_COMMAND_READFILE, TRUE)) {
  	    len = (s32)arg->length;
  	  } else {
  	    if ((p_file->error == FS_RESULT_INVALID_PARAMETER) ||
  	        (p_file->error == FS_RESULT_ERROR)) {
  	      len = -1; // If not read at all
  	    } else {
  	      len = (s32)arg->length; // If reading was tried, a value higher than
  	                                 // -1 is entered
  	    }
  	  }
  	}
  	return len;
	#endif
}

s32 FS_WriteFileAsync (FSFile *p_file, const void *src, s32 len)
{
	#if SDK_VERSION_MAJOR == 4
	return FSi_WriteFileCore(p_file, src, len, TRUE);
	#elif SDK_VERSION_MAJOR == 5
  	SDK_NULL_ASSERT(p_file);
  	SDK_ASSERT(FSi_IsValidTransferRegion(src, len));
  	SDK_ASSERT(FS_IsAvailable());
  	SDK_ASSERT(FS_IsFile(p_file) && !FS_IsBusy(p_file));

  	{
  	  u32 end, pos;
  	  if (FSi_GetFilePositionIfProc(p_file, &pos) &&
  	      FSi_GetFileLengthIfProc(p_file, &end) && (pos + len > end)) {
  	    len = (s32)(end - pos);
  	  }
  	}
  	{
  	  FSArgumentForWriteFile *arg = (FSArgumentForWriteFile *)p_file->reserved2;
  	  p_file->argument = arg;
  	  arg->buffer = src;
  	  arg->length = (u32)len;
  	  (void)FSi_SendCommand(p_file, FS_COMMAND_WRITEFILE, FALSE);
  	}
  	return len;
  	#endif
}

s32 FS_WriteFile (FSFile *p_file, const void *src, s32 len)
{
	#if SDK_VERSION_MAJOR == 4
	FS_ASSERT_IRQ_ENABLED(-1);
	return FSi_WriteFileCore(p_file, src, len, FALSE);
	#elif SDK_VERSION_MAJOR == 5
  	FS_DEBUG_TRACE("%s\n", __FUNCTION__);
  	SDK_NULL_ASSERT(p_file);
  	SDK_ASSERT(FSi_IsValidTransferRegion(src, len));
  	SDK_ASSERT(FS_IsAvailable());
  	SDK_ASSERT(FS_IsFile(p_file) && !FS_IsBusy(p_file));
  	SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
  	{
  	  FSArgumentForWriteFile arg[1];
  	  p_file->argument = arg;
  	  arg->buffer = src;
  	  arg->length = (u32)len;
  	  if (FSi_SendCommand(p_file, FS_COMMAND_WRITEFILE, TRUE)) {
  	    len = (s32)arg->length;
  	  } else {
  	    if (p_file->error == FS_RESULT_INVALID_PARAMETER) {
  	      len = -1; // If not written at all
  	    } else {
  	      len = (s32)arg->length; // If writing was tried, a value higher than
  	                                 // -1 is entered
  	    }
  	  }
  	}
  	return len;
	#endif
}

BOOL FS_SeekFile (FSFile *p_file, s32 offset, FSSeekFileMode origin)
{
	#if SDK_VERSION_MAJOR == 4
	#ifdef SDK_BUILD_ARM
	FS_ASSERT_INIT(FALSE);
	FS_ASSERT_ARG(p_file, FALSE);
	FS_ASSERT_FILE(p_file, FALSE);
	#endif
	#elif SDK_VERSION_MAJOR == 5
  	BOOL retval = FALSE;
  	FS_DEBUG_TRACE("%s\n", __FUNCTION__);
  	SDK_NULL_ASSERT(file);
  	SDK_ASSERT(FS_IsAvailable());
  	SDK_ASSERT(FS_IsFile(file));
	#endif

	#ifdef SDK_PORT
	s32 temp_offset;
	temp_offset = offset;
	#endif

	#if SDK_VERSION_MAJOR == 4
	{
		switch (origin) {
			case FS_SEEK_SET:
				offset += p_file->prop.file.top;
				break;
			case FS_SEEK_CUR:
				offset += p_file->prop.file.pos;
				break;
			case FS_SEEK_END:
				offset += p_file->prop.file.bottom;
				break;
			default:
				FS_ASSERT_ARG(FALSE, FALSE);
				return FALSE;
		}

		if (offset < (s32)p_file->prop.file.top)
			offset = (s32)p_file->prop.file.top;

		if (offset > (s32)p_file->prop.file.bottom)
			offset = (s32)p_file->prop.file.bottom;

		p_file->prop.file.pos = (u32)offset;
	}
	#elif SDK_VERSION_MAJOR == 5
  	if (!(retval = FSi_SeekFileIfProc(file, offset, origin))) {
  	  FSArgumentForSeekFile arg[1];
  	  file->argument = arg;
  	  arg->offset = (int)offset;
  	  arg->from = origin;
  	  retval = FSi_SendCommand(file, FS_COMMAND_SEEKFILE, TRUE);
  	}
	#endif

	#ifdef SDK_PORT
	if( p_file->pcFilePtr != NULL )
	{
		switch(origin)
		{
			case FS_SEEK_SET:
				fseek(p_file->pcFilePtr, temp_offset, SEEK_SET);
				break;
			case FS_SEEK_END:
				fseek(p_file->pcFilePtr, temp_offset, SEEK_END);
				break;
			case FS_SEEK_CUR:
				fseek(p_file->pcFilePtr, temp_offset, SEEK_CUR);
				break;
			default:
				FS_ASSERT_ARG(FALSE, FALSE);
				return FALSE;
				break;
		}
	}
	#endif

	#if SDK_VERSION_MAJOR == 4
	return TRUE;
	#elif SDK_VERSION_MAJOR == 5
	return retVal;
	#endif
}

#if SDK_VERSION_MAJOR == 5
BOOL FS_OpenDirectory(FSFile *file, const char *path, u32 mode) {
  BOOL retval = FALSE;
  FS_DEBUG_TRACE("%s\n", __FUNCTION__);
  SDK_NULL_ASSERT(path);
  SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
  {
    char relpath[FS_ARCHIVE_FULLPATH_MAX + 1];
    u32 baseid = 0;
    FSArchive *arc = FS_NormalizePath(path, &baseid, relpath);
    if (arc) {
      FSArgumentForOpenDirectory arg[1];
      FS_InitFile(file);
      file->arc = arc;
      file->argument = arg;
      arg->baseid = baseid;
      arg->relpath = relpath;
      arg->mode = mode;
      if (FSi_SendCommand(file, FS_COMMAND_OPENDIRECTORY, TRUE)) {
        retval = TRUE;
      } else {
        file->arc = NULL;
      }
    }
  }
  return retval;
}

BOOL FS_CloseDirectory(FSFile *file) {
  BOOL retval = FALSE;
  FS_DEBUG_TRACE("%s\n", __FUNCTION__);
  SDK_NULL_ASSERT(file);
  SDK_ASSERT(FS_IsAvailable());
  SDK_ASSERT(FS_IsDir(file));
  SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
  {
    if (FSi_SendCommand(file, FS_COMMAND_CLOSEDIRECTORY, TRUE)) {
      retval = TRUE;
    }
  }
  return retval;
}

BOOL FS_ReadDirectory(FSFile *file, FSDirectoryEntryInfo *info) {
  BOOL retval = FALSE;
  SDK_NULL_ASSERT(file);
  SDK_NULL_ASSERT(info);
  SDK_ASSERT(FS_IsAvailable());
  SDK_ASSERT(FS_IsDir(file));
  SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
  {
    FSArgumentForReadDirectory arg[1];
    file->argument = arg;
    arg->info = info;
    MI_CpuFill8(info, 0x00, sizeof(info));
    info->id = FS_INVALID_FILE_ID;
    if (FSi_SendCommand(file, FS_COMMAND_READDIR, TRUE)) {
      retval = TRUE;
    }
  }
  return retval;
}
#endif

BOOL FS_SeekDir (FSFile *p_dir, const FSDirPos *p_pos)
{
	#if SDK_VERSION_MAJOR == 4
	FS_ASSERT_INIT(FALSE);
	FS_ASSERT_ARG(p_dir && p_pos->arc && p_pos, FALSE);
	FS_ASSERT_IRQ_ENABLED(-1);

	p_dir->arc = p_pos->arc;
	p_dir->arg.seekdir.pos = *p_pos;

	if (!FSi_SendCommand(p_dir, FS_COMMAND_SEEKDIR))
		return FALSE;

	p_dir->stat |= FS_FILE_STATUS_IS_DIR;
	return TRUE;
	#elif SDK_VERSION_MAJOR == 5
  	BOOL retval = FALSE;
  	SDK_NULL_ASSERT(file);
  	SDK_NULL_ASSERT(pos);
  	SDK_NULL_ASSERT(pos->arc);
  	SDK_ASSERT(FS_IsAvailable());
  	SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
  	{
  	  FSArgumentForSeekDirectory arg[1];
  	  arg->id = (u32)((pos->own_id << 0) | (pos->index << 16));
  	  arg->position = pos->pos;
  	  file->arc = pos->arc;
  	  file->argument = arg;
  	  if (FSi_SendCommand(file, FS_COMMAND_SEEKDIR, TRUE)) {
  	    file->stat |= FS_FILE_STATUS_IS_DIR;
  	    retval = TRUE;
  	  }
  	}
  	return retval;
  	#endif
}

BOOL FS_ReadDir (FSFile *p_dir, FSDirEntry *p_entry)
{
	#if SDK_VERSION_MAJOR == 4
	FS_ASSERT_INIT(FALSE);
	FS_ASSERT_ARG(p_dir && p_entry, FALSE);
	FS_ASSERT_DIR(p_dir, FALSE);
	FS_ASSERT_IRQ_ENABLED(-1);

	p_dir->arg.readdir.p_entry = p_entry;
	p_dir->arg.readdir.skip_string = FALSE;

	return FSi_SendCommand(p_dir, FS_COMMAND_READDIR);
	#elif SDK_VERSION_MAJOR == 5
  	BOOL retval = FALSE;
  	FSDirectoryEntryInfo info[1];
  	if (FS_ReadDirectory(file, info)) {
  	  FSi_ConvertToDirEntry(entry, FS_GetAttachedArchive(file), info);
  	  retval = TRUE;
  	}
  	return retval;
	#endif
}

BOOL FS_FindDir (FSFile *p_dir, const char *path)
{
	#if SDK_VERSION_MAJOR == 4
	FSDirPos pos;

	if (!FSi_FindPath(p_dir, path, NULL, &pos))
		return FALSE;

	return FS_SeekDir(p_dir, &pos);
	#elif SDK_VERSION_MAJOR == 5
	return FS_OpenDirectory(dir, path, FS_FILEMODE_R);
	#endif
}

BOOL FS_ChangeDir (const char *path)
{
	#if SDK_VERSION_MAJOR == 4
	FSDirPos pos;
	FSFile dir;

	FS_InitFile(&dir);

	if (!FSi_FindPath(&dir, path, NULL, &pos))
		return FALSE;

	current_dir_pos = pos;
	return TRUE;
	#elif SDK_VERSION_MAJOR == 5
	return FS_SetCurrentDirectory(path);
	#endif
}

BOOL FS_TellDir (const FSFile *p_dir, FSDirPos *p_pos)
{
	#if SDK_VERSION_MAJOR == 4
	FS_ASSERT_INIT(FALSE);
	FS_ASSERT_ARG(p_dir && p_pos, FALSE);
	FS_ASSERT_DIR(p_dir, FALSE);

	{
		*p_pos = p_dir->prop.dir.pos;
		return TRUE;
	}
	#elif SDK_VERSION_MAJOR == 5
  	BOOL retval = FALSE;
  	SDK_NULL_ASSERT(dir);
  	SDK_NULL_ASSERT(pos);
  	SDK_ASSERT(FS_IsAvailable());
  	SDK_ASSERT(FS_IsDir(dir));
  	{
  	  *pos = dir->prop.dir.pos;
  	  retval = TRUE;
  	}
  	return retval;
	#endif
}

BOOL FS_RewindDir (FSFile *p_dir)
{
	FS_ASSERT_INIT(FALSE);
	FS_ASSERT_ARG(p_dir, FALSE);
	FS_ASSERT_DIR(p_dir, FALSE);
	FS_ASSERT_IRQ_ENABLED(-1);

	{
		FSDirPos pos;

		pos.arc = p_dir->arc;
		pos.own_id = p_dir->prop.dir.pos.own_id;
		pos.pos = 0;
		pos.index = 0;

		return FS_SeekDir(p_dir, &pos);
	}
}


#if SDK_VERSION_MAJOR == 5

enum {
  FS_UNICODE_CONVSRC_ASCII,
  FS_UNICODE_CONVSRC_SHIFT_JIS,
  FS_UNICODE_CONVSRC_UNICODE
};

static int FSi_CopySafeUnicodeString(u16 *dst, int dstlen, const void *srcptr,
                                     int srclen, int srctype,
                                     BOOL *stickyFailure) {
  int srcpos = 0;
  int dstpos = 0;
  if (srctype == FS_UNICODE_CONVSRC_ASCII) {
    const char *src = (const char *)srcptr;
    int n = (dstlen - 1 < srclen) ? (dstlen - 1) : srclen;
    while ((dstpos < n) && src[srcpos]) {
      dst[dstpos++] = (u8)src[srcpos++];
    }
    if ((srcpos < srclen) && src[srcpos]) {
      *stickyFailure = TRUE;
    }
  } else if (srctype == FS_UNICODE_CONVSRC_UNICODE) {
    const u16 *src = (const u16 *)srcptr;
    int n = (dstlen - 1 < srclen) ? (dstlen - 1) : srclen;
    while ((dstpos < n) && src[srcpos]) {
      dst[dstpos++] = src[srcpos++];
    }
    if ((srcpos < srclen) && src[srcpos]) {
      *stickyFailure = TRUE;
    }
  } else if (srctype == FS_UNICODE_CONVSRC_SHIFT_JIS) {
    const char *src = (const char *)srcptr;
    srcpos = srclen;
    dstpos = dstlen - 1;
    (void)FSi_ConvertStringSjisToUnicode(dst, &dstpos, src, &srcpos, NULL);
    if ((srcpos < srclen) && src[srcpos]) {
      *stickyFailure = TRUE;
    }
  }
  dst[dstpos] = L'\0';
  return dstpos;
}

FSArchive *FSi_NormalizePathWtoW(const u16 *path, u32 *baseid, u16 *relpath);
FSArchive *FSi_NormalizePathWtoW(const u16 *path, u32 *baseid, u16 *relpath) {
  FSArchive *arc = NULL;
  int pathlen = 0;
  int pathmax = FS_ARCHIVE_FULLPATH_MAX + 1;
  BOOL stickyFailure = FALSE;

  BOOL absolute = FALSE;
  int arcnameLen;
  for (arcnameLen = 0; arcnameLen < FS_ARCHIVE_NAME_LONG_MAX + 1;
       ++arcnameLen) {
    if (path[arcnameLen] == L'\0') {
      break;
    } else if (FSi_IsUnicodeSlash(path[arcnameLen])) {
      break;
    } else if (path[arcnameLen] == L':') {
      char arcname[FS_ARCHIVE_NAME_LONG_MAX + 1];
      int j;
      for (j = 0; j < arcnameLen; ++j) {
        arcname[j] = (char)path[j];
      }
      arcname[arcnameLen] = '\0';
      arc = FS_FindArchive(arcname, arcnameLen);
      break;
    }
  }
  if (arc) {
    absolute = TRUE;
    *baseid = 0;
  } else {
    arc = FS_NormalizePath("", baseid, NULL);
  }
  if (arc) {

    u32 caps = 0;
    (void)arc->vtbl->GetArchiveCaps(arc, &caps);
    if ((caps & FS_ARCHIVE_CAPS_UNICODE) == 0) {
      arc = NULL;
    } else {

      pathlen += FSi_CopySafeUnicodeString(
          &relpath[pathlen], pathmax - pathlen, FS_GetArchiveName(arc),
          FS_ARCHIVE_NAME_LONG_MAX, FS_UNICODE_CONVSRC_ASCII, &stickyFailure);
      pathlen += FSi_CopySafeUnicodeString(&relpath[pathlen], pathmax - pathlen,
                                           L":", 1, FS_UNICODE_CONVSRC_UNICODE,
                                           &stickyFailure);

      if (absolute) {
        path += arcnameLen + 1 + FSi_IsUnicodeSlash(path[arcnameLen + 1]);
      }

      else if (FSi_IsUnicodeSlash(*path)) {
        path += 1;
      }

      else {
        pathlen += FSi_CopySafeUnicodeString(
            &relpath[pathlen], pathmax - pathlen, L"/", 1,
            FS_UNICODE_CONVSRC_UNICODE, &stickyFailure);
        pathlen += FSi_CopySafeUnicodeString(
            &relpath[pathlen], pathmax - pathlen, FS_GetCurrentDirectory(),
            FS_ENTRY_LONGNAME_MAX, FS_UNICODE_CONVSRC_SHIFT_JIS,
            &stickyFailure);
      }

      pathlen += FSi_CopySafeUnicodeString(&relpath[pathlen], pathmax - pathlen,
                                           L"/", 1, FS_UNICODE_CONVSRC_UNICODE,
                                           &stickyFailure);
      {

        int curlen = 0;
        while (!stickyFailure) {
          u16 c = path[curlen];
          if ((c != L'\0') && !FSi_IsUnicodeSlash(c)) {
            curlen += 1;
          } else {

            if (curlen == 0) {
            }

            else if ((curlen == 1) && (path[0] == L'.')) {
            }

            else if ((curlen == 2) && (path[0] == '.') && (path[1] == '.')) {
              if ((pathlen > 2) && (relpath[pathlen - 2] != L':')) {
                --pathlen;
                pathlen =
                    FSi_DecrementUnicodePositionToSlash(relpath, pathlen) + 1;
              }
            }

            else {
              pathlen += FSi_CopySafeUnicodeString(
                  &relpath[pathlen], pathmax - pathlen, path, curlen,
                  FS_UNICODE_CONVSRC_UNICODE, &stickyFailure);
              if (c != L'\0') {
                pathlen += FSi_CopySafeUnicodeString(
                    &relpath[pathlen], pathmax - pathlen, L"/", 1,
                    FS_UNICODE_CONVSRC_UNICODE, &stickyFailure);
              }
            }
            if (c == L'\0') {
              break;
            }
            path += curlen + 1;
            curlen = 0;
          }
        }
      }
      relpath[pathlen] = L'\0';
    }
  }
  return stickyFailure ? NULL : arc;
}

BOOL FS_OpenFileExW(FSFile *file, const u16 *path, u32 mode) {
  BOOL retval = FALSE;
  FS_DEBUG_TRACE("%s\n", __FUNCTION__);
  SDK_NULL_ASSERT(file);
  SDK_NULL_ASSERT(path);
  SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);

  if (((mode & FS_FILEMODE_L) != 0) &&
      ((mode & FS_FILEMODE_RW) == FS_FILEMODE_W)) {
    OS_TWarning(
        "\"FS_FILEMODE_WL\" seems useless.\n"
        "(this means creating empty file and prohibiting any modifications)");
  }
  {
    u16 relpath[FS_ARCHIVE_FULLPATH_MAX + 1];
    u32 baseid = 0;
    FSArchive *arc = FSi_NormalizePathWtoW(path, &baseid, relpath);

    if (!arc) {
      file->error = FS_RESULT_UNSUPPORTED;
    } else {
      FSArgumentForOpenFile arg[1];
      FS_InitFile(file);
      file->arc = arc;
      file->argument = arg;
      arg->baseid = baseid;
      arg->relpath = (char *)relpath;
      arg->mode = mode;
      file->stat |= FS_FILE_STATUS_UNICODE_MODE;
      if (FSi_SendCommand(file, FS_COMMAND_OPENFILE, TRUE)) {
        retval = TRUE;
      } else {
        file->arc = NULL;
      }
    }
  }
  return retval;
}

BOOL FS_OpenDirectoryW(FSFile *file, const u16 *path, u32 mode) {
  BOOL retval = FALSE;
  FS_DEBUG_TRACE("%s\n", __FUNCTION__);
  SDK_NULL_ASSERT(path);
  SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
  {
    u16 relpath[FS_ARCHIVE_FULLPATH_MAX + 1];
    u32 baseid = 0;
    FSArchive *arc = FSi_NormalizePathWtoW(path, &baseid, relpath);

    if (!arc) {
      file->error = FS_RESULT_UNSUPPORTED;
    } else {
      FSArgumentForOpenDirectory arg[1];
      FS_InitFile(file);
      file->arc = arc;
      file->argument = arg;
      arg->baseid = baseid;
      arg->relpath = (char *)relpath;
      arg->mode = mode;
      file->stat |= FS_FILE_STATUS_UNICODE_MODE;
      if (FSi_SendCommand(file, FS_COMMAND_OPENDIRECTORY, TRUE)) {
        retval = TRUE;
      } else {
        file->arc = NULL;
      }
    }
  }
  return retval;
}

BOOL FS_ReadDirectoryW(FSFile *file, FSDirectoryEntryInfoW *info) {
  BOOL retval = FALSE;
  SDK_NULL_ASSERT(file);
  SDK_NULL_ASSERT(info);
  SDK_ASSERT(FS_IsAvailable());
  SDK_ASSERT(FS_IsDir(file));
  SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
  {
    FSArchive *arc = file->arc;

    u32 caps = 0;
    (void)arc->vtbl->GetArchiveCaps(arc, &caps);
    if ((caps & FS_ARCHIVE_CAPS_UNICODE) == 0) {
      file->error = FS_RESULT_UNSUPPORTED;
    } else {
      FSArgumentForReadDirectory arg[1];
      file->argument = arg;
      arg->info = (FSDirectoryEntryInfo *)info;
      MI_CpuFill8(info, 0x00, sizeof(info));
      info->id = FS_INVALID_FILE_ID;
      file->stat |= FS_FILE_STATUS_UNICODE_MODE;
      if (FSi_SendCommand(file, FS_COMMAND_READDIR, TRUE)) {
        retval = TRUE;
      }
    }
  }
  return retval;
}

static void FSi_ConvertToDirEntry(FSDirEntry *entry, FSArchive *arc,
                                  const FSDirectoryEntryInfo *info) {
  entry->name_len = info->longname_length;
  if (entry->name_len > sizeof(entry->name) - 1) {
    entry->name_len = sizeof(entry->name) - 1;
  }
  MI_CpuCopy8(info->longname, entry->name, entry->name_len);
  entry->name[entry->name_len] = '\0';
  if (info->id == FS_INVALID_FILE_ID) {
    entry->is_directory = FALSE;
    entry->file_id.file_id = FS_INVALID_FILE_ID;
    entry->file_id.arc = NULL;
  } else if ((info->attributes & FS_ATTRIBUTE_IS_DIRECTORY) != 0) {
    entry->is_directory = TRUE;
    entry->dir_id.arc = arc;
    entry->dir_id.own_id = (u16)(info->id >> 0);
    entry->dir_id.index = (u16)(info->id >> 16);
    entry->dir_id.pos = 0;
  } else {
    entry->is_directory = FALSE;
    entry->file_id.file_id = info->id;
    entry->file_id.arc = arc;
  }
}

u32 FS_GetLength(FSFile *file) { return FS_GetFileLength(file); }

u32 FS_GetPosition(FSFile *file) { return FS_GetFilePosition(file); }
#endif /* FS_IMPLEMENT */
#endif /* SDK_VERSION_MAJOR */

#if SDK_VERSION_MAJOR == 5

#if defined(SDK_TWL) && defined(SDK_ARM7)
#include <twl/ltdmain_begin.h>
#endif

static const int FSiUnicodeBufferQueueMax = 4;
static OSMessageQueue FSiUnicodeBufferQueue[1];
#ifdef SDK_BUILD_ARM
static OSMessage FSiUnicodeBufferQueueArray[FSiUnicodeBufferQueueMax];
#else
static OSMessage FSiUnicodeBufferQueueArray[4];
#endif
static BOOL FSiUnicodeBufferQueueInitialized = FALSE;
#ifdef SDK_BUILD_ARM
static u16 FSiUnicodeBufferTable[FSiUnicodeBufferQueueMax]
                                [FS_ARCHIVE_FULLPATH_MAX + 1];
#else
static u16 FSiUnicodeBufferTable[4][FS_ARCHIVE_FULLPATH_MAX + 1];
#endif

u16 *FSi_GetUnicodeBuffer(const char *src) {
  u16 *retval = NULL;

  OSIntrMode bak = OS_DisableInterrupts();
  if (!FSiUnicodeBufferQueueInitialized) {
    int i;
    FSiUnicodeBufferQueueInitialized = TRUE;
    OS_InitMessageQueue(FSiUnicodeBufferQueue, FSiUnicodeBufferQueueArray, 4);
    for (i = 0; i < FSiUnicodeBufferQueueMax; ++i) {
      (void)OS_SendMessage(FSiUnicodeBufferQueue, FSiUnicodeBufferTable[i],
                           OS_MESSAGE_BLOCK);
    }
  }
  (void)OS_RestoreInterrupts(bak);

  (void)OS_ReceiveMessage(FSiUnicodeBufferQueue, (OSMessage *)&retval,
                          OS_MESSAGE_BLOCK);
  if (src) {
    int dstlen = FS_ARCHIVE_FULLPATH_MAX;
    (void)FSi_ConvertStringSjisToUnicode(retval, &dstlen, src, NULL, NULL);
    retval[dstlen] = L'\0';
  }
  return retval;
}

void FSi_ReleaseUnicodeBuffer(const void *buf) {
  if (buf) {

    (void)OS_SendMessage(FSiUnicodeBufferQueue, (OSMessage)buf,
                         OS_MESSAGE_BLOCK);
  }
}

SDK_WEAK_SYMBOL
STDResult FSi_ConvertStringSjisToUnicode(u16 *dst, int *dst_len,
                                         const char *src, int *src_len,
                                         STDConvertUnicodeCallback callback)
#ifdef SDK_BUILD_ARM
    __attribute__((never_inline))
#endif
{
  return STD_ConvertStringSjisToUnicode(dst, dst_len, src, src_len, callback);
}

SDK_WEAK_SYMBOL
STDResult FSi_ConvertStringUnicodeToSjis(char *dst, int *dst_len,
                                         const u16 *src, int *src_len,
                                         STDConvertSjisCallback callback)
#ifdef SDK_BUILD_ARM
    __attribute__((never_inline))
#endif
{
  return STD_ConvertStringUnicodeToSjis(dst, dst_len, src, src_len, callback);
}

#if defined(SDK_TWL) && defined(SDK_ARM7)
#include <twl/ltdmain_end.h>
#endif

#endif /* SDK_VERSION_MAJOR */
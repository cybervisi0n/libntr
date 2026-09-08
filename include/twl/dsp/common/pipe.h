#ifndef TWL_DSP_PIPE_H_
#define TWL_DSP_PIPE_H_

#include <twl/dsp/common/byteaccess.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SDK_TWL
#define DSP_PIPE_INPUT 0
#define DSP_PIPE_OUTPUT 1
#else
#define DSP_PIPE_INPUT 1
#define DSP_PIPE_OUTPUT 0
#endif
#define DSP_PIPE_PEER_MAX 2

#define DSP_PIPE_PORT_MAX 8 // Maximum number of usable ports for pipes
#define DSP_PIPE_DEFAULT_BUFFER_LENGTH 256 // Default ring buffer size

#define DSP_PIPE_CONSOLE 0 // DSP -> ARM: Debugging console
#define DSP_PIPE_DMA 1     // DSP <-> ARM: Pseudo-DMA
#define DSP_PIPE_AUDIO 2   // DSP <-> ARM: General-purpose audio communications
#define DSP_PIPE_BINARY 3  // DSP <-> ARM: General-purpose binary
#define DSP_PIPE_EPHEMERAL                                                     \
  4 // Free region that can be allocated with DSP_CreatePipe()

#define DSP_PIPE_FLAG_INPUT 0x0000    // Input side
#define DSP_PIPE_FLAG_OUTPUT 0x0001   // Output side
#define DSP_PIPE_FLAG_PORTMASK 0x00FF // Negative field for port numbers
#define DSP_PIPE_FLAG_BOUND 0x0100    // Opened
#define DSP_PIPE_FLAG_EOF 0x0200      // EOF

#define DSP_PIPE_FLAG_EXIT_OS 0x8000 // Exit processing for the DSP's AHB master

#define DSP_PIPE_COMMAND_REGISTER 2

#define DSP_PIPE_IO_COMMAND_OPEN 0
#define DSP_PIPE_IO_COMMAND_CLOSE 1
#define DSP_PIPE_IO_COMMAND_SEEK 2
#define DSP_PIPE_IO_COMMAND_READ 3
#define DSP_PIPE_IO_COMMAND_WRITE 4
#define DSP_PIPE_IO_COMMAND_MEMMAP 5

#define DSP_PIPE_IO_MODE_R 0x0001
#define DSP_PIPE_IO_MODE_W 0x0002
#define DSP_PIPE_IO_MODE_RW 0x0004
#define DSP_PIPE_IO_MODE_TRUNC 0x0008
#define DSP_PIPE_IO_MODE_CREATE 0x0010

#define DSP_PIPE_IO_SEEK_SET 0
#define DSP_PIPE_IO_SEEK_CUR 1
#define DSP_PIPE_IO_SEEK_END 2

typedef struct DSPPipe {
  DSPAddr address; // Starting address of the buffer
  DSPByte length;  // Buffer size
  DSPByte rpos;    // First unread region
  DSPByte wpos;    // Last appended region
  u16 flags;       // Attribute flags
} DSPPipe;

typedef struct DSPPipeMonitor {
  DSPPipe pipe[DSP_PIPE_PORT_MAX][DSP_PIPE_PEER_MAX];
} DSPPipeMonitor;

typedef void (*DSPPipeCallback)(void *userdata, int port, int peer);

void DSP_InitPipe(void);

void DSP_SetPipeCallback(int port, void (*callback)(void *, int, int),
                         void *userdata);

DSPPipe *DSP_LoadPipe(DSPPipe *pipe, int port, int peer);

void DSP_SyncPipe(DSPPipe *pipe);

void DSP_FlushPipe(DSPPipe *pipe);

u16 DSP_GetPipeReadableSize(const DSPPipe *pipe);

u16 DSP_GetPipeWritableSize(const DSPPipe *pipe);

void DSP_ReadPipe(DSPPipe *pipe, void *buffer, u16 length);

void DSP_WritePipe(DSPPipe *pipe, const void *buffer, u16 length);

void DSP_HookPipeNotification(void);

#ifdef SDK_TWL

#else

void DSP_Printf(const char *format, ...);

int DSP_OpenFile(const char *path, int mode);
int DSP_OpenMemoryFile(DSPAddrInARM address, DSPWord32 length);
void DSP_CloseFile(int port);
s32 DSP_GetFilePosition(int port);
s32 DSP_GetFileLength(int port);
s32 DSP_SeekFile(int port, s32 offset, int whence);
s32 DSP_ReadFile(int port, void *buffer, DSPWord length);
s32 DSP_WriteFile(int port, const void *buffer, DSPWord length);

#if 0
typedef void FILE;
#define fopen(path, mode) (FILE *)DSP_OpenFile(path, mode)
#define fclose(f) DSP_CloseFile((int)f)
#define fseek(f, ofs, whence) DSP_SeekFile((int)f, ofs, whence)
#define fread(buf, len, unit, f) DSP_ReadFile((int)f, buf, (len) * (unit))
#define fwrite(buf, len, unit, f) DSP_WriteFile((int)f, buf, (len) * (unit))
#define rewind(f) (void)DSP_SeekFile((int)f, 0, DSP_PIPE_IO_SEEK_SET)
#define ftell(f) (void)DSP_SeekFile((int)f, 0, DSP_PIPE_IO_SEEK_CUR)
#define fgetpos(f, ppos) (((*(ppos) = ftell((int)f)) != -1) ? 0 : -1)
#define fsetpos(f, ppos) fseek((int)f, *(ppos), DSP_PIPE_IO_SEEK_SET)
#endif

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TWL_DSP_PIPE_H_ */

#ifdef SDK_TWL
#include <twl.h>
#else
#include <nitro.h>
#endif
#include <nitro/demo/DEMOWave.h>

#define MAKE_FOURCC(cc1, cc2, cc3, cc4)                                        \
  (u32)((cc1) | (cc2 << 8) | (cc3 << 16) | (cc4 << 24))

#define FOURCC_RIFF MAKE_FOURCC('R', 'I', 'F', 'F')
#define FOURCC_WAVE MAKE_FOURCC('W', 'A', 'V', 'E')
#define FOURCC_fmt MAKE_FOURCC('f', 'm', 't', ' ')
#define FOURCC_data MAKE_FOURCC('d', 'a', 't', 'a')

#define L_CHANNEL 4
#define R_CHANNEL 5
#define ALARM_NUM 0
#define STREAM_THREAD_PRIO 12
#define THREAD_STACK_SIZE 1024
#define STRM_BUF_PAGESIZE 1024 * 32
#define STRM_BUF_SIZE STRM_BUF_PAGESIZE * 2

typedef struct WaveFormat {
  u16 format;
  u16 channels;
  s32 sampleRate;
  u32 dataRate;
  u16 blockAlign;
  u16 bitPerSample;
} WaveFormat;

typedef struct StreamInfo {
  FSFile file;
  WaveFormat format;
  u32 beginPos;
  s32 dataSize;
  BOOL isPlay;
} StreamInfo;

static BOOL ReadWaveFormat(StreamInfo *strm);
static void CloseWave(void);

static StreamInfo StrmInfo;

int DEMOReadWave(char *dst, const char *filename) {
  int readSize;

  StreamInfo *strm = &StrmInfo;

  if (FS_IsFile(&strm->file)) {
    (void)FS_CloseFile(&strm->file);
  }
  if (!FS_OpenFileEx(&strm->file, filename, FS_FILEMODE_R)) {
    OS_Panic("Error: failed to open file %s\n", filename);
  }
  if (!ReadWaveFormat(strm)) {
    OS_Panic("Error: failed to read wavefile\n");
  }

  (void)FS_SeekFile(&strm->file, (s32)strm->beginPos, FS_SEEK_SET);

  readSize = FS_ReadFile(&strm->file, dst, strm->dataSize);
  if (readSize == -1) {
    OS_Panic("read file end\n");
  }

  CloseWave();

  return readSize;
}

static void CloseWave(void) {
  StreamInfo *strm = &StrmInfo;
  if (FS_IsFile(&strm->file)) {
    (void)FS_CloseFile(&strm->file);
  }
}

static BOOL ReadWaveFormat(StreamInfo *strm) {
  u32 tag;
  s32 size;
  BOOL fFmt = FALSE, fData = FALSE;

  (void)FS_SeekFileToBegin(&strm->file);

  (void)FS_ReadFile(&strm->file, &tag, 4);
  if (tag != FOURCC_RIFF)
    return FALSE;

  (void)FS_ReadFile(&strm->file, &size, 4);

  (void)FS_ReadFile(&strm->file, &tag, 4);
  if (tag != FOURCC_WAVE)
    return FALSE;

  while (size > 0) {
    s32 chunkSize;
    if (FS_ReadFile(&strm->file, &tag, 4) == -1) {
      return FALSE;
    }
    if (FS_ReadFile(&strm->file, &chunkSize, 4) == -1) {
      return FALSE;
    }

    switch (tag) {
    case FOURCC_fmt:
      if (chunkSize != 0x10) {
        return FALSE;
      }
      if (FS_ReadFile(&strm->file, (u8 *)&strm->format, chunkSize) == -1) {
        return FALSE;
      }
      fFmt = TRUE;
      break;
    case FOURCC_data:
      strm->beginPos = FS_GetFilePosition(&strm->file);
      strm->dataSize = chunkSize;
      (void)FS_SeekFile(&strm->file, chunkSize, FS_SEEK_CUR);
      fData = TRUE;
      break;
    default:
      (void)FS_SeekFile(&strm->file, chunkSize, FS_SEEK_CUR);
      break;
    }
    if (fFmt && fData) {
      return TRUE;
    }

    size -= chunkSize;
  }

  if (size != 0)
    return FALSE;
  return TRUE;
}

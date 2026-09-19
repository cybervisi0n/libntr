#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define ISTDFIO_FILEIO_PATH_MAX 260

#define ISTDFIO_ENVNAME_MAX 256

enum {
  ISTDFIO_FILEIO_FLAG_READ = 1 << 0,
  ISTDFIO_FILEIO_FLAG_WRITE = 1 << 1,

  ISTDFIO_FILEIO_FLAG_FORCE = 1 << 3,
  ISTDFIO_FILEIO_FLAG_INCENVVAR = 1 << 4,
  ISTDFIO_FILEIO_FLAG_CREATEDIR = 1 << 5,

  ISTDFIO_FILEIO_FLAG_AUTOFLUSH = 1 << 31
};

enum {
  ISTDFIO_FILEIO_ERROR_SUCCESS = 0,

  ISTDFIO_FILEIO_ERROR_COMERROR,

  ISTDFIO_FILEIO_ERROR_NOTCONNECT,

  ISTDFIO_FILEIO_ERROR_SERVERERROR,

  ISTDFIO_FILEIO_ERROR_NOMOREFILES,

  ISTDFIO_FILEIO_ERROR_FILENOTFOUND,

  ISTDFIO_FILEIO_ERROR_PATHTOOLONG,

  ISTDFIO_FILEIO_ERROR_WAITRESULT = 0x80
};

enum {

  ISTDFIO_FILEIO_SEEK_BEGIN,

  ISTDFIO_FILEIO_SEEK_CURRENT,

  ISTDFIO_FILEIO_SEEK_END
};

enum ISTDFIOAttrib {
  ISTDFIO_FILEIO_ATTRIBUTE_READONLY = 0x00000001,
  ISTDFIO_FILEIO_ATTRIBUTE_HIDDEN = 0x00000002,
  ISTDFIO_FILEIO_ATTRIBUTE_SYSTEM = 0x00000004,
  ISTDFIO_FILEIO_ATTRIBUTE_DIRECTORY = 0x00000010,
  ISTDFIO_FILEIO_ATTRIBUTE_ARCHIVE = 0x00000020,
  ISTDFIO_FILEIO_ATTRIBUTE_DEVICE = 0x00000040,
  ISTDFIO_FILEIO_ATTRIBUTE_NORMAL = 0x00000080,
  ISTDFIO_FILEIO_ATTRIBUTE_TEMPORARY = 0x00000100,
  ISTDFIO_FILEIO_ATTRIBUTE_SPARSE_FILE = 0x00000200,
  ISTDFIO_FILEIO_ATTRIBUTE_REPARSE_POINT = 0x00000400,
  ISTDFIO_FILEIO_ATTRIBUTE_COMPRESSED = 0x00000800,
  ISTDFIO_FILEIO_ATTRIBUTE_OFFLINE = 0x00001000,
  ISTDFIO_FILEIO_ATTRIBUTE_NOT_CONTENT_INDEXED = 0x00002000,
  ISTDFIO_FILEIO_ATTRIBUTE_ENCRYPTED = 0x00004000,
  ISTDFIO_FILEIO_ATTRIBUTE_LAST
};

typedef struct {

  u32 m_hFile;

  u32 m_nErrCode;

  u32 m_nServerCode;

  u32 m_nTempData;

  volatile u32 m_bBusy;
} ISTDFIOFile;

typedef struct {

  u32 m_nAttrib;

  u32 m_nSize;

  char m_bufName[ISTDFIO_FILEIO_PATH_MAX];
} ISTDFIOFindData;

typedef struct {

  u16 m_nYear;

  u8 m_nMonth;

  u8 m_nDay;

  u8 m_nHour;

  u8 m_nMinute;

  u8 m_nSecond;

  u8 m_nReserve;
} CFIODateTime;

typedef struct {

  u32 m_nAtrb;

  CFIODateTime m_ctime;

  CFIODateTime m_mtime;

  CFIODateTime m_atime;
} CFIOAtrb;

#if defined(DEMO_HOSTIO_H_)
#include <nitro/version.h>

#if (SDK_CURRENT_VERSION_NUMBER >= SDK_VERSION_NUMBER(5, 0, 0)) &&             \
    (SDK_CURRENT_VERSION_NUMBER <= SDK_VERSION_NUMBER(5, 0, 30011))
#define ISTWL_ISFIO_COMPATFUNC_WRITE
#endif

#if (SDK_CURRENT_VERSION_NUMBER >= SDK_VERSION_NUMBER(5, 1, 0)) &&             \
    (SDK_CURRENT_VERSION_NUMBER <= SDK_VERSION_NUMBER(5, 1, 30005))
#define ISTWL_ISFIO_COMPATFUNC_WRITE
#endif

#if (SDK_CURRENT_VERSION_NUMBER >= SDK_VERSION_NUMBER(5, 2, 0)) &&             \
    (SDK_CURRENT_VERSION_NUMBER <= SDK_VERSION_NUMBER(5, 2, 10100))
#define ISTWL_ISFIO_COMPATFUNC_WRITE
#endif
#endif // DEMO_HOSTIO_H_

u32 ISTDFIOOpen(ISTDFIOFile *pFile, const char *pFilename, u32 nFlags);

u32 ISTDFIOClose(ISTDFIOFile *pFile);

u32 ISTDFIORead(ISTDFIOFile *pFile, void *pBuffer, u32 nSize, u32 *pnReadSize);

#ifdef ISTWL_ISFIO_COMPATFUNC_WRITE

u32 ISTDFIOWrite(ISTDFIOFile *pFile, void *pBuffer, u32 nSize);
#else
u32 ISTDFIOWrite(ISTDFIOFile *pFile, const void *pBuffer, u32 nSize);
#endif

u32 ISTDFIOSeek(ISTDFIOFile *pFile, s32 nMove, u32 nOrigin, u32 *pnNewPosition);

u32 ISTDFIOLength(ISTDFIOFile *pFile, u32 *pnLength);

u32 ISTDFIOFindFirst(ISTDFIOFile *pFile, ISTDFIOFindData *pFindData,
                     const char *pPattern);
u32 ISTDFIOFindNext(ISTDFIOFile *pFile, ISTDFIOFindData *pFindData);
u32 ISTDFIOFindClose(ISTDFIOFile *pFile);

u32 ISTDFIOMkDir(const char *pPath);

u32 ISTDFIORmDir(const char *pPath);

u32 ISTDFIORemove(const char *pPath);

u32 ISTDFIORename(const char *pPathOld, const char *pPathNew);

u32 ISTDFIOGetCWD(char *pPathCwd);

u32 ISTDFIOChDir(const char *pPath);

u32 ISTDFIOGetAttribute(const char *pFile, CFIOAtrb *pnAtrb);

u32 ISTDFIOGetServerErrorCode(ISTDFIOFile *pFile);

u32 ISTDFIOExpandEnv(const char *pEnv, char *pEnvExpand, u32 nEnvExpandBufSize);

u32 ISTDFIOSetAutoPathEnvExpand(BOOL bExpand);

#ifdef __cplusplus
} /* extern "C" */
#endif

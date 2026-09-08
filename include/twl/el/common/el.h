#ifndef TWL_COMMON_EL_H_
#define TWL_COMMON_EL_H_

#ifdef SDK_TWL
#include <twl.h>
#else
#include <nitro.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef u32 ELDlld;
typedef void *(*ELAlloc)(size_t size);
typedef void (*ELFree)(void *ptr);
typedef u32 (*ELReadImage)(u32 offset, void *buf, u32 size);

typedef enum ELLinkMode {
  EL_LINKMODE_ROUGH = 0,
  EL_LINKMODE_ONESHOT,
  EL_LINKMODE_RELATION
} ELLinkMode;

#define EL_SUCCESS 0
#define EL_FAILED 1
#define EL_RELOCATED 0xF1 // For EL_ResolveAll only

typedef enum ELResult {
  EL_RESULT_SUCCESS = 0,
  EL_RESULT_FAILURE = 1,
  EL_RESULT_INVALID_PARAMETER,
  EL_RESULT_INVALID_ELF,
  EL_RESULT_UNSUPPORTED_ELF,
  EL_RESULT_CANNOT_ACCESS_ELF, // Error when opening or reading an ELF file
  EL_RESULT_NO_MORE_RESOURCE   // malloc failed
} ELResult;

typedef struct {
  void *next;     /*Next address entry*/
  char *name;     /*String*/
  void *adr;      /*Address*/
  u16 func_flag;  /*0: Data. 1: Function.*/
  u16 thumb_flag; /*0: ARM code. 1: Thumb code.*/
} ELAdrEntry;

s32 EL_Init(ELAlloc alloc, ELFree free);

s32 EL_CalcEnoughBufferSizeforLinkFile(const char *FilePath, const void *buf,
                                       ELLinkMode link_mode);
s32 EL_CalcEnoughBufferSizeforLink(ELReadImage readfunc, u32 len,
                                   const void *buf, ELLinkMode link_mode);
s32 EL_CalcEnoughBufferSizeforLinkImage(void *obj_image, u32 obj_len,
                                        const void *buf, ELLinkMode link_mode);

ELDlld EL_LinkFileEx(const char *FilePath, void *buf, u32 buf_size);
ELDlld EL_LinkEx(ELReadImage readfunc, u32 len, void *buf, u32 buf_size);
ELDlld EL_LinkImageEx(void *obj_image, u32 obj_len, void *buf, u32 buf_size);

ELDlld EL_LinkFile(const char *FilePath, void *buf);
ELDlld EL_Link(ELReadImage readfunc, u32 len, void *buf);
ELDlld EL_LinkImage(void *obj_image, u32 obj_len, void *buf);

u16 EL_ResolveAll(void);

BOOL EL_Export(ELAdrEntry *AdrEnt);

void EL_AddStaticSym(void);

void *EL_GetGlobalAdr(ELDlld my_dlld, const char *ent_name);

u16 EL_Unlink(ELDlld my_dlld);

BOOL EL_IsResolved(ELDlld my_dlld);

u32 EL_GetLibSize(ELDlld my_dlld);

ELResult EL_GetResultCode(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*TWL_COMMON_EL_H_*/

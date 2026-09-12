#ifndef NITRO_OS_ARGUMENT_H_
#define NITRO_OS_ARGUMENT_H_

#if (SDK_VERSION_MAJOR == 5) && defined(SDK_TWL)
#include <twl/hw/common/mmap_parameter.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SDK_FINALROM
    #define OS_NO_ARGUMENT    TRUE
#endif

#ifdef OS_ARGUMENT_FORCE_TO_BE_AVAILABLE
    #ifdef OS_NO_ARGUMENT
        #undef OS_NO_ARGUMENT
    #endif
#endif

#if (defined(SDK_WIN32) || defined(SDK_FROM_TOOL))
    #define SDK_ASSERT(exp)                         ((void) 0)
    #define SDK_ALIGN4_ASSERT(exp)                  ((void) 0)
    #define SDK_MINMAX_ASSERT(exp, min, max)        ((void) 0)
    #define SDK_NULL_ASSERT(exp)                    ((void) 0)

    #if (defined(_MSC_VER) && !defined(__cplusplus))
        #define inline __inline
    #endif
#endif

#define OS_ARGUMENT_BUFFER_SIZE     256

#define OS_ARGUMENT_ID_STRING       ":$@$Argument$@$:"
#define OS_ARGUMENT_ID_STRING_BUFFER_SIZE 18

typedef struct OSArgumentBuffer {
	char argMark[OS_ARGUMENT_ID_STRING_BUFFER_SIZE];
	u16 size;
#if defined(SDK_WIN32) || defined(SDK_FROM_TOOL)
	char buffer[OS_ARGUMENT_BUFFER_SIZE];
#else
	const char buffer[OS_ARGUMENT_BUFFER_SIZE];
#endif
} OSArgumentBuffer;

#ifndef OS_NO_ARGUMENT
    extern int OS_GetArgc(void);
#else
    static inline int OS_GetArgc (void)
    {
        return 0;
    }
#endif

#ifndef OS_NO_ARGUMENT
    extern const char * OS_GetArgv(int n);
#else
    static inline const char * OS_GetArgv (int n)
    {
    #pragma unused(n)
        return NULL;
    }
#endif

#ifndef OS_NO_ARGUMENT
    int OS_GetOpt(const char * optstring);
#else
    static inline int OS_GetOpt (const char * optstring)
    {
    #pragma unused(optstring)
        return -1;
    }
#endif

extern const char * OSi_OptArg;
extern int OSi_OptInd;
extern int OSi_OptOpt;

static inline const char * OS_GetOptArg (void)
{
	return OSi_OptArg;
}

static inline int OS_GetOptInd (void)
{
	return OSi_OptInd;
}

static inline int OS_GetOptOpt (void)
{
	return OSi_OptOpt;
}

#ifndef OS_NO_ARGUMENT
    extern void OS_ConvertToArguments(const char * str, char cs, char * buffer, u32 bufSize);
#else
    static inline void OS_ConvertToArguments (const char * str, char cs, char * buffer, u32 bufSize)
    {
    #pragma unused(str, cs, buffer, bufSize)
    }
#endif

#ifndef OS_NO_ARGUMENT
    extern void OS_SetArgumentBuffer(const char * buffer);
#else
    static inline void OS_SetArgumentBuffer (const char * buffer)
    {
    #pragma unused(buffer)
    }
#endif

#ifndef OS_NO_ARGUMENT
    extern const char * OS_GetArgumentBuffer(void);
#else
    static inline const char * OS_GetArgumentBuffer (void)
    {
        return NULL;
    }
#endif

#if SDK_VERSION_MAJOR == 5
#ifdef SDK_TWL

typedef struct {
  u64 titleId;
  u8 reserved1;
  u8 flag;
  u16 makerCode;
  u16 argBufferSize;
  u16 binarySize;
  u16 crc;
  u16 sysParam;
} OSDeliverArgHeader;

#define OS_DELIVER_ARG_BUFFER_SIZE                                             \
  (HW_PARAM_DELIVER_ARG_SIZE - sizeof(OSDeliverArgHeader))

typedef struct {
  OSDeliverArgHeader header;
  u8 buf[OS_DELIVER_ARG_BUFFER_SIZE];

} OSDeliverArgInfo;

#define OS_DELIVER_ARG_BUF_INVALID 0
#define OS_DELIVER_ARG_BUF_ACCESSIBLE 1
#define OS_DELIVER_ARG_BUF_WRITABLE 2

#define OS_DELIVER_ARG_SUCCESS 0
#define OS_DELIVER_ARG_NOT_READY -1
#define OS_DELIVER_ARG_OVER_SIZE -2

#define OS_DELIVER_ARG_ENCODE_FLAG 1
#define OS_DELIVER_ARG_VALID_FLAG 2

void OS_InitDeliverArgInfo(OSDeliverArgInfo *info, int binSize);
int OS_SetStringToDeliverArg(const char *str);
int OS_SetBinaryToDeliverArg(const void *bin, int size);
int OS_ConvertStringToDeliverArg(const char *str, char cs);
int OS_EncodeDeliverArg(void);
int OS_DecodeDeliverArg(void);
u32 OS_GetDeliverArgState(void);
void OS_SetDeliverArgStateInvalid(void);
int OS_GetBinarySizeFromDeliverArg(void);
int OS_GetBinaryFromDeliverArg(void *buffer, int *size, int maxSize);
OSTitleId OS_GetTitleIdFromDeliverArg(void);
u32 OS_GetGameCodeFromDeliverArg(void);
u16 OS_GetMakerCodeFromDeliverArg(void);
BOOL OS_IsValidDeliverArg(void);
BOOL OS_IsDeliverArgEncoded(void);
OSTitleId OS_GetTitleIdLastEncoded(void);
int OS_SetSysParamToDeliverArg(u16 param);
u16 OS_GetSysParamFromDeliverArg(void);
int OS_GetDeliverArgc(void);
const char *OS_GetDeliverArgv(int n);
void OSi_SetDeliverArgState(u32 state);

#endif // ifdef SDK_TWL
#endif

#ifdef __cplusplus
}
#endif

#endif

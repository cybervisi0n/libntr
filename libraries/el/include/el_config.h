#ifndef __ELF_LOADER_CONFIG_H__
#define __ELF_LOADER_CONFIG_H__

#define DEBUG_PRINT_ON (0)

#ifdef SDK_ARM7
#define TARGET_ARM_V4 (1)
#else
#define TARGET_ARM_V4 (0)
#endif

#define TARGET_ARM_V5 (TARGET_ARM_V4 ^ 1)

#if (DEBUG_PRINT_ON == 1)
#define PRINTDEBUG OS_TPrintf
#else
#define PRINTDEBUG(...) ((void)0)
#endif

#define OSAPI_CPUFILL8 MI_CpuFill8
#define OSAPI_CPUCOPY8 MI_CpuCopy8

#ifdef SDK_ARM7
#define ISTDRELOCATIONPROC_AUTO ISTDOVERLAYPROC_ARM7
#else
#define ISTDRELOCATIONPROC_AUTO ISTDOVERLAYPROC_ARM9
#endif

#if (TARGET_ARM_V5 == 1)
#define ISTDVENEERTYPE_AUTO ISTDVENEERTYPE_ARM
#else
#define ISTDVENEERTYPE_AUTO ISTDVENEERTYPE_ARMV4T
#endif

#if 0 // Do not use OS_Alloc unchanged
#define OSAPI_MALLOC OS_Alloc
#define OSAPI_FREE OS_Free
#define OSAPI_STRLEN STD_GetStringLength
#define OSAPI_STRNCMP STD_CompareNString
#define OSAPI_STRCMP STD_CompareString
#else

#define OSAPI_MALLOC i_elAlloc
#define OSAPI_FREE i_elFree
#define OSAPI_STRLEN STD_GetStringLength
#define OSAPI_STRNCMP STD_CompareNString
#define OSAPI_STRCMP STD_CompareString

#endif

#define OSAPI_FLUSHCACHEALL IC_InvalidateAll(), DC_FlushAll
#define OSAPI_WAITCACHEBUF DC_WaitWriteBufferEmpty

#endif /*__ELF_LOADER_CONFIG_H__*/

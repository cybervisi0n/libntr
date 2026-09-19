#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NDEBUG
#define ISTDPrint(ARG_)
#define ISTDPrintf(ARG_...)
#define ISTDPrintEx(CONS_, ARG_)
#define ISTDPrintfEx(CONS_, ARG_...)
#define ISTDPrintSetBlockingMode(ENABLE_)
#define ISTDPrintGetBlockingMode() 0
#define ISTDAssert(FILE_, LINE_, INFO_)
#else
void ISTDPrint(const char *pBuf);
void ISTDPrintf(const char *pBuf, ...);
void ISTDPrintEx(int nWindow, const char *pBuf);
void ISTDPrintfEx(int nWindow, const char *pBuf, ...);
void ISTDPrintSetBlockingMode(BOOL bBlocking);
BOOL ISTDPrintGetBlockingMode(void);
void ISTDAssert(const char *pFile, int nLine, const char *pExpression,
                int nStopProgram);
#endif

#undef ISTDASSERT
#ifdef NDEBUG
#define ISTDASSERT(exp)
#else
#define ISTDASSERT(exp)                                                        \
  (exp) ? ((void)0) : ISTDAssert(__FILE__, __LINE__, #exp, 1);
#endif

#undef ISTDWARNING
#ifdef NDEBUG
#define ISTDWARNING(exp)
#else
#define ISTDWARNING(exp)                                                       \
  (exp) ? ((void)0) : ISTDAssert(__FILE__, __LINE__, #exp, 0);
#endif

#ifdef __cplusplus
}
#endif

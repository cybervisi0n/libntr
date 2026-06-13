#include <stdarg.h>
#include <stdio.h>
#include "simulator/assert.h"

#ifdef SDK_BUILD_LINUX
#include <signal.h>
#endif

#ifdef SDK_BUILD_WIN64
#include "debugapi.h"
#endif

void SIM_handleAssertionFailure(const char* filename, unsigned int lineno, const char* exp) {
	printf("[SIM ASSERTION FAILED] %s:%d\n", filename, lineno);
	if(exp[0] != 0) {
		printf("%s\n", exp);
	}
	SIM_debugBreak();
}

void SIM_handleAssertionFailureMsg(const char* filename, unsigned int lineno, const char* fmt, ...) {
	printf("[SIM ASSERTION FAILED] %s:%d\n", filename, lineno);
	va_list vlist;
	va_start(vlist, fmt);
	vprintf(fmt, vlist);
	va_end(vlist);
	printf("\n");
	SIM_debugBreak();
}

void SIM_debugBreak() {
	#ifdef SDK_BUILD_WIN64
	if(IsDebuggerPresent()){
		__debugbreak();
	}
	#endif
    #ifdef SDK_BUILD_LINUX
    raise(SIGTRAP);
    #endif
}
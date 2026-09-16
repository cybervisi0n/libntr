#include <simulator/sim.h>
#include <nitro.h>

#if SDK_BUILD_LINUX
#include <execinfo.h>
#include <signal.h>
#endif

#include <string>

#include <SDL2/SDL.h>

namespace SIM::ErrorHandler {

void SegfaultHandler(int param) {
  void           *array[128];    /* Array to store backtrace symbols */
  size_t          size;             /* To store the exact no of values stored */
  char          **strings;          /* To store functions from the backtrace list in ARRAY */
  size_t          nCnt;

  size = backtrace(array, 32);

  strings = backtrace_symbols(array, size);

  /* prints each string of function names of trace*/
  for (nCnt = 0; nCnt < size; nCnt++)
    fprintf(stderr, "%s\n", strings[nCnt]);


  exit(-1);
}

void Init() {
#if SDK_BUILD_LINUX
    signal(SIGSEGV, SegfaultHandler);
#endif
}




}
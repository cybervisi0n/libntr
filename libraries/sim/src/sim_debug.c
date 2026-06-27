#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#include <nitro.h>

#define NANO 1000000000L
#define MILLI_TO_NANO 1000000L

static const char * DebugPrintFilePath = "DbgLog.txt";
static const char * s_profileStartFile = "";
static int s_profileStartLine;

static struct timespec s_profileStartTime;

void SIM_Dbg_Init() {
    FILE * dbgPrintFile = NULL;

    dbgPrintFile = fopen(DebugPrintFilePath, "w");
    if(dbgPrintFile) {
        fclose(dbgPrintFile);
    }
}

void SIM_Dbg_Printf(const char * fmt, ...) {
    va_list args;
    FILE * dbgPrintFile = NULL;

    dbgPrintFile = fopen(DebugPrintFilePath, "a");
    if(!dbgPrintFile) {
        return;  
    }

    va_start(args, fmt);
    vfprintf(dbgPrintFile, fmt, args);
    va_end(args);


    fclose(dbgPrintFile);
}

void SIM_Dbg_ProfileStartReal(const char * file, int line) {
    s_profileStartFile = file;
    s_profileStartLine = line;
    clock_gettime(CLOCK_MONOTONIC, &s_profileStartTime);
}

void SIM_Dbg_ProfileStopReal(const char * file, int line) {
    struct timespec profileEndTime;
    clock_gettime(CLOCK_MONOTONIC, &profileEndTime);

    u64 diffSeconds = profileEndTime.tv_sec - s_profileStartTime.tv_sec;
    u64 diffNs = profileEndTime.tv_nsec - s_profileStartTime.tv_nsec;
    u64 totalDiffNs = (diffSeconds * NANO) + diffNs;


    SIM_Dbg_Printf("[%d.%06dms] %s:%d -> %s:%d\n", totalDiffNs / MILLI_TO_NANO, totalDiffNs % MILLI_TO_NANO,
                                s_profileStartFile, s_profileStartLine, file, line);
}
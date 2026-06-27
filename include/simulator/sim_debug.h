#ifndef SIM_DEBUG_H
#define SIM_DEBUG_H

void SIM_Dbg_Init();
void SIM_Dbg_Printf(const char * fmt, ...);

#define SIM_Dbg_ProfileStart() SIM_Dbg_ProfileStartReal(__FILE__,__LINE__)
#define SIM_Dbg_ProfileStop() SIM_Dbg_ProfileStopReal(__FILE__,__LINE__)
void SIM_Dbg_ProfileStartReal(const char * file, int line);
void SIM_Dbg_ProfileStopReal(const char * file, int line);

#endif
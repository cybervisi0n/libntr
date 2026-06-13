#ifndef SIM_SND_H
#define SIM_SND_H

#include <pthread.h>

//This file is DEPRECATED!!!

//See GBATEK DS Sound Channels 0..15
typedef struct SIM_sndChCnt {
	u8 volumeMul;
	u8 volumeDiv;
	u8 hold;
	u8 panning;
	u8 waveDuty;
	u8 repeatMode;
	u8 format;
	u8 start;
} SIM_sndChCnt;

extern SIM_sndChCnt s_SIM_sndChCnt[16];
extern u8 * s_SIM_sndsad[16];
extern u16 s_SIM_sndtmr[16];
extern u16 s_SIM_sndpnt[16];
extern u32 s_SIM_sndlen[16];

extern u8 s_SIM_dbg_dumpSound;
extern u8 s_SIM_dbg_soundSoloChannel;

#endif

#ifndef SIM_H
#define SIM_H
#include <nitro.h>
#include <nitro/types.h>
#include <nitro/gx/gx_vramcnt.h>
#include <SDL2/SDL.h>
#include <simulator/config/sim_config.h>

#include "simulator/assert.h"

#define SIM_NDS_SCREEN_WIDTH 256
#define SIM_NDS_SCREEN_HEIGHT 192

#define SIM_NDS_SCREEN_SCALE_FACTOR 4
#define SIM_NDS_WINDOW_SCALE_FACTOR 2

typedef struct WIN_snd_channel_info {
	u8 playing;
	u32 preLoopLen;
	u32 preLoopSamples;
	u32 loopLen;
	u32 loopSamples;
	u32 alarms_bitfield;
	u32 audio_len;
	u32 total_audio_len;
	u32 total_num_samples;
	u8 * audio_pos;
	u8 * audio_data;
	u8 inLoop;
	u32 posPreLoop;
	u32 posInLoop;
	u32 pos;
	u8 stream[500000];
	u8 resampStream[1800000];
	SDL_AudioStream * sdlStream;
	u32 streamSampleRate;
	double pitchRatio;
	u32 prevTimer;
	u8 pitchBend;
} WIN_snd_channel_info;

typedef struct WIN_snd_alarm_info {
	u32 alarmNo;
	u32 time;
	u32 period;
	u32 id;
	pthread_t thread;
	u8 isRunning;
} WIN_snd_alarm_info;

void SIM_PreRenderVBlank(void);
void SIM_PostRenderVBlank(void);
void SIM_procPXI(void);
void SIM_procPXI7to9(void);
int SIM_procPxiThread( void * arg );
void SIM_PostPxiThread();
int SIM_procPxi7to9Thread( void * arg );
void SIM_PostPxi7to9Thread();
int SIM_procOSThreadThread( void * arg );
void SIM_procThread(void);
void SIM_pxiInit(void);
void * SIM_Render(void *arg);
void * SIM_RenderInit(void * arg);
int SIM_runNitroOSThread(void * thread);
void * SIM_sndAlarm(void * arg);
void SIM_u16ToRGB( u16 in, u8 * r, u8 *g, u8 * b );
SIM_config_type * SIM_GetConfigPtr();

extern GXVRamTex s_SIM_GXVRamTex;
extern GXVRamTexPltt s_SIM_GXVRamTexPltt;
extern GXVRamBGExtPltt s_SIM_GXBgExtPltt;
extern GXVRamSubBGExtPltt s_SIM_GXSubBgExtPltt;

extern SNDSharedWork * win_SNDi_SharedWork;

extern void * win_snd_channel_bufs[32];
extern WIN_snd_channel_info win_snd_channels[32];
extern WIN_snd_channel_info win_sndi_channels[16];
extern WIN_snd_alarm_info win_snd_alarms[32];

#endif

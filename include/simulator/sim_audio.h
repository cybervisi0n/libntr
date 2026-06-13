#ifndef SIM_AUDIO_H
#define SIM_AUDIO_H

#include <nitro.h>
#include <nitro/types.h>
#include <simulator/sim.h>
#include <SDL2/SDL.h>

extern u32 s_SIM_sndcnt[16];
extern u8 * s_SIM_sndsad[16];
extern u16 s_SIM_sndtmr[16];
extern u16 s_SIM_sndpnt[16];
extern u32 s_SIM_sndlen[16];

void SIM_Audio_Init(int aAudioFrequency);
void SIM_Audio_Callback(void *userdata, Uint8 *stream, int len);

void SIM_Audio_StartChannel(int chNo);

s32 SIM_Audio_RunChannel(u32 cycles, int chNo);

void SIM_Audio_NextSamplePCM8(int chNo); 
void SIM_Audio_NextSamplePCM16(int chNo);
void SIM_Audio_NextSampleADPCM(int chNo);
void SIM_Audio_NextSamplePSG(int chNo);
void SIM_Audio_NextSampleNoise(int chNo);

#endif

#include <nitro.h>
#include <simulator/sim_audio.h>
#include <simulator/sim.h>
#include <simulator/config/sim_config.h>
#include <SDL2/SDL.h>

#include "blip_buf.h"

#include "tracy/TracyC.h"

static SDL_AudioSpec s_requestedAudioSpec, s_actualAudioSpec;
static SDL_AudioDeviceID s_audioDevice;

u32 s_SIM_sndcnt[16] = {0};
u8 * s_SIM_sndsad[16] = {0};
u16 s_SIM_sndtmr[16] = {0};
u16 s_SIM_sndpnt[16] = {0};
u32 s_SIM_sndlen[16] = {0};

static s32 s_SIM_internalSoundTimer[16] = {0};
static s32 s_SIM_internalSoundPos[16] = {0};
static s32 s_SIM_internalSoundSample[16] = {0};
static s32 s_SIM_internalADPCMValLoop[16] = {0};
static s32 s_SIM_internalADPCMIndexLoop[16] = {0};
static s32 s_SIM_internalADPCMVal[16] = {0};
static s32 s_SIM_internalADPCMIndex[16] = {0};
static u8 s_SIM_internalADPCMCurByte[16] = {0};
static s32 s_SIM_internalNextADPCMByte[16] = {0};
static u16 s_SIM_internalNoiseVal[16] = {0};
SIM_sndChCnt s_SIM_sndChCnt[16] = {0};

static int s_blipTimer = 0;
static blip_t* s_BlipLeft;
static blip_t* s_BlipRight;
static s16* s_outputBuffer;
static u32 s_outputBufferWritePos = 0;
static u32 s_outputBufferReadPos = 0;

static int s_outputLastLeftSample;
static int s_outputLastRightSample;

static const u16 s_ADPCMTable[89] =
{
    0x0007, 0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E,
    0x0010, 0x0011, 0x0013, 0x0015, 0x0017, 0x0019, 0x001C, 0x001F,
    0x0022, 0x0025, 0x0029, 0x002D, 0x0032, 0x0037, 0x003C, 0x0042,
    0x0049, 0x0050, 0x0058, 0x0061, 0x006B, 0x0076, 0x0082, 0x008F,
    0x009D, 0x00AD, 0x00BE, 0x00D1, 0x00E6, 0x00FD, 0x0117, 0x0133,
    0x0151, 0x0173, 0x0198, 0x01C1, 0x01EE, 0x0220, 0x0256, 0x0292,
    0x02D4, 0x031C, 0x036C, 0x03C3, 0x0424, 0x048E, 0x0502, 0x0583,
    0x0610, 0x06AB, 0x0756, 0x0812, 0x08E0, 0x09C3, 0x0ABD, 0x0BD0,
    0x0CFF, 0x0E4C, 0x0FBA, 0x114C, 0x1307, 0x14EE, 0x1706, 0x1954,
    0x1BDC, 0x1EA5, 0x21B6, 0x2515, 0x28CA, 0x2CDF, 0x315B, 0x364B,
    0x3BB9, 0x41B2, 0x4844, 0x4F7E, 0x5771, 0x602F, 0x69CE, 0x7462,
    0x7FFF
};

static const s8 s_ADPCMIndexTable[8] = {-1, -1, -1, -1, 2, 4, 6, 8};

static const s16 s_PSGTable[8][8] =
{
    {-0x7FFF, -0x7FFF, -0x7FFF, -0x7FFF, -0x7FFF, -0x7FFF, -0x7FFF,  0x7FFF},
    {-0x7FFF, -0x7FFF, -0x7FFF, -0x7FFF, -0x7FFF, -0x7FFF,  0x7FFF,  0x7FFF},
    {-0x7FFF, -0x7FFF, -0x7FFF, -0x7FFF, -0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF},
    {-0x7FFF, -0x7FFF, -0x7FFF, -0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF},
    {-0x7FFF, -0x7FFF, -0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF},
    {-0x7FFF, -0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF},
    {-0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF},
    {-0x7FFF, -0x7FFF, -0x7FFF, -0x7FFF, -0x7FFF, -0x7FFF, -0x7FFF, -0x7FFF}
};

#define INTERNAL_SAMPLE_RATE 16756991.f


void SIM_Audio_Init(int aAudioFrequency)
{
    s_BlipLeft = blip_new(512*64);
    s_BlipRight = blip_new(512*64);

    blip_set_rates(s_BlipLeft, INTERNAL_SAMPLE_RATE * 1.0f, aAudioFrequency);
    blip_set_rates(s_BlipRight, INTERNAL_SAMPLE_RATE * 1.0f, aAudioFrequency);

    memset(&s_requestedAudioSpec, 0, sizeof(SDL_AudioSpec));
    s_requestedAudioSpec.freq = aAudioFrequency;
    s_requestedAudioSpec.format = AUDIO_S16LSB;
    s_requestedAudioSpec.channels = 2;

    // Set up the number of samples so NitroComposer runs at the exact right interval
    s_requestedAudioSpec.samples = (int)((double)aAudioFrequency * 0.005215419);
    if(s_requestedAudioSpec.samples & 1) {
        s_requestedAudioSpec.samples += 1;
    }
    s_requestedAudioSpec.callback = SIM_Audio_Callback;
    if(SDL_OpenAudio(&s_requestedAudioSpec, NULL) < 0) {
        printf("SDL Error %s\n", SDL_GetError());
    }
    SDL_PauseAudio(0);
}

static u8 GetNextADPCMByte(int chNo);
static void PanOutput(s32 in, s32 * left, s32 * right, int chNo);

void SIM_Audio_Callback(void *userdata, Uint8 *stream, int len)
{
    TracyCZone(ctx, 1);
    // Run NitroComposer
    SND_UpdateExChannel();
    SND_SeqMain(TRUE);
    SND_ExChannelMain(TRUE);

    // get samples from each channel and do the thing
    

    while(blip_samples_avail(s_BlipLeft) < len / 4) {
        s32 left = 0;
        s32 right = 0;
        s32 leftOutput = 0;
        s32 rightOutput = 0;

        s32 ch0 = SIM_Audio_RunChannel(512, 0);
        s32 ch1 = SIM_Audio_RunChannel(512, 1);
        s32 ch2 = SIM_Audio_RunChannel(512, 2);
        s32 ch3 = SIM_Audio_RunChannel(512, 3);

        // hack for now
        PanOutput(ch0, &left, &right, 0);
        PanOutput(ch1, &left, &right, 1);
        PanOutput(ch2, &left, &right, 2);
        PanOutput(ch3, &left, &right, 3);
        //left += ch0;
        //left += ch1;
        //left += ch2;
        //left += ch3;

        // pan output

        for(int chNo = 4; chNo < 16; chNo++) {
            s32 channel = SIM_Audio_RunChannel(512, chNo);
            PanOutput(channel, &left, &right, chNo);
        }

        right = left;
        s_blipTimer += 512;

        if(left != 0) {
            blip_add_delta(s_BlipLeft, s_blipTimer, left - s_outputLastLeftSample);
        }
        if(right != 0) {
            blip_add_delta(s_BlipRight, s_blipTimer, right - s_outputLastRightSample);
        }

        s_outputLastLeftSample = left;
        s_outputLastRightSample = right;

        if(s_blipTimer >= 512 * 128) {
            blip_end_frame(s_BlipLeft, s_blipTimer);
            blip_end_frame(s_BlipRight, s_blipTimer);
            s_blipTimer = 0;
        }
    }

    int avail = blip_samples_avail(s_BlipLeft);
    if(avail > len / 4) {
        avail = len / 4;
    }
    s16 * tempbuf = (s16*)stream;
    blip_read_samples(s_BlipLeft, tempbuf, avail, TRUE);
    blip_read_samples(s_BlipRight, tempbuf+1, avail, TRUE);
    TracyCZoneEnd(ctx);
}

void SIM_Audio_StartChannel(int chNo)
{
    if((s_SIM_sndcnt[chNo]>>29)&0x3 == 3) {
        s_SIM_internalSoundPos[chNo] = -1;
    } else {
        s_SIM_internalSoundPos[chNo] = -3;
    }
    s_SIM_internalSoundSample[chNo] = 0;
    s_SIM_internalSoundTimer[chNo] = s_SIM_sndtmr[chNo];
    s_SIM_internalNoiseVal[chNo] = 0x7FFF;
}

s32 SIM_Audio_RunChannel(u32 cycles, int chNo)
{
    if(!(s_SIM_sndcnt[chNo] & (1<<31))) {
        return 0;
    }

    int type = (s_SIM_sndcnt[chNo] >> 29) & 0x3;
    if(type == 3) {
        if(chNo >= 14) {
            type = 4;
        } else if (chNo >= 8) {
            type = 3;
        } else {
            // these channels cant do psg or noise
            return 0;
        }
    }

    if((type < 3) && 
       (s_SIM_sndlen[chNo] + s_SIM_sndpnt < 16)) {
        return 0;
    }

    s_SIM_internalSoundTimer[chNo] += cycles;

    while(s_SIM_internalSoundTimer[chNo] >> 16) {
        s_SIM_internalSoundTimer[chNo] = s_SIM_sndtmr[chNo] + (s_SIM_internalSoundTimer[chNo] - 0x10000);

        switch(type) {
            case 0: SIM_Audio_NextSamplePCM8(chNo); break;
            case 1: SIM_Audio_NextSamplePCM16(chNo); break;
            case 2: SIM_Audio_NextSampleADPCM(chNo); break;
            case 3: SIM_Audio_NextSamplePSG(chNo); break;
            case 4: SIM_Audio_NextSampleNoise(chNo); break;
        }
    }

    s32 ret = s_SIM_internalSoundSample[chNo];
    int volumeShift = (s_SIM_sndcnt[chNo] & (0x3 << 8)) >> 8;
    int volume = s_SIM_sndcnt[chNo] & 0b1111111;

    //ret <<= volumeShift;
    ret = ret >> volumeShift;
    //ret *= volume;
    //ret = ret / 128;
    ret = (s16)((double)ret * ((double)volume/128.0));

    return ret;
}

void SIM_Audio_NextSamplePCM8(int chNo)
{
    s_SIM_internalSoundPos[chNo]++;
    if(s_SIM_internalSoundPos[chNo] < 0) {
        return;
    }
    
    if(s_SIM_internalSoundPos[chNo] >= s_SIM_sndpnt[chNo] + s_SIM_sndlen[chNo]) {
        u32 repeat = (s_SIM_sndcnt[chNo] >> 27) & 0x3;
        if(repeat & 1) {
            s_SIM_internalSoundPos[chNo] = s_SIM_sndpnt[chNo];
        } else if(repeat & 2) {
            s_SIM_internalSoundSample[chNo] = 0;
            s_SIM_sndcnt[chNo] &= ~(1<<31);
            return;
        }
    }
    s8 * ptr = (s8*)s_SIM_sndsad[chNo];
    if(ptr) {
        s8 val = *(ptr+s_SIM_internalSoundPos[chNo]);
        s_SIM_internalSoundSample[chNo] = val << 8;
    }
}

void SIM_Audio_NextSamplePCM16(int chNo)
{
    s_SIM_internalSoundPos[chNo]++;
    if(s_SIM_internalSoundPos[chNo] < 0) {
        return;
    }

    if((s_SIM_internalSoundPos[chNo]<<1) >= s_SIM_sndpnt[chNo] + s_SIM_sndlen[chNo]) {
        u32 repeat = (s_SIM_sndcnt[chNo] >> 27) & 0x3;
        if(repeat & 1) {
            s_SIM_internalSoundPos[chNo] = s_SIM_sndpnt[chNo]>>1;
        } else if(repeat & 2) {
            s_SIM_internalSoundSample[chNo] = 0;
            s_SIM_sndcnt[chNo] &= ~(1<<31);
            return;
        }
    }
    s16 * ptr = (s16*)s_SIM_sndsad[chNo];
    if(ptr) {
        s16 val = *(ptr+s_SIM_internalSoundPos[chNo]);
        s_SIM_internalSoundSample[chNo] = val;
    }
}

void SIM_Audio_NextSampleADPCM(int chNo)
{
    s_SIM_internalSoundPos[chNo]++;
    if(s_SIM_internalSoundPos[chNo] < 8) {
        if(s_SIM_internalSoundPos[chNo] == 0) {
            //setup adpcm
            u32 header = *(u32*)(s_SIM_sndsad[chNo]);
            s_SIM_internalADPCMVal[chNo] = (s32)(s16)(header & 0xFFFF);
            s_SIM_internalADPCMIndex[chNo] = (header >> 16) & 0x7F;
            s_SIM_internalNextADPCMByte[chNo] = 4;
            if(s_SIM_internalADPCMIndex[chNo] > 88) {
                s_SIM_internalADPCMIndex[chNo] = 88;
            }

            s_SIM_internalADPCMValLoop[chNo] = s_SIM_internalADPCMVal[chNo];
            s_SIM_internalADPCMIndexLoop[chNo] = s_SIM_internalADPCMIndex[chNo];
        }

        return;
    }

    if((s_SIM_internalSoundPos[chNo] >> 1) >= (s_SIM_sndpnt[chNo] + s_SIM_sndlen[chNo])) {
        u32 repeat = (s_SIM_sndcnt[chNo]>>27) & 0x3;
        if(repeat & 1) {
            s_SIM_internalSoundPos[chNo] = s_SIM_sndpnt[chNo]<<1;
            s_SIM_internalNextADPCMByte[chNo] = s_SIM_sndpnt[chNo];
            s_SIM_internalADPCMVal[chNo] = s_SIM_internalADPCMValLoop[chNo];
            s_SIM_internalADPCMIndex[chNo] = s_SIM_internalADPCMIndexLoop[chNo];
            u8 * ptr = (u8*)s_SIM_sndsad[chNo];
            s_SIM_internalADPCMCurByte[chNo] = GetNextADPCMByte(chNo);
            //s_SIM_internalADPCMCurByte[chNo] = ptr[s_SIM_internalSoundPos[chNo]>>1];
        } else if (repeat & 2) {
            s_SIM_internalSoundSample[chNo] = 0;
            s_SIM_sndcnt[chNo] &= ~(1<<31);
            return;
        }
    } else {
        if(!(s_SIM_internalSoundPos[chNo] & 0x1)) {
            //u8 * ptr = (u8*)s_SIM_sndsad[chNo];
            //s_SIM_internalADPCMCurByte[chNo] = ptr[s_SIM_internalSoundPos[chNo]>>1];
            s_SIM_internalADPCMCurByte[chNo] = GetNextADPCMByte(chNo);
        } else {
            s_SIM_internalADPCMCurByte[chNo] >>= 4;
        }

        u16 val = s_ADPCMTable[s_SIM_internalADPCMIndex[chNo]];
        u16 diff = val >> 3;
        if (s_SIM_internalADPCMCurByte[chNo] & 0x1) diff += (val >> 2);
        if (s_SIM_internalADPCMCurByte[chNo] & 0x2) diff += (val >> 1);
        if (s_SIM_internalADPCMCurByte[chNo] & 0x4) diff += val;

        if (s_SIM_internalADPCMCurByte[chNo] & 0x8)
        {
            s_SIM_internalADPCMVal[chNo] -= diff;
            if (s_SIM_internalADPCMVal[chNo] < -0x7FFF) s_SIM_internalADPCMVal[chNo] = -0x7FFF;
        }
        else
        {
            s_SIM_internalADPCMVal[chNo] += diff;
            if (s_SIM_internalADPCMVal[chNo] > 0x7FFF) s_SIM_internalADPCMVal[chNo] = 0x7FFF;
        }

        s_SIM_internalADPCMIndex[chNo] += s_ADPCMIndexTable[s_SIM_internalADPCMCurByte[chNo] & 0x7];
        if      (s_SIM_internalADPCMIndex[chNo] < 0)  s_SIM_internalADPCMIndex[chNo] = 0;
        else if (s_SIM_internalADPCMIndex[chNo] > 88) s_SIM_internalADPCMIndex[chNo] = 88;

        if (s_SIM_internalSoundPos[chNo] == (s_SIM_sndpnt[chNo]<<1))
        {
            s_SIM_internalADPCMValLoop[chNo] = s_SIM_internalADPCMVal[chNo];
            s_SIM_internalADPCMIndexLoop[chNo] = s_SIM_internalADPCMIndex[chNo];
        }
    }

    s_SIM_internalSoundSample[chNo] = s_SIM_internalADPCMVal[chNo];
}

void SIM_Audio_NextSamplePSG(int chNo)
{
    s_SIM_internalSoundPos[chNo]++;
    s_SIM_internalSoundSample[chNo] = s_PSGTable[(s_SIM_sndcnt[chNo] >> 24) & 0x7][s_SIM_internalSoundPos[chNo] & 0x7];
}

void SIM_Audio_NextSampleNoise(int chNo)
{
    if(s_SIM_internalNoiseVal[chNo] & 0x1) {
        s_SIM_internalNoiseVal[chNo] = (s_SIM_internalNoiseVal[chNo] >> 1) ^ 0x6000;
        s_SIM_internalSoundSample[chNo] = -0x7FFF;
    } else {
        s_SIM_internalNoiseVal[chNo] >>= 1;
        s_SIM_internalSoundSample[chNo] = 0x7FFF;
    }
}

static u8 GetNextADPCMByte(int chNo)
{
    u8 ret = s_SIM_sndsad[chNo][s_SIM_internalNextADPCMByte[chNo]];
    s_SIM_internalNextADPCMByte[chNo]++;

    return ret;
}

static void PanOutput(s32 in, s32 * left, s32 * right, int chNo)
{
    int pan = (s_SIM_sndcnt[chNo] & (0b1111111 << 16)) >> 16;
    *left += ((s64)in * (128-pan)) >> 10;
    *right += ((s64)in * pan) >> 10;
}
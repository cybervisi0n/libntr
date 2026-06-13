#include <nitro/snd/common/channel.h>

#ifdef SDK_PORT
#include <simulator/sim.h>
#include <simulator/sim_audio.h>
#include <math.h>
#endif

#define M_PI       3.14159265358979323846


#define SURROUND_CHANNEL_MASK 0xfff5

#define IMAMax(samp) (samp > 0x7FFF) ? ((short)0x7FFF) : samp
#define IMAMin(samp) (samp < -0x7FFF) ? ((short)-0x7FFF) : samp
#define IMAIndexMinMax(index, min, max) (index > max) ? max : ((index < min) ? min : index)


int     sSurroundDecay = 0;
int     sMasterPan = -1;
u8      sOrgVolume[SND_CHANNEL_NUM];
u8      sOrgPan[SND_CHANNEL_NUM];

static const unsigned AdpcmTable[89] = {
    0x0007, 0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x0010, 0x0011, 0x0013, 0x0015,
    0x0017, 0x0019, 0x001C, 0x001F, 0x0022, 0x0025, 0x0029, 0x002D, 0x0032, 0x0037, 0x003C, 0x0042,
    0x0049, 0x0050, 0x0058, 0x0061, 0x006B, 0x0076, 0x0082, 0x008F, 0x009D, 0x00AD, 0x00BE, 0x00D1,
    0x00E6, 0x00FD, 0x0117, 0x0133, 0x0151, 0x0173, 0x0198, 0x01C1, 0x01EE, 0x0220, 0x0256, 0x0292,
    0x02D4, 0x031C, 0x036C, 0x03C3, 0x0424, 0x048E, 0x0502, 0x0583, 0x0610, 0x06AB, 0x0756, 0x0812,
    0x08E0, 0x09C3, 0x0ABD, 0x0BD0, 0x0CFF, 0x0E4C, 0x0FBA, 0x114C, 0x1307, 0x14EE, 0x1706, 0x1954,
    0x1BDC, 0x1EA5, 0x21B6, 0x2515, 0x28CA, 0x2CDF, 0x315B, 0x364B, 0x3BB9, 0x41B2, 0x4844, 0x4F7E,
    0x5771, 0x602F, 0x69CE, 0x7462, 0x7FFF};

static int IMA_IndexTable[9] = {-1, -1, -1, -1, 2, 4, 6, 8};

static const s16 WIN_SND_PSGTable[8][8] =
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

static const float WIN_SND_PSGDutyCycles[8] = {0.125f, 0.25f, 0.375f, 0.5f, 0.625f, 0.75f, 0.875f, 0.0f};


int     CalcSurroundDecay(int volume, int pan);
static void process_nibble(unsigned char data4bit, int * Index, int * Pcm16bit);
static void convertImaAdpcm(u8 *bufIn, u8 *bufOut, u32 dataOff, u32 dataLength);
static void SoundEffectCallback(int chNo, void * stream, int len, void * userData);
static void DummySoundEffectCallback(int chNo, void * stream, int len, void * userData);
static void ChannelFinishedCallback(int chNo, void * arg);
static void OneShotFinishedCallback(int chNo, void * arg);


void SND_SetupChannelPcm7(int chNo,
                         const void *dataaddr,
                         SNDWaveFormat format,
                         SNDChannelLoop loop,
                         int loopStart,
                         int loopLen, int volume, SNDChannelDataShift shift, int timer, int pan)
{
    const int offset = SND_CHANNEL_REG_OFFSET(chNo);

    SDK_MINMAX_ASSERT(chNo, SND_CHANNEL_MIN, SND_CHANNEL_MAX);
    SDK_MINMAX_ASSERT(loopLen, 0, SND_CHANNEL_LOOP_LEN_MAX);
    SDK_ASSERT(((u32)dataaddr & ~SND_CHANNEL_SAD_MASK) == 0);
    SDK_MINMAX_ASSERT(loopStart, 0, SND_CHANNEL_LOOP_START_MAX);
    SDK_MINMAX_ASSERT(loopLen, 0, SND_CHANNEL_LOOP_LEN_MAX);
    SDK_MINMAX_ASSERT(volume, SND_CHANNEL_VOLUME_MIN, SND_CHANNEL_VOLUME_MAX);
    SDK_MINMAX_ASSERT(timer, SND_CHANNEL_TIMER_MIN, SND_CHANNEL_TIMER_MAX);
    SDK_MINMAX_ASSERT(pan, SND_CHANNEL_PAN_MIN, SND_CHANNEL_PAN_MAX);

    sOrgPan[chNo] = (u8)pan;
    if (sMasterPan >= 0)
        pan = sMasterPan;

    sOrgVolume[chNo] = (u8)volume;
    if (sSurroundDecay > 0 && ((1 << chNo) & SURROUND_CHANNEL_MASK))
    {
        volume = CalcSurroundDecay(volume, pan);
    }

#ifdef SDK_TEG
    pan = 127 - pan;
#endif
    #ifdef SDK_BUILD_ARM
    *((REGType32v *)(REG_SOUND0CNT_ADDR + offset)) = REG_SND_SOUND0CNT_FIELD(FALSE, format, loop, 0,
                                                                             pan, 0,
                                                                             shift, volume);

    *((REGType16v *)(REG_SOUND0TMR_ADDR + offset)) = (u16)(0x10000 - timer);
    *((REGType16v *)(REG_SOUND0RPT_PT_ADDR + offset)) = (u16)loopStart;
    *((REGType32v *)(REG_SOUND0RPT_LEN_ADDR + offset)) = (u32)loopLen;
    *((REGType32v *)(REG_SOUND0SAD_ADDR + offset)) = (u32)dataaddr;
    #endif

    #ifdef SDK_PORT
    s_SIM_sndcnt[chNo] = REG_SND_SOUND0CNT_FIELD(FALSE, format, loop, 0,
                                                 pan, 0,
                                                 shift, volume);
    s_SIM_sndsad[chNo] = dataaddr;
    s_SIM_sndtmr[chNo] = 0x10000 - timer;
    s_SIM_sndpnt[chNo] = loopStart << 2;
    s_SIM_sndlen[chNo] = loopLen << 2;
    #endif
}

void SND_SetupChannelPsg7(int chNo,
                         SNDDuty duty, int volume, SNDChannelDataShift shift, int timer, int pan)
{
    const int offset = SND_CHANNEL_REG_OFFSET(chNo);

    SDK_MINMAX_ASSERT(chNo, SND_PSG_CHANNEL_MIN, SND_PSG_CHANNEL_MAX);
    SDK_MINMAX_ASSERT(volume, SND_CHANNEL_VOLUME_MIN, SND_CHANNEL_VOLUME_MAX);
    SDK_MINMAX_ASSERT(timer, SND_CHANNEL_TIMER_MIN, SND_CHANNEL_TIMER_MAX);
    SDK_MINMAX_ASSERT(pan, SND_CHANNEL_PAN_MIN, SND_CHANNEL_PAN_MAX);

    sOrgPan[chNo] = (u8)pan;
    if (sMasterPan >= 0)
        pan = sMasterPan;

    sOrgVolume[chNo] = (u8)volume;
    if (sSurroundDecay > 0 && ((1 << chNo) & SURROUND_CHANNEL_MASK))
    {
        volume = CalcSurroundDecay(volume, pan);
    }

#ifdef SDK_TEG
    pan = 127 - pan;
#endif

    *((REGType32v *)(REG_SOUND0CNT_ADDR + offset)) = REG_SND_SOUND0CNT_FIELD(FALSE, SND_WAVE_FORMAT_PSG, 0,
                                                                             duty, pan, 0,
                                                                             shift, volume);

    *((REGType16v *)(REG_SOUND0TMR_ADDR + offset)) = (u16)(0x10000 - timer);

    #ifdef SDK_PORT
    s_SIM_sndcnt[chNo] = REG_SND_SOUND0CNT_FIELD(FALSE, SND_WAVE_FORMAT_PSG, 0,
                                                 duty, pan, 0,
                                                 shift, volume);
    s_SIM_sndtmr[chNo] = 0x10000 - timer;
    #endif
}

void SND_SetupChannelNoise7(int chNo, int volume, SNDChannelDataShift shift, int timer, int pan)
{
    const int offset = SND_CHANNEL_REG_OFFSET(chNo);

    SDK_MINMAX_ASSERT(chNo, SND_NOISE_CHANNEL_MIN, SND_NOISE_CHANNEL_MAX);
    SDK_MINMAX_ASSERT(volume, SND_CHANNEL_VOLUME_MIN, SND_CHANNEL_VOLUME_MAX);
    SDK_MINMAX_ASSERT(timer, SND_CHANNEL_TIMER_MIN, SND_CHANNEL_TIMER_MAX);
    SDK_MINMAX_ASSERT(pan, SND_CHANNEL_PAN_MIN, SND_CHANNEL_PAN_MAX);

    sOrgPan[chNo] = (u8)pan;
    if (sMasterPan >= 0)
        pan = sMasterPan;

    sOrgVolume[chNo] = (u8)volume;
    if (sSurroundDecay > 0 && ((1 << chNo) & SURROUND_CHANNEL_MASK))
    {
        volume = CalcSurroundDecay(volume, pan);
    }

#ifdef SDK_TEG
    pan = 127 - pan;
#endif

    *((REGType32v *)(REG_SOUND0CNT_ADDR + offset)) = REG_SND_SOUND0CNT_FIELD(FALSE, SND_WAVE_FORMAT_NOISE, 0,
                                                                             0,
                                                                             pan, 0,
                                                                             shift, volume);

    *((REGType16v *)(REG_SOUND0TMR_ADDR + offset)) = (u16)(0x10000 - timer);

    #ifdef SDK_PORT
    s_SIM_sndcnt[chNo] = REG_SND_SOUND0CNT_FIELD(FALSE, SND_WAVE_FORMAT_NOISE, 0,
                                                 0,
                                                 pan, 0,
                                                 shift, volume);
    s_SIM_sndtmr[chNo] = (0x10000 - timer);
    #endif
}

void SND_StopChannel7(int chNo, s32 flags)
{
    REGType32v *reg;
    u32     ctrl;

    SDK_MINMAX_ASSERT(chNo, SND_CHANNEL_MIN, SND_CHANNEL_MAX);

    #ifdef SDK_BUILD_ARM
    reg = (REGType32v *)(REG_SOUND0CNT_ADDR + SND_CHANNEL_REG_OFFSET(chNo));
    #else
    reg = (REGType32v *)(&s_SIM_sndcnt[chNo]);  
    #endif
    ctrl = *reg;
    ctrl &= ~REG_SND_SOUND0CNT_E_MASK;
    if (flags & SND_CHANNEL_STOP_HOLD)
        ctrl |= REG_SND_SOUND0CNT_HOLD_MASK;
    *reg = ctrl;
}

#ifdef SDK_CW_WA_OPT4
#pragma optimization_level 0
#endif
void SND_SetChannelVolume7(int chNo, int volume, SNDChannelDataShift shift)
{
    int     pan;

    SDK_MINMAX_ASSERT(chNo, SND_CHANNEL_MIN, SND_CHANNEL_MAX);
    SDK_MINMAX_ASSERT(volume, SND_CHANNEL_VOLUME_MIN, SND_CHANNEL_VOLUME_MAX);

    sOrgVolume[chNo] = (u8)volume;
    if (sSurroundDecay > 0 && ((1 << chNo) & SURROUND_CHANNEL_MASK))
    {
        pan = *((REGType8v *)(REG_SOUND0CNT_PAN_ADDR + SND_CHANNEL_REG_OFFSET(chNo)));
        volume = CalcSurroundDecay(volume, pan);
    }

    *((REGType16v *)(REG_SOUND0CNT_VOL_16_ADDR + SND_CHANNEL_REG_OFFSET(chNo))) = REG_SND_SOUND0CNT_VOL_16_FIELD(0,
                                                                                                                 shift,
                                                                                                                 volume);
    
    #ifdef SDK_PORT
    u32 reg = s_SIM_sndcnt[chNo];

    reg &=     ~((0x3) << REG_SND_SOUND0CNT_SHIFT_SHIFT);
    reg &=     ~((0b1111111));

    reg |= (shift << REG_SND_SOUND0CNT_SHIFT_SHIFT);
    reg |= (volume);

    s_SIM_sndcnt[chNo] = reg;

    
    #endif
}

#ifdef SDK_CW_WA_OPT4
#pragma optimization_level 4
#endif

void SND_SetChannelTimer7(int chNo, int timer)
{
    SDK_MINMAX_ASSERT(chNo, SND_CHANNEL_MIN, SND_CHANNEL_MAX);
    SDK_MINMAX_ASSERT(timer, SND_CHANNEL_TIMER_MIN, SND_CHANNEL_TIMER_MAX);

    *((REGType16v *)(REG_SOUND0TMR_ADDR + SND_CHANNEL_REG_OFFSET(chNo))) = (u16)(0x10000 - timer);

    #ifdef SDK_PORT
    s_SIM_sndtmr[chNo] = (0x10000 - timer) & 0xFFFF;
    #endif
}

void SND_SetChannelPan7(int chNo, int pan)
{
    int     volume;

    SDK_MINMAX_ASSERT(chNo, SND_CHANNEL_MIN, SND_CHANNEL_MAX);
    SDK_MINMAX_ASSERT(pan, SND_CHANNEL_PAN_MIN, SND_CHANNEL_PAN_MAX);

    sOrgPan[chNo] = (u8)pan;
    if (sMasterPan >= 0)
        pan = sMasterPan;

#ifdef SDK_TEG
    pan = 127 - pan;
#endif

    #ifdef SDK_BUILD_ARM
    *((REGType8v *)(REG_SOUND0CNT_PAN_ADDR + SND_CHANNEL_REG_OFFSET(chNo))) = (u8)pan;
    #else
    u32 reg = s_SIM_sndcnt[chNo];
    reg = ~(0b1111111 << 16);

    reg |= (pan << 16);
    s_SIM_sndcnt[chNo] = reg;
    #endif

    if (sSurroundDecay > 0 && ((1 << chNo) & SURROUND_CHANNEL_MASK))
    {
        volume = CalcSurroundDecay(sOrgVolume[chNo], pan);
        *((REGType8v *)(REG_SOUND0CNT_VOL_ADDR + SND_CHANNEL_REG_OFFSET(chNo))) = (u8)volume;
        #ifdef SDK_PORT
        //sndcnt_p->volumeMul = volume & 0b1111111;
        //sndcnt_p->volumeDiv = (volume > 8) & 0b11;
        #endif
    }
}

BOOL SND_IsChannelActive7(int chNo)
{
    #ifdef SDK_BUILD_ARM
    u8      reg;
    #else
    u32 reg;
    #endif

    SDK_MINMAX_ASSERT(chNo, SND_CHANNEL_MIN, SND_CHANNEL_MAX);

    #ifdef SDK_BUILD_ARM
    reg = *((REGType8v *)(REG_SOUND0CNT_8_ADDR + SND_CHANNEL_REG_OFFSET(chNo)));
    #else
    reg = *((REGType32v *)(&s_SIM_sndcnt[chNo]));
    #endif
    #ifdef SDK_BUILD_ARM
    return (reg & REG_SND_SOUND0CNT_8_E_MASK) ? TRUE : FALSE;
    #else
    return (reg & REG_SND_SOUND0CNT_E_MASK) ? TRUE : FALSE;
    #endif
}


void SND_StartChannel7(int chNo)
{
    SDK_MINMAX_ASSERT(chNo, SND_CHANNEL_MIN, SND_CHANNEL_MAX);

    #ifdef SDK_BUILD_ARM
    *((REGType8v *)(REG_SOUND0CNT_8_ADDR + SND_CHANNEL_REG_OFFSET(chNo)))
        |= REG_SND_SOUND0CNT_8_E_MASK;
    #else
    s_SIM_sndcnt[chNo] |= REG_SND_SOUND0CNT_E_MASK;
    SIM_Audio_StartChannel(chNo);
    #endif
}

void SND_SetMasterPan7(int pan)
{
    int     chNo;
    int     offset;

    sMasterPan = pan;

    if (pan >= 0)
    {
        for (chNo = 0; chNo < SND_CHANNEL_NUM; chNo++)
        {
            offset = SND_CHANNEL_REG_OFFSET(chNo);
            *((REGType8v *)(REG_SOUND0CNT_PAN_ADDR + offset)) = (u8)pan;
        }
    }
    else
    {
        for (chNo = 0; chNo < SND_CHANNEL_NUM; chNo++)
        {
            offset = SND_CHANNEL_REG_OFFSET(chNo);
            *((REGType8v *)(REG_SOUND0CNT_PAN_ADDR + offset)) = sOrgPan[chNo];
        }
    }
}

u32 SND_GetChannelControl(int chNo)
{
    SDK_MINMAX_ASSERT(chNo, SND_CHANNEL_MIN, SND_CHANNEL_MAX);

    #ifdef SDK_PORT
    return s_SIM_sndcnt[chNo];
    #else
    return *((REGType32v *)(REG_SOUND0CNT_ADDR + SND_CHANNEL_REG_OFFSET(chNo)));
    #endif
}


void SNDi_SetSurroundDecay7(int decay)
{
    int     chNo;
    int     volume;
    int     pan;
    int     offset;

    sSurroundDecay = decay;

    for (chNo = 0; chNo < SND_CHANNEL_NUM; chNo++)
    {
        if ((1 << chNo) & SURROUND_CHANNEL_MASK)
        {
            offset = SND_CHANNEL_REG_OFFSET(chNo);
            pan = *((REGType8v *)(REG_SOUND0CNT_PAN_ADDR + offset));

            volume = CalcSurroundDecay(sOrgVolume[chNo], pan);

            *((REGType8v *)(REG_SOUND0CNT_VOL_ADDR + offset)) = (u8)volume;
        }
    }
}


int CalcSurroundDecay(int volume, int pan)
{
    if (pan < 24)
    {
        volume *= sSurroundDecay * (pan + 40) + ((32767 - sSurroundDecay) << 6);
        volume >>= 15 + 6;
    }
    else if (pan > 104)
    {
        volume *= -sSurroundDecay * (pan - 40) + ((32767 + sSurroundDecay) << 6);
        volume >>= 15 + 6;
    }

    return volume;
}

static void process_nibble(unsigned char data4bit, int * Index, int * Pcm16bit) {
  int idx = *Index;
  int pcm16bitLcl = *Pcm16bit;
  int Diff = AdpcmTable[idx] / 8;
  if (data4bit & 1)
    Diff = Diff + AdpcmTable[idx] / 4;
  if (data4bit & 2)
    Diff = Diff + AdpcmTable[idx] / 2;
  if (data4bit & 4)
    Diff = Diff + AdpcmTable[idx] / 1;

  if ((data4bit & 8) == 0)
    pcm16bitLcl = IMAMax(pcm16bitLcl + Diff);
  if ((data4bit & 8) == 8)
    pcm16bitLcl = IMAMin(pcm16bitLcl - Diff);
  idx = IMAIndexMinMax(idx + IMA_IndexTable[data4bit & 7], 0, 88);
  *Index = idx;
  *Pcm16bit = pcm16bitLcl;
}

static void convertImaAdpcm(u8 *bufIn, u8 *bufOut, u32 dataOff, u32 dataLength) {
  u32 destOff = 0;
  u32 sampHeader = *(u32*)(bufIn + dataOff);
  int decompSample = sampHeader & 0xFFFF;
  int stepIndex = (sampHeader >> 16) & 0x7F;
  u32 curOffset = dataOff;
  ((s16 *)bufOut)[destOff++] = (s16)decompSample;

  u8 compByte;
  while (curOffset < dataOff + dataLength) {
    compByte = *(u8*)(bufIn + curOffset);
	curOffset++;
    process_nibble(compByte, &stepIndex, &decompSample);
    ((s16 *)bufOut)[destOff++] = (s16)decompSample;
    process_nibble((compByte & 0xF0) >> 4, &stepIndex, &decompSample);
    ((s16 *)bufOut)[destOff++] = (s16)decompSample;
  }
}


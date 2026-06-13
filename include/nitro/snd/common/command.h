#ifndef NITRO_SND_COMMON_COMMAND_H_
#define NITRO_SND_COMMON_COMMAND_H_

#include <nitro/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SND_COMMAND_NOBLOCK       0
#define SND_COMMAND_BLOCK        (1 << 0)
#define SND_COMMAND_IMMEDIATE    (1 << 1)

typedef enum SNDCommandID {
	SND_COMMAND_START_SEQ,
	SND_COMMAND_STOP_SEQ,
	SND_COMMAND_PREPARE_SEQ,
	SND_COMMAND_START_PREPARED_SEQ,
	SND_COMMAND_PAUSE_SEQ,
	SND_COMMAND_SKIP_SEQ,
	SND_COMMAND_PLAYER_PARAM,
	SND_COMMAND_TRACK_PARAM,
	SND_COMMAND_MUTE_TRACK,
	SND_COMMAND_ALLOCATABLE_CHANNEL,
	SND_COMMAND_PLAYER_LOCAL_VAR,
	SND_COMMAND_PLAYER_GLOBAL_VAR,
	SND_COMMAND_START_TIMER,
	SND_COMMAND_STOP_TIMER,
	SND_COMMAND_SETUP_CHANNEL_PCM,
	SND_COMMAND_SETUP_CHANNEL_PSG,
	SND_COMMAND_SETUP_CHANNEL_NOISE,
	SND_COMMAND_SETUP_CAPTURE,
	SND_COMMAND_SETUP_ALARM,
	SND_COMMAND_CHANNEL_TIMER,
	SND_COMMAND_CHANNEL_VOLUME,
	SND_COMMAND_CHANNEL_PAN,
	SND_COMMAND_SURROUND_DECAY,
	SND_COMMAND_MASTER_VOLUME,
	SND_COMMAND_MASTER_PAN,
	SND_COMMAND_OUTPUT_SELECTOR,
	SND_COMMAND_LOCK_CHANNEL,
	SND_COMMAND_UNLOCK_CHANNEL,
	SND_COMMAND_STOP_UNLOCKED_CHANNEL,
	SND_COMMAND_SHARED_WORK,
	SND_COMMAND_INVALIDATE_SEQ,
	SND_COMMAND_INVALIDATE_BANK,
	SND_COMMAND_INVALIDATE_WAVE,
	SND_COMMAND_READ_DRIVER_INFO
} SNDCommandID;

typedef struct SNDCommand {
	struct SNDCommand * next;
	SNDCommandID id;
    #ifdef SDK_PORT
    u64 arg[4];
    #else
	u32 arg[4];
    #endif
} SNDCommand;

void SND_CommandInit(void);

#if (defined(SDK_ARM9) || defined(SDK_PORT))
    struct SNDCommand * SND_AllocCommand(u32 flags);
    void SND_PushCommand(struct SNDCommand * command);

    const SNDCommand * SND_RecvCommandReply(u32 flags);

    BOOL SND_FlushCommand(u32 flags);

    void SND_WaitForCommandProc(u32 tag);
    void SND_WaitForFreeCommand(int count);

    u32 SND_GetCurrentCommandTag(void);
    BOOL SND_IsFinishedCommandTag(u32 tag);

    int SND_CountFreeCommand(void);
    int SND_CountReservedCommand(void);
    int SND_CountWaitingCommand(void);
#endif
#ifdef SDK_ARM7
    void SND_CommandProc(void);
#endif

#ifdef __cplusplus
}
#endif

#endif

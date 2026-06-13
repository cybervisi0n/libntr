

#ifndef LIBRARIES_SPI_SP_H_
#define LIBRARIES_SPI_SP_H_

#include    <nitro_sp.h>

#ifdef __cplusplus
extern "C" {
#endif

#define     SPI_THREAD_STACK_SIZE       512

#define     SPI_DEVICE_COMMAND_DUMMY    0x0000

#define     SPI_MESSAGE_ARRAY_MAX       16

#define     SPI_ENTRY_ARGS_MAX          4

typedef struct SPIEntry
{
    SPIDeviceType type;
    u32     process;
    u32     arg[SPI_ENTRY_ARGS_MAX];

}
SPIEntry;

typedef struct SPIWork
{
    BOOL    exception;
    SPIDeviceType type;

    OSThread thread;
    u64     stack[SPI_THREAD_STACK_SIZE / sizeof(u64)];
    OSMessageQueue message;
    OSMessage msg_buf[SPI_MESSAGE_ARRAY_MAX];
    SPIEntry entry[SPI_MESSAGE_ARRAY_MAX];
    u32     entryIndex;

    OSThreadQueue lock;

#ifndef SDK_THREAD_INFINITY
#if ( OS_THREAD_MAX_NUM <= 16 )
    u8      reserved[2];
#endif
#endif

    u32     lockId;

}
SPIWork;

void    SPI_Lock(u32 id);
void    SPI_Unlock(u32 id);

void    SPIi_ReturnResult(u16 command, u16 result);
BOOL    SPIi_CheckException(SPIDeviceType type);
void    SPIi_GetException(SPIDeviceType type);
void    SPIi_ReleaseException(SPIDeviceType type);
BOOL    SPIi_SetEntry(SPIDeviceType type, u32 process, u16 args, ...);
BOOL    SPIi_CheckEntry(void);

static inline void SPI_Wait(void)
{
    while (reg_SPI_SPICNT & REG_SPI_SPICNT_BUSY_MASK)
    {
    }
}

static inline void SPI_Dummy(void)
{
    reg_SPI_SPID = SPI_DEVICE_COMMAND_DUMMY;
}

static inline void SPI_Send(u16 data)
{
    reg_SPI_SPID = (u16)(data & 0x00ff);
}

static inline u16 SPI_Receive(void)
{
    return (u16)(reg_SPI_SPID & 0x00ff);
}

static inline void SPI_DummyWait(void)
{
    SPI_Dummy();
    SPI_Wait();
}

static inline void SPI_SendWait(u16 data)
{
    SPI_Send(data);
    SPI_Wait();
}

static inline u16 SPI_DummyWaitReceive(void)
{
    SPI_Dummy();
    SPI_Wait();
    return SPI_Receive();
}

static inline u16 SPI_SendWaitReceive(u16 data)
{
    SPI_Send(data);
    SPI_Wait();
    return SPI_Receive();
}

#ifdef __cplusplus
} 
#endif

#endif 


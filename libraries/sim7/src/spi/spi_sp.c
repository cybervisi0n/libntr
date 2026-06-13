
#include    <nitro_sp.h>
#ifdef SDK_PORT
#include <nitro/spi/ARM7/spi.h>

#include    "ARM7/spi_sp.h"
#include    "ARM7/tp_sp.h"
#else
#include    "spi_sp.h"
#include    "tp_sp.h"
#endif

#ifndef SDK_SMALL_BUILD
#ifdef SDK_PORT
#include    "ARM7/mic_sp.h"
#else
#include    "mic_sp.h"
#endif
#endif

#ifndef SDK_TEG
#ifdef SDK_PORT
#include    "ARM7/pm_sp.h"
#else
#include    "pm_sp.h"
#endif
#endif

#ifdef SDK_PORT
#include    "ARM7/nvram_sp.h"
#else
#include    "nvram_sp.h"
#endif

static u16 spiInitialized;

static SPIWork spiWork;

static void SpiCommonThread(void *arg);
#ifdef SDK_PORT
static void SpiPxiCallback(PXIFifoTag tag, u64 data, BOOL err);
#else
static void SpiPxiCallback(PXIFifoTag tag, u32 data, BOOL err);
#endif

void SPI_Init(u32 prio)
{
    if (spiInitialized)
    {
        return;
    }
    spiInitialized = 1;

    spiWork.exception = FALSE;
    spiWork.type = SPI_DEVICE_TYPE_MAX;
    #ifndef SDK_PORT
    TP_Init();
    #endif
    NVRAM_Init();
#ifndef SDK_PORT
#ifndef SDK_SMALL_BUILD
    MIC_Init();
#endif
#ifndef SDK_TEG
    PM_Init();
#endif
#endif

#ifndef SDK_PORT
    PXI_Init();
#endif
    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_TOUCHPANEL, SpiPxiCallback);
#ifndef SDK_SMALL_BUILD
    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_MIC, SpiPxiCallback);
#endif
#ifndef SDK_TEG
    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_PM, SpiPxiCallback);
#endif
    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_NVRAM, SpiPxiCallback);

    OS_InitMessageQueue(&(spiWork.message), spiWork.msg_buf, SPI_MESSAGE_ARRAY_MAX);
    {
        s32     i;

        for (i = 0; i < SPI_MESSAGE_ARRAY_MAX; i++)
        {
            MI_CpuFill8(&spiWork.entry[i], 0, sizeof(SPIEntry));
        }
        spiWork.entryIndex = 0;
    }

    OS_InitThreadQueue(&(spiWork.lock));
    OS_CreateThread(&(spiWork.thread),
                    SpiCommonThread,
                    0,
                    (void *)(spiWork.stack + (SPI_THREAD_STACK_SIZE / sizeof(u64))),
                    SPI_THREAD_STACK_SIZE, prio);
#ifndef SDK_PORT
    OS_WakeupThreadDirect(&(spiWork.thread));
#endif
}

void SPI_Lock(u32 id)
{
    OSIntrMode enabled;

    while (TRUE)
    {
        enabled = OS_DisableInterrupts();
        if (spiWork.exception)
        {
            (void)OS_RestoreInterrupts(enabled);
            OS_SleepThread(&(spiWork.lock));
            continue;
        }
        else
        {
            SPIi_GetException(SPI_DEVICE_TYPE_ARM7);
            spiWork.lockId = id;
            (void)OS_RestoreInterrupts(enabled);
            break;
        }
    }
}

void SPI_Unlock(u32 id)
{
    OSIntrMode enabled;

    if (spiWork.exception && (spiWork.type == SPI_DEVICE_TYPE_ARM7) && (spiWork.lockId == id))
    {
        enabled = OS_DisableInterrupts();
        spiWork.type = SPI_DEVICE_TYPE_MAX;
        spiWork.exception = FALSE;
        spiWork.lockId = 0;
        (void)OS_RestoreInterrupts(enabled);
        OS_WakeupThread(&(spiWork.lock));
    }
}

void SPIi_ReturnResult(u16 command, u16 result)
{
    PXIFifoTag tag;

    switch (command & 0x0070)
    {

    case 0x0000:
    case 0x0010:
        tag = PXI_FIFO_TAG_TOUCHPANEL;
        break;

#ifndef SDK_SMALL_BUILD
    case 0x0040:
    case 0x0050:
        tag = PXI_FIFO_TAG_MIC;
        break;
#endif

#ifndef SDK_TEG
    case 0x0060:
    case 0x0070:
        tag = PXI_FIFO_TAG_PM;
        break;
#endif
    case 0x0020:
    case 0x0030:
        tag = PXI_FIFO_TAG_NVRAM;
        break;
    }

    while (0 > PXI_SendWordByFifo(tag,
                                  SPI_PXI_START_BIT |
                                  SPI_PXI_END_BIT |
                                  (0 << SPI_PXI_INDEX_SHIFT) |
                                  ((u32)((command & 0x00ff) | 0x0080) << 8) |
                                  (u32)(result & 0x00ff), 0))
    {
    }
}

BOOL SPIi_CheckException(SPIDeviceType type)
{
#pragma unused( type )

    if (spiWork.exception)
    {
        return FALSE;
    }
    return TRUE;
}

void SPIi_GetException(SPIDeviceType type)
{
    spiWork.exception = TRUE;
    spiWork.type = type;
}

void SPIi_ReleaseException(SPIDeviceType type)
{
    if (spiWork.type == type)
    {
        spiWork.type = SPI_DEVICE_TYPE_MAX;
        spiWork.exception = FALSE;
        OS_WakeupThread(&(spiWork.lock));
    }
}

BOOL SPIi_SetEntry(SPIDeviceType type, u32 process, u16 args, ...)
{
    OSIntrMode e;
    void   *w;
    va_list vlist;
    s32     i;

    if (args > SPI_ENTRY_ARGS_MAX)
    {
        return FALSE;
    }

    e = OS_DisableInterrupts();
    spiWork.entry[spiWork.entryIndex].type = type;
    spiWork.entry[spiWork.entryIndex].process = process;

    va_start(vlist, args);
    for (i = 0; i < args; i++)
    {
        spiWork.entry[spiWork.entryIndex].arg[i] = va_arg(vlist, u32);
    }
    va_end(vlist);

    w = &(spiWork.entry[spiWork.entryIndex]);
    spiWork.entryIndex = (u32)((spiWork.entryIndex + 1) % SPI_MESSAGE_ARRAY_MAX);
    (void)OS_RestoreInterrupts(e);
    return OS_SendMessage(&(spiWork.message), w, OS_MESSAGE_NOBLOCK);
}

BOOL SPIi_CheckEntry(void)
{
    OSMessage msg;
    BOOL    result;

    result = OS_ReadMessage(&(spiWork.message), &msg, OS_MESSAGE_NOBLOCK);
    return result;
}

static void SpiCommonThread(void *arg)
{
#pragma unused(arg)

    OSMessage msg;
    SPIEntry *entry;

    while (TRUE)
    {
        (void)OS_ReceiveMessage(&(spiWork.message), &msg, OS_MESSAGE_BLOCK);

        entry = (SPIEntry *) msg;

        switch (entry->type)
        {
        case SPI_DEVICE_TYPE_TP:
            #ifdef SDK_BUILD_ARM
            TP_ExecuteProcess(entry);
            #endif
            break;

#ifndef SDK_SMALL_BUILD
        case SPI_DEVICE_TYPE_MIC:
            #ifdef SDK_BUILD_ARM
            MIC_ExecuteProcess(entry);
            #endif
            break;
#endif

#ifndef SDK_TEG
        case SPI_DEVICE_TYPE_PM:
            #ifdef SDK_BUILD_ARM
            PM_ExecuteProcess(entry);
            #endif
            break;
#endif

        case SPI_DEVICE_TYPE_NVRAM:
            NVRAM_ExecuteProcess(entry);
            break;
        }
    }
}

#ifdef SDK_PORT
static void SpiPxiCallback(PXIFifoTag tag, u64 data, BOOL err)
#else
static void SpiPxiCallback(PXIFifoTag tag, u32 data, BOOL err)
#endif
{
    if (err)
    {
        return;
    }

    switch (tag)
    {
    case PXI_FIFO_TAG_TOUCHPANEL:
        #ifdef SDK_BUILD_ARM
        TP_AnalyzeCommand(data);
        #endif
        break;

#ifndef SDK_SMALL_BUILD
    case PXI_FIFO_TAG_MIC:
        #ifdef SDK_BUILD_ARM
        MIC_AnalyzeCommand(data);
        #endif
        break;
#endif

#ifndef SDK_TEG
    case PXI_FIFO_TAG_PM:
        #ifdef SDK_BUILD_ARM
        PM_AnalyzeCommand(data);
        #endif
        break;
#endif

    case PXI_FIFO_TAG_NVRAM:
        NVRAM_AnalyzeCommand(data);
        break;
    }
}


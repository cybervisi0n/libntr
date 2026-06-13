
#ifdef SDK_PORT
#include <nitro/spi/ARM7/spi.h>

#include "ARM7/spi_sp.h"
#include "ARM7/nvram_sp.h"
#else
#include    "spi_sp.h"
#include    "nvram_sp.h"
#endif


static NVRAMWork nvramw;


static BOOL NvramCheckReadyToRead(void);
static BOOL NvramCheckReadyToWrite(void);


void NVRAM_Init(void)
{
    s32     i;

    for (i = 0; i < SPI_PXI_CONTINUOUS_PACKET_MAX; i++)
    {
        nvramw.command[i] = 0x0000;
    }
}

void NVRAM_AnalyzeCommand(u32 data)
{
    if (data & SPI_PXI_START_BIT)
    {
        s32     i;

        for (i = 0; i < SPI_PXI_CONTINUOUS_PACKET_MAX; i++)
        {
            nvramw.command[i] = 0x0000;
        }
    }
    nvramw.command[(data & SPI_PXI_INDEX_MASK) >> SPI_PXI_INDEX_SHIFT] = (u16)((data &
                                                                                SPI_PXI_DATA_MASK)
                                                                               >>
                                                                               SPI_PXI_DATA_SHIFT);

    if (data & SPI_PXI_END_BIT)
    {
        u16     command;
        u32     wu32;
        u32     buf;
        u32     addr;
        u32     size;

        command = (u16)((nvramw.command[0] & 0xff00) >> 8);

        switch (command)
        {
        case SPI_PXI_COMMAND_NVRAM_RDSR:
#ifdef  SDK_NVRAM_ANOTHER_MAKER
        case SPI_PXI_COMMAND_NVRAM_RSI:
#endif
            wu32 = (((u32)(nvramw.command[0] & 0x00ff) << 24) |
                    ((u32)(nvramw.command[1]) << 8) | ((u32)(nvramw.command[2] & 0xff00) >> 8));
            if ((wu32 < HW_MAIN_MEM) || (wu32 >= (HW_MAIN_MEM + HW_MAIN_MEM_EX_SIZE)))
            {
                SPIi_ReturnResult(command, SPI_PXI_RESULT_INVALID_PARAMETER);
                return;
            }
            buf = wu32;
            break;

#ifdef  SDK_NVRAM_USE_READ_HIGHER_SPEED
        case SPI_PXI_COMMAND_NVRAM_FAST_READ:
#endif
        case SPI_PXI_COMMAND_NVRAM_READ:
            wu32 = (((u32)nvramw.command[4] << 16) | ((u32)nvramw.command[5]));
            if ((wu32 < HW_MAIN_MEM) || (wu32 >= (HW_MAIN_MEM + HW_MAIN_MEM_EX_SIZE)))
            {
                SPIi_ReturnResult(command, SPI_PXI_RESULT_INVALID_PARAMETER);
                return;
            }
            buf = wu32;
            addr = (((u32)(nvramw.command[0] & 0x00ff) << 16) | ((u32)nvramw.command[1]));
            size = (((u32)nvramw.command[2] << 16) | ((u32)nvramw.command[3]));
            break;

        case SPI_PXI_COMMAND_NVRAM_PW:
#ifndef SDK_SMALL_BUILD
        case SPI_PXI_COMMAND_NVRAM_PP:
#endif
            wu32 = (((u32)nvramw.command[3] << 16) | ((u32)nvramw.command[4]));
            if ((wu32 < HW_MAIN_MEM) || (wu32 >= (HW_MAIN_MEM + HW_MAIN_MEM_EX_SIZE)))
            {
                SPIi_ReturnResult(command, SPI_PXI_RESULT_INVALID_PARAMETER);
                return;
            }
            buf = wu32;
            addr = (((u32)(nvramw.command[0] & 0x00ff) << 16) | ((u32)nvramw.command[1]));
            size = (u32)nvramw.command[2];
            break;

        case SPI_PXI_COMMAND_NVRAM_PE:
#ifndef SDK_SMALL_BUILD
        case SPI_PXI_COMMAND_NVRAM_SE:
#endif
            addr = (((u32)(nvramw.command[0] & 0x00ff) << 16) | ((u32)nvramw.command[1]));
            break;
        }

        if (!SPIi_SetEntry(SPI_DEVICE_TYPE_NVRAM, (u32)command, 3, addr, size, buf))
        {
            SPIi_ReturnResult(command, SPI_PXI_RESULT_EXCLUSIVE);
        }
    }
}

void NVRAM_ExecuteProcess(SPIEntry * entry)
{
    {
        OSIntrMode e;

        e = OS_DisableInterrupts();
        if (!SPIi_CheckException(SPI_DEVICE_TYPE_NVRAM))
        {
            (void)OS_RestoreInterrupts(e);
            SPIi_ReturnResult((u16)(entry->process), SPI_PXI_RESULT_EXCLUSIVE);
            return;
        }
        SPIi_GetException(SPI_DEVICE_TYPE_NVRAM);
        (void)OS_RestoreInterrupts(e);
    }

    switch (entry->process)
    {

    case SPI_PXI_COMMAND_NVRAM_WREN:
        NVRAM_WriteEnable();
        break;

    case SPI_PXI_COMMAND_NVRAM_WRDI:
        NVRAM_WriteDisable();
        break;

    case SPI_PXI_COMMAND_NVRAM_RDSR:
        NVRAM_ReadStatusRegister((u8 *)(entry->arg[2]));
        break;

    case SPI_PXI_COMMAND_NVRAM_READ:
        if (!NvramCheckReadyToRead())
        {
            SPIi_ReturnResult((u16)(entry->process), SPI_PXI_RESULT_ILLEGAL_STATUS);
            SPIi_ReleaseException(SPI_DEVICE_TYPE_NVRAM);
            return;
        }
        NVRAM_ReadDataBytes(entry->arg[0], entry->arg[1], (u8 *)(entry->arg[2]));
        break;

#ifdef  SDK_NVRAM_USE_READ_HIGHER_SPEED
    case SPI_PXI_COMMAND_NVRAM_FAST_READ:
        if (!NvramCheckReadyToRead())
        {
            SPIi_ReturnResult((u16)(entry->process), SPI_PXI_RESULT_ILLEGAL_STATUS);
            SPIi_ReleaseException(SPI_DEVICE_TYPE_NVRAM);
            return;
        }
        NVRAM_ReadDataBytesAtHigherSpeed(entry->arg[0], entry->arg[1], (u8 *)(entry->arg[2]));
        break;
#endif

    case SPI_PXI_COMMAND_NVRAM_PW:
        if (!NvramCheckReadyToWrite())
        {
            SPIi_ReturnResult((u16)(entry->process), SPI_PXI_RESULT_ILLEGAL_STATUS);
            SPIi_ReleaseException(SPI_DEVICE_TYPE_NVRAM);
            return;
        }
        NVRAM_PageWrite(entry->arg[0], (u16)(entry->arg[1]), (const u8 *)(entry->arg[2]));
        break;

#ifndef SDK_SMALL_BUILD
    case SPI_PXI_COMMAND_NVRAM_PP:
        if (!NvramCheckReadyToWrite())
        {
            SPIi_ReturnResult((u16)(entry->process), SPI_PXI_RESULT_ILLEGAL_STATUS);
            SPIi_ReleaseException(SPI_DEVICE_TYPE_NVRAM);
            return;
        }
        NVRAM_PageProgram(entry->arg[0], (u16)(entry->arg[1]), (const u8 *)(entry->arg[2]));
        break;
#endif

    case SPI_PXI_COMMAND_NVRAM_PE:
        if (!NvramCheckReadyToWrite())
        {
            SPIi_ReturnResult((u16)(entry->process), SPI_PXI_RESULT_ILLEGAL_STATUS);
            SPIi_ReleaseException(SPI_DEVICE_TYPE_NVRAM);
            return;
        }
        NVRAM_PageErase(entry->arg[0]);
        break;

#ifndef SDK_SMALL_BUILD
    case SPI_PXI_COMMAND_NVRAM_SE:
        if (!NvramCheckReadyToWrite())
        {
            SPIi_ReturnResult((u16)(entry->process), SPI_PXI_RESULT_ILLEGAL_STATUS);
            SPIi_ReleaseException(SPI_DEVICE_TYPE_NVRAM);
            return;
        }
        NVRAM_SectorErase(entry->arg[0]);
        break;

    case SPI_PXI_COMMAND_NVRAM_DP:
        NVRAM_DeepPowerDown();
        break;

    case SPI_PXI_COMMAND_NVRAM_RDP:
        NVRAM_ReleaseFromDeepPowerDown();
        break;
#endif

#ifdef  SDK_NVRAM_ANOTHER_MAKER
    case SPI_PXI_COMMAND_NVRAM_CE:
        if (!NvramCheckReadyToWrite())
        {
            SPIi_ReturnResult((u16)(entry->process), SPI_PXI_RESULT_ILLEGAL_STATUS);
            SPIi_ReleaseException(SPI_DEVICE_TYPE_NVRAM);
            return;
        }
        NVRAM_ChipErase();
        break;

    case SPI_PXI_COMMAND_NVRAM_RSI:
        NVRAM_ReadSiliconId((u8 *)(entry->arg[2]));
        break;

    case SPI_PXI_COMMAND_NVRAM_SR:
        NVRAM_SoftwareReset();
        break;
#endif

    default:
        SPIi_ReleaseException(SPI_DEVICE_TYPE_NVRAM);
        SPIi_ReturnResult((u16)(entry->process), SPI_PXI_RESULT_INVALID_COMMAND);
        return;
    }

    SPIi_ReturnResult((u16)(entry->process), SPI_PXI_RESULT_SUCCESS);
    SPIi_ReleaseException(SPI_DEVICE_TYPE_NVRAM);
}

static BOOL NvramCheckReadyToRead(void)
{
    u16     tempStatus;

    NVRAM_ReadStatusRegister((u8 *)(&tempStatus));
    if (tempStatus & NVRAM_STATUS_REGISTER_WIP)
    {
        return FALSE;
    }
    return TRUE;
}

static BOOL NvramCheckReadyToWrite(void)
{
    u16     tempStatus;

    NVRAM_ReadStatusRegister((u8 *)(&tempStatus));
    if (tempStatus & NVRAM_STATUS_REGISTER_WIP)
    {
        return FALSE;
    }
    if (!(tempStatus & NVRAM_STATUS_REGISTER_WEL))
    {
        return FALSE;
    }
    return TRUE;
}


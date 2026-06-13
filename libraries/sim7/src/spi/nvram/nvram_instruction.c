
#include    <nitro_sp.h>
#ifdef SDK_PORT
#include <nitro/spi/ARM7/spi.h>

#include    "ARM7/spi_sp.h"
#include    "ARM7/nvram_sp.h"
#else
#include    "spi_sp.h"
#include    "nvram_sp.h"
#endif

#ifndef SDK_BUILD_ARM
#include <stdio.h>

static FILE * s_nvramFilePtr = NULL;
#define NVRAM_FILE_PATH "firmware.bin"
#endif

void NVRAM_WriteEnable(void)
{
    SPI_Wait();
    NVRAM_SPIChangeMode(SPI_TRANSMODE_1BYTE);
    SPI_SendWait(NVRAM_INSTRUCTION_WREN);
}

void NVRAM_WriteDisable(void)
{
    SPI_Wait();
    NVRAM_SPIChangeMode(SPI_TRANSMODE_1BYTE);
    SPI_SendWait(NVRAM_INSTRUCTION_WRDI);
}

void NVRAM_ReadStatusRegister(u8 *buf)
{
    SPI_Wait();
    NVRAM_SPIChangeMode(SPI_TRANSMODE_CONTINUOUS);
    SPI_SendWait(NVRAM_INSTRUCTION_RDSR);
    NVRAM_SPIChangeMode(SPI_TRANSMODE_1BYTE);
    SPI_Dummy();

#ifdef  SDK_SPI_LOW_SPEED_LOW_CODE_SIZE
    SPI_Wait();
    MI_WriteByte((void *)buf, (u8)SPI_Receive());
#else
    {
        u16     r;
        u16     temp;

        if ((u32)buf % 2)
        {
            temp = *((u16 *)(buf - 1));
            SPI_Wait();
            r = SPI_Receive();
            temp = (u16)((temp & 0x00ff) | ((u16)r << 8));
            *((u16 *)(buf - 1)) = temp;
        }
        else
        {
            temp = *((u16 *)buf);
            SPI_Wait();
            r = SPI_Receive();
            temp = (u16)((temp & 0xff00) | (u16)r);
            *((u16 *)buf) = temp;
        }
    }
#endif
}

void NVRAM_ReadDataBytes(u32 address, u32 size, u8 *buf)
{
    u16     adr[3];
    s32     i;

    if (size < 1)
    {
        return;
    }

    #ifdef SDK_BUILD_ARM

    adr[0] = (u16)((address & 0x00ff0000) >> 16);
    adr[1] = (u16)((address & 0x0000ff00) >> 8);
    adr[2] = (u16)(address & 0x000000ff);

    SPI_Wait();
    NVRAM_SPIChangeMode(SPI_TRANSMODE_CONTINUOUS);
    SPI_Send(NVRAM_INSTRUCTION_READ);
    for (i = 0; i < 3; i++)
    {
        SPI_Wait();
        SPI_Send(adr[i]);
    }

#ifdef  SDK_SPI_LOW_SPEED_LOW_CODE_SIZE
    SPI_Wait();
    for (i = 0; i < (size - 1); i++)
    {
        SPI_Dummy();
        SPI_Wait();
        MI_WriteByte((void *)(buf + i), (u8)SPI_Receive());
    }
    NVRAM_SPIChangeMode(SPI_TRANSMODE_1BYTE);
    SPI_DummyWait();
    MI_WriteByte((void *)(buf + i), (u8)SPI_Receive());
#else
    {
        s32     j;
        u16     r;
        u16     temp;

        if (size == 1)
        {
            SPI_Wait();
            NVRAM_SPIChangeMode(SPI_TRANSMODE_1BYTE);
            SPI_Dummy();
            if ((u32)buf % 2)
            {
                temp = *((u16 *)(buf - 1));
                SPI_Wait();
                r = SPI_Receive();
                temp = (u16)((temp & 0x00ff) | (r << 8));
                *((u16 *)(buf - 1)) = temp;
            }
            else
            {
                temp = *((u16 *)buf);
                SPI_Wait();
                r = SPI_Receive();
                temp = (u16)((temp & 0xff00) | r);
                *((u16 *)buf) = temp;
            }
        }
        else
        {
            SPI_Wait();
            SPI_Dummy();
            if ((u32)buf % 2)
            {
                temp = *((u16 *)(buf - 1));
                SPI_Wait();
                r = SPI_Receive();
                SPI_Dummy();
                temp = (u16)((temp & 0x00ff) | (r << 8));
                *((u16 *)(buf - 1)) = temp;
                i = 1;
            }
            else
            {
                i = 0;
            }
            for (j = 0; i < (size - 1); i++, j++)
            {
                if (j % 2)
                {
                    SPI_Wait();
                    temp |= (u16)((u16)SPI_Receive() << 8);
                    SPI_Dummy();
                    *((u16 *)(buf + i - 1)) = temp;
                }
                else
                {
                    SPI_Wait();
                    temp = (u16)SPI_Receive();
                    SPI_Dummy();
                }
            }
            if (j % 2)
            {
                SPI_Wait();
                temp |= (u16)((u16)SPI_Receive() << 8);
                NVRAM_SPIChangeMode(SPI_TRANSMODE_1BYTE);
                SPI_Dummy();
                *((u16 *)(buf + i - 1)) = temp;
            }
            else
            {
                temp = *((u16 *)(buf + i));
                SPI_Wait();
                r = SPI_Receive();
                NVRAM_SPIChangeMode(SPI_TRANSMODE_1BYTE);
                SPI_Dummy();
                temp = (u16)((temp & 0xff00) | r);
                *((u16 *)(buf + i)) = temp;
            }
            SPI_Wait();
        }
    }
#endif

#else
    if(s_nvramFilePtr == NULL) {
        s_nvramFilePtr = fopen(NVRAM_FILE_PATH, "rb+");
    }

    fseek(s_nvramFilePtr, address, SEEK_SET);
    fread(buf, 1, size, s_nvramFilePtr);
    fclose(s_nvramFilePtr);
    s_nvramFilePtr = NULL;
#endif

}

#ifdef  SDK_NVRAM_USE_READ_HIGHER_SPEED
void NVRAM_ReadDataBytesAtHigherSpeed(u32 address, u32 size, u8 *buf)
{
    u16     adr[3];
    s32     i;

    if (size < 1)
    {
        return;
    }

    adr[0] = (u16)((address & 0x00ff0000) >> 16);
    adr[1] = (u16)((address & 0x0000ff00) >> 8);
    adr[2] = (u16)(address & 0x000000ff);

    SPI_Wait();
    NVRAM_SPIChangeMode(SPI_TRANSMODE_CONTINUOUS);
    SPI_Send(NVRAM_INSTRUCTION_FAST_READ);
    for (i = 0; i < 3; i++)
    {
        SPI_Wait();
        SPI_Send(adr[i]);
    }
    SPI_Wait();
    SPI_Dummy();

#ifdef  SDK_SPI_LOW_SPEED_LOW_CODE_SIZE
    SPI_Wait();
    for (i = 0; i < (size - 1); i++)
    {
        SPI_Dummy();
        SPI_Wait();
        MI_WriteByte((void *)(buf + i), (u8)SPI_Receive());
    }
    NVRAM_SPIChangeMode(SPI_TRANSMODE_1BYTE);
    SPI_DummyWait();
    MI_WriteByte((void *)(buf + i), (u8)SPI_Receive());
#else
    {
        s32     j;
        u16     r;
        u16     temp;

        if (size == 1)
        {
            SPI_Wait();
            NVRAM_SPIChangeMode(SPI_TRANSMODE_1BYTE);
            SPI_Dummy();
            if ((u32)buf % 2)
            {
                temp = *((u16 *)(buf - 1));
                SPI_Wait();
                r = SPI_Receive();
                temp = (u16)((temp & 0x00ff) | (r << 8));
                *((u16 *)(buf - 1)) = temp;
            }
            else
            {
                temp = *((u16 *)buf);
                SPI_Wait();
                r = SPI_Receive();
                temp = (u16)((temp & 0xff00) | r);
                *((u16 *)buf) = temp;
            }
        }
        else
        {
            SPI_Wait();
            SPI_Dummy();
            if ((u32)buf % 2)
            {
                temp = *((u16 *)(buf - 1));
                SPI_Wait();
                r = SPI_Receive();
                SPI_Dummy();
                temp = (u16)((temp & 0x00ff) | (r << 8));
                *((u16 *)(buf - 1)) = temp;
                i = 1;
            }
            else
            {
                i = 0;
            }
            for (j = 0; i < (size - 1); i++, j++)
            {
                if (j % 2)
                {
                    SPI_Wait();
                    temp |= (u16)((u16)SPI_Receive() << 8);
                    SPI_Dummy();
                    *((u16 *)(buf + i - 1)) = temp;
                }
                else
                {
                    SPI_Wait();
                    temp = (u16)SPI_Receive();
                    SPI_Dummy();
                }
            }
            if (j % 2)
            {
                SPI_Wait();
                temp |= (u16)((u16)SPI_Receive() << 8);
                NVRAM_SPIChangeMode(SPI_TRANSMODE_1BYTE);
                SPI_Dummy();
                *((u16 *)(buf + i - 1)) = temp;
            }
            else
            {
                temp = *((u16 *)(buf + i));
                SPI_Wait();
                r = SPI_Receive();
                NVRAM_SPIChangeMode(SPI_TRANSMODE_1BYTE);
                SPI_Dummy();
                temp = (u16)((temp & 0xff00) | r);
                *((u16 *)(buf + i)) = temp;
            }
            SPI_Wait();
        }
    }
#endif
}
#endif

void NVRAM_PageWrite(u32 address, u16 size, const u8 *buf)
{
    u16     adr[3];
    s32     i;

    if (size < 1)
    {
        return;
    }

    if (((address + size - 1) / SPI_NVRAM_PAGE_SIZE) > (address / SPI_NVRAM_PAGE_SIZE))
    {
        size = (u16)(SPI_NVRAM_PAGE_SIZE - (address % SPI_NVRAM_PAGE_SIZE));
    }

    adr[0] = (u16)((address & 0x00ff0000) >> 16);
    adr[1] = (u16)((address & 0x0000ff00) >> 8);
    adr[2] = (u16)(address & 0x000000ff);

    SPI_Wait();
    NVRAM_SPIChangeMode(SPI_TRANSMODE_CONTINUOUS);
    SPI_Send(NVRAM_INSTRUCTION_PW);
    for (i = 0; i < 3; i++)
    {
        SPI_Wait();
        SPI_Send(adr[i]);
    }

#ifdef  SDK_SPI_LOW_SPEED_LOW_CODE_SIZE
    for (i = 0; i < (size - 1); i++)
    {
        SPI_Wait();
        SPI_Send((u16)MI_ReadByte((void *)(buf + i)));
    }
    SPI_Wait();
    NVRAM_SPIChangeMode(SPI_TRANSMODE_1BYTE);
    SPI_SendWait((u16)MI_ReadByte((void *)(buf + i)));
#else
    {
        u16     temp;

        if ((u32)buf % 2)
        {
            temp = *((u16 *)(buf - 1));
        }
        for (i = 0; i < (size - 1); i++)
        {
            if (((u32)buf + i) % 2)
            {
                SPI_Wait();
                SPI_Send((u16)((temp & 0xff00) >> 8));
            }
            else
            {
                temp = *((u16 *)(buf + i));
                SPI_Wait();
                SPI_Send((u16)(temp & 0x00ff));
            }
        }
        if (((u32)buf + i) % 2)
        {
            SPI_Wait();
            NVRAM_SPIChangeMode(SPI_TRANSMODE_1BYTE);
            SPI_Send((u16)((temp & 0xff00) >> 8));
        }
        else
        {
            temp = *((u16 *)(buf + i));
            SPI_Wait();
            NVRAM_SPIChangeMode(SPI_TRANSMODE_1BYTE);
            SPI_Send((u16)(temp & 0x00ff));
        }
        SPI_Wait();
    }
#endif
}

void NVRAM_PageProgram(u32 address, u16 size, const u8 *buf)
{
    u16     adr[3];
    s32     i;

    if (size < 1)
    {
        return;
    }

    if (((address + size - 1) / SPI_NVRAM_PAGE_SIZE) > (address / SPI_NVRAM_PAGE_SIZE))
    {
        size = (u16)(SPI_NVRAM_PAGE_SIZE - (address % SPI_NVRAM_PAGE_SIZE));
    }

    adr[0] = (u16)((address & 0x00ff0000) >> 16);
    adr[1] = (u16)((address & 0x0000ff00) >> 8);
    adr[2] = (u16)(address & 0x000000ff);

    SPI_Wait();
    NVRAM_SPIChangeMode(SPI_TRANSMODE_CONTINUOUS);
    SPI_Send(NVRAM_INSTRUCTION_PP);
    for (i = 0; i < 3; i++)
    {
        SPI_Wait();
        SPI_Send(adr[i]);
    }

#ifdef  SDK_SPI_LOW_SPEED_LOW_CODE_SIZE
    for (i = 0; i < (size - 1); i++)
    {
        SPI_Wait();
        SPI_Send((u16)MI_ReadByte((void *)(buf + i)));
    }
    SPI_Wait();
    NVRAM_SPIChangeMode(SPI_TRANSMODE_1BYTE);
    SPI_SendWait((u16)MI_ReadByte((void *)(buf + i)));
#else
    {
        u16     temp;

        if ((u32)buf % 2)
        {
            temp = *((u16 *)(buf - 1));
        }
        for (i = 0; i < (size - 1); i++)
        {
            if (((u32)buf + i) % 2)
            {
                SPI_Wait();
                SPI_Send((u16)((temp & 0xff00) >> 8));
            }
            else
            {
                temp = *((u16 *)(buf + i));
                SPI_Wait();
                SPI_Send((u16)(temp & 0x00ff));
            }
        }
        if (((u32)buf + i) % 2)
        {
            SPI_Wait();
            NVRAM_SPIChangeMode(SPI_TRANSMODE_1BYTE);
            SPI_Send((u16)((temp & 0xff00) >> 8));
        }
        else
        {
            temp = *((u16 *)(buf + i));
            SPI_Wait();
            NVRAM_SPIChangeMode(SPI_TRANSMODE_1BYTE);
            SPI_Send((u16)(temp & 0x00ff));
        }
        SPI_Wait();
    }
#endif
}

void NVRAM_PageErase(u32 address)
{
    u16     adr[3];

    adr[0] = (u16)((address & 0x00ff0000) >> 16);
    adr[1] = (u16)((address & 0x0000ff00) >> 8);
    adr[2] = (u16)(address & 0x000000ff);

    SPI_Wait();
    NVRAM_SPIChangeMode(SPI_TRANSMODE_CONTINUOUS);
    SPI_SendWait(NVRAM_INSTRUCTION_PE);
    SPI_SendWait(adr[0]);
    SPI_SendWait(adr[1]);
    NVRAM_SPIChangeMode(SPI_TRANSMODE_1BYTE);
    SPI_SendWait(adr[2]);
}

void NVRAM_SectorErase(u32 address)
{
    u16     adr[3];

    adr[0] = (u16)((address & 0x00ff0000) >> 16);
    adr[1] = (u16)((address & 0x0000ff00) >> 8);
    adr[2] = (u16)(address & 0x000000ff);

    SPI_Wait();
    NVRAM_SPIChangeMode(SPI_TRANSMODE_CONTINUOUS);
    SPI_SendWait(NVRAM_INSTRUCTION_SE);
    SPI_SendWait(adr[0]);
    SPI_SendWait(adr[1]);
    NVRAM_SPIChangeMode(SPI_TRANSMODE_1BYTE);
    SPI_SendWait(adr[2]);
}

void NVRAM_DeepPowerDown(void)
{
    SPI_Wait();
    NVRAM_SPIChangeMode(SPI_TRANSMODE_1BYTE);
    SPI_SendWait(NVRAM_INSTRUCTION_DP);
}

void NVRAM_ReleaseFromDeepPowerDown(void)
{
    SPI_Wait();
    NVRAM_SPIChangeMode(SPI_TRANSMODE_1BYTE);
    SPI_SendWait(NVRAM_INSTRUCTION_RDP);
}

#ifdef  SDK_NVRAM_ANOTHER_MAKER
void NVRAM_ChipErase(void)
{
    SPI_Wait();
    NVRAM_SPIChangeMode(SPI_TRANSMODE_1BYTE);
    SPI_SendWait(NVRAM_INSTRUCTION_CE);
}

void NVRAM_ReadSiliconId(u8 *buf)
{
    SPI_Wait();
    NVRAM_SPIChangeMode(SPI_TRANSMODE_CONTINUOUS);
    SPI_SendWait(NVRAM_INSTRUCTION_RSI);
    SPI_Dummy();

#ifdef  SDK_SPI_LOW_SPEED_LOW_CODE_SIZE
    SPI_Wait();
    MI_WriteByte(buf, (u8)SPI_Receive());
    NVRAM_SPIChangeMode(SPI_TRANSMODE_1BYTE);
    SPI_Dummy();
    SPI_Wait();
    MI_WriteByte((u8 *)(buf + 1), (u8)SPI_Receive());
#else
    {
        u16     r;
        u16     temp;
        s32     i;

        for (i = 0; i < 2; i++)
        {
            if ((u32)buf % 2)
            {
                temp = *((u16 *)(buf - 1));
                SPI_Wait();
                r = SPI_Receive();
                temp = (u16)((temp & 0x00ff) | ((u16)r << 8));
                *((u16 *)(buf - 1)) = temp;
            }
            else
            {
                temp = *((u16 *)buf);
                SPI_Wait();
                r = SPI_Receive();
                temp = (u16)((temp & 0xff00) | (u16)r);
                *((u16 *)(buf - 1)) = temp;
            }

            buf = (u8 *)(buf + 1);
            NVRAM_SPIChangeMode(SPI_TRANSMODE_1BYTE);
            SPI_Dummy();
        }
    }
#endif
}

void NVRAM_SoftwareReset(void)
{
    SPI_Wait();
    NVRAM_SPIChangeMode(SPI_TRANSMODE_1BYTE);
    SPI_SendWait(NVRAM_INSTRUCTION_SR);
}

#endif


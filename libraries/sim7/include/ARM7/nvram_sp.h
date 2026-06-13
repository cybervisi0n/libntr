#ifndef LIBRARIES_NVRAM_SP_H_
#define LIBRARIES_NVRAM_SP_H_

#include    <nitro/types.h>
#include    "spi_sp.h"
#include <nitro/spi/ARM7/spi.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifndef SDK_SMALL_BUILD
#define     SDK_NVRAM_USE_READ_HIGHER_SPEED
#endif
#define     SDK_NVRAM_ANOTHER_MAKER

#define     SDK_SPI_LOW_SPEED_LOW_CODE_SIZE

#define     NVRAM_INSTRUCTION_DUMMY         0x00
#define     NVRAM_INSTRUCTION_WREN          0x06
#define     NVRAM_INSTRUCTION_WRDI          0x04
#define     NVRAM_INSTRUCTION_RDSR          0x05
#define     NVRAM_INSTRUCTION_READ          0x03
#define     NVRAM_INSTRUCTION_FAST_READ     0x0b
#define     NVRAM_INSTRUCTION_PW            0x0a
#define     NVRAM_INSTRUCTION_PP            0x02
#define     NVRAM_INSTRUCTION_PE            0xdb
#define     NVRAM_INSTRUCTION_SE            0xd8
#define     NVRAM_INSTRUCTION_DP            0xb9
#define     NVRAM_INSTRUCTION_RDP           0xab
#ifdef  SDK_NVRAM_ANOTHER_MAKER
#define     NVRAM_INSTRUCTION_CE            0xc7
#define     NVRAM_INSTRUCTION_RSI           0x9f
#define     NVRAM_INSTRUCTION_SR            0xff
#endif

#define     NVRAM_STATUS_REGISTER_WIP       0x01
#define     NVRAM_STATUS_REGISTER_WEL       0x02
#ifdef  SDK_NVRAM_ANOTHER_MAKER
#define     NVRAM_STATUS_REGISTER_ERSER     0x20
#endif


typedef struct NVRAMWork
{
    u16     command[SPI_PXI_CONTINUOUS_PACKET_MAX];

}
NVRAMWork;


void    NVRAM_Init();
void    NVRAM_AnalyzeCommand(u32 data);
void    NVRAM_ExecuteProcess(SPIEntry * entry);

void    NVRAM_WriteEnable(void);
void    NVRAM_WriteDisable(void);
void    NVRAM_ReadStatusRegister(u8 *buf);
void    NVRAM_ReadDataBytes(u32 address, u32 size, u8 *buf);
#ifdef  SDK_NVRAM_USE_READ_HIGHER_SPEED
void    NVRAM_ReadDataBytesAtHigherSpeed(u32 address, u32 size, u8 *buf);
#endif
void    NVRAM_PageWrite(u32 address, u16 size, const u8 *buf);
void    NVRAM_PageProgram(u32 address, u16 size, const u8 *buf);
void    NVRAM_PageErase(u32 address);
void    NVRAM_SectorErase(u32 address);
void    NVRAM_DeepPowerDown(void);
void    NVRAM_ReleaseFromDeepPowerDown(void);
#ifdef  SDK_NVRAM_ANOTHER_MAKER
void    NVRAM_ChipErase(void);
void    NVRAM_ReadSiliconId(u8 *buf);
void    NVRAM_SoftwareReset(void);
#endif



static inline void NVRAM_SPIChangeMode(SPITransMode continuous)
{
    reg_SPI_SPICNT = (u16)((0x0001 << REG_SPI_SPICNT_E_SHIFT) |
                           (0x0000 << REG_SPI_SPICNT_I_SHIFT) |
                           (SPI_COMMPARTNER_EEPROM << REG_SPI_SPICNT_SEL_SHIFT) |
                           (continuous << REG_SPI_SPICNT_MODE_SHIFT) |
                           (0x0000 << REG_SPI_SPICNT_BUSY_SHIFT) |
                           (SPI_BAUDRATE_4MHZ << REG_SPI_SPICNT_BAUDRATE_SHIFT));
}



#ifdef __cplusplus
}
#endif

#endif


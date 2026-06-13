#ifndef LIBRARIES_PM_SP_H_
#define LIBRARIES_PM_SP_H_

#include <nitro/types.h>
#include <nitro/spi/common/type.h>
#include "spi_sp.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum PMStatus
{
    PM_STATUS_READY = 0,
    PM_STATUS_START_SLEEP,
    PM_STATUS_UTILITY,
    PM_STATUS_READ_REGISTER,
    PM_STATUS_WRITE_REGISTER
}
PMStatus;

typedef struct PMWork
{
    u16     command[SPI_PXI_CONTINUOUS_PACKET_MAX];
    PMStatus status;
    u32     param;
    u32     regNumber;
}
PMWork;


void    PM_Init(void);

void    PM_AnalyzeCommand(u32 data);

void    PM_ExecuteProcess(SPIEntry * entry);


extern BOOL PMi_Initialized;
static inline BOOL PM_IsAvailable(void)
{
    return PMi_Initialized;
}


#ifdef __cplusplus
}
#endif

#endif


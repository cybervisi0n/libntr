#ifndef SIM_WMSP_INDICATE_H
#define SIM_WMSP_INDICATE_H

#include <nitro/types.h>
#include <simulator/sim_net.h>

typedef struct {
    SIM_Net_MPPacketCmd_t cmd;
    u16 aid;
    u8 port;
    u8 * data;
    int dataLen;
} SIM_WMSP_Indicate_t;

void WMSP_Indicate(SIM_WMSP_Indicate_t * indicate);
void WMSP_IndicateSendBeacon();


#endif

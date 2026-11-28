#ifndef LIBNTR_SIM_CTRDG_H
#define LIBNTR_SIM_CTRDG_H

// sim_ctrdg.h : C APIs for SIM_CTRDG ( Slot 2 )

// TODO: Migrate SIM_Config to c++ so there doesnt need to be a C copy of this enum
typedef enum {
    SIM_CTRDG_SLOT2TYPE_NOTHING,
    SIM_CTRDG_SLOT2TYPE_AGBCARTRIDGE,

    // Below types are not currently supported
    SIM_CTRDG_SLOT2TYPE_RUMBLEPAK,
    SIM_CTRDG_SLOT2TYPE_MEMORYEXPANSIONPAK,
    SIM_CTRDG_SLOT2TYPE_GUITARGRIP,
    SIM_CTRDG_SLOT2TYPE_COUNT
} SIM_CTRDG_Slot2Type;

#ifdef __cplusplus
extern "C" {
#endif



#ifdef __cplusplus
}
#endif

#endif
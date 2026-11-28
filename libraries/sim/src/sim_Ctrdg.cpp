#include "simulator/sim_Ctrdg.hpp"

#include "simulator/sim.h"
#include "simulator/config/sim_config.h"

namespace SIM {

static CTRDG sCtrdgInstance;

CTRDG::CTRDG() {}
CTRDG::~CTRDG() {}


void CTRDG::Init() {
    auto * simConfig = SIM_GetConfigPtr();

    
}


CTRDG& GetCtrdg() {
    return sCtrdgInstance;
}

}
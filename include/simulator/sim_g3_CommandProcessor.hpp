#ifndef LIBNTR_SIM_G3_COMMANDPROCESSOR_HPP
#define LIBNTR_SIM_G3_COMMANDPROCESSOR_HPP

#include <nitro/types.h>
#include <nitro/gx/g3.h>
#include <simulator/drawmsg.h>

namespace SIM::G3 {

void HandleCommandMessage(draw_msg_t *msg);

}


#endif

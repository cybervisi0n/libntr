#include <nitro.h>
#include <simulator/sim.h>
#include <simulator/glad/glad.h>

#include "gui_internal.h"

#include "simulator/gui.h"

#include <nitro/os.h>

// External variables

static ImVec2 s_btnSize = {50, 20};


// Static functions



void GUI_AppOSMain(bool * p_open)
{
    igBegin("OS", p_open, 0);
    igText("OwnerInfo\n");
    igEnd();
    return;
}
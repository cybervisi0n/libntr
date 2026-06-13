#include <nitro.h>
#include <simulator/sim.h>
#include <simulator/sim_net.h>
#include <simulator/glad/glad.h>

#include "gui_internal.h"

#include "simulator/sim.h"
#include "simulator/gui.h"
#include "simulator/config/sim_config.h"

// External variables

static ImVec2 s_btnSize = {100, 20};
static char s_hostBuf[16] = {0};


// Static functions
static const char * NetStateToString(SIM_Net_State_t state)
{
    switch(state) {
        default:
        case SIM_NET_STATE_UNINITIALIZED:
            return "Uninitialized";
        case SIM_NET_STATE_DISCONNECTED:
            return "Idle";
        case SIM_NET_STATE_HOST:
            return "Hosting";
        case SIM_NET_STATE_CLIENT:
            return "Client Connected";
    }
}


void GUI_AppNetInit()
{
    SIM_config_type * myConfig = SIM_GetConfigPtr();
    strncpy(s_hostBuf, myConfig->netSettings.serverIp, 15);
    s_hostBuf[15] = 0;
}

void GUI_AppNetMain(bool * p_open)
{
    SIM_config_type * myConfig = SIM_GetConfigPtr();

    igBegin("Net", p_open, 0);
    igPushID_Int(1);
    igText("Host:");
    igSameLine(0.0f, 1.0f);
    igInputText("", s_hostBuf, sizeof(s_hostBuf)-1, 0, NULL, NULL);
    igPopID();
    if(igButton("Connect", s_btnSize)) {
        SIM_Net_StartClient(s_hostBuf);
    }
    if(igButton("Start Host", s_btnSize)) {
        SIM_Net_StartHost();
    }

    igText("State: ");
    igSameLine(0.0f, 0.0f);
    igText(NetStateToString(SIM_Net_GetCurrentState()));

    if(SIM_Net_GetCurrentState() != SIM_NET_STATE_UNINITIALIZED) {
        char playerNum[50] = {0};
        sprintf(playerNum, "Players: %d", SIM_Net_GetPlayerCount());
        igText(playerNum);

        ImVec2 tableSize = {400, 800};
        igBeginTable("playersTable", 6, ImGuiTableFlags_SizingStretchSame, tableSize, 400 );
        igTableHeader("Players");
        igTableSetupColumn("ID", 0, 0.1, 0);
        igTableSetupColumn("Status", 0, 0.2, 0);
        igTableSetupColumn("Username", 0, 0.5, 0);
        igTableSetupColumn("AID", 0, 0.1, 0);
        igTableSetupColumn("MPState", 0, 0.2, 0);
        igTableSetupColumn("GGID", 0, 0.2, 0);
        igTableHeadersRow();
        const SIM_Net_Player_t * playerList = SIM_Net_GetPlayerList();
        for(int i=0; i < SIM_NET_MAX_PLAYERS; i++) {
            igTableNextColumn();
            if(SIM_Net_GetCurrentPlayerID() == playerList[i].id) {
                igText("%d*", playerList[i].id);
            } else {
                igText("%d", playerList[i].id);
            }

            igTableNextColumn();
            const char * statusStr;
            switch(playerList[i].status) {
                default:
                case SIM_NET_PLAYERSTATUS_NONE:
                    statusStr = "None";
                    break;
                case SIM_NET_PLAYERSTATUS_DISCONNECTED:
                    statusStr = "Disconnected";
                    break;
                case SIM_NET_PLAYERSTATUS_CLIENT:
                    statusStr = "Client";
                    break;
                case SIM_NET_PLAYERSTATUS_HOST:
                    statusStr = "Host";
                    break;
            }
            igText("%s", statusStr);
            
            igTableNextColumn();
            igText("%s", playerList[i].userName);
            igTableNextColumn();
            igText("%d", playerList[i].aid);
            igTableNextColumn();
            const char * mpStateStr = "Off";
            if(playerList[i].mpState == SIM_NET_MPSTATE_PARENT) {
                mpStateStr = "Parent";
            } else if(playerList[i].mpState == SIM_NET_MPSTATE_CHILD) {
                mpStateStr = "Child";
            }
            igText(mpStateStr);
            igTableNextColumn();
            igText("%d", playerList[i].ggid);
        }
        igEndTable();
    }



    igEnd();
    return;
}
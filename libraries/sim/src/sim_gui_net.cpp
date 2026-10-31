#include "simulator/sim.h"
#include "simulator/sim_gui.hpp"
#include "simulator/sim_net.h"

#include "simulator/imgui/imgui.hpp"
#include "simulator/config/sim_config.h"

#include "gui_internal.hpp"

#include <format>
#include <string>
#include <SDL2/SDL.h>

namespace SIM::GUI {

static constexpr ImVec2 BtnSize = {100, 20};
static char sHostBuf[16] = {0};
static SIM_config_type * sConfig;


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

void AppNetInit() {
    sConfig = SIM_GetConfigPtr();
    strncpy(sHostBuf, sConfig->netSettings.serverIp, 15);
}

void AppNetMain(bool *openState) {
    ImGui::Begin("Multiplayer", openState);

    #ifdef SDK_BUILD_PROSPERO
    ImGui::Text("Not currently supported on PS5");
    #else
    ImGui::PushID(1);
    ImGui::Text("Host:");
    ImGui::SameLine(0.0f, 1.0f);
    ImGui::InputText("", sHostBuf, sizeof(sHostBuf)-1, 0, nullptr, nullptr);
    ImGui::PopID();

    if(ImGui::Button("Connect", BtnSize)) {
        SIM_Net_StartClient(sHostBuf);
    }

    if(ImGui::Button("Start Host", BtnSize)) {
        SIM_Net_StartHost();
    }

    ImGui::Text("State: %s", NetStateToString(SIM_Net_GetCurrentState()));

    if(SIM_Net_GetCurrentState() != SIM_NET_STATE_UNINITIALIZED) {
        std::string playerCountString = std::format("Players: {}", SIM_Net_GetPlayerCount());
        ImGui::Text(playerCountString.c_str());

        ImVec2 tableSize = {400, 800};
        ImGui::BeginTable("playersTable", 6, ImGuiTableFlags_SizingStretchSame, tableSize, 400);
        ImGui::TableHeader("Players");
        ImGui::TableSetupColumn("ID", 0, 0.1f);
        ImGui::TableSetupColumn("Status", 0, 0.2f);
        ImGui::TableSetupColumn("Username", 0, 0.5f);
        ImGui::TableSetupColumn("AID", 0, 0.1f);
        ImGui::TableSetupColumn("MPState", 0, 0.2f);
        ImGui::TableSetupColumn("GGID", 0, 0.2f);
        ImGui::TableHeadersRow();

        const SIM_Net_Player_t* playerList = SIM_Net_GetPlayerList();
        for(size_t i=0; i< SIM_NET_MAX_PLAYERS; i++) {
            ImGui::TableNextColumn();
            if(SIM_Net_GetCurrentPlayerID() == playerList[i].id) {
                ImGui::Text("%d*", playerList[i].id);
            } else {
                ImGui::Text("%d", playerList[i].id);
            }

            ImGui::TableNextColumn();
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
            ImGui::Text(statusStr);

            ImGui::TableNextColumn();
            ImGui::Text("%s", playerList[i].userName);
            ImGui::TableNextColumn();
            ImGui::Text("%d", playerList[i].aid);
            ImGui::TableNextColumn();
            const char * mpStateStr = "Off";
            if(playerList[i].mpState == SIM_NET_MPSTATE_PARENT) {
                mpStateStr = "Parent";
            } else if(playerList[i].mpState == SIM_NET_MPSTATE_CHILD) {
                mpStateStr = "Child";
            }
            ImGui::Text(mpStateStr);
            ImGui::TableNextColumn();
            ImGui::Text("%d", playerList[i].ggid);
        }
        ImGui::EndTable();
    }
    #endif

    ImGui::End();
}

}
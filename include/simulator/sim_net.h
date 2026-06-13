#ifndef SIM_NET_H
#define SIM_NET_H

#include <nitro.h>
#include <nitro/types.h>

#include <enet/enet.h>
#include <SDL2/SDL.h>

#define SIM_NET_PORT 2009
#define SIM_NET_MAX_PLAYERS 16

/* NTR0 */
#define SIM_NET_MAGIC_NUM 0x4e545230
#define SIM_NET_PROTOCOL_VER 0

#define SIM_NET_USERNAME_LEN 10

typedef enum {
    SIM_NET_PLAYERSTATUS_NONE,
    SIM_NET_PLAYERSTATUS_CLIENT,
    SIM_NET_PLAYERSTATUS_HOST,
    SIM_NET_PLAYERSTATUS_CONNECTING,
    SIM_NET_PLAYERSTATUS_DISCONNECTED,

    SIM_NET_PLAYERSTATUS_COUNT
} SIM_Net_PlayerStatus_t;

typedef enum {
    SIM_NET_MPSTATE_OFF,
    SIM_NET_MPSTATE_PARENT,
    SIM_NET_MPSTATE_CHILD,

    SIM_NET_MPSTATE_COUNT
} SIM_Net_MPState_t;

typedef struct {
    int id;
    SIM_Net_PlayerStatus_t status;
    u32 ipAddr;
    char userName[SIM_NET_USERNAME_LEN + 1];
    u8 ndsMacAddr[6];
    SIM_Net_MPState_t mpState;
    u8 aid;
    int ggid;
    int tgid;
    u8 userGameInfo[112]; //Max size of user game info, see NitroSDK documentation
    u8 userGameInfoLength;
} SIM_Net_Player_t;

typedef enum {
    SIM_NET_STATE_UNINITIALIZED,
    SIM_NET_STATE_DISCONNECTED,
    SIM_NET_STATE_HOST,
    SIM_NET_STATE_CLIENT,

    SIM_NET_STATE_COUNT
} SIM_Net_State_t;

typedef struct {
    BOOL initialized;
    SIM_Net_State_t state;
    ENetHost * enetHost;
    int playerCount;
    SDL_mutex * playerListMutex;
    SIM_Net_Player_t playerList[SIM_NET_MAX_PLAYERS];
    ENetPeer * peerList[SIM_NET_MAX_PLAYERS];
    SIM_Net_Player_t curPlayer;
    u16 aidBitmask;
    SDL_sem * joinReqSemaphore;
} SIM_Net_t;

typedef struct {
    u32 magicNum;
    u32 protocolVer;
} SIM_Net_PacketHeader_t;

// MGMT Packets
typedef enum {
    SIM_NET_MGMTPACKETCMD_CLIENT_INIT,
    SIM_NET_MGMTPACKETCMD_PLAYER_INFO,
    SIM_NET_MGMTPACKETCMD_PLAYER_LIST,
    SIM_NET_MGMTPACKETCMD_MP_CONNECT_REQUEST,

    SIM_NET_MGMTPACKETCMD_COUNT
} SIM_Net_MgmtPacketCmd_t;

typedef struct {
    SIM_Net_PacketHeader_t header;
    u8 cmd; //SIM_Net_MgmtPacketCmd_t
} SIM_Net_MgmtPacketCommon_t;

typedef struct {
    SIM_Net_MgmtPacketCommon_t common;
    int playerId;
} SIM_Net_ClientInitPacket_t;

typedef struct {
    SIM_Net_MgmtPacketCommon_t common;
    SIM_Net_Player_t player;
} SIM_Net_PlayerInfoPacket_t;

typedef struct {
    SIM_Net_MgmtPacketCommon_t common;
    u8 numPlayers;
    SIM_Net_Player_t playerList[SIM_NET_MAX_PLAYERS];
} SIM_Net_PlayerListPacket_t;

// MP Packets
typedef enum {
    SIM_NET_MPPACKETCMD_JOIN_REQ, /* MP Join request. Replaces Authentication+Asociation sequence */
    SIM_NET_MPPACKETCMD_JOIN_RESP, /* MP Join response from parent->child with AID */
    SIM_NET_MPPACKETCMD_DISCONNECT, /* MP Disconnect (equivalent to a deauth) */
    SIM_NET_MPPACKETCMD_DATA, /* Sending application data */

    SIM_NET_MPPACKETCMD_COUNT
} SIM_Net_MPPacketCmd_t;

typedef struct {
    SIM_Net_PacketHeader_t header;
    u8 cmd; //SIM_Net_MpPacketCmd_t
} SIM_Net_MpPacketCommon_t;

typedef struct {
    SIM_Net_MpPacketCommon_t common;
    u8 playerId;
    u8 ssid[WM_SIZE_CHILD_SSID];
} SIM_Net_JoinReqPacket_t;

typedef struct {
    SIM_Net_MpPacketCommon_t common;
    u8 aid;
} SIM_Net_JoinRespPacket_t;

typedef struct {
    SIM_Net_MpPacketCommon_t common;
    u8 sendingAid;
} SIM_Net_DisconnectPacket_t;

typedef struct {
    SIM_Net_MpPacketCommon_t common;
    u16 aid;
    u8 port;
    int dataLen;
    u8 data[];
} SIM_Net_MpDataPacket_t;

void SIM_Net_Init();
void SIM_Net_Process();
void SIM_Net_SendBeaconIndication();
void SIM_Net_StartThread();

BOOL SIM_Net_StartHost();
BOOL SIM_Net_StartClient(char * ipAddress);

int SIM_Net_GetAID();
int SIM_Net_GetCurrentPlayerID();
SIM_Net_State_t SIM_Net_GetCurrentState();
int SIM_Net_GetPlayerCount();
const SIM_Net_Player_t * SIM_Net_GetPlayerList();

void SIM_Net_SetGameInfo(int ggid, int tgid, u8 * userGameInfo, int userGameInfoLength);
void SIM_Net_SetMPState(SIM_Net_MPState_t state);

BOOL SIM_Net_JoinMP(u8 * bssid, u8 * ssid);
BOOL SIM_Net_DisconnectMP(u16 aidBitmap, u16 * resultBitmap);
BOOL SIM_Net_SendMPData(u16 aidBitmap, u8 port, u8 * data, int dataLen);


int SIM_Net_Scan(WMBssDesc * bssDescOut);

#endif

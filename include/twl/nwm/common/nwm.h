#ifndef LIBRARIES_NWM_ARM9_NWM_H__
#define LIBRARIES_NWM_ARM9_NWM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <twl.h>
#include <nitro/wm.h>

#define NWM_NUM_MAX_AP_AID                                                     \
  2007 // The maximum value possible for the AssociationID provided by the
       // wireless router

#define NWM_SIZE_SSID 32
#define NWM_SIZE_MACADDR 6
#define NWM_SIZE_BSSID NWM_SIZE_MACADDR

#define NWM_SIZE_WEP_40BIT 5
#define NWM_SIZE_WEP_104BIT 13
#define NWM_SIZE_WEP_128BIT 16

#define NWM_SIZE_WEP                                                           \
  20 // For preserving WM (DS connection settings) compatibility

#define NWM_WEPMODE_OPEN WM_WEPMODE_NO
#define NWM_WEPMODE_40BIT WM_WEPMODE_40BIT
#define NWM_WEPMODE_104BIT WM_WEPMODE_104BIT
#define NWM_WEPMODE_128BIT WM_WEPMODE_128BIT

#define NWM_WPAMODE_WPA_TKIP (WM_WEPMODE_128BIT + 1)
#define NWM_WPAMODE_WPA2_TKIP (WM_WEPMODE_128BIT + 2)
#define NWM_WPAMODE_WPA_AES (WM_WEPMODE_128BIT + 3)
#define NWM_WPAMODE_WPA2_AES (WM_WEPMODE_128BIT + 4)

#define NWM_BSS_DESC_SIZE                                                      \
  64 // Size of the buffer transferred with the NWM_StartScan function to store
     // parent information
#define NWM_BSS_DESC_SIZE_MAX 512
#define NWM_FRAME_SIZE_MAX                                                     \
  1522 // Maximum data frame size: 1500(MTU) + 22(802.3 header)
#define NWM_SCAN_NODE_MAX                                                      \
  32 // Maximum number of parent devices that can be found by a single Scan
#define NWM_SIZE_SCANBUF_MAX 65535 // Maximum buffer size

#define NWM_SYSTEM_BUF_SIZE (0x00003000)

#define NWM_CAPABILITY_ESS_MASK 0x0001
#define NWM_CAPABILITY_ADHOC_MASK 0x0002
#define NWM_CAPABILITY_CP_POLLABLE_MASK 0x0004
#define NWM_CAPABILITY_CP_POLL_REQ_MASK 0x0008
#define NWM_CAPABILITY_PRIVACY_MASK 0x0010
#define NWM_CAPABILITY_SPREAMBLE_MASK 0x0020
#define NWM_CAPABILITY_PBCC_MASK 0x0040
#define NWM_CAPABILITY_CH_AGILITY_MASK 0x0800
#define NWM_CAPABILITY_SPECTRUM_MASK 0x0100
#define NWM_CAPABILITY_QOS_MASK 0x0200
#define NWM_CAPABILITY_SSLOTTIME_MASK 0x0400
#define NWM_CAPABILITY_APSD_MASK 0x0800

#define NWM_SCANTYPE_PASSIVE 0
#define NWM_SCANTYPE_ACTIVE 1

#define NWM_DEFAULT_PASSIVE_SCAN_PERIOD                                        \
  105 /* Note that the default NWM values are larger than the default WM       \
         values */
#define NWM_DEFAULT_ACTIVE_SCAN_PERIOD                                         \
  30 /* Note that NWM uses different default values for active and passive     \
        scans */

#define NWM_RATESET_1_0M 0x0001
#define NWM_RATESET_2_0M 0x0002
#define NWM_RATESET_5_5M 0x0004
#define NWM_RATESET_6_0M 0x0008
#define NWM_RATESET_9_0M 0x0010
#define NWM_RATESET_11_0M 0x0020
#define NWM_RATESET_12_0M 0x0040
#define NWM_RATESET_18_0M 0x0080
#define NWM_RATESET_24_0M 0x0100
#define NWM_RATESET_36_0M 0x0200
#define NWM_RATESET_48_0M 0x0400
#define NWM_RATESET_54_0M 0x0800
#define NWM_RATESET_11B_MASK                                                   \
  (NWM_RATESET_1_0M | NWM_RATESET_2_0M | NWM_RATESET_5_5M | NWM_RATESET_11_0M)
#define NWM_RATESET_11G_MASK                                                   \
  (NWM_RATESET_1_0M | NWM_RATESET_2_0M | NWM_RATESET_5_5M |                    \
   NWM_RATESET_11_0M | NWM_RATESET_6_0M | NWM_RATESET_9_0M |                   \
   NWM_RATESET_12_0M | NWM_RATESET_18_0M | NWM_RATESET_24_0M |                 \
   NWM_RATESET_36_0M | NWM_RATESET_48_0M | NWM_RATESET_54_0M)

#define NWM_WPA_PSK_LENGTH 32
#define NWM_WPA_PASSPHRASE_LENGTH_MAX 64

#define NWM_RSSI_INFRA_LINK_LEVEL_1 12
#define NWM_RSSI_INFRA_LINK_LEVEL_2 17
#define NWM_RSSI_INFRA_LINK_LEVEL_3 22

#define NWM_RSSI_ADHOC_LINK_LEVEL_1 12 /* [TODO] TBD */
#define NWM_RSSI_ADHOC_LINK_LEVEL_2 17 /* [TODO] TBD */
#define NWM_RSSI_ADHOC_LINK_LEVEL_3 22 /* [TODO] TBD */

typedef enum NWMApiid {
  NWM_APIID_INIT = 0, // NWM_Init()
  NWM_APIID_RESET,    // NWM_Reset()
  NWM_APIID_END,      // NWM_End()

  NWM_APIID_LOAD_DEVICE,   // NWM_LoadDevice()
  NWM_APIID_UNLOAD_DEVICE, // NWM_UnloadDevice()
  NWM_APIID_OPEN,          // NWM_Open()
  NWM_APIID_CLOSE,         // NWM_Close()

  NWM_APIID_START_SCAN,          // NWM_StartScan()
  NWM_APIID_CONNECT,             // NWM_Connect()
  NWM_APIID_DISCONNECT,          // NWM_Disconnect()
  NWM_APIID_SET_RECEIVING_BUF,   // NWM_SetReceivingFrameBuffer()
  NWM_APIID_SEND_FRAME,          // NWM_SendFrame()
  NWM_APIID_UNSET_RECEIVING_BUF, // NWM_UnsetReceivingFrameBuffer()
  NWM_APIID_SET_WEPKEY,          // NWM_SetWEPKey()
  NWM_APIID_SET_PS_MODE,         // NWM_SetPowerSaveMode()

  NWM_APIID_SET_WPA_KEY,    //
  NWM_APIID_SET_WPA_PARAMS, //

  NWM_APIID_CREATE_QOS,       //
  NWM_APIID_SET_WPA_PSK,      // NWM_SetWPAPSK()
  NWM_APIID_INSTALL_FIRMWARE, // NWMi_InstallFirmware()
  NWM_APIID_ASYNC_KIND_MAX,   //  : Type of asynchronous process

  NWM_APIID_INDICATION = 128, //  : For indication callback

  NWM_APIID_UNKNOWN =
      255 //  : Value returned from ARM7 at unknown command number
} NWMApiid;

typedef enum NWMRetCode {
  NWM_RETCODE_SUCCESS = 0,
  NWM_RETCODE_FAILED = 1,
  NWM_RETCODE_OPERATING = 2,
  NWM_RETCODE_ILLEGAL_STATE = 3,
  NWM_RETCODE_NWM_DISABLE = 4,
  NWM_RETCODE_INVALID_PARAM = 5,
  NWM_RETCODE_FIFO_ERROR = 6,
  NWM_RETCODE_FATAL_ERROR = 7, // Error that the software cannot handle
  NWM_RETCODE_NETBUF_ERROR = 8,
  NWM_RETCODE_WMI_ERROR = 9,
  NWM_RETCODE_SDIO_ERROR = 10,
  NWM_RETCODE_RECV_IND = 11,
  NWM_RETCODE_INDICATION = 12, // Internal use only

  NWM_RETCODE_MAX
} NWMRetCode;

typedef enum NWMState {
  NWM_STATE_NONE = 0x0000,
  NWM_STATE_INITIALIZED = 0x0001,     // INITIALIZED state
  NWM_STATE_LOADED = 0x0002,          // LOADED state
  NWM_STATE_DISCONNECTED = 0x0003,    // DISCONNECTED state
  NWM_STATE_INFRA_CONNECTED = 0x0004, // CONNECTED STA (infrastructure) state
  NWM_STATE_ADHOC_CONNECTED = 0x0005  // CONNECTED STA (ad hoc) state
} NWMState;

typedef enum NWMReasonCode {
  NWM_REASON_API_SUCCESS = 0,

  NWM_REASON_NO_NETWORK_AVAIL = 1,
  NWM_REASON_LOST_LINK = 2,
  NWM_REASON_DISCONNECT_CMD = 3,
  NWM_REASON_BSS_DISCONNECTED = 4,
  NWM_REASON_AUTH_FAILED = 5,
  NWM_REASON_ASSOC_FAILED = 6,
  NWM_REASON_NO_RESOURCES_AVAIL = 7,
  NWM_REASON_CSERV_DISCONNECT = 8,
  NWM_REASON_INVAILD_PROFILE = 9,

  NWM_REASON_WEP_KEY_ERROR = 10,

  NWM_REASON_WPA_KEY_ERROR = 11,
  NWM_REASON_TKIP_MIC_ERROR = 12,

  NWM_REASON_NO_QOS_RESOURCES_AVAIL = 13,

  NWM_REASON_UNKNOWN

} NWMReasonCode;

typedef enum NWMAuthMode {
  NWM_AUTHMODE_OPEN,   /* DOT11 authentication */
  NWM_AUTHMODE_SHARED, /* DOT11 authentication */
  NWM_AUTHMODE_WPA_PSK_TKIP =
      NWM_WPAMODE_WPA_TKIP, /* Be careful to keep this from overlapping with
                               WCM_WEPMODE_* */
  NWM_AUTHMODE_WPA2_PSK_TKIP = NWM_WPAMODE_WPA2_TKIP,
  NWM_AUTHMODE_WPA_PSK_AES = NWM_WPAMODE_WPA_AES,
  NWM_AUTHMODE_WPA2_PSK_AES = NWM_WPAMODE_WPA2_AES
} NWMAuthMode,
    NWMauthMode;

typedef enum NWMElementID {
  NWM_ELEMENTID_SSID = 0,
  NWM_ELEMENTID_SUPPORTED_RATES = 1,
  NWM_ELEMENTID_FH_PARAMETER_SET = 2,
  NWM_ELEMENTID_DS_PARAMETER_SET = 3,
  NWM_ELEMENTID_CF_PARAMETER_SET = 4,
  NWM_ELEMENTID_TIM = 5,
  NWM_ELEMENTID_IBSS_PARAMETER_SET = 6,
  NWM_ELEMENTID_COUNTRY = 7,
  NWM_ELEMENTID_HP_PARAMETERS = 8,
  NWM_ELEMENTID_HP_TABLE = 9,
  NWM_ELEMENTID_REQUEST = 10,
  NWM_ELEMENTID_QBSS_LOAD = 11,
  NWM_ELEMENTID_EDCA_PARAMETER_SET = 12,
  NWM_ELEMENTID_TSPEC = 13,
  NWM_ELEMENTID_TRAFFIC_CLASS = 14,
  NWM_ELEMENTID_SCHEDULE = 15,
  NWM_ELEMENTID_CHALLENGE_TEXT = 16,

  NWM_ELEMENTID_POWER_CONSTRAINT = 32,
  NWM_ELEMENTID_POWER_CAPABILITY = 33,
  NWM_ELEMENTID_TPC_REQUEST = 34,
  NWM_ELEMENTID_TPC_REPORT = 35,
  NWM_ELEMENTID_SUPPORTED_CHANNELS = 36,
  NWM_ELEMENTID_CH_SWITCH_ANNOUNCE = 37,
  NWM_ELEMENTID_MEASURE_REQUEST = 38,
  NWM_ELEMENTID_MEASURE_REPORT = 39,
  NWM_ELEMENTID_QUIET = 40,
  NWM_ELEMENTID_IBSS_DFS = 41,
  NWM_ELEMENTID_ERP_INFORMATION = 42,
  NWM_ELEMENTID_TS_DELAY = 43,
  NWM_ELEMENTID_TCLASS_PROCESSING = 44,
  NWM_ELEMENTID_HT_CAPABILITY = 45,
  NWM_ELEMENTID_QOS_CAPABILITY = 46,
  NWM_ELEMENTID_RSN = 48,
  NWM_ELEMENTID_EX_SUPPORTED_RATES = 50,
  NWM_ELEMENTID_HT_INFORMATION = 61,

  NWM_ELEMENTID_VENDOR = 221,
  NWM_ELEMENTID_NINTENDO = 221
} NWMElementID;

typedef enum NWMPowerMode {
  NWM_POWERMODE_ACTIVE,
  NWM_POWERMODE_STANDARD,
  NWM_POWERMODE_UAPSD
} NWMPowerMode;

typedef enum NWMAccessCategory {
  NWM_AC_BE, /* Best effort */
  NWM_AC_BK, /* Background */
  NWM_AC_VI, /* Video */
  NWM_AC_VO, /* Voice */
  NWM_AC_NUM
} NWMAccessCategory;

typedef enum NWMNwType {
  NWM_NWTYPE_INFRA,
  NWM_NWTYPE_ADHOC,
  NWM_NWTYPE_WPS,
  NWM_NWTYPE_NUM
} NWMNwType;

typedef enum NWMFramePort {
  NWM_PORT_IPV4_ARP, /* For TCP/IP */
  NWM_PORT_EAPOL,    /* For WPA supplicant */
  NWM_PORT_OTHERS,
  NWM_PORT_NUM
} NWMFramePort,
    NWMframePort;

typedef void (*NWMCallbackFunc)(void *arg); // Callback type for the NWM API

typedef struct NWMBssDesc {
  u16 length;               // 2
  s16 rssi;                 // 4
  u8 bssid[NWM_SIZE_BSSID]; // 10
  u16 ssidLength;           // 12
  u8 ssid[NWM_SIZE_SSID];   // 44
  u16 capaInfo;             // 46
  struct {
    u16 basic;   // 48
    u16 support; // 50
  } rateSet;
  u16 beaconPeriod;   // 52
  u16 dtimPeriod;     // 54
  u16 channel;        // 56
  u16 cfpPeriod;      // 58
  u16 cfpMaxDuration; // 60
  u16 reserved;       // 62 just for compatibility with WM (must be 0)
  u16 elementCount;   // 64
  u16 elements[1];
} NWMBssDesc, NWMbssDesc;

typedef struct NWMInfoElements {
  u8 id;
  u8 length;
  u16 element[1];
} NWMInfoElements;

typedef struct NWMScanParam {
  NWMBssDesc *scanBuf; // Buffer that stores parent information
  u16 scanBufSize;     // Size of scanBuf
  u16 channelList; // List of channels (more than one can be specified) to scan
  u16 channelDwellTime;     // Per-channel scan time (in ms)
  u16 scanType;             // passive or active
  u8 bssid[NWM_SIZE_BSSID]; // MAC address to scan for (all parent devices will
                            // be targeted if this is 0xff)
  u16 ssidLength; // Length of the SSID to scan for (all nodes will be targeted
                  // if this is 0)
  u8 ssid[NWM_SIZE_SSID]; // SSID to scan for
  u16 rsv[6];
} NWMScanParam, NWMscanParam;

typedef struct NWMScanExParam {
  NWMBssDesc *scanBuf; // Buffer that stores parent information
  u16 scanBufSize;     // Size of scanBuf
  u16 channelList; // List of channels (more than one can be specified) to scan
  u16 channelDwellTime;     // Per-channel scan time (in ms)
  u16 scanType;             // passive or active
  u8 bssid[NWM_SIZE_BSSID]; // MAC address to scan for (all parent devices will
                            // be targeted if this is 0xff)
  u16 ssidLength; // Length of the SSID to scan for (all nodes will be targeted
                  // if this is 0)
  u8 ssid[NWM_SIZE_SSID]; // SSID to scan for
  u16 ssidMatchLength;    // Matching length of the SSID to scan for
  u16 rsv[5];
} NWMScanExParam, NWMscanExParam;

typedef struct NWMWpaParam {
  u16 auth; // NWMAuthMode (can use PSK only)
  u8 psk[NWM_WPA_PSK_LENGTH];
} NWMWpaParam;

typedef struct NWMRecvFrameHdr {
  u8 da[NWM_SIZE_MACADDR];
  u8 sa[NWM_SIZE_MACADDR];
  u8 pid[2];
  u8 frame[2];
} NWMRecvFrameHdr;

typedef struct NWMCallback {
  u16 apiid;
  u16 retcode;

} NWMCallback;

typedef struct NWMStartScanCallback {
  u16 apiid;
  u16 retcode;
  u32 channelList;  // Scanned channel list, regardless of whether found
  u8 reserved[2];   // Padding
  u16 bssDescCount; // Number of parents that were found
  u32 allBssDescSize;
  NWMBssDesc *
      bssDesc[NWM_SCAN_NODE_MAX]; // Beginning address of the parent information
  u16 linkLevel[NWM_SCAN_NODE_MAX]; // Reception signal strength

} NWMStartScanCallback, NWMstartScanCallback;

typedef struct NWMConnectCallback {
  u16 apiid;
  u16 retcode;
  u16 channel;
  u8 bssid[NWM_SIZE_BSSID];
  s16 rssi;
  u16 aid;    // Only when CONNECTED. AID assigned to self
  u16 reason; // reason when disconnecting. This is defined in NWMReasonCode
  u16 listenInterval;
  u8 networkType;
  u8 beaconIeLen;
  u8 assocReqLen;
  u8 assocRespLen;
  u8 assocInfo[2]; /* This field consists of beaconIe, assocReq, assocResp */
} NWMConnectCallback, NWMconnectCallback;

typedef struct NWMDisconnectCallback {
  u16 apiid;
  u16 retcode;
  u16 reason;
  u16 rsv;
} NWMDisconnectCallback, NWMdisconnectCallback;

typedef struct NWMSendFrameCallback {
  u16 apiid;
  u16 retcode;
  NWMCallbackFunc callback;
} NWMSendFrameCallback;

typedef struct NWMReceivingFrameCallback {
  u16 apiid;
  u16 retcode;
  u16 port;
  s16 rssi;
  u32 length;
  NWMRecvFrameHdr *recvBuf;

} NWMReceivingFrameCallback;

NWMRetCode NWM_Init(void *sysBuf, u32 bufSize, u8 dmaNo);

NWMRetCode NWM_Reset(NWMCallbackFunc callback);

NWMRetCode NWM_LoadDevice(NWMCallbackFunc callback);

NWMRetCode NWM_UnloadDevice(NWMCallbackFunc callback);

NWMRetCode NWM_Open(NWMCallbackFunc callback);

NWMRetCode NWM_Close(NWMCallbackFunc callback);

NWMRetCode NWM_End(void);

NWMRetCode NWM_StartScan(NWMCallbackFunc callback, const NWMScanParam *param);

NWMRetCode NWM_StartScanEx(NWMCallbackFunc callback,
                           const NWMScanExParam *param);

NWMRetCode NWM_Connect(NWMCallbackFunc callback, u8 nwType,
                       const NWMBssDesc *pBdesc);

NWMRetCode NWM_Disconnect(NWMCallbackFunc callback);

NWMRetCode NWM_SetWEPKey(NWMCallbackFunc callback, u16 wepmode, u16 wepkeyid,
                         const u8 *wepkey, u16 authMode);

void NWM_Passphrase2PSK(const u8 passphrase[NWM_WPA_PASSPHRASE_LENGTH_MAX],
                        const u8 *ssid, u8 ssidlen, u8 psk[NWM_WPA_PSK_LENGTH]);

NWMRetCode NWM_SetWPAPSK(NWMCallbackFunc callback, NWMWpaParam *param);

NWMRetCode NWM_SetReceivingFrameBuffer(NWMCallbackFunc callback, u8 *recvBuf,
                                       u16 recvBufSize, u16 protocol);

NWMRetCode NWM_SendFrame(NWMCallbackFunc callback, const u8 *destAddr,
                         const u16 *sendFrame, u16 sendFrameSize);

NWMRetCode NWM_UnsetReceivingFrameBuffer(NWMCallbackFunc callback,
                                         u16 protocol);

NWMRetCode NWM_SetPowerSaveMode(NWMCallbackFunc callback,
                                NWMPowerMode powerSave);

NWMRetCode NWM_GetMacAddress(u8 *macAddr);

NWMBssDesc *NWM_GetBssDesc(void *bssbuf, u32 bsssize, int index);

u16 NWM_GetAllowedChannel(void);

u16 NWM_CalcLinkLevel(s16 rssi);

u16 NWM_GetDispersionScanPeriod(u16 scanType);

u16 NWM_GetState(void);

NWMInfoElements *NWM_GetInfoElements(NWMBssDesc *bssDesc, u8 elementID);

NWMInfoElements *NWM_GetVenderInfoElements(NWMBssDesc *bssDesc, u8 elementID,
                                           const u8 ouiType[4]);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LIBRARIES_NWM_ARM9_NWM_H__ */

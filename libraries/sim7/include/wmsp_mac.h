#ifndef LIBRARIES_WM_ARM7_WMSP_MAC_H__
#define LIBRARIES_WM_ARM7_WMSP_MAC_H__

#ifndef SDK_BUILD_ARM
#include <nitro/types.h>
#endif

#ifdef  __cplusplus
extern "C" {
#endif



#ifndef HW_WIRELESS_INTF0
#ifdef SDK_BUILD_ARM
#define HW_WIRELESS_INTF0       0x04800000
#else
extern u8 s_HW_WIRELESS_INTF0[0x8000];
#define HW_WIRELESS_INTF0 ((u64)s_HW_WIRELESS_INTF0)
#endif
#endif
#ifndef HW_WIRELESS_INTF1
#ifdef SDK_BUILD_ARM
#define HW_WIRELESS_INTF1       0x04808000
#else
extern u8 s_HW_WIRELESS_INTF1[0x8000];
#define HW_WIRELESS_INTF1 ((u64)s_HW_WIRELESS_INTF1)
#endif
#endif
#ifndef MAC_REG_BASE
#define MAC_REG_BASE            (HW_WIRELESS_INTF1)
#endif
#ifndef MAC_MEM_BASE
#define MAC_MEM_BASE            (HW_WIRELESS_INTF0 + 0x4000)
#endif
#define OFST_MREG_KEYIN_ADRS    0x094
#define OFST_MREG_KEYOUT_ADRS   0x098
#define MREG_KEYIN_ADRS         (MAC_REG_BASE + OFST_MREG_KEYIN_ADRS)
#define MREG_KEYOUT_ADRS        (MAC_REG_BASE + OFST_MREG_KEYOUT_ADRS)

#define OFST_MREG_TMPTT_ACT_TIME    0x124
#define MREG_TMPTT_ACT_TIME         (MAC_REG_BASE + OFST_MREG_TMPTT_ACT_TIME)
#define OFST_MREG_TBTT_ACT_TIME     0x128
#define MREG_TBTT_ACT_TIME          (MAC_REG_BASE + OFST_MREG_TBTT_ACT_TIME)
#define OFST_MREG_RF_WAKEUP_TIME    0x150
#define MREG_RF_WAKEUP_TIME         (MAC_REG_BASE + OFST_MREG_RF_WAKEUP_TIME)

#define OFST_MREG_TSF               0x0F8
#define OFST_MREG_TSF0              0x0F8
#define OFST_MREG_TSF1              0x0FA
#define OFST_MREG_TSF2              0x0FC
#define OFST_MREG_TSF3              0x0FE
#define MREG_TSF                    (MAC_REG_BASE + OFST_MREG_TSF)
#define MREG_TSF0                   (MAC_REG_BASE + OFST_MREG_TSF0)
#define MREG_TSF1                   (MAC_REG_BASE + OFST_MREG_TSF1)
#define MREG_TSF2                   (MAC_REG_BASE + OFST_MREG_TSF2)
#define MREG_TSF3                   (MAC_REG_BASE + OFST_MREG_TSF3)

#define GetMacRxAdrs(_x_)           (MAC_MEM_BASE + ((u32)(_x_) * 2))

#define RXSTS_TX_KEY                (0x0001 << 14)
#define RXSTS_POLLED_MP             (0x0001 << 13)
#define RXSTS_KEY_IN                (0x0001 << 12)
#define RXSTS_KEY_OUT               (0x0001 << 11)


#if SDK_VERSION_WL < 19800


typedef union
{
    struct
    {
        u16     Status;
        u16     Status2;
        u16     rsv_RetryCount;
        u16     rsv_AppRate;
        u16     Service_Rate;
        u16     MPDU;
    }
    Tx;

    struct
    {
        u16     Status;
        u16     NextBnry;
        u16     TimeStamp;
        u16     Service_Rate;
        u16     MPDU;
        u16     rsv_RSSI;
    }
    Rx;
}
MAC_HEADER, *LPMAC_HEADER;

typedef union
{
    u16     Data;
    struct
    {
        u16     Version:2;
        u16     Type:2;
        u16     SubType:4;
        u16     ToDS:1;
        u16     FromDS:1;
        u16     MoreFrag:1;
        u16     Retry:1;
        u16     PowerMan:1;
        u16     MoreData:1;
        u16     WEP:1;
        u16     Order:1;
    }
    Bit;
}
FRAME_CTRL, *LPFRAME_CTRL;

typedef union
{
    u16     Data;
    struct
    {
        u16     FragNum:4;
        u16     SeqNum:12;
    }
    Bit;
}
SEQ_CTRL, *LPSEQ_CTRL;

typedef struct
{
    FRAME_CTRL FrameCtrl;
    u16     DurationID;
    u16     Adrs1[3];
    u16     Adrs2[3];
    u16     Adrs3[3];
    SEQ_CTRL SeqCtrl;
}
DATA_HEADER, *LPDATA_HEADER;

typedef struct
{
    MAC_HEADER MacHeader;
    DATA_HEADER Dot11Header;
    u8      Body[4];
}
RXFRM_MAC, *LPRXFRM_MAC;

#endif


#ifdef  __cplusplus
}
#endif

#endif


#include    "wmsp_private.h"

#include <simulator/sim.h>
#include <simulator/wmsp_indicate.h>

static void WmspIndicateJoinReq(int aid, u8 * ssid);
static void WmspIndicateDeAuthenticate(int aid);
static void WmspIndicateMpData(int aid, u8 port, u8 * data, int dataLen);

void WMSP_Indicate(SIM_WMSP_Indicate_t * indicate)
{
    switch(indicate->cmd) {
        case SIM_NET_MPPACKETCMD_JOIN_REQ:
            WmspIndicateJoinReq(indicate->aid, indicate->data);
            break;
        case SIM_NET_MPPACKETCMD_DISCONNECT:
            WmspIndicateDeAuthenticate(indicate->aid);
            break;
        case SIM_NET_MPPACKETCMD_DATA:
            WmspIndicateMpData(indicate->aid, indicate->port, indicate->data, indicate->dataLen);
            break;
        default:
            SIM_assert_always_msg("Unknown indicate cmd");
            break;
    }
}

void WMSP_IndicateSendBeacon()
{
    if (WMSP_GetStatusStructure()->beaconIndicateFlag > 0)
    {
        WMStartParentCallback *callback = (WMStartParentCallback *)WMSP_GetBuffer4Callback2Wm9();

        callback->apiid = WM_APIID_START_PARENT;
        callback->errcode = WM_ERRCODE_SUCCESS;
        callback->state = WM_STATECODE_BEACON_SENT;
        WMSP_ReturnResult2Wm9(callback);
    }
}

static void WmspIndicateJoinReq(int aid, u8 * ssid)
{
    WMStatus *status = WMSP_GetStatusStructure();
    WMStartParentCallback *callback;
    u32    *buf;

    status->child_bitmap |= (0x0001 << aid);
    status->mp_readyBitmap &= ~(0x0001 << aid);
    status->mp_lastRecvTick[aid] = OS_GetTick() | 1;

    callback = (WMStartParentCallback *)WMSP_GetBuffer4Callback2Wm9();
    callback->apiid = WM_APIID_START_PARENT;
    callback->errcode = WM_ERRCODE_SUCCESS;
    callback->state = WM_STATECODE_CONNECTED;
    

    callback->aid = (u16)aid;

    callback->parentSize = status->mp_parentSize;
    callback->childSize = status->mp_childSize;

    memcpy(callback->ssid, ssid, WM_SIZE_CHILD_SSID);

    WMSP_ReturnResult2Wm9(callback);
}

static void WmspIndicateDeAuthenticate(int aid)
{
    int     i;
    WMStatus *status = WMSP_GetStatusStructure();
    u8      tmpMacAddress[6];
    u16     tmpAID;

    if (WM_STATE_PARENT == status->state || WM_STATE_MP_PARENT == status->state)
    {
        tmpAID = 0;

        tmpAID = aid;
        {
            WMStartParentCallback *callback =
                (WMStartParentCallback *)WMSP_GetBuffer4Callback2Wm9();

            callback->apiid = WM_APIID_START_PARENT;
            callback->errcode = WM_ERRCODE_SUCCESS;
            callback->state = WM_STATECODE_DISCONNECTED;
            callback->aid = tmpAID;
            callback->parentSize = status->mp_parentSize;
            callback->childSize = status->mp_childSize;

            WMSP_ReturnResult2Wm9(callback);
        }
    }
    else
    {
        BOOL    fCleanQueue = FALSE;
        OSIntrMode e = OS_DisableInterrupts();

        if (status->child_bitmap == 0)
        {
            (void)OS_RestoreInterrupts(e);
            return;
        }
        status->child_bitmap = 0;  
        status->mp_readyBitmap = 0;
        status->ks_flag = FALSE;   
        status->dcf_flag = FALSE;  
        status->VSyncFlag = FALSE; 

        status->wep_flag = FALSE;
        status->wepMode = WL_CMDLABEL_WEP_NO;
        MI_CpuClear8(status->wepKey, WM_SIZE_WEPKEY);

        WMSP_ResetSizeVars();

        status->beaconIndicateFlag = 0;
        status->state = WM_STATE_CLASS1;

        (void)OS_RestoreInterrupts(e);

        {
            WMStartConnectCallback *callback =
                (WMStartConnectCallback *)WMSP_GetBuffer4Callback2Wm9();

            callback->apiid = WM_APIID_START_CONNECT;   
            callback->errcode = WM_ERRCODE_SUCCESS;     
            callback->state = WM_STATECODE_DISCONNECTED;
            callback->aid = status->aid;
            callback->parentSize = status->mp_parentSize;
            callback->childSize = status->mp_childSize;

            WMSP_ReturnResult2Wm9(callback);
        }
    }
}

static void WmspIndicateMpData(int aid, u8 port, u8 * data, int dataLen)
{
    WMStatus *status = WMSP_GetStatusStructure();
 	WMPortRecvCallback *cb_Port;
	cb_Port = (WMPortRecvCallback *)WMSP_GetBuffer4Callback2Wm9();

	cb_Port->apiid = WM_APIID_PORT_RECV;
	cb_Port->errcode = WM_ERRCODE_SUCCESS;
	cb_Port->state = WM_STATECODE_PORT_RECV;
	cb_Port->port = port;
	cb_Port->recvBuf = NULL;
	cb_Port->data = (u16*)data;
	cb_Port->length = (u16)dataLen;
	cb_Port->aid = aid;
	cb_Port->myAid = status->aid;
	cb_Port->seqNo = (u16)0;

	{
	    u16     parentSize = status->mp_parentSize;
	    u16     childSize = status->mp_childSize;
	    cb_Port->maxSendDataSize = (status->aid == 0) ? parentSize : childSize;
	    cb_Port->maxRecvDataSize = (status->aid == 0) ? childSize : parentSize;
	}

	WMSP_ReturnResult2Wm9(cb_Port);
}
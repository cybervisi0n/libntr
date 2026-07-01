
#include    "wmsp_private.h"

#ifndef SDK_BUILD_ARM
#include <simulator/sim_net.h>
#include <simulator/assert.h>

#include "tracy/TracyC.h"
#endif

static void WmspRequestFuncDummy(OSMessage msg);

static void ChangeWmState(WMStatus * status, int newState);

static const WMSPRequestFunc WmspRequestFuncTable[WM_APIID_ASYNC_KIND_MAX] = {
    WMSP_Initialize,
    WMSP_Reset,
    WMSP_End,
    WMSP_Enable,
    WMSP_Disable,
    WMSP_PowerOn,
    WMSP_PowerOff,
    WMSP_SetParentParam,
    WMSP_StartParent,
    WMSP_EndParent,
    WMSP_StartScan,
    WMSP_EndScan,
    WMSP_StartConnectEx,
    WMSP_Disconnect,
    WMSP_StartMP,
    WMSP_SetMPData,
    WMSP_EndMP,
    WmspRequestFuncDummy,
    WmspRequestFuncDummy,
    WmspRequestFuncDummy,
    WmspRequestFuncDummy,
    WmspRequestFuncDummy,
    WmspRequestFuncDummy,
    WmspRequestFuncDummy,
    WMSP_SetGameInfo,
    WmspRequestFuncDummy,
    WmspRequestFuncDummy,
    WmspRequestFuncDummy,
    WmspRequestFuncDummy,
    WmspRequestFuncDummy,
    WMSP_MeasureChannel,
    WmspRequestFuncDummy,
    WmspRequestFuncDummy,
    WmspRequestFuncDummy,
    WmspRequestFuncDummy,
    WmspRequestFuncDummy,
    WmspRequestFuncDummy,
    WmspRequestFuncDummy,
    WMSP_StartScanEx,
    WmspRequestFuncDummy,
    WmspRequestFuncDummy,
    WmspRequestFuncDummy,
    WmspRequestFuncDummy,
    WmspRequestFuncDummy,
    WmspRequestFuncDummy,
    WmspRequestFuncDummy,
};

static int requestNo = 0;

void WMSP_RequestThread(void *arg)
{
#pragma unused( arg )

    WMSPWork *p = WMSP_GetSystemWork();
    WMStatus *status = WMSP_GetStatusStructure();
    OSMessage msg;
    u16     apiid;

    while (TRUE)
    {

        (void)OS_ReceiveMessage(&(p->requestQ), &msg, OS_MESSAGE_BLOCK);

        if (msg == 0)
        {
            OS_ExitThread();
            return;
        }

        #ifndef SDK_BUILD_ARM
        TracyCZone(ctx, 1);
        #endif
        apiid = *((u16 *)msg);
        if (apiid & WM_API_REQUEST_ACCEPTED)
        {
            apiid &= ~(WM_API_REQUEST_ACCEPTED);
        }
        if (apiid < WM_APIID_ASYNC_KIND_MAX)
        {
            WMSP_DLOGF_REQUEST_QUEUE("[R]>>%d", apiid);

            requestNo++;

            status->apiBusy = TRUE;
            status->BusyApiid = apiid;
            (WmspRequestFuncTable[apiid]) (msg);
            status->apiBusy = FALSE;
        }
        else
        {

        }

        *((u16 *)msg) = (u16)(apiid | WM_API_REQUEST_ACCEPTED);

        #ifndef SDK_BUILD_ARM
        TracyCZoneEnd(ctx);
        #endif
    }
}

static void WmspRequestFuncDummy(OSMessage msg)
{
#pragma unused( msg )

    OS_Printf("ARM7: Dummy request function is called.\n");
    return;
}

static void ChangeWmState(WMStatus * status, int newState)
{

    status->state = newState;
}

void WMSP_Initialize(OSMessage msg)
{
    #ifdef SDK_PORT
    u64    *reqBuf = (u64 *)msg;
    #else
    u32    *reqBuf = (u32 *)msg;
    #endif
    WMSPWork *sys = WMSP_GetSystemWork();
    WMArm7Buf *p;
    WMCallback *cb;

    sys->wm7buf = (WMArm7Buf *)(reqBuf[1]);
    p = sys->wm7buf;
    p->status = sys->status = (WMStatus *)(reqBuf[2]);
    p->fifo7to9 = (u32 *)(reqBuf[3]);

    ChangeWmState(p->status, WM_STATE_IDLE);

    cb = (WMCallback *)WMSP_GetBuffer4Callback2Wm9();
    cb->apiid = WM_APIID_INITIALIZE;
    cb->errcode = WM_ERRCODE_SUCCESS;
    WMSP_ReturnResult2Wm9((void *)cb);
}

void WMSP_Reset(OSMessage msg)
{

    #ifdef SDK_PORT
    u64    *reqBuf = (u64 *)msg;
    #else
    u32    *reqBuf = (u32 *)msg;
    #endif
    WMSPWork *sys = WMSP_GetSystemWork();
    WMArm7Buf *p;
    WMCallback *cb;

    cb = (WMCallback *)WMSP_GetBuffer4Callback2Wm9();
    cb->apiid = WM_APIID_RESET;
    cb->errcode = WM_ERRCODE_SUCCESS;
    WMStatus *status = WMSP_GetStatusStructure();
    ChangeWmState(status, WM_STATE_IDLE);
    WMSP_ReturnResult2Wm9((void *)cb);
}

void WMSP_End(OSMessage msg)
{
    WMStatus *status = WMSP_GetStatusStructure();
    u32     wlBuf[128];
    WMCallback *cb;
    WlCmdCfm *pConfirm;

    if (status->state != WM_STATE_IDLE)
    {
        cb = (WMCallback *)WMSP_GetBuffer4Callback2Wm9();
        cb->apiid = WM_APIID_END;
        cb->errcode = WM_ERRCODE_ILLEGAL_STATE;
        WMSP_ReturnResult2Wm9((void *)cb);
        return;
    }

    ChangeWmState(status, WM_STATE_STOP);

    status->state = WM_STATE_READY;
    ChangeWmState(status, WM_STATE_READY);

    cb = (WMCallback *)WMSP_GetBuffer4Callback2Wm9();
    cb->apiid = WM_APIID_END;
    cb->errcode = WM_ERRCODE_SUCCESS;
    WMSP_ReturnResult2Wm9((void *)cb);
}

void WMSP_Enable(OSMessage msg)
{
    u64    *reqBuf = (u64 *)msg;
    WMCallback *cb;
    WMSPWork *sys = WMSP_GetSystemWork();
    WMArm7Buf *p;

    sys->wm7buf = (WMArm7Buf *)(reqBuf[1]);
    p = sys->wm7buf;
    p->status = sys->status = (WMStatus *)(reqBuf[2]);
    p->fifo7to9 = (u32 *)(reqBuf[3]);

    p->status->state = WM_STATE_STOP;

    cb = (WMCallback *)WMSP_GetBuffer4Callback2Wm9();
    cb->apiid = WM_APIID_ENABLE;
    cb->errcode = WM_ERRCODE_SUCCESS;
    WMSP_ReturnResult2Wm9((void *)cb);
}

void WMSP_Disable(OSMessage msg)
{
    WMCallback *cb;
    WMStatus *status = WMSP_GetStatusStructure();

    if (status->state != WM_STATE_STOP)
    {
        cb = (WMCallback *)WMSP_GetBuffer4Callback2Wm9();
        cb->apiid = WM_APIID_DISABLE;
        cb->errcode = WM_ERRCODE_ILLEGAL_STATE;
        WMSP_ReturnResult2Wm9((void *)cb);
        return;
    }

    status->state = WM_STATE_READY;

    cb = (WMCallback *)WMSP_GetBuffer4Callback2Wm9();
    cb->apiid = WM_APIID_DISABLE;
    cb->errcode = WM_ERRCODE_SUCCESS;
    WMSP_ReturnResult2Wm9((void *)cb);
}

void WMSP_PowerOn(OSMessage msg)
{
    WMCallback *cb;
    WMStatus *status = WMSP_GetStatusStructure();

    if (status->state != WM_STATE_STOP)
    {
        cb = (WMCallback *)WMSP_GetBuffer4Callback2Wm9();
        cb->apiid = WM_APIID_POWER_ON;
        cb->errcode = WM_ERRCODE_ILLEGAL_STATE;
        WMSP_ReturnResult2Wm9((void *)cb);
        return;
    }

    status->state = WM_STATE_IDLE;

    cb = (WMCallback *)WMSP_GetBuffer4Callback2Wm9();
    cb->apiid = WM_APIID_POWER_ON;
    cb->errcode = WM_ERRCODE_SUCCESS;
    WMSP_ReturnResult2Wm9((void *)cb);
}

void WMSP_PowerOff(OSMessage msg)
{
    WMStatus *status = WMSP_GetStatusStructure();
    WMCallback *cb;
    WlCmdCfm *pConfirm;

    if (status->state != WM_STATE_IDLE)
    {
        cb = (WMCallback *)WMSP_GetBuffer4Callback2Wm9();
        cb->apiid = WM_APIID_POWER_OFF;
        cb->errcode = WM_ERRCODE_ILLEGAL_STATE;
        WMSP_ReturnResult2Wm9((void *)cb);
        return;
    }

    status->state = WM_STATE_STOP;

    cb = (WMCallback *)WMSP_GetBuffer4Callback2Wm9();
    cb->apiid = WM_APIID_POWER_OFF;
    cb->errcode = WM_ERRCODE_SUCCESS;
    WMSP_ReturnResult2Wm9((void *)cb);
    return;
}

void WMSP_SetParentParam(OSMessage msg)
{
    #ifdef SDK_BUILD_ARM
    u32    *reqBuf = (u32 *)msg;
    #else
    u64    *reqBuf = (u64 *)msg;
    #endif
    WMStatus *status = WMSP_GetStatusStructure();
    u32     wlBuf[128];
    WMCallback *cb;
    WMParentParam *param;

    param = (WMParentParam *)(reqBuf[1]);

    MI_CpuCopy8((void *)param, (void *)&(status->pparam), WM_PARENT_PARAM_SIZE);

    SIM_Net_SetGameInfo(param->ggid, param->tgid, (u8*)param->userGameInfo, param->userGameInfoLength);

    cb = (WMCallback *)WMSP_GetBuffer4Callback2Wm9();
    cb->apiid = WM_APIID_SET_P_PARAM;
    cb->errcode = WM_ERRCODE_SUCCESS;
    WMSP_ReturnResult2Wm9((void *)cb);

    return;
}

void WMSP_StartParent(OSMessage msg)
{
    WMStatus *status = WMSP_GetStatusStructure();
    u32     wlBuf[128];
    u16    *buf = (u16 *)wlBuf;
    WMStartParentCallback *cb;
    u16     ssidLength;
    BOOL    powerSave;

    if (status->state != WM_STATE_IDLE)
    {
        WMStartParentCallback *callback = (WMStartParentCallback *)WMSP_GetBuffer4Callback2Wm9();
        callback->apiid = WM_APIID_START_PARENT;
        callback->errcode = WM_ERRCODE_ILLEGAL_STATE;
        callback->state = WM_STATECODE_PARENT_START;
        WMSP_ReturnResult2Wm9(callback);
        return;
    }

    status->mode = WL_CMDLABEL_MODE_PARENT;
    status->aid = 0;
    {

        OSIntrMode e = OS_DisableInterrupts();
        status->child_bitmap = 0;
        status->mp_readyBitmap = 0;
        (void)OS_RestoreInterrupts(e);
    }

    #ifndef SDK_BUILD_ARM
    SIM_Net_SetMPState(SIM_NET_MPSTATE_PARENT);
    #endif

    cb = (WMStartParentCallback *)WMSP_GetBuffer4Callback2Wm9();

    ChangeWmState(status, WM_STATE_PARENT);
    cb->apiid = WM_APIID_START_PARENT;
    cb->errcode = WM_ERRCODE_SUCCESS;
    cb->state = WM_STATECODE_PARENT_START;
    cb->parentSize = status->mp_parentSize;
    cb->childSize = status->mp_childSize;
    WMSP_ReturnResult2Wm9((void *)cb);
    status->beaconIndicateFlag = 1;

    return;

}

void WMSP_EndParent(OSMessage msg)
{
    WMStatus *status = WMSP_GetStatusStructure();
    u32     wlBuf[128];
    WMCallback *cb;
    WlCmdCfm *pConfirm;

    if (status->state != WM_STATE_PARENT)
    {
        WMCallback *callback = (WMCallback *)WMSP_GetBuffer4Callback2Wm9();
        callback->apiid = WM_APIID_END_PARENT;
        callback->errcode = WM_ERRCODE_ILLEGAL_STATE;
        WMSP_ReturnResult2Wm9(callback);
        return;
    }

    ChangeWmState(status, WM_STATE_IDLE);
    

    status->wep_flag = FALSE;
    status->wepMode = WL_CMDLABEL_WEP_NO;
    MI_CpuClear8(status->wepKey, WM_SIZE_WEPKEY);
    

    WMSP_ResetSizeVars();

    cb = (WMCallback *)WMSP_GetBuffer4Callback2Wm9();
    cb->apiid = WM_APIID_END_PARENT;
    cb->errcode = WM_ERRCODE_SUCCESS;
    WMSP_ReturnResult2Wm9((void *)cb);
}

void WMSP_StartScan(OSMessage msg)
{
    WMSPWork *const work = WMSP_GetSystemWork();
    WMStatus *status = WMSP_GetStatusStructure();

    u16     current_state;
    u16     scanChannel;
    u16     scanBssid[3];
    u16     scanMaxChannelTime;

    WMStartScanReq *args = (WMStartScanReq *)msg;

    if (status->state != WM_STATE_IDLE && status->state != WM_STATE_CLASS1 &&
        status->state != WM_STATE_SCAN)
    {
        WMStartScanCallback *callback = (WMStartScanCallback *)WMSP_GetBuffer4Callback2Wm9();
        callback->apiid = WM_APIID_START_SCAN;
        callback->errcode = WM_ERRCODE_ILLEGAL_STATE;
        callback->state = WM_STATECODE_PARENT_NOT_FOUND;
        WMSP_ReturnResult2Wm9(callback);
        return;
    }

    status->pInfoBuf = args->scanBuf;
    status->scan_channel = scanChannel = args->channel;
    scanMaxChannelTime = args->maxChannelTime;
    MI_CpuCopy8(args->bssid, scanBssid, WM_SIZE_MACADDR);

    if ((scanBssid[0] != 0xffff) && (scanBssid[0] & 0x0001))
    {
        OS_Printf("[ARM7] WM_StartScan: assigned Bssid is MulticastAddress. LSB is cleared.\n");
        scanBssid[0] &= ~(0x0001);
    }

    status->mode = WL_CMDLABEL_MODE_CHILD;

    ChangeWmState(status, WM_STATE_SCAN);

    WMStartScanCallback *callback = (WMStartScanCallback *)WMSP_GetBuffer4Callback2Wm9();

    WMBssDesc * dest = status->pInfoBuf;

    int parentsFound = SIM_Net_Scan(dest);

    SDL_Delay(1);

    if(parentsFound == 0) {
        callback->apiid = WM_APIID_START_SCAN;
        callback->errcode = WM_ERRCODE_SUCCESS;
        callback->state = WM_STATECODE_PARENT_NOT_FOUND;
        callback->channel = scanChannel;
        callback->linkLevel = WM_LINK_LEVEL_0;

        WMSP_ReturnResult2Wm9((void *)callback);
    } else {
        callback->apiid = WM_APIID_START_SCAN;
        callback->errcode = WM_ERRCODE_SUCCESS;
        callback->state = WM_STATECODE_PARENT_FOUND;

        callback->gameInfoLength = dest[0].gameInfoLength;
        callback->gameInfo.ggid = dest[0].gameInfo.ggid;
        callback->gameInfo.tgid = dest[0].gameInfo.tgid;
        callback->gameInfo.gameNameCount_attribute = dest[0].gameInfo.gameNameCount_attribute;

        WMSP_ReturnResult2Wm9((void *)callback);
    }
}

void WMSP_EndScan(OSMessage msg)
{
    WMSPWork *const work = WMSP_GetSystemWork();
    WMStatus *status = WMSP_GetStatusStructure();

 
    if ((status->state != WM_STATE_SCAN) && (status->state != WM_STATE_IDLE))
    {
        WMCallback *callback = (WMCallback *)WMSP_GetBuffer4Callback2Wm9();
        callback->apiid = WM_APIID_END_SCAN;
        callback->errcode = WM_ERRCODE_ILLEGAL_STATE;
        WMSP_ReturnResult2Wm9(callback);
        return;
    }

    ChangeWmState(status, WM_STATE_IDLE);

    {
        WMCallback *callback = (WMCallback *)WMSP_GetBuffer4Callback2Wm9();
        callback->apiid = WM_APIID_END_SCAN;
        callback->errcode = WM_ERRCODE_SUCCESS;
        WMSP_ReturnResult2Wm9(callback);
        return;
    }
}

void WMSP_StartConnectEx(OSMessage msg)
{
    WMSPWork *const work = WMSP_GetSystemWork();
    WMArm7Buf *const p = work->wm7buf;
    WMStatus *status = WMSP_GetStatusStructure();

    WMStartConnectReq *args = (WMStartConnectReq *)msg;

    if (status->state != WM_STATE_IDLE)
    {
        WMStartConnectCallback *callback = (WMStartConnectCallback *)WMSP_GetBuffer4Callback2Wm9();
        callback->apiid = WM_APIID_START_CONNECT;
        callback->errcode = WM_ERRCODE_ILLEGAL_STATE;
        callback->state = WM_STATECODE_CONNECT_START;
        WMSP_ReturnResult2Wm9(callback);
        return;
    }

    MI_CpuCopy8((void *)(args->pInfo), &(p->connectPInfo), WM_BSS_DESC_SIZE);

    {

        if (p->connectPInfo.gameInfoLength >= 16)
        {

            if (!(p->connectPInfo.gameInfo.attribute & WM_ATTR_FLAG_ENTRY))
            {

                WMStartConnectCallback *cb =
                    (WMStartConnectCallback *)WMSP_GetBuffer4Callback2Wm9();
                cb->apiid = WM_APIID_START_CONNECT;
                cb->errcode = WM_ERRCODE_NO_ENTRY;
                cb->state = WM_STATECODE_CONNECT_START;
                WMSP_ReturnResult2Wm9(cb);
                return;
            }
        }
    }

    {
        WMStartConnectCallback *callback = (WMStartConnectCallback *)WMSP_GetBuffer4Callback2Wm9();
        callback->apiid = WM_APIID_START_CONNECT;
        callback->errcode = WM_ERRCODE_SUCCESS;
        callback->state = WM_STATECODE_CONNECT_START;
        (void)WMSP_ReturnResult2Wm9((void *)callback);
    }

    SIM_Net_JoinMP(args->pInfo->bssid, args->ssid);

    status->aid = SIM_Net_GetAID();
    status->child_bitmap = 0x0001;
    status->mp_readyBitmap = 0x0001;

    ChangeWmState(status, WM_STATE_CHILD);
    status->pwrMgtMode = 1;

    status->beaconIndicateFlag = 1;

    {
        WMStartConnectCallback *callback =
            (WMStartConnectCallback *)WMSP_GetBuffer4Callback2Wm9();
        callback->apiid = WM_APIID_START_CONNECT;
        callback->errcode = WM_ERRCODE_SUCCESS;
        callback->state = WM_STATECODE_CONNECTED;
        callback->aid = status->aid;
        MI_CpuCopy8(status->parentMacAddress, callback->macAddress, WM_SIZE_MACADDR);
        callback->parentSize = status->mp_parentSize;
        callback->childSize = status->mp_childSize;
        WMSP_ReturnResult2Wm9((void *)callback);
    }
}

void WMSP_Disconnect(OSMessage msg)
{
    BOOL    result;
    u16     tryBmp;
    u16     resBmp;

    WMStatus *status = WMSP_GetStatusStructure();

    tryBmp = (u16)(((u32 *)msg)[1]);

    result = SIM_Net_DisconnectMP(tryBmp, &resBmp);

    if (result == TRUE)
    {
        if(status->state == WM_STATE_CHILD || status->state == WM_STATE_MP_CHILD) {
            ChangeWmState(status, WM_STATE_IDLE);
        }
        WMDisconnectCallback *const callback =
            (WMDisconnectCallback *)WMSP_GetBuffer4Callback2Wm9();
        callback->apiid = WM_APIID_DISCONNECT;
        callback->errcode = WM_ERRCODE_SUCCESS;
        callback->tryBitmap = tryBmp;
        callback->disconnectedBitmap = resBmp;
        (void)WMSP_ReturnResult2Wm9(callback);
        return;
    } else {
        SIM_assert_always();
    }
}

void WMSP_StartMP(OSMessage msg)
{

    u32    *buf = (u32 *)msg;
    WMSPWork *sys = WMSP_GetSystemWork();
    WMStatus *status = WMSP_GetStatusStructure();
    WMErrCode err = WM_ERRCODE_SUCCESS;

    WMStartMPReq *req;
    WMMpRecvBuf *recvBuf;
    u32     recvBufSize;
    u32    *sendBuf;
    u32     sendBufSize;
    WMMPParam *mpParam;

    req = (WMStartMPReq *) buf;
    recvBuf = (WMMpRecvBuf *)req->recvBuf;
    recvBufSize = req->recvBufSize;
    sendBuf = req->sendBuf;
    sendBufSize = req->sendBufSize;
    mpParam = &req->param;

    if (status->state == WM_STATE_CHILD)
    {

        ChangeWmState(status, WM_STATE_MP_CHILD);
        SIM_Net_SetMPState(SIM_NET_MPSTATE_CHILD);
    }
    else if (status->state == WM_STATE_PARENT)
    {

        ChangeWmState(status, WM_STATE_MP_PARENT);
    }

    status->mp_flag = TRUE;

    WMStartMPCallback *cb = (WMStartMPCallback *)WMSP_GetBuffer4Callback2Wm9();
    cb->apiid = WM_APIID_START_MP;
    cb->errcode = WM_ERRCODE_SUCCESS;
    cb->state = WM_STATECODE_MP_START;
    WMSP_ReturnResult2Wm9((void *)cb);
}

void WMSP_SetMPData(OSMessage msg)
{
    #ifdef SDK_BUILD_ARM
    u32    *buf = (u32 *)msg;
    #else
    u64 * buf = (u64*)msg;
    #endif
    WMSPWork *sys = WMSP_GetSystemWork();
    WMStatus *status = WMSP_GetStatusStructure();

    u16    *sendData;
    u16     sendSize;
    u32     destBitmap;
    u16     port;
    u16     priority;
    WMCallbackFunc callback;
    void   *arg;
    int     result;
    u16     myAid = status->aid;
    u32     childBitmap = status->child_bitmap;

    sendData = (u16 *)buf[1];
    sendSize = (u16)buf[2];
    destBitmap = buf[3];
    port = (u16)buf[4];
    priority = (u16)buf[5];
    callback = (WMCallbackFunc)buf[6];
    arg = (void *)buf[7];

    if (myAid != 0)
    {
        destBitmap = 0x0001U;
    }

    if (status->mp_flag == 0)
    {

        result = WM_ERRCODE_ILLEGAL_STATE;
    }
    else
    {
        if ((destBitmap & childBitmap) == 0)
        {

            result = WM_ERRCODE_SUCCESS;
        }
        else
        {

            BOOL sendResult = SIM_Net_SendMPData(destBitmap, port, (u8*)sendData, sendSize);
            if (!sendResult) {
                printf("SIM_Net_SendMPData failed!\n");
            } else {
                result = WM_ERRCODE_SUCCESS;
            }
        }
    }

    if (result != WM_ERRCODE_OPERATING)
    {

        WMPortSendCallback *cb_PortSend = (WMPortSendCallback *)WMSP_GetBuffer4Callback2Wm9();

        cb_PortSend->apiid = WM_APIID_PORT_SEND;
        cb_PortSend->errcode = (u16)result;
        cb_PortSend->state = WM_STATECODE_PORT_SEND;
        cb_PortSend->port = (u16)port;
        cb_PortSend->destBitmap = (u16)destBitmap;
        if (result == WM_ERRCODE_SEND_QUEUE_FULL)
        {
            cb_PortSend->restBitmap = (u16)(destBitmap & childBitmap);
        }
        else
        {
            cb_PortSend->restBitmap = 0;
        }
        cb_PortSend->sentBitmap = 0;
        cb_PortSend->size = sendSize;
        cb_PortSend->data = sendData;
        cb_PortSend->callback = callback;
        cb_PortSend->arg = arg;
        cb_PortSend->seqNo = 0xffff;

        {
            u16     parentSize = status->mp_parentSize;
            u16     childSize = status->mp_childSize;
            cb_PortSend->maxSendDataSize = (status->aid == 0) ? parentSize : childSize;
            cb_PortSend->maxRecvDataSize = (status->aid == 0) ? childSize : parentSize;
        }

        WMSP_ReturnResult2Wm9((void *)cb_PortSend);
    }
}

void WMSP_EndMP(OSMessage msg)
{
    WMStatus *status = WMSP_GetStatusStructure();

    if (status->state != WM_STATE_MP_PARENT && status->state != WM_STATE_MP_CHILD)
    {
        WMCallback *callback = (WMCallback *)WMSP_GetBuffer4Callback2Wm9();
        callback->apiid = WM_APIID_END_MP;
        callback->errcode = WM_ERRCODE_ILLEGAL_STATE;
        WMSP_ReturnResult2Wm9(callback);
        return;
    }

    if (status->state == WM_STATE_MP_CHILD)
    {

        ChangeWmState(status, WM_STATE_CHILD);
    }
    else if (status->state == WM_STATE_MP_PARENT)
    {

        ChangeWmState(status, WM_STATE_PARENT);
    }

    status->mp_setDataFlag = FALSE;
    status->mp_flag = FALSE;

    SIM_Net_SetMPState(SIM_NET_MPSTATE_OFF);

    {
        WMCallback *cb = (WMCallback *)WMSP_GetBuffer4Callback2Wm9();

        cb->apiid = WM_APIID_END_MP;
        cb->errcode = WM_ERRCODE_SUCCESS;

        WMSP_ReturnResult2Wm9((void *)cb);
    }
}

void WMSP_SetGameInfo(OSMessage msg)
{
    u64    *buf = (u64 *)msg;
    WMSPWork *sys = WMSP_GetSystemWork();
    WMStatus *status = WMSP_GetStatusStructure();
    u8      attribute;
    u16     length;

    u32     wlBuf[128];
    WlParamSetCfm *wlResult;

    status->pparam.userGameInfo = (u16 *)(buf[1]);
    status->pparam.userGameInfoLength = (u16)(buf[2]);
    status->pparam.ggid = buf[3];
    status->pparam.tgid = (u16)(buf[4]);
    attribute = (u8)(buf[5]);

    status->pparam.entryFlag = (u16)((attribute & WM_ATTR_FLAG_ENTRY) ? 1 : 0);
    status->pparam.multiBootFlag = (u16)((attribute & WM_ATTR_FLAG_MB) ? 1 : 0);
    status->pparam.KS_Flag = (u16)((attribute & WM_ATTR_FLAG_KS) ? 1 : 0);
    status->pparam.CS_Flag = (u16)((attribute & WM_ATTR_FLAG_CS) ? 1 : 0);

    SIM_Net_SetGameInfo(status->pparam.ggid, status->pparam.tgid, (u8*)status->pparam.userGameInfo, status->pparam.userGameInfoLength);

    {
        WMCallback *cb = (WMCallback *)WMSP_GetBuffer4Callback2Wm9();

        cb->apiid = WM_APIID_SET_GAMEINFO;
        cb->errcode = WM_ERRCODE_SUCCESS;

        WMSP_ReturnResult2Wm9((void *)cb);
    }
}

void WMSP_MeasureChannel(OSMessage msg)
{
    WMStatus *status = WMSP_GetStatusStructure();
    u32     wlBuf[128];
    u16    *buf = (u16 *)wlBuf;
    u16     ccaMode;
    u16     edThreshold;
    u16     measureTime;
    u16     channel;
    u16     current_state;
    WMMeasureChannelReq *args = (WMMeasureChannelReq *)msg;

    if (status->state != WM_STATE_IDLE)
    {
        WMMeasureChannelCallback *callback =
            (WMMeasureChannelCallback *)WMSP_GetBuffer4Callback2Wm9();
        callback->apiid = WM_APIID_MEASURE_CHANNEL;
        callback->errcode = WM_ERRCODE_ILLEGAL_STATE;
        WMSP_ReturnResult2Wm9(callback);
        return;
    }

    SDL_Delay(1);

    ChangeWmState(status, WM_STATE_IDLE);

    u16     cb_channel = args->channel;
    u16     cb_ccaBusyRatio = 0;
    WMMeasureChannelCallback *cb;

    cb = (WMMeasureChannelCallback *)WMSP_GetBuffer4Callback2Wm9();
    cb->apiid = WM_APIID_MEASURE_CHANNEL;
    cb->errcode = WM_ERRCODE_SUCCESS;
    cb->channel = cb_channel;
    cb->ccaBusyRatio = cb_ccaBusyRatio;
    WMSP_ReturnResult2Wm9((void *)cb);
}

void WMSP_StartScanEx(OSMessage msg)
{
    WMSPWork *const work = WMSP_GetSystemWork();
	WMStatus *status = WMSP_GetStatusStructure();
    WMStartScanExCallback *callback = (WMStartScanExCallback *)WMSP_GetBuffer4Callback2Wm9();
    WMStartScanExReq *args = (WMStartScanExReq *)msg;

    status->pInfoBuf = args->scanBuf;

    WMBssDesc * dest = status->pInfoBuf;

    int numParentsFound = SIM_Net_Scan(dest);

    strcpy(dest[numParentsFound].ssid, "SimAP");
    dest[numParentsFound].ssidLength = strlen("SimAP");
    numParentsFound++;

    SDL_Delay(1);

    if(numParentsFound == 0) {
 	    callback->apiid = WM_APIID_START_SCAN_EX;
	    callback->errcode = WM_ERRCODE_SUCCESS;
	    callback->state = WM_STATECODE_PARENT_NOT_FOUND;
	    callback->bssDescCount = 0;

        WMSP_ReturnResult2Wm9((void *)callback);
    } else {
 	    callback->apiid = WM_APIID_START_SCAN_EX;
	    callback->errcode = WM_ERRCODE_SUCCESS;
	    callback->state = WM_STATECODE_PARENT_FOUND;
	    callback->bssDescCount = numParentsFound;
        callback->bssDesc[0] = &dest[0];

        ChangeWmState(status, WM_STATE_SCAN);

        WMSP_ReturnResult2Wm9((void *)callback);
    }
}


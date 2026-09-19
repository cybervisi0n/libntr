#ifndef LIBRARIES_NWM_ARM9_NWM_ARM9_PRIVATE_H__
#define LIBRARIES_NWM_ARM9_NWM_ARM9_PRIVATE_H__

#if SDK_VERSION_MAJOR != 5
#error This file is for SDK 5 only!
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SDK_TWL
#include <twl.h>
#else
#include <nitro.h>
#endif

#include "nwm_common_private.h"

#define NWM_CHECK_RESULT(res)                                                  \
  if (((res) != NWM_RETCODE_SUCCESS) && ((res) != NWM_RETCODE_OPERATING)) {    \
    return (res);                                                              \
  }

#ifdef NWM_SUPPORT_HWRESET
extern PMExitCallbackInfo hwResetCbInfo;
#endif

#ifdef SDK_PORT
void NWMi_ReceiveFifo9(PXIFifoTag tag, u64 fifo_buf_adr, BOOL err);
#else
void NWMi_ReceiveFifo9(PXIFifoTag tag, u32 fifo_buf_adr, BOOL err);
#endif
void NWMi_SetCallbackTable(NWMApiid id, NWMCallbackFunc callback);
void NWMi_SetReceiveCallbackTable(NWMFramePort port, NWMCallbackFunc callback);
NWMRetCode NWMi_SendCommand(NWMApiid id, u16 paramNum, ...);
NWMRetCode NWMi_SendCommandDirect(void *data, u32 length);
NWMRetCode NWMi_CheckState(s32 paramNum, ...);
NWMArm9Buf *NWMi_GetSystemWork(void);
NWMRetCode NWMi_CheckInitialized(void);
void NWMi_ClearFifoRecvFlag(void);
NWMRetCode NWMi_RegisterFirmware(void *addr, u32 size);

BOOL NWMi_CheckEnableFlag(void);

#ifdef NWM_SUPPORT_HWRESET
void NWMi_ForceExit(void *arg);
#endif

inline BOOL NWMi_IsAPIActive(u16 apiid) {
  NWMArm9Buf *sys = NWMi_GetSystemWork();

  if (sys) {

    DC_InvalidateRange(&sys->status->apiActiveFlag[apiid], sizeof(u8));

    return (sys->status->apiActiveFlag[apiid] == 1) ? TRUE : FALSE;
  }
  return FALSE;
}

NWMRetCode NWMi_InstallFirmware(NWMCallbackFunc callback, void *addr, u32 size,
                                BOOL isColdstart);
u32 NWMi_GetFirmImageOffset(void *addr, u32 fwType);
u32 NWMi_GetFirmImageLength(void *addr, u32 fwType);
u8 *NWMi_GetFirmImageHashAddress(void *addr, u32 fwType);

BOOL NWMi_CheckFirmDataParamIntegrity(void);

#ifdef NWM_INCLUDE_FIRM
NWMRetCode
NWMi_LoadDeviceEx(NWMCallbackFunc callback); /* Transfers wireless firmware */
#endif

NWMRetCode NWMi_SetWPAKey(NWMCallbackFunc callback, u8 cipherType,
                          u8 macAddr[6], u8 keyIndex, u8 keyLen, u8 *keyData,
                          u8 keyRsc[8]);

NWMRetCode NWMi_SetWPAParams(NWMCallbackFunc callback, u32 operation,
                             u32 value);

NWMRetCode NWMi_CreateQoS(NWMCallbackFunc callback, NWMQoSParam *param);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LIBRARIES_NWM_ARM9_NWM_ARM9_PRIVATE_H__ */

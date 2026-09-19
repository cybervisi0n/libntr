#pragma once

#ifdef __cplusplus
extern "C" {
#endif

enum {

  ISTDSIO_MAX_PAYLOAD_SIZE = 16384
};

BOOL ISTDSIOSend(u16 chn, const void *pSrc, u32 nSize);

typedef void (*ISTDSIOSendCbFunc)(void *pUser);
BOOL ISTDSIOSendAsync(u16 chn, const void *pSrc, u32 nSize,
                      ISTDSIOSendCbFunc fncSendCompleteCb, void *pUser);

typedef void (*ISTDSIORecvCbFunc)(void *pUser, u16 chn, const void *pBuf,
                                  u32 nSize);
void ISTDSIOSetRecvCallback(ISTDSIORecvCbFunc cbRecv, void *pUser);

#ifdef __cplusplus
}
#endif

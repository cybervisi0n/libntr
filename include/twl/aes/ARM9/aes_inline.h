#ifndef TWL_AES_ARM9_AES_INLINE_H_
#define TWL_AES_ARM9_AES_INLINE_H_

#include <nitro/os/common/emulator.h>
#include <twl/aes/common/types.h>
#include <twl/aes/ARM9/aes_internal.h>

#ifdef __cplusplus
extern "C" {
#endif

SDK_INLINE void AES_Init(void) {
  if (OS_IsRunOnTwl()) {
    AESi_Init();
  }
}

SDK_INLINE AESResult AES_SetKey(const AESKey *pKey) {
  if (OS_IsRunOnTwl()) {
    return AESi_SetKey(pKey);
  } else {
    return AES_RESULT_ON_DS;
  }
}

SDK_INLINE void AES_AddToCounter(AESCounter *pCounter, u32 value) {
  if (OS_IsRunOnTwl()) {
    AESi_AddToCounter(pCounter, value);
  }
}

SDK_INLINE void AES_ReverseBytes(const void *src, void *dst, u32 size) {
  if (OS_IsRunOnTwl()) {
    AESi_ReverseBytes(src, dst, size);
  }
}

SDK_INLINE void AES_SwapEndianEach128(const void *src, void *dst, u32 size) {
  if (OS_IsRunOnTwl()) {
    AESi_SwapEndianEach128(src, dst, size);
  }
}

SDK_INLINE AESResult AES_Ctr(const AESCounter *pCounter, const void *src,
                             u32 srcSize, void *dst, AESCallback callback,
                             void *arg) {
  if (OS_IsRunOnTwl()) {
    return AESi_Ctr(pCounter, src, srcSize, dst, callback, arg);
  } else {
    return AES_RESULT_ON_DS;
  }
}

SDK_INLINE AESResult AES_CtrEncrypt(const AESCounter *pCounter, const void *src,
                                    u32 srcSize, void *dst,
                                    AESCallback callback, void *arg) {
  return AES_Ctr(pCounter, src, srcSize, dst, callback, arg);
}
SDK_INLINE AESResult AES_CtrDecrypt(const AESCounter *pCounter, const void *src,
                                    u32 srcSize, void *dst,
                                    AESCallback callback, void *arg) {
  return AES_Ctr(pCounter, src, srcSize, dst, callback, arg);
}

SDK_INLINE AESResult
AES_CcmEncryptAndSign(const AESNonce *pNonce, const void *src, u32 srcASize,
                      u32 srcPSize, AESMacLength macLength,
                      void *dst, // Require size = srcBSize + macSize
                      AESCallback callback, void *arg) {
  if (OS_IsRunOnTwl()) {
    return AESi_CcmEncryptAndSign(pNonce, src, srcASize, srcPSize, macLength,
                                  dst, callback, arg);
  } else {
    return AES_RESULT_ON_DS;
  }
}

SDK_INLINE AESResult
AES_CcmDecryptAndVerify(const AESNonce *pNonce, const void *src, u32 srcASize,
                        u32 srcCSize, AESMacLength macLength,
                        void *dst, // Require size = srcCSize - macSize
                        AESCallback callback, void *arg) {
  if (OS_IsRunOnTwl()) {
    return AESi_CcmDecryptAndVerify(pNonce, src, srcASize, srcCSize, macLength,
                                    dst, callback, arg);
  } else {
    return AES_RESULT_ON_DS;
  }
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TWL_AES_ARM9_AES_INLINE_H_ */

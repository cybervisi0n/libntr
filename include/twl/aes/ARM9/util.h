#ifndef TWL_AES_ARM9_UTIL_H_
#define TWL_AES_ARM9_UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/os/common/emulator.h>
#include <twl/aes/common/types.h>

#define AES_ENCRYPT_HEADER_SIZE 16
#define AES_SIGN_HEADER_SIZE 32

void AESi_InitRand(void);
void AESi_PrepairEncryptAndSign(AESNonce *pNonce, u32 srcSize, void *dst);
AESResult AESi_PrepairDecryptAndVerify(AESNonce *pNonce, const void *src,
                                       u32 srcSize);

AESResult AESi_Rand(void *pBuffer, u32 size);

AESResult AESi_Encrypt(const void *src, u32 srcSize, void *dst,
                       AESCallback callback, void *arg);

AESResult AESi_Decrypt(const void *src, u32 srcSize, void *dst,
                       AESCallback callback, void *arg);

AESResult AESi_EncryptAndSign(const void *src, u32 srcSize, void *dst,
                              AESCallback callback, void *arg);

AESResult AESi_DecryptAndVerify(const void *src, u32 srcSize, void *dst,
                                AESCallback callback, void *arg);

SDK_INLINE AESResult AES_Rand(void *pBuffer, u32 size) {
  if (OS_IsRunOnTwl()) {
    return AESi_Rand(pBuffer, size);
  } else {
    return AES_RESULT_ON_DS;
  }
}

SDK_INLINE AESResult AES_Encrypt(const void *src, u32 srcSize, void *dst,
                                 AESCallback callback, void *arg) {
  if (OS_IsRunOnTwl()) {
    return AESi_Encrypt(src, srcSize, dst, callback, arg);
  } else {
    return AES_RESULT_ON_DS;
  }
}

SDK_INLINE AESResult AES_Decrypt(const void *src, u32 srcSize, void *dst,
                                 AESCallback callback, void *arg) {
  if (OS_IsRunOnTwl()) {
    return AESi_Decrypt(src, srcSize, dst, callback, arg);
  } else {
    return AES_RESULT_ON_DS;
  }
}

SDK_INLINE AESResult AES_EncryptAndSign(const void *src, u32 srcSize, void *dst,
                                        AESCallback callback, void *arg) {
  if (OS_IsRunOnTwl()) {
    return AESi_EncryptAndSign(src, srcSize, dst, callback, arg);
  } else {
    return AES_RESULT_ON_DS;
  }
}

SDK_INLINE AESResult AES_DecryptAndVerify(const void *src, u32 srcSize,
                                          void *dst, AESCallback callback,
                                          void *arg) {
  if (OS_IsRunOnTwl()) {
    return AESi_DecryptAndVerify(src, srcSize, dst, callback, arg);
  } else {
    return AES_RESULT_ON_DS;
  }
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TWL_AES_ARM9_UTIL_H_ */

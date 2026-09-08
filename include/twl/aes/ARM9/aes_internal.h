#ifndef TWL_AES_COMMON_AES_INTERNAL_H_
#define TWL_AES_COMMON_AES_INTERNAL_H_

#include <twl/aes/common/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AES_SIGN_FOR_JPEG_SIZE 28

u32 AESi_GetExpRegionBegin(void);
u32 AESi_GetExpRegionEnd(void);

void AESi_Init(void);

AESResult AESi_SetKey(const AESKey *pKey);
void AESi_AddToCounter(AESCounter *pCounter, u32 value);
void AESi_ReverseBytes(const void *src, void *dst, u32 size);
void AESi_SwapEndianEach128(const void *src, void *dst, u32 size);

AESResult AESi_SeedRand(const void *pSeed);
AESResult AESi_RandUnit(void *dst);
AESResult AESi_Ctr(const AESCounter *pCounter, const void *src, u32 srcSize,
                   void *dst, AESCallback callback, void *arg);

AESResult AESi_CcmEncryptAndSign(const AESNonce *pNonce, const void *src,
                                 u32 srcASize, u32 srcPSize,
                                 AESMacLength macLength,
                                 void *dst, // Require size = srcBSize + macSize
                                 AESCallback callback, void *arg);

AESResult
AESi_CcmDecryptAndVerify(const AESNonce *pNonce, const void *src, u32 srcASize,
                         u32 srcCSize, AESMacLength macLength,
                         void *dst, // Require size = srcCSize - macSize
                         AESCallback callback, void *arg);

AESResult AESi_CalcMac(const AESNonce *pNonce, const void *src, u32 srcSize,
                       void *mac, AESCallback callback, void *arg);

AESResult AESi_SignForJpeg(void *sign, const void *src, u32 srcSize);
AESResult AESi_VerifySignForJpeg(const void *sign, const void *src,
                                 u32 srcSize);
BOOL AESi_UseCustomKeyForJpeg(BOOL bEnable);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TWL_AES_COMMON_AES_INTERNAL_H_ */

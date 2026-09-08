#ifndef TWL_AES_COMMON_TYPE_H_
#define TWL_AES_COMMON_TYPE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <twl/misc.h>
#include <twl/types.h>

#define AES_BLOCK_SIZE 16   // 128-bit
#define AES_KEY_SIZE 16     // 128-bit
#define AES_NONCE_SIZE 12   //  96-bit
#define AES_MAC_MAX_SIZE 16 // 128-bit
#define AES_COUNTER_SIZE 16 // 128-bit

#define AES_ADATA_BLOCK_NUM_MAX 0xFFFF
#define AES_PDATA_BLOCK_NUM_MAX 0xFFFF

#define AES_ADATA_SIZE_MAX (AES_BLOCK_SIZE * AES_ADATA_BLOCK_NUM_MAX)
#define AES_PDATA_SIZE_MAX (AES_BLOCK_SIZE * AES_PDATA_BLOCK_NUM_MAX)

#define AESi_ASSERT_MAC_LENGTH(x)                                              \
  SDK_TASSERTMSG(((x) == AES_MAC_LENGTH_4) || ((x) == AES_MAC_LENGTH_6) ||     \
                     ((x) == AES_MAC_LENGTH_8) ||                              \
                     ((x) == AES_MAC_LENGTH_10) ||                             \
                     ((x) == AES_MAC_LENGTH_12) ||                             \
                     ((x) == AES_MAC_LENGTH_14) || ((x) == AES_MAC_LENGTH_16), \
                 "%s(=%d) is not valid AESMacLength.", #x, (x))

typedef enum AESResult {
  AES_RESULT_NONE,    // The processing result has not been obtained
  AES_RESULT_SUCCESS, // Encryption, decryption, or verification was successful
  AES_RESULT_VERIFICATION_FAILED, // Authentication failed
  AES_RESULT_INVALID,             // Invalid argument
  AES_RESULT_BUSY,                // AES processing is in progress
  AES_RESULT_ON_DS,   // Unusable because the program is running on a DS
  AES_RESULT_UNKNOWN, // Internal library error
  AES_RESULT_MAX
} AESResult;

typedef enum AESMacLength {
  AES_MAC_LENGTH_4 = 1, // 4 bytes
  AES_MAC_LENGTH_6 = 2,
  AES_MAC_LENGTH_8 = 3,
  AES_MAC_LENGTH_10 = 4,
  AES_MAC_LENGTH_12 = 5,
  AES_MAC_LENGTH_14 = 6,
  AES_MAC_LENGTH_16 = 7, // 16 bytes
  AES_MAC_LENGTH_MAX
} AESMacLength;

typedef union AESKey {
  u8 bytes[AES_KEY_SIZE];
  u32 words[AES_KEY_SIZE / sizeof(u32)];
} AESKey;

typedef union AESNonce {
  u8 bytes[AES_NONCE_SIZE];
  u32 words[AES_NONCE_SIZE / sizeof(u32)];
} AESNonce;

typedef union AESMac {
  u8 bytes[AES_MAC_MAX_SIZE];
  u32 words[AES_MAC_MAX_SIZE / sizeof(u32)];
} AESMac;

typedef union AESCounter {
  u8 bytes[AES_COUNTER_SIZE];
  u32 words[AES_COUNTER_SIZE / sizeof(u32)];
} AESCounter;

typedef void (*AESCallback)(AESResult result, void *arg);

static inline u32 AES_GetMacLengthValue(AESMacLength macLength) {
  AESi_ASSERT_MAC_LENGTH(macLength);

  return (u32)macLength * 2 + 2;
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TWL_AES_COMMON_TYPE_H_ */

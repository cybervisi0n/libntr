#ifndef NITRO_CRYPTO_RSA_H_
#define NITRO_CRYPTO_RSA_H_

#ifdef __cplusplus
extern "C" {
#endif

#define CRYPTO_RSA_VERIFY // Define this to enable RSA signature verification

#define CRYPTO_RSA_CONTEXT_SIZE (4 * 1)
#define CRYPTO_RSA_SIGN_CONTEXT_SIZE (4 * 1)

typedef struct CRYPTORSAContext {

  u8 mem[CRYPTO_RSA_CONTEXT_SIZE];
} CRYPTORSAContext;

typedef struct CRYPTORSAEncryptInitParam {
  void *key;   // [in] Public key string
  u32 key_len; // [in] Public key string length
} CRYPTORSAEncryptInitParam;

typedef struct CRYPTORSAEncryptInitPKParam {
  void *privkey;   // [in] Private key string
  u32 privkey_len; // [in] Private key string length
} CRYPTORSAEncryptInitPKParam;

typedef struct CRYPTORSAEncryptParam {
  void *in;     // [in] Encryption string
  u32 in_len;   // [in] Encryption string length
  void *out;    // [out] Output string buffer
  u32 out_size; // [in] Output string buffer size
} CRYPTORSAEncryptParam;

typedef struct CRYPTORSADecryptInitParam {
  void *key;   // [in] Public key string
  u32 key_len; // [in] Public key string length
} CRYPTORSADecryptInitParam;

typedef struct CRYPTORSADecryptParam {
  void *in;     // [in] Decryption string
  u32 in_len;   // [in] Decryption string length
  void *out;    // [out] Output string buffer
  u32 out_size; // [in] Output string buffer size
} CRYPTORSADecryptParam;

typedef struct CRYPTORSASignContext {

  u8 mem[CRYPTO_RSA_SIGN_CONTEXT_SIZE];
} CRYPTORSASignContext;

typedef struct CRYPTORSASignInitParam {
  void *key;   // [in] Private key string
  u32 key_len; // [in] Private key string length
} CRYPTORSASignInitParam;

typedef struct CRYPTORSASignParam {
  void *in;     // [in] Target signature string
  u32 in_len;   // [in] Target signature string length
  void *out;    // [out] Buffer for the output signature string
  u32 out_size; // [in] Buffer size for the output signature string
} CRYPTORSASignParam;

#if defined(CRYPTO_RSA_VERIFY)

typedef struct CRYPTORSAVerifyInitParam {
  void *key;   // [in] Public key string
  u32 key_len; // [in] Public key string length
} CRYPTORSAVerifyInitParam;

typedef struct CRYPTORSAVerifyParam {
  void *in;     // [in] String to verify
  u32 in_len;   // [in] String length to verify
  void *sign;   // [in] Signature string
  u32 sign_len; // [in] Signature string length
} CRYPTORSAVerifyParam;
#endif

s32 CRYPTO_RSA_EncryptInit(CRYPTORSAContext *context,
                           CRYPTORSAEncryptInitParam *param);

s32 CRYPTO_RSA_EncryptInit_PrivateKey(CRYPTORSAContext *context,
                                      CRYPTORSAEncryptInitPKParam *param);

s32 CRYPTO_RSA_Encrypt(CRYPTORSAContext *context, CRYPTORSAEncryptParam *param);

s32 CRYPTO_RSA_EncryptTerminate(CRYPTORSAContext *context);

s32 CRYPTO_RSA_DecryptInit(CRYPTORSAContext *context,
                           CRYPTORSADecryptInitParam *param);

s32 CRYPTO_RSA_Decrypt(CRYPTORSAContext *context, CRYPTORSADecryptParam *param);

s32 CRYPTO_RSA_DecryptTerminate(CRYPTORSAContext *context);

s32 CRYPTO_RSA_SignInit(CRYPTORSASignContext *context,
                        CRYPTORSASignInitParam *param);

s32 CRYPTO_RSA_Sign(CRYPTORSASignContext *context, CRYPTORSASignParam *param);

s32 CRYPTO_RSA_SignTerminate(CRYPTORSASignContext *context);

#if defined(CRYPTO_RSA_VERIFY)

s32 CRYPTO_RSA_VerifyInit(CRYPTORSASignContext *context,
                          CRYPTORSAVerifyInitParam *param);

s32 CRYPTO_RSA_Verify(CRYPTORSASignContext *context,
                      CRYPTORSAVerifyParam *param);

s32 CRYPTO_RSA_VerifyTerminate(CRYPTORSASignContext *context);
#endif

#ifdef __cplusplus
}
#endif

#endif // NITRO_CRYPTO_RSA_H_

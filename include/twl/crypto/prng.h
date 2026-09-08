#ifndef NITRO_CRYPTO_PRNG_H_
#define NITRO_CRYPTO_PRNG_H_

#ifdef __cplusplus
extern "C" {
#endif

s32 CRYPTO_PRNG_GatherEntropy(void);

s32 CRYPTO_PRNG_GenerateRandom(u8 *randomBytes, u32 size);

#ifdef __cplusplus
}
#endif

#endif // NITRO_CRYPTO_PRNG_H_

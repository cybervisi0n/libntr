#include <twl.h>

#define SHA1_MAC_LEN 20

static void pbkdf2_sha1(const u8 *passphrase, const u8 *ssid, u8 ssid_len,
                        u32 iterations, u8 *buf, u32 buflen);

static void pbkdf2_sha1_f(const u8 *passphrase, const u8 *ssid, u32 ssid_len,
                          u32 iterations, u32 count, u8 *digest);

#ifndef SDK_TWL

static void hmac_sha1_vector(const u8 *key, u32 key_len, u32 num_elem,
                             const u8 *addr[], const u32 *len, u8 *mac);

static void sha1_vector(u32 num_elem, const u8 *addr[], const u32 *len,
                        u8 *mac);

#endif

void NWM_Passphrase2PSK(const u8 passphrase[NWM_WPA_PASSPHRASE_LENGTH_MAX],
                        const u8 *ssid, u8 ssidlen,
                        u8 psk[NWM_WPA_PSK_LENGTH]) {
  pbkdf2_sha1(passphrase, (const u8 *)ssid, ssidlen, 4096, psk,
              NWM_WPA_PSK_LENGTH);
}

void pbkdf2_sha1(const u8 *passphrase, const u8 *ssid, u8 ssid_len,
                 u32 iterations, u8 *buf, u32 buflen) {
  u32 count = 0;
  u8 *pos = buf;
  u32 left = buflen, plen;
  u8 digest[SHA1_MAC_LEN];

  while (left > 0) {
    count++;
    pbkdf2_sha1_f(passphrase, ssid, ssid_len, iterations, count, digest);
    plen = left > SHA1_MAC_LEN ? SHA1_MAC_LEN : left;
    MI_CpuCopy8(digest, pos, plen);
    pos += plen;
    left -= plen;
  }
}

static void pbkdf2_sha1_f(const u8 *passphrase, const u8 *ssid, u32 ssid_len,
                          u32 iterations, u32 count, u8 *digest) {
  int i, j;
  u8 tmp[SHA1_MAC_LEN], tmp2[SHA1_MAC_LEN];
  u8 count_buf[4];
  const u8 *addr[2];
  u32 len[2];
  u32 passphrase_len = (u32)STD_StrLen((const char *)passphrase);
#ifdef SDK_TWL
  SVCHMACSHA1Context ctx;
#endif

  addr[0] = (const u8 *)ssid;
  len[0] = (u32)ssid_len;
  addr[1] = count_buf;
  len[1] = 4;

  count_buf[0] = (u8)((count >> 24) & 0xff);
  count_buf[1] = (u8)((count >> 16) & 0xff);
  count_buf[2] = (u8)((count >> 8) & 0xff);
  count_buf[3] = (u8)(count & 0xff);
#ifdef SDK_TWL
  MI_CpuClear8(&ctx, sizeof(ctx));
  SVC_HMACSHA1Init(&ctx, (const void *)passphrase, (u32)passphrase_len);
  for (i = 0; i < 2; i++) {
    SVC_HMACSHA1Update(&ctx, (const void *)addr[i], (u32)len[i]);
  }
  SVC_HMACSHA1GetHash(&ctx, (void *)tmp);
#else
  hmac_sha1_vector((const u8 *)passphrase, passphrase_len, 2, addr, len, tmp);
#endif
  MI_CpuCopy8(tmp, digest, SHA1_MAC_LEN);

  for (i = 1; i < iterations; i++) {
    addr[0] = (const u8 *)tmp;
    len[0] = SHA1_MAC_LEN;
#ifdef SDK_TWL
    MI_CpuClear8(&ctx, sizeof(ctx));
    SVC_HMACSHA1Init(&ctx, (const void *)passphrase, (u32)passphrase_len);
    SVC_HMACSHA1Update(&ctx, (const void *)addr[0], (u32)len[0]);
    SVC_HMACSHA1GetHash(&ctx, (void *)tmp2);
#else
    hmac_sha1_vector((const u8 *)passphrase, passphrase_len, 1, addr, len,
                     tmp2);
#endif
    MI_CpuCopy8(tmp2, tmp, SHA1_MAC_LEN);
    for (j = 0; j < SHA1_MAC_LEN; j++)
      digest[j] ^= tmp2[j];
  }
}

#ifndef SDK_TWL

void hmac_sha1_vector(const u8 *key, u32 key_len, u32 num_elem,
                      const u8 *addr[], const u32 *len, u8 *mac) {
  u8 k_pad[64]; /* padding - key XORd with ipad/opad */
  u8 tk[20];
  int i;
  const u8 *_addr[6];
  u32 _len[6];

  if (num_elem > 5) {

    return;
  }

  if (key_len > 64) {
    sha1_vector(1, &key, &key_len, tk);
    key = tk;
    key_len = 20;
  }

  MI_CpuClear8(k_pad, sizeof(k_pad));
  MI_CpuCopy8(key, k_pad, key_len);

  for (i = 0; i < 64; i++)
    k_pad[i] ^= 0x36;

  _addr[0] = k_pad;
  _len[0] = 64;
  for (i = 0; i < num_elem; i++) {
    _addr[i + 1] = addr[i];
    _len[i + 1] = len[i];
  }
  sha1_vector(1 + num_elem, _addr, _len, mac);

  MI_CpuClear8(k_pad, sizeof(k_pad));
  MI_CpuCopy8(key, k_pad, key_len);

  for (i = 0; i < 64; i++)
    k_pad[i] ^= 0x5c;

  _addr[0] = k_pad;
  _len[0] = 64;
  _addr[1] = mac;
  _len[1] = SHA1_MAC_LEN;
  sha1_vector(2, _addr, _len, mac);
}

void sha1_vector(u32 num_elem, const u8 *addr[], const u32 *len, u8 *mac) {
  int i;
#ifdef SDK_TWL
  SVCSHA1Context context;
#else
  MATHSHA1Context context;
#endif

  MI_CpuClear8(&context, sizeof(context));

#ifdef SDK_TWL
  SVC_SHA1Init(&context);
#else
  MATH_SHA1Init(&context);
#endif

  for (i = 0; i < num_elem; i++) {
#ifdef SDK_TWL
    SVC_SHA1Update(&context, addr[i], len[i]);
#else
    MATH_SHA1Update(&context, (const void *)addr[i], len[i]);
#endif
  }
#ifdef SDK_TWL
  SVC_SHA1GetHash(&context, mac);
#else
  MATH_SHA1GetHash(&context, (void *)mac);
#endif
}

#endif /* #ifndef SDK_TWL */

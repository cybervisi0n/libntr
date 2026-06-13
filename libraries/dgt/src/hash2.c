#include <nitro.h>
#include <nitro/dgt/dgt.h>

typedef void (*Hash2ProcessMessageBlockFunc)(DGTHash2Context *, u8 *, u32);

#define SHA1HANDSOFF

static int DGTi_OverlayTableMode;

void DGTi_hash2_arm4_small(DGTHash2Context *ctx, u8 *data, u32 num);
void DGTi_Hash2ProcessMessageBlock(DGTHash2Context *ctx, u8 *data, u32 len);

static Hash2ProcessMessageBlockFunc DGTi_Hash2ProcessMessageBlockFunc = DGTi_Hash2ProcessMessageBlock;

#define rol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

#define blk0_le(i) (block->l[i] = (rol(block->l[i],24)&0xFF00FF00) \
    |(rol(block->l[i],8)&0x00FF00FF))
#define blk0_be(i) block->l[i]
#if BYTE_ORDER == LITTLE_ENDIAN
#define blk0(i) blk0_le(i)
#elif BYTE_ORDER == BIG_ENDIAN
#define blk0(i) blk0_be(i)
#else
const union {
    long l;
    char c;
} sha1_endian = { 1 };
#define blk0(i) (sha1_endian.c == 0 ? blk0_be(i) : blk0_le(i))
#endif

#define blk(i) (block->l[i&15] = rol(block->l[(i+13)&15]^block->l[(i+8)&15] \
    ^block->l[(i+2)&15]^block->l[i&15],1))

#define R0(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk0(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R1(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R2(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0x6ED9EBA1+rol(v,5);w=rol(w,30);
#define R3(v,w,x,y,z,i) z+=(((w|x)&y)|(w&x))+blk(i)+0x8F1BBCDC+rol(v,5);w=rol(w,30);
#define R4(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0xCA62C1D6+rol(v,5);w=rol(w,30);



void dgti_SHA1Transform(
    u32 state[5],
    const unsigned char buffer[64]
)
{
    u32 a, b, c, d, e;

    typedef union
    {
        unsigned char c[64];
        u32 l[16];
    } CHAR64LONG16;

#ifdef SHA1HANDSOFF
    CHAR64LONG16 block[1];

    memcpy(block, buffer, 64);
#else
    CHAR64LONG16 *block = (const CHAR64LONG16 *) buffer;
#endif
    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];
    e = state[4];
    R0(a, b, c, d, e, 0);
    R0(e, a, b, c, d, 1);
    R0(d, e, a, b, c, 2);
    R0(c, d, e, a, b, 3);
    R0(b, c, d, e, a, 4);
    R0(a, b, c, d, e, 5);
    R0(e, a, b, c, d, 6);
    R0(d, e, a, b, c, 7);
    R0(c, d, e, a, b, 8);
    R0(b, c, d, e, a, 9);
    R0(a, b, c, d, e, 10);
    R0(e, a, b, c, d, 11);
    R0(d, e, a, b, c, 12);
    R0(c, d, e, a, b, 13);
    R0(b, c, d, e, a, 14);
    R0(a, b, c, d, e, 15);
    R1(e, a, b, c, d, 16);
    R1(d, e, a, b, c, 17);
    R1(c, d, e, a, b, 18);
    R1(b, c, d, e, a, 19);
    R2(a, b, c, d, e, 20);
    R2(e, a, b, c, d, 21);
    R2(d, e, a, b, c, 22);
    R2(c, d, e, a, b, 23);
    R2(b, c, d, e, a, 24);
    R2(a, b, c, d, e, 25);
    R2(e, a, b, c, d, 26);
    R2(d, e, a, b, c, 27);
    R2(c, d, e, a, b, 28);
    R2(b, c, d, e, a, 29);
    R2(a, b, c, d, e, 30);
    R2(e, a, b, c, d, 31);
    R2(d, e, a, b, c, 32);
    R2(c, d, e, a, b, 33);
    R2(b, c, d, e, a, 34);
    R2(a, b, c, d, e, 35);
    R2(e, a, b, c, d, 36);
    R2(d, e, a, b, c, 37);
    R2(c, d, e, a, b, 38);
    R2(b, c, d, e, a, 39);
    R3(a, b, c, d, e, 40);
    R3(e, a, b, c, d, 41);
    R3(d, e, a, b, c, 42);
    R3(c, d, e, a, b, 43);
    R3(b, c, d, e, a, 44);
    R3(a, b, c, d, e, 45);
    R3(e, a, b, c, d, 46);
    R3(d, e, a, b, c, 47);
    R3(c, d, e, a, b, 48);
    R3(b, c, d, e, a, 49);
    R3(a, b, c, d, e, 50);
    R3(e, a, b, c, d, 51);
    R3(d, e, a, b, c, 52);
    R3(c, d, e, a, b, 53);
    R3(b, c, d, e, a, 54);
    R3(a, b, c, d, e, 55);
    R3(e, a, b, c, d, 56);
    R3(d, e, a, b, c, 57);
    R3(c, d, e, a, b, 58);
    R3(b, c, d, e, a, 59);
    R4(a, b, c, d, e, 60);
    R4(e, a, b, c, d, 61);
    R4(d, e, a, b, c, 62);
    R4(c, d, e, a, b, 63);
    R4(b, c, d, e, a, 64);
    R4(a, b, c, d, e, 65);
    R4(e, a, b, c, d, 66);
    R4(d, e, a, b, c, 67);
    R4(c, d, e, a, b, 68);
    R4(b, c, d, e, a, 69);
    R4(a, b, c, d, e, 70);
    R4(e, a, b, c, d, 71);
    R4(d, e, a, b, c, 72);
    R4(c, d, e, a, b, 73);
    R4(b, c, d, e, a, 74);
    R4(a, b, c, d, e, 75);
    R4(e, a, b, c, d, 76);
    R4(d, e, a, b, c, 77);
    R4(c, d, e, a, b, 78);
    R4(b, c, d, e, a, 79);
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
    a = b = c = d = e = 0;
#ifdef SHA1HANDSOFF
    memset(block, '\0', sizeof(block));
#endif
}

void DGT_Hash2Reset(DGTHash2Context* ctx)
{
  MI_CpuClear8(ctx, sizeof(DGTHash2Context));
  ctx->h0 = 0x67452301;
  ctx->h1 = 0xEFCDAB89;
  ctx->h2 = 0x98BADCFE;
  ctx->h3 = 0x10325476;
  ctx->h4 = 0xC3D2E1F0;
  ctx->Nl = ctx->Nh = 0;
  return;
}

void DGT_Hash2SetSource(DGTHash2Context* ctx, const unsigned char* data, unsigned long len)
{
    u8 * ctxData = (u8*)ctx->data;
    u32 * ctxCount = (u32*)&ctx->Nl;
    u32 * ctxState = (u32*)&ctx->h0;

    u32 i;

    u32 j;

    j = ctxCount[0];
    if ((ctxCount[0] += len << 3) < j)
        ctxCount[1]++;
    ctxCount[1] += (len >> 29);
    j = (j >> 3) & 63;
    if ((j + len) > 63)
    {
        memcpy(&ctxData[j], data, (i = 64 - j));
        dgti_SHA1Transform(ctxState, ctxData);
        for (; i + 63 < len; i += 64)
        {
            dgti_SHA1Transform(ctxState, &data[i]);
        }
        j = 0;
    }
    else
        i = 0;
    memcpy(&ctxData[j], &data[i], len - i);
}

void DGT_Hash2GetDigest(DGTHash2Context* ctx, unsigned char* digest)
{
    unsigned char myDigest[20];

    unsigned i;

    unsigned char finalcount[8];

    unsigned char c;

    u32 * ctxState = (u32*)&ctx->h0;
    u32 * ctxCount = (u32*)&ctx->Nl;

    for (i = 0; i < 8; i++)
    {
        finalcount[i] = (unsigned char) ((ctxCount[(i >= 4 ? 0 : 1)] >> ((3 - (i & 3)) * 8)) & 255);
    }
    c = 0200;
    DGT_Hash2SetSource(ctx, &c, 1);
    while ((ctxCount[0] & 504) != 448)
    {
        c = 0000;
        DGT_Hash2SetSource(ctx, &c, 1);
    }
    DGT_Hash2SetSource(ctx, finalcount, 8);
    for (i = 0; i < 20; i++)
    {
        myDigest[i] = (unsigned char)
            ((ctxState[i >> 2] >> ((3 - (i & 3)) * 8)) & 255);
    }
    memset(ctx, '\0', sizeof(*ctx));
    memset(&finalcount, '\0', sizeof(finalcount));

    MI_CpuCopy8(myDigest, digest, 20);
}

void DGTi_Hash2ProcessMessageBlock(DGTHash2Context *ctx, u8 *data, u32 len)
{
  u32 uVar1;
  int iVar2;
  u32 uVar3;
  u32 uVar4;
  
  uVar1 = (int)len >> 0x1f;
  if ((len * 0x4000000 + uVar1 >> 0x1a | uVar1 << 6) != uVar1) {
    OS_Panic("Failed assertion num%64==0");
  }
  for (iVar2 = 0; iVar2 < (int)(len + ((u32)((int)len >> 5) >> 0x1a)) >> 6; iVar2 = iVar2 + 1) {
    uVar3 = *(u32 *)(data + 0x18);
    uVar4 = *(u32 *)(data + 0x38);
    data[0x18] = '\0';
    data[0x19] = '\0';
    data[0x1a] = '\0';
    data[0x1b] = '\0';
    data[0x38] = '\0';
    data[0x39] = '\0';
    data[0x3a] = '\0';
    data[0x3b] = '\0';
    DGTi_hash2_arm4_small(ctx,data,0x40);
    *(u32 *)(data + 0x18) = uVar3;
    *(u32 *)(data + 0x38) = uVar4;
    data = data + 0x40;
  }
  return;
}

int DGT_SetOverlayTableMode(int flag)
{
  int iVar1;
  
  iVar1 = DGTi_OverlayTableMode;
  DGTi_OverlayTableMode = flag;
  #ifdef SDK_BUILD_ARM
  if (flag == 0) {
    DGTi_Hash2ProcessMessageBlockFunc = DGTi_hash2_arm4_small;
  }
  else {
    DGTi_Hash2ProcessMessageBlockFunc = DGTi_Hash2ProcessMessageBlock;
  }
  #else
  DGTi_Hash2ProcessMessageBlockFunc = DGTi_Hash2ProcessMessageBlock;
  #endif
  return iVar1;
}
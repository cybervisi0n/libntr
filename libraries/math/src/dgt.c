#include <nitro/math/dgt.h>

void MATH_CalcMD5 (void *digest, const void *data, u32 dataLength)
{
	MATHMD5Context context;

	MATH_MD5Init(&context);
	MATH_MD5Update(&context, data, dataLength);
	MATH_MD5GetHash(&context, digest);
}

void MATH_CalcSHA1 (void *digest, const void *data, u32 dataLength)
{
	MATHSHA1Context context;

	MATH_SHA1Init(&context);
	MATH_SHA1Update(&context, data, dataLength);
	MATH_SHA1GetHash(&context, digest);
}

#ifdef SDK_PORT
void DGT_Hash1Reset(DGTHash1Context*)
{
	return;
}

void DGT_Hash1SetSource(DGTHash1Context*, const void* ptr, unsigned long lon)
{
  return;
}

void DGT_Hash1GetDigest_R(void * digest, DGTHash1Context* context)
{
  return;
}
#endif
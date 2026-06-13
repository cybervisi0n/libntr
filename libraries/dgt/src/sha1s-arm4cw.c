#include <nitro.h>
#include <nitro/dgt/dgt.h>

void DGTi_hash2_arm4_small(DGTHash2Context *ctx, u8 *data, u32 num)
{
  u32 *puVar1;
  u32 uVar2;
  int iVar3;
  u32 *puVar4;
  u32 uVar5;
  u32 uVar6;
  u32 uVar7;
  u32 uVar8;
  u32 uVar9;
  u32 uVar10;
  u32 uVar11;
  u32 uVar12;
  u32 uVar13;
  u32 local_ac [8];
  u32 local_8c [5];
  u32 local_78 [19];
  u32 local_2c;
  
  uVar2 = ctx->h0;
  uVar5 = ctx->h1;
  uVar8 = ctx->h2;
  uVar9 = ctx->h3;
  uVar11 = ctx->h4;
  local_2c = num;
  do {
    iVar3 = 0;
    puVar1 = (u32 *)data;
    puVar4 = local_ac;
    do {
      uVar12 = uVar9;
      uVar9 = uVar8;
      uVar6 = uVar2;
      data = (u8 *)(puVar1 + 1);
      uVar8 = *puVar1;
      uVar8 = uVar8 >> 0x18 | (uVar8 & 0xff00) << 8 |
              (uVar8 & 0xff00ff) >> 8 | (uVar8 & 0xff00ff) << 0x18;
      puVar4[0x10] = uVar8;
      *puVar4 = uVar8;
      uVar2 = uVar11 + 0x5a827999 + (uVar6 >> 0x1b | uVar6 << 5) + uVar8 +
              ((uVar9 ^ uVar12) & uVar5 ^ uVar12);
      uVar8 = uVar5 >> 2 | uVar5 << 0x1e;
      iVar3 = iVar3 + 4;
      puVar1 = (u32 *)data;
      puVar4 = puVar4 + 1;
      uVar5 = uVar6;
      uVar11 = uVar12;
    } while (iVar3 < 0x40);
    iVar3 = 0;
    puVar1 = local_ac;
    do {
      uVar13 = uVar9;
      uVar9 = uVar8;
      uVar7 = uVar2;
      uVar8 = *puVar1 ^ puVar1[2] ^ puVar1[8] ^ puVar1[0xd];
      uVar8 = uVar8 >> 0x1f | uVar8 << 1;
      puVar1[0x10] = uVar8;
      puVar4 = puVar1 + 1;
      *puVar1 = uVar8;
      uVar2 = uVar8 + uVar12 + 0x5a827999 + (uVar7 >> 0x1b | uVar7 << 5) +
              ((uVar9 ^ uVar13) & uVar6 ^ uVar13);
      uVar8 = uVar6 >> 2 | uVar6 << 0x1e;
      iVar3 = iVar3 + 4;
      puVar1 = puVar4;
      uVar6 = uVar7;
      uVar12 = uVar13;
    } while (iVar3 < 0x10);
    iVar3 = 0;
    do {
      uVar12 = uVar9;
      uVar9 = uVar8;
      uVar6 = uVar2;
      uVar8 = *puVar4 ^ puVar4[2] ^ puVar4[8] ^ puVar4[0xd];
      uVar8 = uVar8 >> 0x1f | uVar8 << 1;
      puVar4[0x10] = uVar8;
      *puVar4 = uVar8;
      uVar2 = uVar8 + uVar13 + 0x6ed9eba1 + (uVar6 >> 0x1b | uVar6 << 5) + (uVar7 ^ uVar9 ^ uVar12);
      uVar8 = uVar7 >> 2 | uVar7 << 0x1e;
      iVar3 = iVar3 + 1;
      puVar4 = puVar4 + 1;
      if (iVar3 == 0xc) {
        puVar4 = local_ac;
      }
      uVar7 = uVar6;
      uVar13 = uVar12;
    } while (iVar3 < 0x14);
    iVar3 = 0;
    do {
      uVar13 = uVar9;
      uVar9 = uVar8;
      uVar7 = uVar2;
      uVar8 = *puVar4 ^ puVar4[2] ^ puVar4[8] ^ puVar4[0xd];
      uVar8 = uVar8 >> 0x1f | uVar8 << 1;
      puVar4[0x10] = uVar8;
      *puVar4 = uVar8;
      uVar2 = uVar8 + uVar12 + -0x70e44324 + (uVar7 >> 0x1b | uVar7 << 5) +
              ((uVar6 | uVar9) & uVar13 | uVar6 & uVar9);
      uVar8 = uVar6 >> 2 | uVar6 << 0x1e;
      iVar3 = iVar3 + 1;
      puVar4 = puVar4 + 1;
      if (iVar3 == 8) {
        puVar4 = local_ac;
      }
      uVar6 = uVar7;
      uVar12 = uVar13;
    } while (iVar3 < 0x14);
    iVar3 = 0;
    do {
      uVar10 = uVar9;
      uVar9 = uVar8;
      uVar12 = uVar2;
      uVar8 = *puVar4 ^ puVar4[2] ^ puVar4[8] ^ puVar4[0xd];
      uVar8 = uVar8 >> 0x1f | uVar8 << 1;
      puVar4[0x10] = uVar8;
      *puVar4 = uVar8;
      uVar6 = local_2c;
      uVar2 = uVar8 + uVar13 + -0x359d3e2a + (uVar12 >> 0x1b | uVar12 << 5) +
              (uVar7 ^ uVar9 ^ uVar10);
      uVar8 = uVar7 >> 2 | uVar7 << 0x1e;
      iVar3 = iVar3 + 1;
      puVar4 = puVar4 + 1;
      if (iVar3 == 4) {
        puVar4 = local_ac;
      }
      uVar7 = uVar12;
      uVar13 = uVar10;
    } while (iVar3 < 0x14);
    uVar2 = uVar2 + ctx->h0;
    uVar5 = uVar12 + ctx->h1;
    uVar8 = uVar8 + ctx->h2;
    uVar9 = uVar9 + ctx->h3;
    uVar11 = uVar10 + ctx->h4;
    ctx->h0 = uVar2;
    ctx->h1 = uVar5;
    ctx->h2 = uVar8;
    ctx->h3 = uVar9;
    ctx->h4 = uVar11;
    local_2c = local_2c - 0x40;
  } while (local_2c != 0 && 0x3f < (int)uVar6);
  return;
}
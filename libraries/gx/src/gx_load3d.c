#include <nitro/gx/gx_load.h>
#include <nitro/gx/gx_vramcnt.h>
#ifdef SDK_PORT
#include <nitro/hw/X86/mmap_global.h>
#else
#include <nitro/hw/ARM9/mmap_global.h>
#endif
#include <nitro/mi/dma.h>

#include "../include/gxstate.h"
#include "../include/gxdma.h"

#ifdef SDK_PORT
static u64 sTexLCDCBlk1 = 0;
static u64 sSzTexBlk1 = 0;
static u64 sTexLCDCBlk2 = 0;
#else
static u32 sTexLCDCBlk1 = 0;
static u32 sSzTexBlk1 = 0;
static u32 sTexLCDCBlk2 = 0;
#endif
static GXVRamTex sTex = (GXVRamTex)(0);

#ifdef SDK_PORT
static struct
{
    u64     blk1;
    u64     blk2;
    u16     szBlk1;
}
sTexStartAddrTable[16] =
{0};

void WIN_Init_sTexStartAddrTable() {
    sTexStartAddrTable[0].blk1 = 0;
    sTexStartAddrTable[0].blk2 = 0;
    sTexStartAddrTable[0].szBlk1 = 0;

    sTexStartAddrTable[1].blk1 = (u64)(HW_LCDC_VRAM_A);
    sTexStartAddrTable[1].blk2 = 0;
    sTexStartAddrTable[1].szBlk1 = 0;

    sTexStartAddrTable[2].blk1 = (u64)(HW_LCDC_VRAM_B);
    sTexStartAddrTable[2].blk2 = 0;
    sTexStartAddrTable[2].szBlk1 = 0;

    sTexStartAddrTable[3].blk1 = (u64)(HW_LCDC_VRAM_A);
    sTexStartAddrTable[3].blk2 = 0;
    sTexStartAddrTable[3].szBlk1 = 0;    

    sTexStartAddrTable[4].blk1 = (u64)(HW_LCDC_VRAM_C);
    sTexStartAddrTable[4].blk2 = 0;
    sTexStartAddrTable[4].szBlk1 = 0;

    sTexStartAddrTable[5].blk1 = (u64)(HW_LCDC_VRAM_A);
    sTexStartAddrTable[5].blk2 = (u64)(HW_LCDC_VRAM_C);
    sTexStartAddrTable[5].szBlk1 = (u16)(HW_VRAM_A_SIZE >> 12);

    sTexStartAddrTable[6].blk1 = (u64)(HW_LCDC_VRAM_B);
    sTexStartAddrTable[6].blk2 = 0;
    sTexStartAddrTable[6].szBlk1 = 0;

    sTexStartAddrTable[7].blk1 = (u64)(HW_LCDC_VRAM_A);
    sTexStartAddrTable[7].blk2 = 0;
    sTexStartAddrTable[7].szBlk1 = 0;

    sTexStartAddrTable[8].blk1 = (u64)(HW_LCDC_VRAM_D);
    sTexStartAddrTable[8].blk2 = 0;
    sTexStartAddrTable[8].szBlk1 = 0;

    sTexStartAddrTable[9].blk1 = (u64)(HW_LCDC_VRAM_A);
    sTexStartAddrTable[9].blk2 = (u64)(HW_LCDC_VRAM_D);
    sTexStartAddrTable[9].szBlk1 = (u16)(HW_VRAM_A_SIZE >> 12);

    sTexStartAddrTable[10].blk1 = (u64)(HW_LCDC_VRAM_B);
    sTexStartAddrTable[10].blk2 = (u64)(HW_LCDC_VRAM_D);
    sTexStartAddrTable[10].szBlk1 = (u16)(HW_VRAM_B_SIZE >> 12);

    sTexStartAddrTable[11].blk1 = (u64)(HW_LCDC_VRAM_A);
    sTexStartAddrTable[11].blk2 = (u64)(HW_LCDC_VRAM_D);
    sTexStartAddrTable[11].szBlk1 = (u16)((HW_VRAM_A_SIZE + HW_VRAM_B_SIZE) >> 12);

    sTexStartAddrTable[12].blk1 = (u64)(HW_LCDC_VRAM_C);
    sTexStartAddrTable[12].blk2 = 0;
    sTexStartAddrTable[12].szBlk1 = 0;

    sTexStartAddrTable[13].blk1 = (u64)(HW_LCDC_VRAM_A);
    sTexStartAddrTable[13].blk2 = (u64)(HW_LCDC_VRAM_C);
    sTexStartAddrTable[13].szBlk1 = (u16)(HW_VRAM_A_SIZE >> 12);

    sTexStartAddrTable[14].blk1 = (u64)(HW_LCDC_VRAM_B);
    sTexStartAddrTable[14].blk2 = 0;
    sTexStartAddrTable[14].szBlk1 = 0;

    sTexStartAddrTable[15].blk1 = (u64)(HW_LCDC_VRAM_A);
    sTexStartAddrTable[15].blk2 = 0;
    sTexStartAddrTable[15].szBlk1 = 0;
    return;
}
#else

static const struct {
	u16 blk1;
	u16 blk2;
	u16 szBlk1;
}

sTexStartAddrTable[16] = {
	{
		0, 0, 0
	},
	{
		(u16)(HW_LCDC_VRAM_A >> 12), 0, 0
	},
	{
		(u16)(HW_LCDC_VRAM_B >> 12), 0, 0
	},
	{
		(u16)(HW_LCDC_VRAM_A >> 12), 0, 0
	},
	{
		(u16)(HW_LCDC_VRAM_C >> 12), 0, 0
	},
	{
		(u16)(HW_LCDC_VRAM_A >> 12), (u16)(HW_LCDC_VRAM_C >> 12), (u16)(HW_VRAM_A_SIZE >> 12)
	},
	{
		(u16)(HW_LCDC_VRAM_B >> 12), 0, 0
	},
	{
		(u16)(HW_LCDC_VRAM_A >> 12), 0, 0
	},
	{
		(u16)(HW_LCDC_VRAM_D >> 12), 0, 0
	},
	{
		(u16)(HW_LCDC_VRAM_A >> 12), (u16)(HW_LCDC_VRAM_D >> 12), (u16)(HW_VRAM_A_SIZE >> 12)
	},
	{
		(u16)(HW_LCDC_VRAM_B >> 12), (u16)(HW_LCDC_VRAM_D >> 12), (u16)(HW_VRAM_B_SIZE >> 12)
	},
	{
		(u16)(HW_LCDC_VRAM_A >> 12),
		(u16)(HW_LCDC_VRAM_D >> 12), (u16)((HW_VRAM_A_SIZE + HW_VRAM_B_SIZE) >> 12)
	},
	{
		(u16)(HW_LCDC_VRAM_C >> 12), 0, 0
	},
	{
		(u16)(HW_LCDC_VRAM_A >> 12), (u16)(HW_LCDC_VRAM_C >> 12), (u16)(HW_VRAM_A_SIZE >> 12)
	},
	{
		(u16)(HW_LCDC_VRAM_B >> 12), 0, 0
	},
	{
		(u16)(HW_LCDC_VRAM_A >> 12), 0, 0
	},
};
#endif

void GX_LoadTexEx (GXVRamTex tex, const void *pSrc, u32 destSlotAddr, u32 szByte)
{
	u32 base1, base2, szBlk1;
	void *pLCDC;

	SDK_ALIGN4_ASSERT(szByte);
	SDK_ALIGN4_ASSERT(destSlotAddr);
	SDK_ALIGN4_ASSERT(pSrc);
	SDK_ASSERTMSG((GX_GetBankForLCDC() & tex) == tex,
	              "Banks specified by tex must be on LCDC space.");

	GX_VRAM_TEX_ASSERT(tex);

	base1 = (u32)(sTexStartAddrTable[tex].blk1 << 12);
	base2 = (u32)(sTexStartAddrTable[tex].blk2 << 12);
	szBlk1 = (u32)(sTexStartAddrTable[tex].szBlk1 << 12);

	SDK_ASSERT(0 != base1);

	GX_RegionCheck_Tex(tex, destSlotAddr, destSlotAddr + szByte);

	if (0 == base2) {
		pLCDC = (void *)(base1 + destSlotAddr);
	} else {
		if (destSlotAddr + szByte < szBlk1) {
			pLCDC = (void *)(base1 + destSlotAddr);
		} else if (destSlotAddr >= szBlk1)   {
			pLCDC = (void *)(base2 + destSlotAddr - szBlk1);
		} else {
			void *pLCDC2 = (void *)base2;
			u32 sz = szBlk1 - destSlotAddr;
			pLCDC = (void *)(base1 + destSlotAddr);

			GXi_DmaCopy32(GXi_DmaId, pSrc, pLCDC, sz);
			GXi_DmaCopy32Async(GXi_DmaId, (void *)((u8 *)pSrc + sz), pLCDC2, szByte - sz, NULL, NULL);
			return;
		}
	}

	GXi_DmaCopy32Async(GXi_DmaId, pSrc, pLCDC, szByte, NULL, NULL);
}

void GX_BeginLoadTex (void)
{
	SDK_ASSERT(0 == sTex && 0 == sTexLCDCBlk1 && 0 == sSzTexBlk1 && 0 == sTexLCDCBlk2);

	sTex = GX_ResetBankForTex();

	GX_VRAM_TEX_ASSERT(sTex);

	#ifdef SDK_PORT
    sTexLCDCBlk1 = (u64)(sTexStartAddrTable[sTex].blk1);
    sTexLCDCBlk2 = (u64)(sTexStartAddrTable[sTex].blk2);
	#else
	sTexLCDCBlk1 = (u32)(sTexStartAddrTable[sTex].blk1 << 12);
	sTexLCDCBlk2 = (u32)(sTexStartAddrTable[sTex].blk2 << 12);
	#endif
	sSzTexBlk1 = (u32)(sTexStartAddrTable[sTex].szBlk1 << 12);
}

void GX_LoadTex (const void *pSrc, u32 destSlotAddr, u32 szByte)
{
	void *pLCDC;
	SDK_NULL_ASSERT(pSrc);

	SDK_ASSERT(GX_VRAM_TEX_NONE != sTex);
	SDK_ASSERT(0 != sTexLCDCBlk1);
	SDK_ALIGN4_ASSERT(szByte);
	SDK_ALIGN4_ASSERT(destSlotAddr);
	SDK_ALIGN4_ASSERT(pSrc);

	GX_RegionCheck_Tex(sTex, destSlotAddr, destSlotAddr + szByte);

	if (0 == sTexLCDCBlk2) {
		pLCDC = (void *)(sTexLCDCBlk1 + destSlotAddr);
	} else {
		if (destSlotAddr + szByte < sSzTexBlk1) {
			pLCDC = (void *)(sTexLCDCBlk1 + destSlotAddr);
		} else if (destSlotAddr >= sSzTexBlk1)   {
			pLCDC = (void *)(sTexLCDCBlk2 + destSlotAddr - sSzTexBlk1);
		} else {
			void *pLCDC2 = (void *)sTexLCDCBlk2;
			u32 sz = sSzTexBlk1 - destSlotAddr;
			pLCDC = (void *)(sTexLCDCBlk1 + destSlotAddr);

			GXi_DmaCopy32(GXi_DmaId, pSrc, pLCDC, sz);
			GXi_DmaCopy32Async(GXi_DmaId, (void *)((u8 *)pSrc + sz), pLCDC2, szByte - sz, NULL, NULL);
			return;
		}
	}

	GXi_DmaCopy32Async(GXi_DmaId, pSrc, pLCDC, szByte, NULL, NULL);
}

void GX_EndLoadTex (void)
{
	GXi_WaitDma(GXi_DmaId);
	GX_SetBankForTex(sTex);

	sTexLCDCBlk1 = sTexLCDCBlk2 = sSzTexBlk1 = 0;
	sTex = (GXVRamTex)0;
}

static GXVRamTexPltt sTexPltt = (GXVRamTexPltt)(0);
#ifdef SDK_PORT
static u64 sTexPlttLCDCBlk = 0;
#else
static u32 sTexPlttLCDCBlk = 0;
#endif

#ifdef SDK_PORT
static u64 sTexPlttStartAddrTable[8] = {0};

void WIN_Init_sTexPlttStartAddrTable() {
    sTexPlttStartAddrTable[0] = 0,
    sTexPlttStartAddrTable[1] = (u64)(HW_LCDC_VRAM_E);
    sTexPlttStartAddrTable[2] = (u64)(HW_LCDC_VRAM_F);
    sTexPlttStartAddrTable[3] = (u64)(HW_LCDC_VRAM_E);
    sTexPlttStartAddrTable[4] = (u64)(HW_LCDC_VRAM_G);
    sTexPlttStartAddrTable[5] = 0;
    sTexPlttStartAddrTable[6] = (u64)(HW_LCDC_VRAM_F);
    sTexPlttStartAddrTable[7] = (u64)(HW_LCDC_VRAM_E);
}
#else
static const u16 sTexPlttStartAddrTable[8] = {
	0,
	(u16)(HW_LCDC_VRAM_E >> 12),
	(u16)(HW_LCDC_VRAM_F >> 12),
	(u16)(HW_LCDC_VRAM_E >> 12),
	(u16)(HW_LCDC_VRAM_G >> 12),
	0,
	(u16)(HW_LCDC_VRAM_F >> 12),
	(u16)(HW_LCDC_VRAM_E >> 12)
};
#endif

void GX_LoadTexPlttEx (GXVRamTexPltt texPltt, const void *pSrc, u32 destSlotAddr, u32 szByte)
{
	#ifdef SDK_PORT
	u64 base;
	#else
	u32 base;
	#endif
	SDK_ASSERTMSG((GX_GetBankForLCDC() & texPltt) == texPltt,
	              "Banks specified by texPltt must be on LCDC space.");

	GX_VRAM_TEXPLTT_ASSERT(texPltt);
	#ifdef SDK_PORT
	base = (u64)(sTexPlttStartAddrTable[texPltt >> 4] << 12);
	#else
	base = (u32)(sTexPlttStartAddrTable[texPltt >> 4] << 12);
	#endif

	SDK_NULL_ASSERT(pSrc);
	SDK_ASSERT(0 != base);
	SDK_ALIGN4_ASSERT(destSlotAddr);
	SDK_ALIGN4_ASSERT(szByte);
	SDK_ALIGN4_ASSERT(pSrc);

	GX_RegionCheck_TexPltt(texPltt, destSlotAddr, destSlotAddr + szByte);
	GXi_DmaCopy32Async(GXi_DmaId, pSrc, (void *)(base + destSlotAddr), szByte, NULL, NULL);
}

void GX_BeginLoadTexPltt (void)
{
	SDK_ASSERT(0 == sTexPltt && 0 == sTexPlttLCDCBlk);

	sTexPltt = GX_ResetBankForTexPltt();

	GX_VRAM_TEXPLTT_ASSERT(sTexPltt);
	#ifdef SDK_PORT
	sTexPlttLCDCBlk = (u64)(sTexPlttStartAddrTable[sTexPltt >> 4]);
	#else
	sTexPlttLCDCBlk = (u32)(sTexPlttStartAddrTable[sTexPltt >> 4] << 12);
	#endif
}

void GX_LoadTexPltt (const void *pSrc, u32 destSlotAddr, u32 szByte)
{
	SDK_NULL_ASSERT(pSrc);
	SDK_ASSERT(GX_VRAM_TEXPLTT_NONE != sTexPltt);
	SDK_ASSERT(0 != sTexPlttLCDCBlk);
	SDK_ALIGN4_ASSERT(destSlotAddr);
	SDK_ALIGN4_ASSERT(szByte);
	SDK_ALIGN4_ASSERT(pSrc);

	GX_RegionCheck_TexPltt(sTexPltt, destSlotAddr, destSlotAddr + szByte);
	GXi_DmaCopy32Async(GXi_DmaId, pSrc, (void *)(sTexPlttLCDCBlk + destSlotAddr), szByte, NULL, NULL);
}

void GX_EndLoadTexPltt (void)
{
	GXi_WaitDma(GXi_DmaId);
	GX_SetBankForTexPltt(sTexPltt);

	sTexPltt = (GXVRamTexPltt)0;
	sTexPlttLCDCBlk = 0;
}

static GXVRamClearImage sClrImg = (GXVRamClearImage)(0);
static u32 sClrImgLCDCBlk = 0;

void GX_BeginLoadClearImage (void)
{
	SDK_ASSERT(0 == sClrImg && 0 == sClrImgLCDCBlk);

	sClrImg = GX_ResetBankForClearImage();

	switch (sClrImg) {
	case GX_VRAM_CLEARIMAGE_256_AB:
	case GX_VRAM_CLEARDEPTH_128_B:
		sClrImgLCDCBlk = HW_LCDC_VRAM_A;
		break;
	case GX_VRAM_CLEARIMAGE_256_CD:
	case GX_VRAM_CLEARDEPTH_128_D:
		sClrImgLCDCBlk = HW_LCDC_VRAM_C;
		break;
	case GX_VRAM_CLEARDEPTH_128_A:
		sClrImgLCDCBlk = HW_LCDC_VRAM_A - 0x20000;
		break;
	case GX_VRAM_CLEARDEPTH_128_C:
		sClrImgLCDCBlk = HW_LCDC_VRAM_C - 0x20000;
		break;
	case GX_VRAM_CLEARIMAGE_NONE:
		break;
	default:
		SDK_INTERNAL_ERROR("unknown case 0x%x", sClrImg);
		break;
	}
}

void GX_LoadClearImageColor (const void *pSrc, u32 szByte)
{
	SDK_NULL_ASSERT(pSrc);
	SDK_ASSERT(GX_VRAM_CLEARIMAGE_256_AB == sClrImg || GX_VRAM_CLEARIMAGE_256_CD == sClrImg);
	SDK_ASSERT(0 != sClrImgLCDCBlk);
	SDK_ALIGN4_ASSERT(szByte);
	SDK_ALIGN4_ASSERT(pSrc);
	SDK_ASSERT(szByte <= 0x20000);

	GXi_DmaCopy32Async(GXi_DmaId, pSrc, (void *)(sClrImgLCDCBlk), szByte, NULL, NULL);
}

void GX_LoadClearImageDepth (const void *pSrc, u32 szByte)
{
	SDK_NULL_ASSERT(pSrc);
	SDK_ASSERT(GX_VRAM_CLEARIMAGE_NONE != sClrImg);
	SDK_ASSERT(0 != sClrImgLCDCBlk);
	SDK_ALIGN4_ASSERT(szByte);
	SDK_ALIGN4_ASSERT(pSrc);
	SDK_ASSERT(szByte <= 0x20000);

	GXi_DmaCopy32Async(GXi_DmaId,
	                   pSrc,
	                   (void *)(sClrImgLCDCBlk + 0x20000),
	                   szByte, NULL, NULL);
}

void GX_EndLoadClearImage (void)
{
	GXi_WaitDma(GXi_DmaId);
	GX_SetBankForClearImage(sClrImg);

	sClrImg = (GXVRamClearImage)0;
	sClrImgLCDCBlk = 0;
}
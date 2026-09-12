#if SDK_VERSION_MAJOR == 4
#include <nitro.h>
#elif SDK_VERSION_MAJOR == 5
#include <nitro/card/rom.h>
#include <nitro/card/pullOut.h>
#include <nitro/card/rom.h>
#endif

#include "../include/card_common.h"
#include "../include/card_rom.h"

#if SDK_VERSION_MAJOR == 5
#if defined(SDK_ARM9) && defined(SDK_TWL)
#define CARD_USING_HASHCHECK
#endif // defined(SDK_ARM9) && defined(SDK_TWL)

#if defined(SDK_ARM9) ||                                                       \
    (defined(SDK_ARM7) && defined(SDK_ARM7_READROM_SUPPORT))
#define CARD_USING_ROMREADER
#endif // defined(SDK_ARM9) || (defined(SDK_ARM7) &&
       // defined(SDK_ARM7_READROM_SUPPORT))

#define CARD_COMMAND_PAGE 0x01000000
#define CARD_COMMAND_ID 0x07000000
#define CARD_COMMAND_REFRESH 0x00000000
#define CARD_COMMAND_STAT CARD_COMMAND_ID
#define CARD_COMMAND_MASK 0x07000000
#define CARD_RESET_HI 0x20000000
#define CARD_COMMAND_OP_G_READID 0xB8
#define CARD_COMMAND_OP_G_READPAGE 0xB7
#define CARD_COMMAND_OP_G_READSTAT 0xD6
#define CARD_COMMAND_OP_G_REFRESH 0xB5
#ifdef SDK_TWL
#define CARD_COMMAND_OP_N_READID 0x90
#define CARD_COMMAND_OP_N_READPAGE 0x00
#define CARD_COMMAND_OP_N_READSTAT CARD_COMMAND_OP_G_READSTAT
#define CARD_COMMAND_OP_N_REFRESH CARD_COMMAND_OP_G_REFRESH
#endif

#define CARD_ROMID_1TROM_MASK 0x80000000UL   // 1T-ROM type
#define CARD_ROMID_TWLROM_MASK 0x40000000UL  // TWL-ROM
#define CARD_ROMID_REFRESH_MASK 0x20000000UL // Refresh support

#define CARD_ROMST_RFS_WARN_L1_MASK 0x00000004UL
#define CARD_ROMST_RFS_WARN_L2_MASK 0x00000008UL
#define CARD_ROMST_RFS_READY_MASK 0x00000020UL
#endif

#if SDK_VERSION_MAJOR == 4
CARDRomStat rom_stat ATTRIBUTE_ALIGN(32);
extern u32 cardi_rom_base;
#endif

u32 cardi_rom_base;
#ifndef SDK_PORT
u32 cardi_rom_header_addr = HW_ROM_HEADER_BUF;
#endif

#if SDK_VERSION_MAJOR == 5
static int (*CARDiReadRomFunction)(void *userdata, void *buffer, u32 offset,
                                   u32 length);

static CARDTransferInfo CARDiDmaReadInfo[1];
static CARDTransferInfo *CARDiDmaReadRegisteredInfo;

static u32 cache_page = 1;
static u8 CARDi_cache_buf[CARD_ROM_PAGE_SIZE] ATTRIBUTE_ALIGN(32);
static BOOL CARDiEnableCacheInvalidationIC = FALSE;
static BOOL CARDiEnableCacheInvalidationDC = TRUE;

extern BOOL OSi_IsThreadInitialized;

static u8
    CARDiOwnSignature[CARD_ROM_DOWNLOAD_SIGNATURE_SIZE] ATTRIBUTE_ALIGN(4);

void CARD_RefreshRom(void);
#endif

#if SDK_VERSION_MAJOR == 4
SDK_COMPILER_ASSERT(sizeof(rom_stat) % 32 == 0);

static inline BOOL CARDi_OnReadPageDirect (CARDRomStat *arg)
{
	CARDiCommon *p = &cardi_common;
	(void)arg;

	p->src += CARD_ROM_PAGE_SIZE;
	p->dst += CARD_ROM_PAGE_SIZE;
	p->len -= CARD_ROM_PAGE_SIZE;

	return (p->len > 0);
}

static BOOL CARDi_ReadFromCache (CARDRomStat *p)
{
	CARDiCommon *c = &cardi_common;
	const u32 cur_page = CARD_ALIGN_HI_BIT(c->src);

	if (cur_page == (u32)p->cache_page) {
		const u32 mod = c->src - cur_page;
		u32 len = CARD_ROM_PAGE_SIZE - mod;

		if (len > c->len)
			len = c->len;

		MI_CpuCopy8(p->cache_buf + mod, (void *)c->dst, len);

		c->src += len;
		c->dst += len;
		c->len -= len;
	}

	return (c->len > 0);
}
#endif

#if defined(SDK_TS) || defined(SDK_ARM7)
    static void CARDi_SetRomOp (u32 cmd1, u32 cmd2)
    {
        #if SDK_VERSION_MAJOR == 4
        while ((*(vu32 *)REG_CARDCNT & CARD_START) != 0);

        *(vu8 *)REG_CARD_MASTER_CNT = CARDMST_SEL_ROM | CARDMST_ENABLE | CARDMST_IF_ENABLE;
        {
            vu8 *const p_cmd = (vu8 *)REG_CARD_CMD;
            p_cmd[0] = (u8)(cmd1 >> (8 * 3));
            p_cmd[1] = (u8)(cmd1 >> (8 * 2));
            p_cmd[2] = (u8)(cmd1 >> (8 * 1));
            p_cmd[3] = (u8)(cmd1 >> (8 * 0));
            p_cmd[4] = (u8)(cmd2 >> (8 * 3));
            p_cmd[5] = (u8)(cmd2 >> (8 * 2));
            p_cmd[6] = (u8)(cmd2 >> (8 * 1));
            p_cmd[7] = (u8)(cmd2 >> (8 * 0));
        }
        #elif SDK_VERSION_MAJOR == 5
        u32 cmd1 = (u32)((offset >> 8) | (command << 24));
        u32 cmd2 = (u32)((offset << 24));
            
        while ((reg_MI_MCCNT1 & REG_MI_MCCNT1_START_MASK) != 0) {
        }
    
        reg_MI_MCCNT0 = (u16)(REG_MI_MCCNT0_E_MASK | REG_MI_MCCNT0_I_MASK |
                              (reg_MI_MCCNT0 & ~REG_MI_MCCNT0_SEL_MASK));
        
        reg_MI_MCCMD0 = MI_HToBE32(cmd1);
        reg_MI_MCCMD1 = MI_HToBE32(cmd2);
        #endif
    }

    #if SDK_VERSION_MAJOR == 4
    static inline void CARDi_SetRomOpReadPage1 (u32 src)
    {
        CARDi_SetRomOp((u32)(MROMOP_G_READ_PAGE | (src >> 8)), (u32)(src << 24));
    }

    static void CARDi_SetCardDma (void)
    {
        CARDiCommon *const c = &cardi_common;
        CARDRomStat *const p = &rom_stat;

        #ifdef SDK_BUILD_ARM
        MIi_CardDmaCopy32(c->dma, (const void *)REG_CARD_DATA, (void *)c->dst, CARD_ROM_PAGE_SIZE);
        #endif

        CARDi_SetRomOpReadPage1(c->src);
        *(vu32 *)REG_CARDCNT = p->ctrl;
    }

    static void CARDi_OnReadCard (void)
    {
        CARDRomStat *const p = &rom_stat;
        CARDiCommon *const c = &cardi_common;

        MI_StopDma(c->dma);
        if (!CARDi_OnReadPageDirect(p)) {
            (void)OS_DisableIrqMask(OS_IE_CARD_DATA);
            (void)OS_ResetRequestIrqMask(OS_IE_CARD_DATA);
            CARDi_ReadEnd();
        } else {
            CARDi_SetCardDma();
        }
    }

    BOOL CARDi_TryReadCardDma (CARDRomStat *p)
    #if defined(SDK_TS)
    {
        CARDiCommon *const c = &cardi_common;
        const u32 dst = c->dst;
        u32 len = c->len;

        const BOOL is_async = !(dst & 31) &&
                            (c->dma <= MI_DMA_MAX_NUM) &&
                            !CARDi_IsInTcm(dst, len) &&
                            !CARD_ALIGN_LO_BIT(c->src | len) &&
                            (len > 0);
        p->ctrl = CARDi_GetRomFlag(CARD_COMMAND_PAGE);
        if (is_async) {
            OSIntrMode bak_psr = OS_DisableInterrupts();
    #if (defined(SDK_ARM9) || defined(SDK_PORT))
            if (len < c->flush_threshold_ic) {
                IC_InvalidateRange((void *)dst, len);
            } else {
                IC_InvalidateAll();
            }
            if (len < c->flush_threshold_dc) {
                u32 pos = dst;
                u32 mod = (dst & (HW_CACHE_LINE_SIZE - 1));
                if (mod) {
                    pos -= mod;
                    DC_StoreRange((void *)(pos), HW_CACHE_LINE_SIZE);
                    DC_StoreRange((void *)(pos + len), HW_CACHE_LINE_SIZE);
                    len += HW_CACHE_LINE_SIZE;
                }
                DC_InvalidateRange((void *)pos, len);
                DC_WaitWriteBufferEmpty();
            } else {
                DC_FlushAll();
            }
    #endif
            (void)OS_SetIrqFunction(OS_IE_CARD_DATA, CARDi_OnReadCard);
            (void)OS_ResetRequestIrqMask(OS_IE_CARD_DATA);
            (void)OS_EnableIrqMask(OS_IE_CARD_DATA);
            (void)OS_RestoreInterrupts(bak_psr);
            CARDi_SetCardDma();
        }
        return is_async;
    }
    #else
    {
        (void)p;
        return FALSE;
    }
    #endif

    void CARDi_ReadCard (CARDRomStat *p)
    {
    #if defined(SDK_ARM9) || defined(SDK_PORT)\
        || (defined(SDK_ARM7) && defined(SDK_ARM7_READROM_SUPPORT))

        CARDiCommon *const c = &cardi_common;
        for (;;) {
            const u32 len = CARD_ROM_PAGE_SIZE;
            u32 src = CARD_ALIGN_HI_BIT(c->src);
            u32 dst;

            if ((src != c->src) || ((c->dst & 3) != 0) || (c->len < len)) {
                dst = (u32)p->cache_buf;
                p->cache_page = (void *)src;
            } else {
                dst = c->dst;
            }

            CARDi_SetRomOpReadPage1(src);
            {
                u32 pos = 0;
                *(vu32 *)REG_CARDCNT = p->ctrl;
                for (;;) {
                    const u32 ctrl = *(vu32 *)REG_CARDCNT;
                    if ((ctrl & CARD_DATA_READY) != 0) {

                        u32 data = *(vu32 *)REG_CARD_DATA;
                        if (pos * sizeof(u32) < len)
                            ((u32 *)dst)[pos++] = data;
                    }
                    if (!(ctrl & CARD_START))
                        break;
                }
            }

            if (dst == c->dst) {
                if (!CARDi_OnReadPageDirect(p))
                    break;
            } else {
                if (!CARDi_ReadFromCache(p))
                    break;
            }
        }
    #else
    #pragma unused(p)
    #endif
    }
    #endif /* SDK_VERSION_MAJOR */

#if SDK_VERSION_MAJOR == 5
SDK_INLINE u32 CARDi_GetRomFlag(u32 flag) {
  u32 rom_ctrl = *(vu32 *)(HW_CARD_ROM_HEADER + 0x60);
  return (u32)(flag | REG_MI_MCCNT1_START_MASK | CARD_RESET_HI |
               (rom_ctrl & ~CARD_COMMAND_MASK));
}

BOOL CARDi_IsTwlRom(void) {
  u32 iplCardID = *(u32 *)(HW_BOOT_CHECK_INFO_BUF);

  if (!iplCardID) {
    return FALSE;
  }

  return (CARDi_ReadRomID() & CARD_ROMID_TWLROM_MASK) ? TRUE : FALSE;
}

#ifdef SDK_TWL

static BOOL CARDi_IsNormalMode(void) {
  const CARDRomHeaderTWL *oh = CARD_GetOwnRomHeaderTWL();

  return OS_IsRunOnTwl() && (OS_GetBootType() != OS_BOOTTYPE_ROM) &&
         oh->access_control.game_card_on &&
         !oh->access_control.game_card_nitro_mode;
}

#endif

static void CARDi_StartRomPageTransfer(u32 offset) {
  u8 op = CARD_COMMAND_OP_G_READPAGE;
#ifdef SDK_TWL
  if (CARDi_IsNormalMode()) {
    op = CARD_COMMAND_OP_N_READPAGE;
  }
#endif

  CARDi_SetRomOp(op, offset);
  reg_MI_MCCNT1 = CARDi_GetRomFlag(CARD_COMMAND_PAGE);
}
#endif /* SDK_VERSION_MAJOR */

    u32 CARDi_ReadRomIDCore (void)
    {
        #if SDK_VERSION_MAJOR == 4
        CARDi_SetRomOp(MROMOP_G_READ_ID, 0);
        *(vu32 *)REG_CARDCNT = (u32)(CARDi_GetRomFlag(CARD_COMMAND_ID) & ~CARD_LATENCY1_MASK);
        while (!(*(vu32 *)REG_CARDCNT & CARD_DATA_READY)) {
        }
        return *(vu32 *)REG_CARD_DATA;
        #elif SDK_VERSION_MAJOR == 5
          u8 op = CARD_COMMAND_OP_G_READID;
        #ifdef SDK_TWL
          if (CARDi_IsNormalMode()) {
            op = CARD_COMMAND_OP_N_READID;
          }
        #endif
      
          CARDi_SetRomOp(op, 0);
          reg_MI_MCCNT1 =
              (u32)(CARDi_GetRomFlag(CARD_COMMAND_ID) & ~REG_MI_MCCNT1_L1_MASK);
          while ((reg_MI_MCCNT1 & REG_MI_MCCNT1_RDY_MASK) == 0) {
          }
          return reg_MI_MCD1;
        #endif
    }
#endif

#if SDK_VERSION_MAJOR == 5
u32 CARDi_ReadRomStatusCore(void) {
  u32 iplCardID = *(u32 *)(HW_BOOT_CHECK_INFO_BUF);

  if (!(iplCardID & CARD_ROMID_REFRESH_MASK)) {
    return CARD_ROMST_RFS_READY_MASK;
  }

  CARDi_SetRomOp(CARD_COMMAND_OP_G_READSTAT, 0);
  reg_MI_MCCNT1 =
      (u32)(CARDi_GetRomFlag(CARD_COMMAND_STAT) & ~REG_MI_MCCNT1_L1_MASK);
  while ((reg_MI_MCCNT1 & REG_MI_MCCNT1_RDY_MASK) == 0) {
  }
  return reg_MI_MCD1;
}

void CARD_RefreshRom(void) {
  SDK_ASSERT(CARD_IsAvailable());
  SDK_TASSERTMSG(CARDi_GetTargetMode() == CARD_TARGET_ROM,
                 "must be locked by CARD_LockRom()");

#if defined(SDK_ARM9)
  (void)CARDi_WaitForTask(&cardi_common, TRUE, NULL, NULL);

  CARDi_CheckPulledOutCore(CARDi_ReadRomIDCore());
#endif // defined(SDK_ARM9)

  CARDi_RefreshRom(CARD_ROMST_RFS_WARN_L1_MASK | CARD_ROMST_RFS_WARN_L2_MASK);

#if defined(SDK_ARM9)
  cardi_common.cmd->result = CARD_RESULT_SUCCESS;
  CARDi_EndTask(&cardi_common);
#endif // defined(SDK_ARM9)
}

void CARDi_RefreshRom(u32 warn_mask) {
  if (CARDi_ReadRomStatusCore() & warn_mask) {
    CARDi_RefreshRomCore();

    while (!(CARDi_ReadRomStatusCore() & CARD_ROMST_RFS_READY_MASK)) {

      if (OSi_IsThreadInitialized && OS_IsAlarmAvailable()) {
        OS_Sleep(1);
      }
    }
  }
}

void CARDi_RefreshRomCore(void) {
  CARDi_SetRomOp(CARD_COMMAND_OP_G_REFRESH, 0);
  reg_MI_MCCNT1 =
      (u32)(CARDi_GetRomFlag(CARD_COMMAND_REFRESH) & ~REG_MI_MCCNT1_L1_MASK);
  while (reg_MI_MCCNT1 & REG_MI_MCCNT1_START_MASK) {
  }
}

int CARDi_ReadRomWithCPU(void *userdata, void *buffer, u32 offset, u32 length) {
  int retval = (int)length;

  u32 cachedPage = cache_page;
  u8 *const cacheBuffer = CARDi_cache_buf;
  while (length > 0) {

    u8 *ptr = (u8 *)buffer;
    u32 n = CARD_ROM_PAGE_SIZE;
    u32 pos = MATH_ROUNDDOWN(offset, CARD_ROM_PAGE_SIZE);

    if (pos == cachedPage) {
      ptr = cacheBuffer;
    } else {

      if (((pos != offset) || (((u32)buffer & 3) != 0) || (length < n))) {
        cachedPage = pos;
        ptr = cacheBuffer;
      }

      CARDi_StartRomPageTransfer(pos);
      {
        u32 word = 0;
        for (;;) {

          u32 ctrl = reg_MI_MCCNT1;
          if ((ctrl & REG_MI_MCCNT1_RDY_MASK) != 0) {

            u32 data = reg_MI_MCD1;
            if (word < (CARD_ROM_PAGE_SIZE / sizeof(u32))) {
              ((u32 *)ptr)[word++] = data;
            }
          }

          if ((ctrl & REG_MI_MCCNT1_START_MASK) == 0) {
            break;
          }
        }
      }
    }

    if (ptr == cacheBuffer) {
      u32 mod = offset - pos;
      n = MATH_MIN(length, CARD_ROM_PAGE_SIZE - mod);
      MI_CpuCopy8(cacheBuffer + mod, buffer, n);
    }
    buffer = (u8 *)buffer + n;
    offset += n;
    length -= n;
  }

  CARDi_CheckPulledOutCore(CARDi_ReadRomIDCore());

  cache_page = cachedPage;
  (void)userdata;
  return retval;
}

static void CARDi_DmaReadPageCallback(void) {
  CARDTransferInfo *info = CARDiDmaReadRegisteredInfo;
  if (info) {
    info->src += CARD_ROM_PAGE_SIZE;
    info->dst += CARD_ROM_PAGE_SIZE;
    info->len -= CARD_ROM_PAGE_SIZE;

    if (info->len > 0) {
      CARDi_StartRomPageTransfer(info->src);
    }

    else {
      cardi_common.DmaCall->Stop(cardi_common.dma);
      (void)OS_DisableIrqMask(OS_IE_CARD_DATA);
      (void)OS_ResetRequestIrqMask(OS_IE_CARD_DATA);
      CARDiDmaReadRegisteredInfo = NULL;

      CARDi_CheckPulledOutCore(CARDi_ReadRomIDCore());
      if (info->callback) {
        (*info->callback)(info->userdata);
      }
    }
  }
}

void CARDi_ReadRomWithDMA(CARDTransferInfo *info) {
  OSIntrMode bak_psr = OS_DisableInterrupts();
  CARDiDmaReadRegisteredInfo = info;

  (void)OS_SetIrqFunction(OS_IE_CARD_DATA, CARDi_DmaReadPageCallback);
  (void)OS_ResetRequestIrqMask(OS_IE_CARD_DATA);
  (void)OS_EnableIrqMask(OS_IE_CARD_DATA);
  (void)OS_RestoreInterrupts(bak_psr);

  cardi_common.DmaCall->Recv(cardi_common.dma, (void *)&reg_MI_MCD1,
                             (void *)info->dst, CARD_ROM_PAGE_SIZE);

  CARDi_StartRomPageTransfer(info->src);
}

static void CARDi_DmaReadDone(void *userdata) {
  (void)userdata;
#ifdef SDK_ARM9

  CARDi_CheckPulledOutCore(CARDi_ReadRomIDCore());
#endif

  CARDi_RefreshRom(CARD_ROMST_RFS_WARN_L2_MASK);

  cardi_common.cmd->result = CARD_RESULT_SUCCESS;
  CARDi_EndTask(&cardi_common);
}

static BOOL CARDi_IsRomDmaAvailable(u32 dma, void *dst, u32 src, u32 len) {
  return (dma <= MI_DMA_MAX_NUM) && (len > 0) && (((u32)dst & 31) == 0) &&
#ifdef SDK_ARM9
         (((u32)dst + len <= OS_GetITCMAddress()) ||
          ((u32)dst >= OS_GetITCMAddress() + HW_ITCM_SIZE)) &&
         (((u32)dst + len <= OS_GetDTCMAddress()) ||
          ((u32)dst >= OS_GetDTCMAddress() + HW_DTCM_SIZE)) &&
#endif
         (((src | len) & (CARD_ROM_PAGE_SIZE - 1)) == 0);
}
#endif /* SDK_VERSION_MAJOR */

#if defined(SDK_ARM7) && defined(SDK_ARM7_READROM_SUPPORT)
    void CARDi_ReadRomCore (const void *src, void *dst, u32 len)
    {
        CARDRomStat *const p = &rom_stat;
        CARDiCommon *const c = &cardi_common;

        c->dma = (MI_DMA_MAX_NUM + 1);
        c->src = (u32)src;
        c->dst = (u32)dst;
        c->len = (u32)len;
        c->callback = NULL;
        c->callback_arg = NULL;
        p->ctrl = CARDi_GetRomFlag(CARD_COMMAND_PAGE);
        cardi_common.cur_th = OS_GetCurrentThread();
        CARDi_ReadCard(p);
    }
#endif

#if defined(SDK_TS)
    u32 CARDi_ReadRomID (void)
    {
        CARDRomStat *const p = &rom_stat;
        CARDiCommon *const c = &cardi_common;

        u32 ret = 0;

        SDK_ASSERT(CARD_IsAvailable());
        SDK_ASSERTMSG(CARDi_GetTargetMode() == CARD_TARGET_ROM, "must be locked by CARD_LockRom()");

        #if SDK_VERSION_MAJOR == 4
        CARDi_WaitTask(c, NULL, NULL);
        #elif SDK_VERSION_MAJOR == 5
        #if defined(SDK_ARM9)
          (void)CARDi_WaitForTask(&cardi_common, TRUE, NULL, NULL);
        #endif // defined(SDK_ARM9)
        #endif

        ret = CARDi_ReadRomIDCore();
        #if SDK_VERSION_MAJOR == 4
        CARDi_ReadEnd();
        #elif SDK_VERSION_MAJOR == 5
        #ifdef SDK_ARM9
          CARDi_CheckPulledOutCore(ret);
        #endif

        #if defined(SDK_ARM9)
          cardi_common.cmd->result = CARD_RESULT_SUCCESS;
          CARDi_EndTask(&cardi_common);
        #endif // defined(SDK_ARM9)
        #endif
        return ret;
    }
#endif

static void CARDi_ReadRomSyncCore (CARDiCommon *c)
{
#if SDK_VERSION_MAJOR == 4
	CARDRomStat *const p = &rom_stat;
	(void)c;

	if (CARDi_ReadFromCache(p)) {
		(*p->read_func)(p);
	}
	CARDi_ReadEnd();
#elif SDK_VERSION_MAJOR == 5
  (void)(*CARDiReadRomFunction)(NULL, (void *)c->dst, c->src, c->len);
#ifdef SDK_ARM9

  CARDi_CheckPulledOutCore(CARDi_ReadRomIDCore());
#endif

  CARDi_RefreshRom(CARD_ROMST_RFS_WARN_L2_MASK);

  cardi_common.cmd->result = CARD_RESULT_SUCCESS;
#endif
}

void CARDi_ReadRom (u32 dma,
                    const void *src, void *dst, u32 len,
                    MIDmaCallback callback, void *arg, BOOL is_async)
{
    #if SDK_VERSION_MAJOR == 4
	CARDRomStat *const p = &rom_stat;
    #endif
	CARDiCommon *const c = &cardi_common;

	SDK_ASSERT(CARD_IsAvailable());
	SDK_ASSERT(CARDi_GetTargetMode() == CARD_TARGET_ROM);
	SDK_ASSERTMSG((dma != 0), "cannot specify DMA channel 0");

	CARD_CheckEnabled();

#if SDK_VERSION_MAJOR == 5
    if ((CARDi_GetAccessLevel() & CARD_ACCESS_LEVEL_ROM) == 0) {
        OS_TPanic("this program cannot access CARD-ROM!");
    }
#endif

	CARDi_WaitTask(
        c, 
        #if SDK_VERSION_MAJOR == 5
        TRUE,
        #endif
        callback, 
        arg);

    #if SDK_VERSION_MAJOR == 4
	c->dma = dma;
    #elif SDK_VERSION_MAJOR == 5
    c->DmaCall = CARDi_GetDmaInterface(dma);
    c->dma = (u32)((c->DmaCall != NULL) ? (dma & MI_DMA_CHANNEL_MASK)
                                        : MI_DMA_NOT_USE);
    if (c->dma <= MI_DMA_MAX_NUM) {
      c->DmaCall->Stop(c->dma);
    }
    #endif
	c->src = (u32)((u32)src + cardi_rom_base);
	c->dst = (u32)dst;
	c->len = (u32)len;
    #if SDK_VERSION_MAJOR == 4
	if (dma <= MI_DMA_MAX_NUM) {
		MI_StopDma(dma);
    }

	if (CARDi_TryReadCardDma(p)) {
		if (!is_async)
			CARD_WaitRomAsync();
	} else if (is_async)   {
		CARDi_SetTask(CARDi_ReadRomSyncCore);
	} else {
		c->cur_th = OS_GetCurrentThread();
		CARDi_ReadRomSyncCore(c);
	}
    #endif

    #if SDK_VERSION_MAJOR == 5
      {
        CARDTransferInfo *info = CARDiDmaReadInfo;
        info->callback = CARDi_DmaReadDone;
        info->userdata = NULL;
        info->src = c->src;
        info->dst = c->dst;
        info->len = c->len;
        info->work = 0;
      }

      if ((CARDiReadRomFunction == CARDi_ReadRomWithCPU) &&
          CARDi_IsRomDmaAvailable(c->dma, (void *)c->dst, c->src, c->len)) {
    #if defined(SDK_ARM9)

        OSIntrMode bak_psr = OS_DisableInterrupts();
        if (CARDiEnableCacheInvalidationIC) {
          CARDi_ICInvalidateSmart((void *)c->dst, c->len, c->flush_threshold_ic);
        }
        if (CARDiEnableCacheInvalidationDC) {
          CARDi_DCInvalidateSmart((void *)c->dst, c->len, c->flush_threshold_dc);
        }
        (void)OS_RestoreInterrupts(bak_psr);
    #endif

        CARDi_ReadRomWithDMA(CARDiDmaReadInfo);

        if (!is_async) {
          CARD_WaitRomAsync();
        }
      } else {

        if (CARDiEnableCacheInvalidationIC) {
          CARDi_ICInvalidateSmart((void *)c->dst, c->len, c->flush_threshold_ic);
        }

        (void)CARDi_ExecuteOldTypeTask(CARDi_ReadRomSyncCore, is_async);
      }
    #endif
}

#if SDK_VERSION_MAJOR == 4
void CARD_Init (void)
{
	CARDiCommon *const p = &cardi_common;

	if (!p->flag) {
		p->flag = CARD_STAT_INIT;
		p->src = p->dst = p->len = 0;
		p->dma = (u32) ~0;
		p->callback = NULL;
		p->callback_arg = NULL;

		cardi_rom_base = 0;

		CARDi_InitCommon();

		rom_stat.read_func = CARDi_GetRomAccessor();

#if !defined(SDK_SMALL_BUILD)

		CARD_InitPulledOutCallback();
#endif
	}
}
#endif

void CARD_WaitRomAsync (void)
{
	(void)CARDi_WaitAsync();
}

BOOL CARD_TryWaitRomAsync (void)
{
	return CARDi_TryWaitAsync();
}

#if SDK_VERSION_MAJOR == 4
void (*CARDi_GetRomAccessor(void)) (CARDRomStat *)
{
	return CARDi_ReadCard;
}
#endif


#if SDK_VERSION_MAJOR == 5
#if defined(CARD_USING_HASHCHECK)
#include <twl/ltdmain_begin.h>

u8 *CARDiHashBufferAddress = NULL;
u32 CARDiHashBufferLength = 0;
static CARDRomHashContext context[1];

static int CARDi_ReadCardWithHash(void *userdata, void *buffer, u32 offset,
                                  u32 length) {
  (void)userdata;
  CARD_ReadRomHashImage(context, buffer, offset, length);
  return (int)length;
}

static int CARDi_ReadCardWithHashInternalAsync(void *userdata, void *buffer,
                                               u32 offset, u32 length) {
  if (cardi_common.dma == MI_DMA_NOT_USE) {
    length = 0;
  } else {
    CARDRomHashContext *context = (CARDRomHashContext *)userdata;
    static CARDTransferInfo card_hash_info[1];
    DC_FlushRange(buffer, length);
    card_hash_info->callback = (void (*)(void *))CARD_NotifyRomHashReadAsync;
    card_hash_info->userdata = context;
    card_hash_info->src = offset;
    card_hash_info->dst = (u32)buffer;
    card_hash_info->len = length;
    card_hash_info->command = 0;
    card_hash_info->work = 0;
    CARDi_ReadRomWithDMA(card_hash_info);
  }
  return (int)length;
}

static void CARDi_InitRomHash(void) {

  u8 *lo = (u8 *)MATH_ROUNDUP((u32)OS_GetMainArenaLo(), 32);
  u8 *hi = (u8 *)MATH_ROUNDDOWN((u32)OS_GetMainArenaHi(), 32);
  u32 len = CARD_CalcRomHashBufferLength(CARD_GetOwnRomHeaderTWL());
  if (&lo[len] > hi) {
    OS_TPanic("cannot allocate memory for ROM-hash from ARENA");
  } else {
    OS_SetMainArenaLo(&lo[len]);
    CARDiHashBufferAddress = lo;
    CARDiHashBufferLength = len;

    if ((OS_GetBootType() == OS_BOOTTYPE_ROM) &&
        ((((const u8 *)HW_TWL_ROM_HEADER_BUF)[0x1C] & 0x01) != 0)) {
      cardi_common.dma = MI_DMA_NOT_USE;
      CARDiReadRomFunction = CARDi_ReadCardWithHash;
      {
        u16 id = (u16)OS_GetLockID();
        CARD_LockRom(id);
        CARD_InitRomHashContext(context, CARD_GetOwnRomHeaderTWL(),
                                CARDiHashBufferAddress, CARDiHashBufferLength,
                                CARDi_ReadRomWithCPU,
                                CARDi_ReadCardWithHashInternalAsync, context);

        CARDiHashBufferAddress = NULL;
        CARDiHashBufferLength = 0;
        CARD_UnlockRom(id);
        OS_ReleaseLockID(id);
      }
    }
  }
}
#include <twl/ltdmain_end.h>
#endif

void CARDi_InitRom(void) {
#if defined(CARD_USING_ROMREADER)
  CARDiReadRomFunction = CARDi_ReadRomWithCPU;

  if ((OS_GetBootType() == OS_BOOTTYPE_ROM) &&
      CARD_GetOwnRomHeader()->rom_size) {
    u16 id = (u16)OS_GetLockID();
    CARD_LockRom(id);
    (void)CARDi_ReadRomWithCPU(NULL, CARDiOwnSignature,
                               CARD_GetOwnRomHeader()->rom_size,
                               CARD_ROM_DOWNLOAD_SIGNATURE_SIZE);
    CARD_UnlockRom(id);
    OS_ReleaseLockID(id);
  }
#if defined(CARD_USING_HASHCHECK)

  if (OS_IsRunOnTwl()) {
    CARDi_InitRomHash();
  }
#endif
#else
  CARDiReadRomFunction = NULL;
#endif
}

const u8 *CARDi_GetOwnSignature(void) { return CARDiOwnSignature; }

void CARDi_SetOwnSignature(const void *signature) {
  MI_CpuCopy8(signature, CARDiOwnSignature, CARD_ROM_DOWNLOAD_SIGNATURE_SIZE);
}

#if defined(SDK_ARM9)

void CARD_GetCacheFlushFlag(BOOL *icache, BOOL *dcache) {
  SDK_ASSERT(CARD_IsAvailable());
  if (icache) {
    *icache = CARDiEnableCacheInvalidationIC;
  }
  if (dcache) {
    *dcache = CARDiEnableCacheInvalidationDC;
  }
}

void CARD_SetCacheFlushFlag(BOOL icache, BOOL dcache) {
  SDK_ASSERT(CARD_IsAvailable());
  CARDiEnableCacheInvalidationIC = icache;
  CARDiEnableCacheInvalidationDC = dcache;
}
#endif
#endif
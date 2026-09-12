#include <nitro.h>
#include <nitro/mb.h>
#if SDK_VERSION_MAJOR == 5
#include <nitro/card/rom.h>
#endif

#include "../include/card_common.h"
#if SDK_VERSION_MAJOR == 5
#include "../include/card_event.h"
#endif
#include "../include/card_spi.h"

CARDiCommon cardi_common ATTRIBUTE_ALIGN(32);
static CARDiCommandArg cardi_arg ATTRIBUTE_ALIGN(32);

#if SDK_VERSION_MAJOR == 4
static u8 cardi_thread_stack[0x400] ATTRIBUTE_ALIGN(4);

static void CARDi_LockResource(CARDiOwner owner, CARDTargetMode target);
static void CARDi_UnlockResource(CARDiOwner owner, CARDTargetMode target);

void CARDi_SetTask (void (*task)(CARDiCommon *))
{
	CARDiCommon *const p = &cardi_common;

	(void)OS_SetThreadPriority(p->thread, p->priority);

	p->cur_th = p->thread;
	p->task_func = task;
	p->flag |= CARD_STAT_TASK;

	OS_WakeupThreadDirect(p->thread);
}

static 
#endif
void CARDi_LockResource (CARDiOwner owner, CARDTargetMode target)
{
	CARDiCommon *const p = &cardi_common;
	OSIntrMode bak_psr = OS_DisableInterrupts();

	if (p->lock_owner == owner) {
		if (p->lock_target != target) {
            #ifdef SDK_BUILD_ARM
			OS_TPanic("card-lock : can not reuse same ID for locking without unlocking!");
            #endif
		}
	} else {
		while (p->lock_owner != OS_LOCK_ID_ERROR)
			OS_SleepThread(p->lock_queue);
		p->lock_owner = owner;
		p->lock_target = target;
	}

	++p->lock_ref;
	#if SDK_VERSION_MAJOR == 4
	p->cmd->result = CARD_RESULT_SUCCESS;
	#endif
	(void)OS_RestoreInterrupts(bak_psr);
}

static void CARDi_UnlockResource (CARDiOwner owner, CARDTargetMode target)
{
	CARDiCommon *p = &cardi_common;
	OSIntrMode bak_psr = OS_DisableInterrupts();

	if ((p->lock_owner != owner) || !p->lock_ref) {
		OS_Panic("card-unlock : specified ID for unlocking is not locking one!");
	} else {
		if (p->lock_target != target) {
			OS_Panic("card-unlock : locking target and unlocking one are different!");
		}
		if (!--p->lock_ref) {
			p->lock_owner = OS_LOCK_ID_ERROR;
			p->lock_target = CARD_TARGET_NONE;
			OS_WakeupThread(p->lock_queue);
		}
	}
	#if SDK_VERSION_MAJOR == 4
	p->cmd->result = CARD_RESULT_SUCCESS;
	#endif
	(void)OS_RestoreInterrupts(bak_psr);
}

#if SDK_VERSION_MAJOR == 5
CARDAccessLevel CARDi_GetAccessLevel(void) {
  CARDAccessLevel level = CARD_ACCESS_LEVEL_NONE;
  if (OS_GetBootType() == OS_BOOTTYPE_ROM) {
    level = CARD_ACCESS_LEVEL_FULL;
  } else if (!OS_IsRunOnTwl()) {
    level = CARD_ACCESS_LEVEL_BACKUP;
  }
#ifdef SDK_TWL
  else {
    const CARDRomHeaderTWL *header = CARD_GetOwnRomHeaderTWL();
    BOOL backupPowerOn = FALSE;
    if (header->access_control.game_card_nitro_mode) {
      level |= CARD_ACCESS_LEVEL_ROM;
      backupPowerOn = TRUE;
    } else if (header->access_control.game_card_on) {
      backupPowerOn = TRUE;
    }
    if (backupPowerOn) {
      if (header->access_control.backup_access_read) {
        level |= CARD_ACCESS_LEVEL_BACKUP_R;
      }
      if (header->access_control.backup_access_write) {
        level |= CARD_ACCESS_LEVEL_BACKUP_W;
      }
    }
  }
#endif
  return level;
}
#endif

#if SDK_VERSION_MAJOR == 4
void CARDi_InitCommon (void)
{
	CARDiCommon *p = &cardi_common;

	p->lock_owner = OS_LOCK_ID_ERROR;
	p->lock_ref = 0;
	p->lock_target = CARD_TARGET_NONE;

#if (defined(SDK_ARM9) || defined(SDK_PORT))
	p->cmd = &cardi_arg;
	MI_CpuFillFast(&cardi_arg, 0x00, sizeof(cardi_arg));
	DC_FlushRange(&cardi_arg, sizeof(cardi_arg));
#else
	p->cmd = NULL;
	p->recv_step = 0;
#endif

#if (defined(SDK_ARM9) || defined(SDK_PORT))
	p->flush_threshold_ic = 0xFFFFFFFF;
	p->flush_threshold_dc = 0xFFFFFFFF;
#endif

#if !defined(SDK_SMALL_BUILD) && (defined(SDK_ARM9) || defined(SDK_PORT))
	if (!MB_IsMultiBootChild()) {
		MI_CpuCopy8((const void *)HW_ROM_HEADER_BUF, (void *)HW_CARD_ROM_HEADER,
		            HW_CARD_ROM_HEADER_SIZE);
	}
#endif

#if !defined(SDK_NO_THREAD)
	OS_InitThreadQueue(p->lock_queue);
	OS_InitThreadQueue(p->busy_q);
	p->priority = CARD_THREAD_PRIORITY_DEFAULT;
	OS_CreateThread(p->thread,
	                CARDi_TaskThread, NULL,
	                cardi_thread_stack + sizeof(cardi_thread_stack),
	                sizeof(cardi_thread_stack), p->priority);
	OS_WakeupThreadDirect(p->thread);
#endif

	PXI_SetFifoRecvCallback(PXI_FIFO_TAG_FS, CARDi_OnFifoRecv);

	if (!MB_IsMultiBootChild()) {
		CARD_Enable(TRUE);
	}
}

static BOOL CARDi_EnableFlag = FALSE;

BOOL CARD_IsEnabled (void)
{
	return CARDi_EnableFlag;
}

void CARD_CheckEnabled (void)
{
	if (!CARD_IsEnabled()) {
		OS_Panic("NITRO-CARD permission denied");
	}
}

void CARD_Enable (BOOL enable)
{
	CARDi_EnableFlag = enable;
}
#endif

BOOL CARDi_WaitAsync (void)
{
#if SDK_VERSION_MAJOR == 4
	CARDiCommon *const p = &cardi_common;
	SDK_ASSERT(CARD_IsAvailable());

	{
		OSIntrMode bak_psr = OS_DisableInterrupts();
        #ifndef SDK_PORT
		while ((p->flag & CARD_STAT_BUSY) != 0) {
			OS_SleepThread(p->busy_q);
		}
        #endif
		(void)OS_RestoreInterrupts(bak_psr);
	}

	return (p->cmd->result == CARD_RESULT_SUCCESS);
#elif SDK_VERSION_MAJOR == 5
  	SDK_ASSERT(CARD_IsAvailable());
  	return CARDi_WaitForTask(&cardi_common, FALSE, NULL, NULL);
#endif
}

BOOL CARDi_TryWaitAsync (void)
{
	CARDiCommon *const p = &cardi_common;
	SDK_ASSERT(CARD_IsAvailable());

	return !(p->flag & CARD_STAT_BUSY);
}

#if SDK_VERSION_MAJOR == 4
BOOL CARD_IsAvailable (void)
{
	CARDiCommon *const p = &cardi_common;
	return (p->flag != 0);
}

CARDResult CARD_GetResultCode (void)
{
	CARDiCommon *const p = &cardi_common;
	SDK_ASSERT(CARD_IsAvailable());

	return p->cmd->result;
}

u32 CARD_GetThreadPriority (void)
{
	CARDiCommon *const p = &cardi_common;
	SDK_ASSERT(CARD_IsAvailable());

	return p->priority;
}

u32 CARD_SetThreadPriority (u32 prior)
{
	CARDiCommon *const p = &cardi_common;
	SDK_ASSERT(CARD_IsAvailable());

	{
		OSIntrMode bak_psr = OS_DisableInterrupts();
		u32 ret = p->priority;
		SDK_ASSERT((prior >= OS_THREAD_PRIORITY_MIN) && (prior <= OS_THREAD_PRIORITY_MAX));
		p->priority = prior;
		(void)OS_SetThreadPriority(p->thread, p->priority);
		(void)OS_RestoreInterrupts(bak_psr);
		return ret;
	}
}

void CARD_LockRom (u16 lock_id)
{
	SDK_ASSERT(CARD_IsAvailable());

	CARDi_LockResource(lock_id, CARD_TARGET_ROM);

#if defined(SDK_TEG)
	if (!CARDi_IsTrueRom())
		(void)OS_LockCartridge(lock_id);
	else
#endif
	{
		(void)OS_LockCard(lock_id);
	}
}

void CARD_UnlockRom (u16 lock_id)
{
	SDK_ASSERT(CARD_IsAvailable());
	SDK_ASSERT(cardi_common.lock_target == CARD_TARGET_ROM);

#if defined(SDK_TEG)
	if (!CARDi_IsTrueRom())
		(void)OS_UnLockCartridge(lock_id);
	else
#endif
	{
		(void)OS_UnlockCard(lock_id);
	}

	CARDi_UnlockResource(lock_id, CARD_TARGET_ROM);

}

void CARD_LockBackup (u16 lock_id)
{
	SDK_ASSERT(CARD_IsAvailable());
	CARDi_LockResource(lock_id, CARD_TARGET_BACKUP);

#if defined(SDK_ARM7)
	(void)OS_LockCard(lock_id);
#endif
}

void CARD_UnlockBackup (u16 lock_id)
{
	SDK_ASSERT(CARD_IsAvailable());
	SDK_ASSERT(cardi_common.lock_target == CARD_TARGET_BACKUP);

	if (!CARD_TryWaitBackupAsync()) {
		OS_TWarning("called CARD_UnlockBackup() during backup asynchronous operation. (force to wait)\n");
		(void)CARD_WaitBackupAsync();
	}

#if defined(SDK_ARM7)
	(void)OS_UnlockCard(lock_id);
#endif

	CARDi_UnlockResource(lock_id, CARD_TARGET_BACKUP);
}

void CARD_GetCacheFlushThreshold (u32 *icache, u32 *dcache)
{
#if (defined(SDK_ARM9) || defined(SDK_PORT))
	SDK_ASSERT(CARD_IsAvailable());
	if (icache) {
		*icache = cardi_common.flush_threshold_ic;
	}
	if (dcache) {
		*dcache = cardi_common.flush_threshold_dc;
	}
#else
	(void)icache;
	(void)dcache;
#endif
}

void CARD_SetCacheFlushThreshold (u32 icache, u32 dcache)
{
#if (defined(SDK_ARM9) || defined(SDK_PORT))
	SDK_ASSERT(CARD_IsAvailable());
	cardi_common.flush_threshold_ic = icache;
	cardi_common.flush_threshold_dc = dcache;
#else
	(void)icache;
	(void)dcache;
#endif
}

#if !defined(SDK_SMALL_BUILD)

const u8 *CARD_GetRomHeader (void)
{
	return (const u8 *)HW_CARD_ROM_HEADER;
}

#endif
#endif /* SDK_VERSION_MAJOR */

#if SDK_VERSION_MAJOR == 5
void CARDi_InitResourceLock(void) {
  CARDiCommon *p = &cardi_common;
  p->lock_owner = OS_LOCK_ID_ERROR;
  p->lock_ref = 0;
  p->lock_target = CARD_TARGET_NONE;
  OS_InitThreadQueue(p->lock_queue);
}

void CARDi_InitCommand(void) {
  CARDiCommon *p = &cardi_common;

#if defined(SDK_ARM9) || defined(SDK_PORT)
  p->cmd = &cardi_arg;
  MI_CpuFillFast(&cardi_arg, 0x00, sizeof(cardi_arg));
  DC_FlushRange(&cardi_arg, sizeof(cardi_arg));
#else
  p->cmd = CARD_UNSYNCHRONIZED_BUFFER;
#endif

  PXI_SetFifoRecvCallback(PXI_FIFO_TAG_FS, CARDi_OnFifoRecv);
}
#endif

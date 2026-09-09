#include <nitro.h>

#include "../include/card_common.h"
#include "../include/card_event.h"
#include "../include/card_rom.h"

static BOOL CARDi_EnableFlag = FALSE;

static BOOL CARDi_LockBusCondition(void *userdata) {
  u16 lockID = *(const u16 *)userdata;
  return (OS_TryLockCard(lockID) == OS_LOCK_SUCCESS);
}

void CARD_Init(void) {
  CARDiCommon *p = &cardi_common;

  if (!p->flag) {
    p->flag = CARD_STAT_INIT;

#if !defined(SDK_SMALL_BUILD) && defined(SDK_ARM9)

    if (OS_GetBootType() == OS_BOOTTYPE_ROM) {
      MI_CpuCopy8((const void *)HW_ROM_HEADER_BUF, (void *)HW_CARD_ROM_HEADER,
                  HW_CARD_ROM_HEADER_SIZE);
    }
#endif // !defined(SDK_SMALL_BUILD) && defined(SDK_ARM9)

#if defined(SDK_ARM9)

    p->src = 0;
    p->dst = 0;
    p->len = 0;
    p->dma = MI_DMA_NOT_USE;
    p->DmaCall = NULL;
    p->flush_threshold_ic = 0x400;
    p->flush_threshold_dc = 0x2400;
#endif
    cardi_rom_base = 0;
    p->priority = CARD_THREAD_PRIORITY_DEFAULT;

    CARDi_InitResourceLock();

#if defined(SDK_ARM9) || defined(SDK_PORT)

    p->callback = NULL;
    p->callback_arg = NULL;
    OS_InitThreadQueue(p->busy_q);
    OS_CreateThread(p->thread.context, CARDi_OldTypeTaskThread, NULL,
                    p->thread.stack + sizeof(p->thread.stack),
                    sizeof(p->thread.stack), p->priority);
    OS_WakeupThreadDirect(p->thread.context);
#else // defined(SDK_ARM9)

    CARDi_InitTaskQueue(p->task_q);
    OS_CreateThread(p->thread.context, CARDi_TaskWorkerProcedure, p->task_q,
                    p->thread.stack + sizeof(p->thread.stack),
                    sizeof(p->thread.stack), p->priority);
    OS_WakeupThreadDirect(p->thread.context);
#endif

    CARDi_InitCommand();

    CARDi_InitRom();

    if (OS_GetBootType() == OS_BOOTTYPE_ROM) {
      CARD_Enable(TRUE);
    }

#if !defined(SDK_SMALL_BUILD)

    CARD_InitPulledOutCallback();
#endif
  }
}

BOOL CARD_IsAvailable(void) {
  CARDiCommon *const p = &cardi_common;
  return (p->flag != 0);
}

BOOL CARD_IsEnabled(void) { return CARDi_EnableFlag; }

void CARD_CheckEnabled(void) {
  if (!CARD_IsEnabled()) {
    OS_TPanic("NITRO-CARD permission denied");
  }
}

void CARD_Enable(BOOL enable) { CARDi_EnableFlag = enable; }

u32 CARD_GetThreadPriority(void) {
  CARDiCommon *const p = &cardi_common;
  SDK_ASSERT(CARD_IsAvailable());

  return p->priority;
}

u32 CARD_SetThreadPriority(u32 prior) {
  CARDiCommon *const p = &cardi_common;
  SDK_ASSERT(CARD_IsAvailable());

  {
    OSIntrMode bak_psr = OS_DisableInterrupts();
    u32 ret = p->priority;
    SDK_ASSERT((prior >= OS_THREAD_PRIORITY_MIN) &&
               (prior <= OS_THREAD_PRIORITY_MAX));
    p->priority = prior;
    (void)OS_SetThreadPriority(p->thread.context, p->priority);
    (void)OS_RestoreInterrupts(bak_psr);
    return ret;
  }
}

CARDResult CARD_GetResultCode(void) {
  CARDiCommon *const p = &cardi_common;
  SDK_ASSERT(CARD_IsAvailable());

  return p->cmd->result;
}

const u8 *CARD_GetRomHeader(void) { return (const u8 *)HW_CARD_ROM_HEADER; }

const CARDRomHeader *CARD_GetOwnRomHeader(void) {
  return (const CARDRomHeader *)HW_ROM_HEADER_BUF;
}

#if defined(SDK_TWL)

const CARDRomHeaderTWL *CARD_GetOwnRomHeaderTWL(void) {
  return (const CARDRomHeaderTWL *)HW_TWL_ROM_HEADER_BUF;
}

#endif // SDK_TWL

void CARD_GetCacheFlushThreshold(u32 *icache, u32 *dcache) {
#if defined(SDK_ARM9)
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

void CARD_SetCacheFlushThreshold(u32 icache, u32 dcache) {
#if defined(SDK_ARM9)
  SDK_ASSERT(CARD_IsAvailable());
  cardi_common.flush_threshold_ic = icache;
  cardi_common.flush_threshold_dc = dcache;
#else
  (void)icache;
  (void)dcache;
#endif
}

void CARD_LockRom(u16 lock_id) {
  SDK_ASSERT(CARD_IsAvailable());

  CARDi_LockResource(lock_id, CARD_TARGET_ROM);

  {

#define CARD_USING_SLEEPY_LOCK
#ifdef CARD_USING_SLEEPY_LOCK
    CARDEventListener el[1];
    CARDi_InitEventListener(el);
    CARDi_SetEventListener(el, CARDi_LockBusCondition, &lock_id);
    CARDi_WaitForEvent(el);
#else

    (void)OS_LockCard(lock_id);
#endif
  }
}

void CARD_UnlockRom(u16 lock_id) {
  SDK_ASSERT(CARD_IsAvailable());
  SDK_ASSERT(cardi_common.lock_target == CARD_TARGET_ROM);

  {
    (void)OS_UnlockCard(lock_id);
  }

  CARDi_UnlockResource(lock_id, CARD_TARGET_ROM);
}

void CARD_LockBackup(u16 lock_id) {
  SDK_ASSERT(CARD_IsAvailable());

  CARDi_LockResource(lock_id, CARD_TARGET_BACKUP);

#if defined(SDK_ARM7)
  (void)OS_LockCard(lock_id);
#endif
}

void CARD_UnlockBackup(u16 lock_id) {
  SDK_ASSERT(CARD_IsAvailable());
  SDK_ASSERT(cardi_common.lock_target == CARD_TARGET_BACKUP);

#if defined(SDK_ARM9)

  if (!CARD_TryWaitBackupAsync()) {
    OS_TWarning("called CARD_UnlockBackup() during backup asynchronous "
                "operation. (force to wait)\n");
    (void)CARD_WaitBackupAsync();
  }
#endif // defined(SDK_ARM9)

#if defined(SDK_ARM7)
  (void)OS_UnlockCard(lock_id);
#endif

  CARDi_UnlockResource(lock_id, CARD_TARGET_BACKUP);
}

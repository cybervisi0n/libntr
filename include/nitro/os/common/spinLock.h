#ifndef NITRO_OS_SPINLOCK_H_
#define NITRO_OS_SPINLOCK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/types.h>

#if SDK_VERSION_MAJOR == 5
#ifndef SDK_TWL
#ifdef SDK_PORT
#include <nitro/hw/X86/mmap_global.h>
#else
#ifdef SDK_ARM9
#include <nitro/hw/ARM9/mmap_global.h>
#else // SDK_ARM7
#include <nitro/hw/ARM7/mmap_global.h>
#endif
#endif
#else // SDK_TWL
#include <twl/hw/common/mmap_shared.h>
#ifdef SDK_PORT
#include <twl/hw/X86/mmap_global.h>
#else
#ifdef SDK_ARM9
#include <twl/hw/ARM9/mmap_global.h>
#else // SDK_ARM7
#include <twl/hw/ARM7/mmap_global.h>
#endif
#endif
#endif // SDK_TWL
#endif

#define OS_UNLOCK_ID            0
#define OS_MAINP_LOCKED_FLAG    0x40
#define OS_MAINP_LOCK_ID_START  0x40
#define OS_MAINP_LOCK_ID_END    0x6f
#define OS_MAINP_DBG_LOCK_ID    0x70
#define OS_MAINP_SYSTEM_LOCK_ID 0x7f
#define OS_SUBP_LOCKED_FLAG     0x80
#define OS_SUBP_LOCK_ID_START   0x80
#define OS_SUBP_LOCK_ID_END     0xaf
#define OS_SUBP_DBG_LOCK_ID     0xb0
#define OS_SUBP_SYSTEM_LOCK_ID  0xbf

#define OS_LOCK_SUCCESS         0
#define OS_LOCK_ERROR           (-1)

#define OS_UNLOCK_SUCCESS       0
#define OS_UNLOCK_ERROR         (-2)

#define OS_LOCK_FREE            0

#define OS_LOCK_ID_ERROR        (-3)

#if(defined(SDK_PORT) && defined(__cplusplus))
typedef volatile struct OSLockWord_struct
#else
typedef volatile struct OSLockWord 
#endif
{
	u32 lockFlag;
	u16 ownerID;
	u16 extension;
} OSLockWord;

void OS_InitLock(void);

s32 OS_LockByWord(u16 lockID, OSLockWord * lockp, void (*ctrlFuncp)(void));
s32 OS_LockCartridge(u16 lockID);
s32 OS_LockCard(u16 lockID);

s32 OS_UnlockByWord(u16 lockID, OSLockWord * lockp, void (*ctrlFuncp)(void));
s32 OS_UnlockCartridge(u16 lockID);
s32 OS_UnlockCard(u16 lockID);

s32 OS_UnLockByWord(u16 lockID, OSLockWord * lockp, void (*ctrlFuncp)(void));
s32 OS_UnLockCartridge(u16 lockID);
s32 OS_UnLockCard(u16 lockID);

s32 OS_TryLockByWord(u16 lockID, OSLockWord * lockp, void (*crtlFuncp)(void));
s32 OS_TryLockCartridge(u16 lockID);
s32 OS_TryLockCard(u16 lockID);

u16 OS_ReadOwnerOfLockWord(OSLockWord * lockp);

#define OS_ReadOwnerOfLockCartridge()  OS_ReadOwnerOfLockWord((OSLockWord *)HW_CTRDG_LOCK_BUF)
#define OS_ReadOwnerOfLockCard()       OS_ReadOwnerOfLockWord((OSLockWord *)HW_CARD_LOCK_BUF)

s32 OS_GetLockID(void);
void OS_ReleaseLockID(u16 lockID);

#if SDK_VERSION_MAJOR == 5
#ifdef SDK_TWL

#define OSi_SYNCTYPE_SENDER 0
#define OSi_SYNCTYPE_RECVER 1

#define OSi_SYNCVAL_NOT_READY 0
#define OSi_SYNCVAL_READY 1

void OSi_SyncWithOtherProc(int type, void *syncBuf);

static inline void OSi_SetSyncValue(u8 n) {
  *(vu8 *)(HW_INIT_LOCK_BUF + 4) = n;
}
static inline u8 OSi_GetSyncValue(void) {
  return *(vu8 *)(HW_INIT_LOCK_BUF + 4);
}

#endif
#endif

#ifdef __cplusplus
}
#endif

#endif

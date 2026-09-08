#ifndef _APPLICATION_JUMP_H_
#define _APPLICATION_JUMP_H_

#include <twl.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OS_MCU_RESET_VALUE_BUF_HOTBT_MASK 0x00000001
#define OS_MCU_RESET_VALUE_OFS 0

typedef enum OSAppJumpType {
  OS_APP_JUMP_NORMAL = 0,
  OS_APP_JUMP_TMP = 1
} OSAppJumpType;

#define OS_TMP_APP_PATH "nand:/<tmpjump>"

#if defined(SDK_ARM9) || defined(SDK_PORT)

BOOL OS_JumpToSystemMenu(void);

BOOL OS_JumpToInternetSetting(void);

BOOL OS_JumpToEULAViewer(void);

BOOL OS_JumpToWirelessSetting(void);

BOOL OS_RebootSystem(void);

BOOL OS_IsBootFromSystemMenu(void);

BOOL OS_IsTemporaryApplication(void);

BOOL OS_IsRebooted(void);

BOOL OS_ReturnToPrevApplication(void);
BOOL OSi_CanApplicationJumpTo(OSTitleId titleID);
OSTitleId OSi_GetPrevTitleId(void);
BOOL OS_DoApplicationJump(OSTitleId id, OSAppJumpType jumpType);
#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _APPLICATION_JUMP_H_

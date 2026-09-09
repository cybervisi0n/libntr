#include <nitro/card/common.h>

static CARDHookContext *CARDiHookChain = NULL;

void CARDi_RegisterHook(CARDHookContext *hook, CARDHookFunction callback,
                        void *arg) {
  OSIntrMode bak = OS_DisableInterrupts();
  hook->callback = callback;
  hook->userdata = arg;
  hook->next = CARDiHookChain;
  CARDiHookChain = hook;
  (void)OS_RestoreInterrupts(bak);
}

void CARDi_UnregisterHook(CARDHookContext *hook) {
  OSIntrMode bak = OS_DisableInterrupts();
  CARDHookContext **pp;
  for (pp = &CARDiHookChain; *pp; pp = &(*pp)->next) {
    if (*pp == hook) {
      *pp = (*pp)->next;
      break;
    }
  }
  (void)OS_RestoreInterrupts(bak);
}

void CARDi_NotifyEvent(CARDEvent event, void *arg) {
  OSIntrMode bak = OS_DisableInterrupts();
  CARDHookContext **pp = &CARDiHookChain;
  while (*pp) {
    CARDHookContext *hook = *pp;
    if (hook->callback) {
      (*hook->callback)(hook->userdata, event, arg);
    }

    if (*pp == hook) {
      pp = &(*pp)->next;
    }
  }
  (void)OS_RestoreInterrupts(bak);
}

#include <nitro.h>

#include "../include/card_task.h"

void CARDi_InitTaskQueue(CARDTaskQueue *queue) {
  queue->list = NULL;
  queue->quit = FALSE;
  OS_InitThreadQueue(queue->workers);
}

void CARDi_QuitTaskQueue(CARDTaskQueue *queue) {
  OSIntrMode bak_cpsr = OS_DisableInterrupts();
  queue->quit = TRUE;
  OS_WakeupThread(queue->workers);
  (void)OS_RestoreInterrupts(bak_cpsr);
}

void CARDi_InitTask(CARDTask *task, u32 priority, void *userdata,
                    CARDTaskFunction function, CARDTaskFunction callback) {
  task->next = NULL;
  task->priority = priority;
  task->userdata = userdata;
  task->function = function;
  task->callback = callback;
}

void CARDi_ProcessTask(CARDTaskQueue *queue, CARDTask *task, BOOL blocking,
                       BOOL changePriority) {

  if (!blocking) {
    OSIntrMode bak_cpsr = OS_DisableInterrupts();
    CARDTask **pp = (CARDTask **)&queue->list;
    for (; *pp; pp = &(*pp)->next) {
    }
    *pp = task;

    if (pp == &queue->list) {
      OS_WakeupThread(queue->workers);
    }
    (void)OS_RestoreInterrupts(bak_cpsr);
  }

  else {

    OSThread *curth = OS_GetCurrentThread();
    u32 prio = 0;
    if (changePriority) {
      prio = OS_GetThreadPriority(curth);
      (void)OS_SetThreadPriority(curth, task->priority);
    }
    if (task->function) {
      (*task->function)(task);
    }

    if (task->callback) {
      (*task->callback)(task);
    }

    if (changePriority) {
      (void)OS_SetThreadPriority(curth, prio);
    }
  }
}

CARDTask *CARDi_ReceiveTask(CARDTaskQueue *queue, BOOL blocking) {
  CARDTask *retval = NULL;
  OSIntrMode bak_cpsr = OS_DisableInterrupts();
  while (!queue->quit) {
    retval = queue->list;
    if ((retval != NULL) || !blocking) {
      break;
    }
    OS_SleepThread(queue->workers);
  }
  if (retval) {
    queue->list = retval->next;
  }
  (void)OS_RestoreInterrupts(bak_cpsr);
  return retval;
}

void CARDi_TaskWorkerProcedure(void *arg) {
  CARDTaskQueue *queue = (CARDTaskQueue *)arg;

  (void)OS_SetThreadPriority(OS_GetCurrentThread(), 0);
  for (;;) {

    CARDTask *task = CARDi_ReceiveTask(queue, TRUE);

    if (!task) {
      break;
    }

    CARDi_ProcessTask(queue, task, TRUE, TRUE);
  }
}

#if defined(SDK_ARM9) || defined(SDK_PORT)

#include "../include/card_common.h"

BOOL CARDi_ExecuteOldTypeTask(void (*task)(CARDiCommon *), BOOL async) {
  CARDiCommon *p = &cardi_common;
  if (async) {

    (void)OS_SetThreadPriority(p->thread.context, p->priority);

    p->task_func = task;
    p->flag |= CARD_STAT_TASK;
    OS_WakeupThreadDirect(p->thread.context);
  } else {
    (*task)(p);
    CARDi_EndTask(p);
  }
  return async ? TRUE : (p->cmd->result == CARD_RESULT_SUCCESS);
}

void CARDi_OldTypeTaskThread(void *arg) {
  CARDiCommon *p = &cardi_common;
  (void)arg;
  for (;;) {

    OSIntrMode bak_psr = OS_DisableInterrupts();
    for (;;) {
      if ((p->flag & CARD_STAT_TASK) != 0) {
        break;
      }
      OS_SleepThread(NULL);
    }
    (void)OS_RestoreInterrupts(bak_psr);

    (void)CARDi_ExecuteOldTypeTask(p->task_func, FALSE);
  }
}

BOOL CARDi_WaitForTask(CARDiCommon *p, BOOL restart, MIDmaCallback callback,
                       void *callback_arg) {
  OSIntrMode bak_psr = OS_DisableInterrupts();

  while ((p->flag & CARD_STAT_BUSY) != 0) {
    OS_SleepThread(p->busy_q);
  }

  if (restart) {
    p->flag |= CARD_STAT_BUSY;
    p->callback = callback;
    p->callback_arg = callback_arg;
  }
  (void)OS_RestoreInterrupts(bak_psr);
  return (p->cmd->result == CARD_RESULT_SUCCESS);
}

void CARDi_EndTask(CARDiCommon *p) {
  MIDmaCallback callback = p->callback;
  void *userdata = p->callback_arg;

  OSIntrMode bak_psr = OS_DisableInterrupts();

  p->flag &= ~(CARD_STAT_BUSY | CARD_STAT_TASK | CARD_STAT_CANCEL);
  OS_WakeupThread(p->busy_q);
  (void)OS_RestoreInterrupts(bak_psr);

  if (callback) {
    (*callback)(userdata);
  }
}

#endif // defined(SDK_ARM9)

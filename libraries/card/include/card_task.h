#ifndef NITRO_LIBRARIES_CARD_TASK_H__
#define NITRO_LIBRARIES_CARD_TASK_H__

#include <nitro/os.h>

#if SDK_VERSION_MAJOR != 5
#error This file is for SDK version 5 only!
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct CARDTask;
typedef void (*CARDTaskFunction)(struct CARDTask *);

typedef struct CARDTask {
  struct CARDTask *next;
  u32 priority;
  void *userdata;
  CARDTaskFunction function;
  CARDTaskFunction callback;
} CARDTask;

typedef struct CARDTaskQueue {
  CARDTask *volatile list;
  OSThreadQueue workers[1];
  u32 quit : 1;
  u32 dummy : 31;
} CARDTaskQueue;

void CARDi_InitTaskQueue(CARDTaskQueue *queue);

void CARDi_QuitTaskQueue(CARDTaskQueue *queue);

void CARDi_InitTask(CARDTask *task, u32 priority, void *userdata,
                    CARDTaskFunction function, CARDTaskFunction callback);

void CARDi_ProcessTask(CARDTaskQueue *queue, CARDTask *task, BOOL blocking,
                       BOOL changePriority);

CARDTask *CARDi_ReceiveTask(CARDTaskQueue *queue, BOOL blocking);

void CARDi_TaskWorkerProcedure(void *arg);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // NITRO_LIBRARIES_CARD_TASK_H__

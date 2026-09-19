#include <nitro/types.h>
#include <nitro.h>

void OSi_EnqueueTail(OSThread * thread, OSMutex * mutex);
void OSi_DequeueItem(OSThread * thread, OSMutex * mutex);
OSMutex * OSi_DequeueHead(OSThread * thread);

void OS_InitMutex (OSMutex * mutex)
{
    SDK_ASSERT(mutex);

    #ifdef SDK_PORT
    mutex->sdlMutex = SDL_CreateMutex();
    #else
    OS_InitThreadQueue(&mutex->queue);

    mutex->thread = NULL;
    mutex->count = 0;
    #endif
}

void OS_LockMutex (OSMutex * mutex)
{
    OSIntrMode saved;
    OSThread * currentThread;
    OSThread * ownerThread;

    SDK_ASSERT(mutex);

    #ifdef SDK_PORT
    SDL_LockMutex(mutex->sdlMutex);
    #else
    saved = OS_DisableInterrupts();
    currentThread = OS_GetCurrentThread();

    for (;;) {
        ownerThread = ((volatile OSMutex *)mutex)->thread;

        if (ownerThread == NULL) {
            mutex->thread = currentThread;
            mutex->count++;
            OSi_EnqueueTail(currentThread, mutex);
            break;
        } else if (ownerThread == currentThread)   {
            mutex->count++;
            break;
        } else {
            currentThread->mutex = mutex;
            OS_SleepThread(&mutex->queue);
            currentThread->mutex = NULL;
        }
    }

    (void)OS_RestoreInterrupts(saved);
    #endif
}

void OS_UnlockMutex (OSMutex * mutex)
{
    OSIntrMode saved;
    OSThread * currentThread;

    SDK_ASSERT(mutex);

    #ifdef SDK_PORT
    SDL_UnlockMutex(mutex->sdlMutex);
    #else
    saved = OS_DisableInterrupts();
    currentThread = OS_GetCurrentThread();

    if (mutex->thread == currentThread && --mutex->count == 0) {
        OSi_DequeueItem(currentThread, mutex);
        mutex->thread = NULL;
        OS_WakeupThread(&mutex->queue);
    }

    (void)OS_RestoreInterrupts(saved);
    #endif
}

void OSi_UnlockAllMutex (OSThread * thread)
{
    OSMutex * mutex;

    SDK_ASSERT(thread);

#ifndef SDK_THREAD_INFINITY
    while (thread->mutexQueueHead) {
        mutex = OSi_DequeueHead(thread);
        SDK_ASSERT(mutex->thread == thread);

        mutex->count = 0;
        mutex->thread = NULL;
        OS_WakeupThread(&(mutex->queue));
    }
#else
    while (thread->mutexQueue.head) {
        mutex = OSi_RemoveMutexLinkFromQueue(&thread->mutexQueue);

        SDK_ASSERT(mutex->thread == thread);
        mutex->count = 0;
        mutex->thread = NULL;
        OS_WakeupThread(&mutex->queue);

    }
#endif
}

BOOL OS_TryLockMutex (OSMutex * mutex)
{
    OSIntrMode saved;
    OSThread * currentThread;
    BOOL locked;

    SDK_ASSERT(mutex);

    #ifdef SDK_PORT
    locked = (SDL_TryLockMutex(mutex->sdlMutex) == 0);
    #else
    saved = OS_DisableInterrupts();
    currentThread = OS_GetCurrentThread();

    if (mutex->thread == NULL) {
        mutex->thread = currentThread;
        mutex->count++;
        OSi_EnqueueTail(currentThread, mutex);
        locked = TRUE;
    } else if (mutex->thread == currentThread)   {
        mutex->count++;
        locked = TRUE;
    } else {
        locked = FALSE;
    }

    (void)OS_RestoreInterrupts(saved);
    #endif
    return locked;
}

#if SDK_VERSION_MAJOR == 5
void OS_LockMutexR(OSMutex *mutex) {
  OSIntrMode e = OS_DisableInterrupts();
  OSThread *currentThread = OS_GetCurrentThread();

  while (1) {

    if (OS_TryLockMutexR(mutex)) {
      break;
    }

    currentThread->mutex = mutex;
    OS_SleepThread(&mutex->queue);
    currentThread->mutex = NULL;
  }

  (void)OS_RestoreInterrupts(e);
}

void OS_LockMutexW(OSMutex *mutex) {
  OSIntrMode e = OS_DisableInterrupts();
  OSThread *currentThread = OS_GetCurrentThread();

  while (1) {

    if (OS_TryLockMutexW(mutex)) {
      break;
    }

    currentThread->mutex = mutex;
    OS_SleepThread(&mutex->queue);
    currentThread->mutex = NULL;
  }

  (void)OS_RestoreInterrupts(e);
}

BOOL OS_TryLockMutexR(OSMutex *mutex) {
  OSIntrMode e = OS_DisableInterrupts();
  BOOL locked = FALSE;
  OSThread *currentThread = OS_GetCurrentThread();

  switch (OS_GetMutexType(mutex)) {
  case OS_MUTEX_TYPE_NONE:
    mutex->thread = currentThread;
    OS_SetMutexType(mutex, OS_MUTEX_TYPE_R);
    OS_SetMutexCount(mutex, 1);
    OSi_EnqueueTail(currentThread, mutex);
    locked = TRUE;
    break;

  case OS_MUTEX_TYPE_R:
    OS_IncreaseMutexCount(mutex);
    locked = TRUE;
    break;

  case OS_MUTEX_TYPE_W:
  default:
    break;
  }

  (void)OS_RestoreInterrupts(e);
  return locked;
}

BOOL OS_TryLockMutexW(OSMutex *mutex) {
  OSIntrMode e = OS_DisableInterrupts();
  BOOL locked = FALSE;
  OSThread *currentThread = OS_GetCurrentThread();

  switch (OS_GetMutexType(mutex)) {
  case OS_MUTEX_TYPE_NONE:
    mutex->thread = currentThread;
    OS_SetMutexType(mutex, OS_MUTEX_TYPE_W);
    OS_SetMutexCount(mutex, 1);
    OSi_EnqueueTail(currentThread, mutex);
    locked = TRUE;
    break;

  case OS_MUTEX_TYPE_W:
    if (mutex->thread == currentThread) {
      OS_IncreaseMutexCount(mutex);
      locked = TRUE;
    }
    break;

  case OS_MUTEX_TYPE_R:
  default:
    break;
  }

  (void)OS_RestoreInterrupts(e);
  return locked;
}

void OSi_UnlockMutexCore(OSMutex *mutex, u32 type) {
  OSIntrMode e = OS_DisableInterrupts();
  OSThread *currentThread = OS_GetCurrentThread();
  BOOL unlocked = FALSE;

  SDK_ASSERT(mutex);

  if (type != OS_MUTEX_TYPE_NONE && type != OS_GetMutexType(mutex)) {

    OS_TWarning("Illegal unlock mutex");
    (void)OS_RestoreInterrupts(e);
    return;
  }

  switch (OS_GetMutexType(mutex)) {
  case OS_MUTEX_TYPE_STD:
  case OS_MUTEX_TYPE_W:
    if (mutex->thread == currentThread) {
      OS_DecreaseMutexCount(mutex);
      if (OS_GetMutexCount(mutex) == 0) {
        unlocked = TRUE;
      }
    }
    break;

  case OS_MUTEX_TYPE_R:
    OS_DecreaseMutexCount(mutex);
    if (OS_GetMutexCount(mutex) == 0) {
      unlocked = TRUE;
    }
    break;

  default:
    OS_TWarning("Illegal unlock mutex");
    (void)OS_RestoreInterrupts(e);
    return;
  }

  if (unlocked) {
    OSi_DequeueItem(currentThread, mutex);
    mutex->thread = NULL;
    OS_SetMutexType(mutex, OS_MUTEX_TYPE_NONE);
    OS_WakeupThread(&mutex->queue);
  }

  (void)OS_RestoreInterrupts(e);
}

void OS_UnlockMutexR(OSMutex *mutex) {
  OSi_UnlockMutexCore(mutex, OS_MUTEX_TYPE_R);
}

void OS_UnlockMutexW(OSMutex *mutex) {
  OSi_UnlockMutexCore(mutex, OS_MUTEX_TYPE_W);
}

void OS_UnlockMutexRW(OSMutex *mutex) {
  OSi_UnlockMutexCore(mutex, OS_MUTEX_TYPE_NONE);
}

void OS_LockMutexFromRToW(OSMutex *mutex) {
  OSIntrMode e = OS_DisableInterrupts();
  OSThread *currentThread = OS_GetCurrentThread();

  while (1) {
    if (OS_TryLockMutexFromRToW(mutex)) {
      break;
    }

    currentThread->mutex = mutex;
    OS_SleepThread(&mutex->queue);
    currentThread->mutex = NULL;
  }

  (void)OS_RestoreInterrupts(e);
}

BOOL OS_TryLockMutexFromRToW(OSMutex *mutex) {
  OSIntrMode e = OS_DisableInterrupts();
  BOOL locked = FALSE;

  if (OS_GetMutexCount(mutex) == 1 && mutex->queue.head == NULL &&
      OS_GetMutexType(mutex) == OS_MUTEX_TYPE_R) {
    OS_SetMutexType(mutex, OS_MUTEX_TYPE_W);
    locked = TRUE;
  }

  (void)OS_RestoreInterrupts(e);
  return locked;
}

void OS_LockMutexFromWToR(OSMutex *mutex) {
  OSIntrMode e = OS_DisableInterrupts();
  OSThread *currentThread = OS_GetCurrentThread();

  while (1) {
    if (OS_TryLockMutexFromWToR(mutex)) {
      break;
    }

    currentThread->mutex = mutex;
    OS_SleepThread(&mutex->queue);
    currentThread->mutex = NULL;
  }

  (void)OS_RestoreInterrupts(e);
}

BOOL OS_TryLockMutexFromWToR(OSMutex *mutex) {
  OSIntrMode e = OS_DisableInterrupts();
  BOOL locked = FALSE;

  if (OS_GetMutexCount(mutex) == 1 && mutex->queue.head == NULL &&
      OS_GetMutexType(mutex) == OS_MUTEX_TYPE_W) {
    OS_SetMutexType(mutex, OS_MUTEX_TYPE_R);
    locked = TRUE;
  }

  (void)OS_RestoreInterrupts(e);
  return locked;
}
#endif

void OSi_EnqueueTail (OSThread * thread, OSMutex * mutex)
{
#ifndef SDK_THREAD_INFINITY
    OSMutex * prev = thread->mutexQueueTail;

    SDK_ASSERT(thread && mutex);

    if (!prev) {
        thread->mutexQueueHead = mutex;
    } else {
        prev->next = mutex;
    }

    mutex->prev = prev;
    mutex->next = NULL;
    thread->mutexQueueTail = mutex;
#else
    OSMutex * prev = thread->mutexQueue.tail;

    SDK_ASSERT(thread && mutex);

    if (!prev) {
        thread->mutexQueue.head = mutex;
    } else {
        prev->link.next = mutex;
    }

    mutex->link.prev = prev;
    mutex->link.next = NULL;
    thread->mutexQueue.tail = mutex;
#endif
}

void OSi_DequeueItem (OSThread * thread, OSMutex * mutex)
{
#ifndef SDK_THREAD_INFINITY
    OSMutex * next = mutex->next;
    OSMutex * prev = mutex->prev;

    SDK_ASSERT(thread && mutex);

    if (!next) {
        thread->mutexQueueTail = prev;
    } else {
        next->prev = prev;
    }

    if (!prev) {
        thread->mutexQueueHead = next;
    } else {
        prev->next = next;
    }
#else
    OSMutex * next = mutex->link.next;
    OSMutex * prev = mutex->link.prev;

    SDK_ASSERT(thread && mutex);

    if (!next) {
        thread->mutexQueue.tail = prev;
    } else {
        next->link.prev = prev;
    }

    if (!prev) {
        thread->mutexQueue.head = next;
    } else {
        prev->link.next = next;
    }
#endif
}

OSMutex * OSi_DequeueHead (OSThread * thread)
{
#ifndef SDK_THREAD_INFINITY
    OSMutex * mutex = thread->mutexQueueHead;
    OSMutex * next = mutex->next;

    SDK_ASSERT(thread);

    if (!next) {
        thread->mutexQueueTail = NULL;
    } else {
        next->prev = NULL;
    }

    thread->mutexQueueHead = next;
#else
    OSMutex * mutex = thread->mutexQueue.head;
    OSMutex * next = mutex->link.next;

    SDK_ASSERT(thread);

    if (!next) {
        thread->mutexQueue.tail = NULL;
    } else {
        next->link.prev = NULL;
    }

    thread->mutexQueue.head = next;
#endif

    return mutex;
}

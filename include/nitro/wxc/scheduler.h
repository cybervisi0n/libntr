#ifndef NITRO_WXC_SCHEDULER_H_
#define NITRO_WXC_SCHEDULER_H_

#include <nitro.h>

#define WXC_SCHEDULER_PATTERN_MAX 4
#define WXC_SCHEDULER_SEQ_MAX 4

typedef struct WXCScheduler {

  int seq;
  int pattern;
  int start;
  BOOL child_mode;

  BOOL table[WXC_SCHEDULER_PATTERN_MAX][WXC_SCHEDULER_SEQ_MAX];
} WXCScheduler;

#ifdef __cplusplus
extern "C" {
#endif

void WXCi_InitScheduler(WXCScheduler *p);

void WXCi_SetChildMode(WXCScheduler *p, BOOL enable);

BOOL WXCi_UpdateScheduler(WXCScheduler *p);

#ifdef __cplusplus
}
#endif

#endif /* NITRO_WXC_SCHEDULER_H_ */

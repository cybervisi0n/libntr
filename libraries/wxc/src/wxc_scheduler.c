#include <nitro.h>

#include <nitro/wxc/common.h>
#include <nitro/wxc/scheduler.h>

void WXCi_InitScheduler(WXCScheduler *p) {

  static const BOOL default_table[WXC_SCHEDULER_PATTERN_MAX]
                                 [WXC_SCHEDULER_SEQ_MAX] = {
                                     {TRUE, FALSE, TRUE, TRUE},
                                     {FALSE, TRUE, TRUE, TRUE},
                                     {FALSE, TRUE, TRUE, TRUE},
                                     {TRUE, FALSE, TRUE, TRUE},
                                 };
  p->seq = (int)((OS_GetTick() >> 0) % WXC_SCHEDULER_SEQ_MAX);
  p->pattern = (int)((OS_GetTick() >> 2) % WXC_SCHEDULER_PATTERN_MAX);
  p->start = 0;
  p->child_mode = FALSE;
  MI_CpuCopy32(default_table, p->table, sizeof(default_table));
}

void WXCi_SetChildMode(WXCScheduler *p, BOOL enable) { p->child_mode = enable; }

BOOL WXCi_UpdateScheduler(WXCScheduler *p) {
  if (++p->seq >= WXC_SCHEDULER_SEQ_MAX) {
    p->seq = 0;
    if (++p->pattern >= WXC_SCHEDULER_PATTERN_MAX) {
      p->pattern = 0;
    }
    if (p->pattern == p->start) {

      p->start = (int)(OS_GetTick() % WXC_SCHEDULER_PATTERN_MAX);
      p->pattern = p->start;
    }
  }
  return p->table[p->pattern][p->seq] && !p->child_mode;
}

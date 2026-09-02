#ifndef DEMO_KEY_H_
#define DEMO_KEY_H_
#include <nitro.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  u16 trigger;
  u16 press;
} DEMOKeyWork;

extern DEMOKeyWork gKeyWork;
void DEMOReadKey(void);

#define DEMO_IS_TRIG(key) (gKeyWork.trigger & key)
#define DEMO_IS_PRESS(key) (gKeyWork.press & key)

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* DEMO_KEY_H_ */

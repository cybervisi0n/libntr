#ifndef DEMO_KEY_H_
#define DEMO_KEY_H_
#if SDK_VERSION_MAJOR == 4
#include <nitro.h>
#elif SDK_VERSION_MAJOR == 5
#ifdef SDK_TWL
#include <twl.h>
#else
#include <nitro.h>
#endif
#endif

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

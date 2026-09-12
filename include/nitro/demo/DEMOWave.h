#ifndef DEMO_WAVE_H_
#define DEMO_WAVE_H_

#if SDK_VERSION_MAJOR != 5
#error This file is for SDK version 5 only!
#endif

#ifdef __cplusplus
extern "C" {
#endif

int DEMOReadWave(char *dst, const char *filename);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* DEMO_WAVE_H_ */

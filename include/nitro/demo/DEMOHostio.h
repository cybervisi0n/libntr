#ifndef DEMO_HOSTIO_H_
#define DEMO_HOSTIO_H_

#if SDK_VERSION_MAJOR != 5
#error This file is for SDK version 5 only!
#endif

#ifdef __cplusplus
extern "C" {
#endif

void DEMOMountHostIO(const char *basepath);

#ifdef __cplusplus
}/* extern "C" */
#endif
#endif

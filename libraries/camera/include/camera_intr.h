#ifndef TWL_CAMERA_INTR_H_
#define TWL_CAMERA_INTR_H_

#include <twl/camera.h>

#ifdef __cplusplus
extern "C" {
#endif

void CAMERA_SetMasterInterruptCore(BOOL enabled);
BOOL CAMERA_GetMasterInterruptCore(void);
void CAMERA_SetVsyncInterruptCore(CAMERAIntrVsync type);
CAMERAIntrVsync CAMERA_GetVsyncInterruptCore(void);
void CAMERA_SetBufferErrorInterruptCore(BOOL enabled);
BOOL CAMERA_GetBufferErrorInterruptCore(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TWL_CAMERA_INTR_H_ */

#ifndef TWL_CAMERA_TRANSFER_H_
#define TWL_CAMERA_TRANSFER_H_

#include <twl/types.h>
#include <twl/mi/common/dma.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline void CAMERA_DmaRecv(u32 dmaNo, void *dest, u32 unit, u32 length) {
  MI_Camera_NDmaRecv(dmaNo, dest, unit / 4, length, FALSE);
}

static inline void CAMERA_DmaRecvAsync(u32 dmaNo, void *dest, u32 unit,
                                       u32 length, MINDmaCallback callback,
                                       void *arg) {
  MI_Camera_NDmaRecvAsync(dmaNo, dest, unit / 4, length, FALSE, callback, arg);
}

static inline void CAMERA_DmaPipeInfinity(u32 dmaNo, void *dest, u32 unit,
                                          MINDmaCallback callback, void *arg) {

  MI_Camera_NDmaRecvAsync(dmaNo, dest, unit / 4, unit, TRUE, callback, arg);
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TWL_CAMERA_TRANSFER_H_ */

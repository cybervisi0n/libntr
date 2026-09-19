#ifndef TWL_LIBRARIES_SPI_ARM9_MICEX_H_
#define TWL_LIBRARIES_SPI_ARM9_MICEX_H_
#ifdef __cplusplus
extern "C" {
#endif

#if SDK_VERSION_MAJOR != 5
#error This file is for SDK 5 only!
#endif

#include <nitro/spi/ARM9/mic.h>

typedef enum MICLock {
  MIC_LOCK_OFF = 0, // Unlock status
  MIC_LOCK_ON,      // Lock status
  MIC_LOCK_MAX
} MICLock;

typedef struct MICWork {
  MICLock lock;           // Exclusive lock
  MICCallback callback;   // For saving an asynchronous callback function
  void *callbackArg;      // For saving arguments to the callback function
  MICResult commonResult; // For saving asynchronous function processing results
  MICCallback full;       // For saving the sampling completion callback
  void *fullArg; // For saving arguments to the completion callback function
  void *dst_buf; // For saving a storage area for individual sampling results

} MICWork;

MICWork *MICi_GetSysWork(void);

MICResult MICEXi_StartLimitedSampling(const MICAutoParam *param);
MICResult MICEXi_StartLimitedSamplingAsync(const MICAutoParam *param,
                                           MICCallback callback, void *arg);
MICResult MICEXi_StopLimitedSampling(void);
MICResult MICEXi_StopLimitedSamplingAsync(MICCallback callback, void *arg);
MICResult MICEXi_AdjustLimitedSampling(u32 rate);
MICResult MICEXi_AdjustLimitedSamplingAsync(u32 rate, MICCallback callback,
                                            void *arg);

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* TWL_LIBRARIES_SPI_ARM9_MICEX_H_ */

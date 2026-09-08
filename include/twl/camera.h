#ifndef TWL_CAMERA_H_
#define TWL_CAMERA_H_

#include <twl/camera/common/fifo.h>
#include <twl/camera/common/types.h>
#include <twl/camera/common/util.h>

#ifdef SDK_ARM7

#include <twl/camera/ARM7/i2c.h>
#include <twl/camera/ARM7/control.h>

#else

#include <twl/camera/ARM9/camera.h>
#include <twl/camera/ARM9/camera_api.h>
#include <twl/camera/ARM9/transfer.h>

#endif

#endif

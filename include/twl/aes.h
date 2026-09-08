#ifndef TWL_AES_H_
#define TWL_AES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <twl/aes/common/types.h>

#ifdef SDK_ARM9
#include <twl/aes/ARM9/aes_inline.h>
#include <twl/aes/ARM9/util.h>
#else // ifdef SDK_ARM9
#include <twl/aes/ARM7/hi.h>
#endif // ifdef SDK_ARM9 else

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TWL_AES_H_ */

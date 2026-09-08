#ifndef TWL_RTC_H_
#define TWL_RTC_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/rtc.h>
#include <twl/rtc/common/type_ex.h>
#include <twl/rtc/common/fifo_ex.h>

#ifdef SDK_ARM7
#include <twl/rtc/ARM7/instruction_ex.h>

#else /* SDK_ARM9 */
#include <twl/rtc/ARM9/api_ex.h>

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* TWL_RTC_H_ */

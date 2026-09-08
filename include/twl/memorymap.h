#ifndef TWL_MEMORYMAP_H_
#define TWL_MEMORYMAP_H_
#ifdef __cplusplus
extern "C" {
#endif

#ifdef SDK_TWL

#ifdef SDK_PORT
#include <twl/hw/X86/mmap_global.h>
#include <twl/hw/X86/mmap_main.h>
#include <twl/hw/X86/mmap_tcm.h>
#include <twl/hw/X86/mmap_vram.h>
#include <twl/hw/common/mmap_shared.h>
#include <twl/hw/common/mmap_parameter.h>
#include <twl/hw/X86/ioreg.h>
#else
#ifdef SDK_ARM9
#include <twl/hw/ARM9/mmap_global.h>
#include <twl/hw/ARM9/mmap_main.h>
#include <twl/hw/ARM9/mmap_tcm.h>
#include <twl/hw/ARM9/mmap_vram.h>
#include <twl/hw/common/mmap_shared.h>
#include <twl/hw/common/mmap_parameter.h>
#include <twl/hw/ARM9/ioreg.h>

#else /* SDK_ARM7 */
#include <twl/hw/ARM7/mmap_global.h>
#include <twl/hw/ARM7/mmap_main.h>
#include <twl/hw/ARM7/mmap_wram.h>
#include <twl/hw/common/mmap_shared.h>
#include <twl/hw/common/mmap_parameter.h>
#include <twl/hw/ARM7/ioreg.h>
#endif
#endif

#endif // SDK_TWL

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* TWL_MEMORYMAP_H_ */

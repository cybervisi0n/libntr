#ifndef TWL_HW_ARM9_MMAP_TCM_H_
#define TWL_HW_ARM9_MMAP_TCM_H_
#ifdef __cplusplus
extern "C" {
#endif

#define HW_SVC_STACK_SIZE 0x40 // 64 bytes

#define HW_DTCM_SYSRV_SIZE 0x40 // 64 bytes

#ifndef SDK_ASM
#include <nitro/types.h>
extern u32 SDK_IRQ_STACKSIZE[];
#define HW_DTCM_IRQ_STACK_SIZE ((u32)SDK_IRQ_STACKSIZE)
#else
.extern SDK_IRQ_STACKSIZE
#define HW_DTCM_IRQ_STACK_SIZE SDK_IRQ_STACKSIZE
#endif

#define HW_SYS_AND_IRQ_STACK_SIZE_MAX                                          \
  (HW_DTCM_SIZE - HW_SVC_STACK_SIZE - HW_DTCM_SYSRV_SIZE)

#ifndef SDK_ASM
#include <nitro/types.h>

typedef volatile struct {

  u8 sys_and_irq_stack[HW_SYS_AND_IRQ_STACK_SIZE_MAX]; // 0000-3f80 system & irq
                                                       // stack
  u8 svc_stack[HW_SVC_STACK_SIZE];                     // 3f80-3fbf svc stack

  u8 reserved[HW_DTCM_SYSRV_SIZE - 8]; // 3fc0-3ff7 ????
  u32 intr_check;                      // 3ff8-3ffb intr_check for svc
  void *intr_vector;                   // 3ffc-3fff intr handler

} OS_DTCM;
#endif

#define HW_DTCM_SYS_STACK_DEFAULT HW_DTCM
#define HW_DTCM_SYS_STACK_DEFAULT_END HW_DTCM_IRQ_STACK
#define HW_DTCM_IRQ_STACK (HW_DTCM_IRQ_STACK_END - HW_DTCM_IRQ_STACK_SIZE)
#define HW_DTCM_IRQ_STACK_END HW_DTCM_SVC_STACK
#define HW_DTCM_SVC_STACK (HW_DTCM_SVC_STACK_END - HW_SVC_STACK_SIZE)
#define HW_DTCM_SVC_STACK_END HW_DTCM_SYSRV

#define HW_DTCM_SYSRV (HW_DTCM + HW_DTCM_SIZE - HW_DTCM_SYSRV_SIZE)
#define HW_INTR_CHECK_BUF (HW_DTCM_SYSRV + HW_DTCM_SYSRV_OFS_INTR_CHECK)
#define HW_INTR_VECTOR_BUF (HW_DTCM_SYSRV + HW_DTCM_SYSRV_OFS_INTR_VECTOR)

#define HW_DTCM_SYSRV_OFS_DEBUGGER 0x00
#define HW_DTCM_SYSRV_OFS_RESERVED2 0x1c
#define HW_DTCM_SYSRV_OFS_RESERVED 0x20
#define HW_DTCM_SYSRV_OFS_INTR_CHECK 0x38
#define HW_DTCM_SYSRV_OFS_INTR_VECTOR 0x3c

#define HW_DTCM_ARENA_LO_DEFAULT HW_DTCM
#define HW_DTCM_ARENA_HI_DEFAULT HW_DTCM

#define HW_ITCM_ARENA_LO_DEFAULT HW_ITCM
#define HW_ITCM_ARENA_HI_DEFAULT HW_ITCM_END

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* TWL_HW_ARM9_MMAP_TCM_H_ */

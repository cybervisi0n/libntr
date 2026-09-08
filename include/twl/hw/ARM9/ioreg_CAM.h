#ifndef TWL_HW_ARM9_IOREG_CAM_H_
#define TWL_HW_ARM9_IOREG_CAM_H_

#ifndef SDK_ASM
#include <nitro/types.h>
#include <twl/hw/ARM9/mmap_global.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define REG_MCNT_OFFSET 0x4200
#define REG_MCNT_ADDR (HW_REG_BASE + REG_MCNT_OFFSET)
#define reg_CAM_MCNT (*(REGType16v *)REG_MCNT_ADDR)

#define REG_CNT_OFFSET 0x4202
#define REG_CNT_ADDR (HW_REG_BASE + REG_CNT_OFFSET)
#define reg_CAM_CNT (*(REGType16v *)REG_CNT_ADDR)

#define REG_DAT_OFFSET 0x4204
#define REG_DAT_ADDR (HW_REG_BASE + REG_DAT_OFFSET)
#define reg_CAM_DAT (*(const REGType32v *)REG_DAT_ADDR)

#define REG_SOFS_OFFSET 0x4210
#define REG_SOFS_ADDR (HW_REG_BASE + REG_SOFS_OFFSET)
#define reg_CAM_SOFS (*(REGType32v *)REG_SOFS_ADDR)

#define REG_EOFS_OFFSET 0x4214
#define REG_EOFS_ADDR (HW_REG_BASE + REG_EOFS_OFFSET)
#define reg_CAM_EOFS (*(REGType32v *)REG_EOFS_ADDR)

#define REG_CAM_MCNT_INI_SHIFT 7
#define REG_CAM_MCNT_INI_SIZE 1
#define REG_CAM_MCNT_INI_MASK 0x0080

#define REG_CAM_MCNT_V28_SHIFT 6
#define REG_CAM_MCNT_V28_SIZE 1
#define REG_CAM_MCNT_V28_MASK 0x0040

#define REG_CAM_MCNT_VIO_SHIFT 5
#define REG_CAM_MCNT_VIO_SIZE 1
#define REG_CAM_MCNT_VIO_MASK 0x0020

#define REG_CAM_MCNT_V18_SHIFT 4
#define REG_CAM_MCNT_V18_SIZE 1
#define REG_CAM_MCNT_V18_MASK 0x0010

#define REG_CAM_MCNT_IRCLK_SHIFT 3
#define REG_CAM_MCNT_IRCLK_SIZE 1
#define REG_CAM_MCNT_IRCLK_MASK 0x0008

#define REG_CAM_MCNT_SYNC_SHIFT 2
#define REG_CAM_MCNT_SYNC_SIZE 1
#define REG_CAM_MCNT_SYNC_MASK 0x0004

#define REG_CAM_MCNT_RST_SHIFT 1
#define REG_CAM_MCNT_RST_SIZE 1
#define REG_CAM_MCNT_RST_MASK 0x0002

#define REG_CAM_MCNT_STBY_SHIFT 0
#define REG_CAM_MCNT_STBY_SIZE 1
#define REG_CAM_MCNT_STBY_MASK 0x0001

#ifndef SDK_ASM
#define REG_CAM_MCNT_FIELD(ini, v28, vio, v18, irclk, sync, rst, stby)         \
  (u16)(((u32)(ini) << REG_CAM_MCNT_INI_SHIFT) |                               \
        ((u32)(v28) << REG_CAM_MCNT_V28_SHIFT) |                               \
        ((u32)(vio) << REG_CAM_MCNT_VIO_SHIFT) |                               \
        ((u32)(v18) << REG_CAM_MCNT_V18_SHIFT) |                               \
        ((u32)(irclk) << REG_CAM_MCNT_IRCLK_SHIFT) |                           \
        ((u32)(sync) << REG_CAM_MCNT_SYNC_SHIFT) |                             \
        ((u32)(rst) << REG_CAM_MCNT_RST_SHIFT) |                               \
        ((u32)(stby) << REG_CAM_MCNT_STBY_SHIFT))
#endif

#define REG_CAM_CNT_E_SHIFT 15
#define REG_CAM_CNT_E_SIZE 1
#define REG_CAM_CNT_E_MASK 0x8000

#define REG_CAM_CNT_T_SHIFT 14
#define REG_CAM_CNT_T_SIZE 1
#define REG_CAM_CNT_T_MASK 0x4000

#define REG_CAM_CNT_F_SHIFT 13
#define REG_CAM_CNT_F_SIZE 1
#define REG_CAM_CNT_F_MASK 0x2000

#define REG_CAM_CNT_IREQI_SHIFT 11
#define REG_CAM_CNT_IREQI_SIZE 1
#define REG_CAM_CNT_IREQI_MASK 0x0800

#define REG_CAM_CNT_IREQBE_SHIFT 10
#define REG_CAM_CNT_IREQBE_SIZE 1
#define REG_CAM_CNT_IREQBE_MASK 0x0400

#define REG_CAM_CNT_IREQVS_SHIFT 8
#define REG_CAM_CNT_IREQVS_SIZE 2
#define REG_CAM_CNT_IREQVS_MASK 0x0300

#define REG_CAM_CNT_CL_SHIFT 5
#define REG_CAM_CNT_CL_SIZE 1
#define REG_CAM_CNT_CL_MASK 0x0020

#define REG_CAM_CNT_ERR_SHIFT 4
#define REG_CAM_CNT_ERR_SIZE 1
#define REG_CAM_CNT_ERR_MASK 0x0010

#define REG_CAM_CNT_TL_SHIFT 0
#define REG_CAM_CNT_TL_SIZE 4
#define REG_CAM_CNT_TL_MASK 0x000f

#ifndef SDK_ASM
#define REG_CAM_CNT_FIELD(e, t, f, ireqi, ireqbe, ireqvs, cl, err, tl)         \
  (u16)(((u32)(e) << REG_CAM_CNT_E_SHIFT) |                                    \
        ((u32)(t) << REG_CAM_CNT_T_SHIFT) |                                    \
        ((u32)(f) << REG_CAM_CNT_F_SHIFT) |                                    \
        ((u32)(ireqi) << REG_CAM_CNT_IREQI_SHIFT) |                            \
        ((u32)(ireqbe) << REG_CAM_CNT_IREQBE_SHIFT) |                          \
        ((u32)(ireqvs) << REG_CAM_CNT_IREQVS_SHIFT) |                          \
        ((u32)(cl) << REG_CAM_CNT_CL_SHIFT) |                                  \
        ((u32)(err) << REG_CAM_CNT_ERR_SHIFT) |                                \
        ((u32)(tl) << REG_CAM_CNT_TL_SHIFT))
#endif

#define REG_CAM_DAT_DATA_SHIFT 0
#define REG_CAM_DAT_DATA_SIZE 32
#define REG_CAM_DAT_DATA_MASK 0xffffffff

#ifndef SDK_ASM
#define REG_CAM_DAT_FIELD(data) (u32)(((u32)(data) << REG_CAM_DAT_DATA_SHIFT))
#endif

#define REG_CAM_SOFS_VOFS_SHIFT 16
#define REG_CAM_SOFS_VOFS_SIZE 9
#define REG_CAM_SOFS_VOFS_MASK 0x01ff0000

#define REG_CAM_SOFS_HOFS_SHIFT 0
#define REG_CAM_SOFS_HOFS_SIZE 10
#define REG_CAM_SOFS_HOFS_MASK 0x000003ff

#ifndef SDK_ASM
#define REG_CAM_SOFS_FIELD(vofs, hofs)                                         \
  (u32)(((u32)(vofs) << REG_CAM_SOFS_VOFS_SHIFT) |                             \
        ((u32)(hofs) << REG_CAM_SOFS_HOFS_SHIFT))
#endif

#define REG_CAM_EOFS_VOFS_SHIFT 16
#define REG_CAM_EOFS_VOFS_SIZE 9
#define REG_CAM_EOFS_VOFS_MASK 0x01ff0000

#define REG_CAM_EOFS_HOFS_SHIFT 0
#define REG_CAM_EOFS_HOFS_SIZE 10
#define REG_CAM_EOFS_HOFS_MASK 0x000003ff

#ifndef SDK_ASM
#define REG_CAM_EOFS_FIELD(vofs, hofs)                                         \
  (u32)(((u32)(vofs) << REG_CAM_EOFS_VOFS_SHIFT) |                             \
        ((u32)(hofs) << REG_CAM_EOFS_HOFS_SHIFT))
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

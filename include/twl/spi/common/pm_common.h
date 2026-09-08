#ifndef TWL_PM_COMMON_H_
#define TWL_PM_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

#define REG_PMIC_CTL2_ADDR 0x10 // R/W

#define PMIC_CTL2_RESET (1 << 0)
#define PMIC_CTL2_FREE (1 << 1)
#define PMIC_CTL2_BACK_LIGHT_1 (1 << 2)
#define PMIC_CTL2_BACK_LIGHT_2 (1 << 3)

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

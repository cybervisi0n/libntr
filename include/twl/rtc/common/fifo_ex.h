#ifndef TWL_RTC_COMMON_FIFO_EX_H_
#define TWL_RTC_COMMON_FIFO_EX_H_
#ifdef __cplusplus
extern "C" {
#endif

#define RTC_PXI_COMMAND_READ_COUNTER 0x50 /* Read the up counter */
#define RTC_PXI_COMMAND_READ_FOUT 0x51    /* Read the FOUT setting value */
#define RTC_PXI_COMMAND_READ_ALARM_EX1                                         \
  0x52 /* Read the Alarm 1 setting value (extended version) */
#define RTC_PXI_COMMAND_READ_ALARM_EX2                                         \
  0x53 /* Read the Alarm 2 setting value (extended version) */
#define RTC_PXI_COMMAND_WRITE_FOUT 0x61 /* Write the FOUT setting value */
#define RTC_PXI_COMMAND_WRITE_ALARM_EX1                                        \
  0x62 /* Write the Alarm 1 setting value (extended version) */
#define RTC_PXI_COMMAND_WRITE_ALARM_EX2                                        \
  0x63 /* Write the Alarm 2 setting value (extended version) */

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* TWL_RTC_COMMON_FIFO_EX_H_ */

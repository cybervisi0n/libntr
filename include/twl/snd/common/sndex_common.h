#ifndef TWL_SND_COMMON_SNDEX_COMMON_H_
#define TWL_SND_COMMON_SNDEX_COMMON_H_
#ifdef __cplusplus
extern "C" {
#endif

#define SNDEX_PXI_COMMAND_MASK 0x00ff0000
#define SNDEX_PXI_COMMAND_SHIFT 16
#define SNDEX_PXI_RESULT_MASK 0x0000ff00
#define SNDEX_PXI_RESULT_SHIFT 8
#define SNDEX_PXI_PARAMETER_MASK 0x000000ff
#define SNDEX_PXI_PARAMETER_SHIFT 0
#define SNDEX_PXI_PARAMETER_MASK_IIR 0x0000ffff
#define SNDEX_PXI_PARAMETER_SHIFT_IIR 0

#define SNDEX_PXI_PARAMETER_MASK_VOL_VALUE 0x1f // Speaker volume
#define SNDEX_PXI_PARAMETER_MASK_VOL_KEEP                                      \
  0x80 // Maintained value for pending speaker volume
#define SNDEX_PXI_PARAMETER_SHIFT_VOL_KEEP 7
#define SNDEX_PXI_PARAMETER_MASK_VOL_8LV 0x40 // Speaker volume (8 levels)
#define SNDEX_PXI_PARAMETER_SHIFT_VOL_8LV 6

#define SNDEX_PXI_COMMAND_GET_SMIX_MUTE 0x01 // Get the mute state
#define SNDEX_PXI_COMMAND_GET_SMIX_FREQ 0x02 // Get the I2S frequency
#define SNDEX_PXI_COMMAND_GET_SMIX_DSP 0x03  // Get the CPU/DSP output mix rate
#define SNDEX_PXI_COMMAND_GET_VOLUME 0x04    // Get the volume
#define SNDEX_PXI_COMMAND_GET_SND_DEVICE                                       \
  0x05 // Get the setting for the audio output device
#define SNDEX_PXI_COMMAND_SET_SMIX_MUTE 0x81 // Change the mute state
#define SNDEX_PXI_COMMAND_SET_SMIX_FREQ 0x82 // Change the I2S frequency
#define SNDEX_PXI_COMMAND_SET_SMIX_DSP                                         \
  0x83 // Change the CPU/DSP output mix rate
#define SNDEX_PXI_COMMAND_SET_VOLUME                                           \
  0x84 // Change the volume (by reservation; do not change immediately)
#define SNDEX_PXI_COMMAND_SET_SND_DEVICE                                       \
  0x85 // Change the setting for the audio output device
#define SNDEX_PXI_COMMAND_SET_VOLUME_ACTUALLY 0x86 // Actually change the volume
#define SNDEX_PXI_COMMAND_SET_IIRFILTER_TARGET                                 \
  0xc1 // Notification indicating where to apply IIR filters
#define SNDEX_PXI_COMMAND_SET_IIRFILTER_PARAM_N0                               \
  0xc2 // Notification for IIR filter parameters
#define SNDEX_PXI_COMMAND_SET_IIRFILTER_PARAM_N1                               \
  0xc3 // Notification for IIR filter parameters
#define SNDEX_PXI_COMMAND_SET_IIRFILTER_PARAM_N2                               \
  0xc4 // Notification for IIR filter parameters
#define SNDEX_PXI_COMMAND_SET_IIRFILTER_PARAM_D1                               \
  0xc5 // Notification for IIR filter parameters
#define SNDEX_PXI_COMMAND_SET_IIRFILTER_PARAM_D2                               \
  0xc6 // Notification for IIR filter parameters
#define SNDEX_PXI_COMMAND_SET_IIRFILTER 0xc7 // IIR filter registration
#define SNDEX_PXI_COMMAND_PRE_PROC_SHUTTER                                     \
  0xfb // Pre-processing for shutter sound playback
#define SNDEX_PXI_COMMAND_POST_PROC_SHUTTER                                    \
  0xfc // Post-processing for shutter sound playback
#define SNDEX_PXI_COMMAND_PRESS_VOLSWITCH                                      \
  0xfd // Notification for a volume control press
#define SNDEX_PXI_COMMAND_HP_CONNECT                                           \
  0xfe // Whether there are headphones connected

#define SNDEX_PXI_RESULT_SUCCESS 0x00         // Success
#define SNDEX_PXI_RESULT_INVALID_COMMAND 0x01 // Unknown command
#define SNDEX_PXI_RESULT_INVALID_PARAM 0x02   // Unknown parameter
#define SNDEX_PXI_RESULT_EXCLUSIVE 0x03       // Mutex in effect
#define SNDEX_PXI_RESULT_ILLEGAL_STATE                                         \
  0x04 // Unprocessable state (other than mutex)
#define SNDEX_PXI_RESULT_DEVICE_ERROR 0x05 // A device operation failed
#define SNDEX_PXI_RESULT_FATAL_ERROR 0xff  // Fatal error that should not occur

#define SNDEX_SMIX_MUTE_OFF 0x00 // Audible
#define SNDEX_SMIX_MUTE_ON 0x01  // Muted

#define SNDEX_SMIX_FREQ_32730 0x00 // 32.73 kHz
#define SNDEX_SMIX_FREQ_47610 0x01 // 47.61 kHz

#define SNDEX_MCU_DEVICE_AUTO                                                  \
  0x00 // Automatically select the output target according to the state
#define SNDEX_MCU_DEVICE_SPEAKER 0x01   // Output only to the speakers
#define SNDEX_MCU_DEVICE_HEADPHONE 0x02 // Output only to the headphones
#define SNDEX_MCU_DEVICE_BOTH 0x03 // Output to both the speakers and headphones

#define SNDEX_FORCEOUT_OFF 0x00 // Normal setting
#define SNDEX_FORCEOUT_CAMERA_SHUTTER_ON                                       \
  0x01 // State with forced output when the camera takes a photo

#ifdef __cplusplus
} // extern "C"
#endif
#endif // TWL_SND_COMMON_SNDEX_COMMON_H_

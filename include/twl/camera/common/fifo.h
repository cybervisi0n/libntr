#ifndef TWL_CAMERA_FIFO_H_
#define TWL_CAMERA_FIFO_H_

#define CAMERA_PXI_CONTINUOUS_PACKET_MAX                                       \
  4 // Maximum number of consecutive packets in succession
#define CAMERA_PXI_DATA_SIZE_MAX                                               \
  ((CAMERA_PXI_CONTINUOUS_PACKET_MAX - 1) * 3 + 1) // Maximum data count

#define CAMERA_PXI_START_BIT 0x02000000  // Starting packet
#define CAMERA_PXI_RESULT_BIT 0x00008000 // PXI response

#define CAMERA_PXI_DATA_NUMS_MASK 0x00ff0000 // Data count region
#define CAMERA_PXI_DATA_NUMS_SHIFT 16        // Data count position
#define CAMERA_PXI_COMMAND_MASK 0x00007f00   // Mask for command storage portion
#define CAMERA_PXI_COMMAND_SHIFT 8 // Position of command storage portion
#define CAMERA_PXI_1ST_DATA_MASK                                               \
  0x000000ff                        // Data region for the starting packet
#define CAMERA_PXI_1ST_DATA_SHIFT 0 // Data position for the starting packet

#define CAMERA_PXI_DATA_MASK 0x00ffffff // Data region
#define CAMERA_PXI_DATA_SHIFT 0         // Data position

#ifdef __cplusplus
extern "C" {
#endif

typedef enum CAMERAPxiCommand {
  CAMERA_PXI_COMMAND_INIT = 0x00, // Initialization

  CAMERA_PXI_COMMAND_ACTIVATE = 0x10, // Active selection
#if 0 /* Held back because it is not very useful */
    CAMERA_PXI_COMMAND_OUTPUT_WITH_DUAL_ACTIVATION  = 0x11, // Output selection (both active)
#endif
  CAMERA_PXI_COMMAND_CONTEXT_SWITCH = 0x12, // Context switch

  CAMERA_PXI_COMMAND_SIZE = 0x30,          // Set size
  CAMERA_PXI_COMMAND_FRAME_RATE = 0x31,    // Frame rate settings
  CAMERA_PXI_COMMAND_EFFECT = 0x32,        // Effect settings
  CAMERA_PXI_COMMAND_FLIP = 0x33,          // Rotation settings
  CAMERA_PXI_COMMAND_PHOTO_MODE = 0x34,    // Photo mode
  CAMERA_PXI_COMMAND_WHITE_BALANCE = 0x35, // White balance
  CAMERA_PXI_COMMAND_EXPOSURE = 0x36,      // Exposure
  CAMERA_PXI_COMMAND_SHARPNESS = 0x37,     // Sharpness
  CAMERA_PXI_COMMAND_TEST_PATTERN = 0x38,  // Test pattern
  CAMERA_PXI_COMMAND_AUTO_EXPOSURE = 0x39, // Auto-exposure on/off
  CAMERA_PXI_COMMAND_AUTO_WHITE_BALANCE =
      0x3A, // Automatic white balance on/off

  CAMERA_PXI_COMMAND_SET_LED = 0x40, // Camera indicator LED

  CAMERA_PXI_COMMAND_UNKNOWN
} CAMERAPxiCommand;

typedef enum CAMERAPxiSize {
  CAMERA_PXI_SIZE_INIT = 1, // Camera

  CAMERA_PXI_SIZE_ACTIVATE = 1, // Camera
#if 0                           /* Held back because it is not very useful */
    CAMERA_PXI_SIZE_OUTPUT_WITH_DUAL_ACTIVATION     = 1,    // Camera
#endif
  CAMERA_PXI_SIZE_CONTEXT_SWITCH = 2, // Camera, context

  CAMERA_PXI_SIZE_SIZE = 3,               // Camera, context, size
  CAMERA_PXI_SIZE_FRAME_RATE = 2,         // Camera, rate
  CAMERA_PXI_SIZE_EFFECT = 3,             // Camera, context, effect
  CAMERA_PXI_SIZE_FLIP = 3,               // Camera, context, flip
  CAMERA_PXI_SIZE_PHOTO_MODE = 2,         // Camera, mode
  CAMERA_PXI_SIZE_WHITE_BALANCE = 2,      // Camera, white balance
  CAMERA_PXI_SIZE_EXPOSURE = 2,           // Camera, exposure
  CAMERA_PXI_SIZE_SHARPNESS = 2,          // Camera, sharpness
  CAMERA_PXI_SIZE_TEST_PATTERN = 2,       // Camera, pattern
  CAMERA_PXI_SIZE_AUTO_EXPOSURE = 2,      // Camera, on
  CAMERA_PXI_SIZE_AUTO_WHITE_BALANCE = 2, // Camera, on

  CAMERA_PXI_SIZE_SET_LED = 1, // isBlink

  CAMERA_PXI_SIZE_UNKNOWN
} CAMERAPxiSize;

typedef enum CAMERAPxiResult {
  CAMERA_PXI_RESULT_SUCCESS = 0, // Processing success (void/void*) // There are
                                 // sometimes subsequent packets
  CAMERA_PXI_RESULT_SUCCESS_TRUE = 0,  // Processing success (Boolean)
  CAMERA_PXI_RESULT_SUCCESS_FALSE,     // Processing failure (Boolean)
  CAMERA_PXI_RESULT_INVALID_COMMAND,   // Invalid PXI command
  CAMERA_PXI_RESULT_INVALID_PARAMETER, // Illegal parameter
  CAMERA_PXI_RESULT_ILLEGAL_STATUS, // Processing cannot be run because of the
                                    // camera state
  CAMERA_PXI_RESULT_BUSY,           // Another request is being processed
  CAMERA_PXI_RESULT_FATAL_ERROR, // Some other error caused processing to fail
  CAMERA_PXI_RESULT_MAX
} CAMERAPxiResult;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TWL_CAMERA_FIFO_H_ */

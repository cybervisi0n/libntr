#ifndef SIM_CONFIG_H
#define SIM_CONFIG_H

#include <nitro.h>

typedef enum {
    SIM_CONFIG_SCREEN_LAYOUT_VERTICAL, /* Vertical screen layout (like a real DS) */
    SIM_CONFIG_SCREEN_LAYOUT_HORIZONTAL, /* DS screens side by side */
    SIM_CONFIG_SCREEN_LAYOUT_LARGE /* One large screen and one small screen next to it */
} SIM_config_screen_layout_type;

#define SIM_CONFIG_JOY_HAT_MASK (0x10000)
#define SIM_CONFIG_JOY_AXIS_PLUS_MASK (0x20000)
#define SIM_CONFIG_JOY_AXIS_MINUS_MASK (0x40000)
#define SIM_CONFIG_JOY_INVALID_KEY (-1)

typedef struct {
    int aKey;
    int bKey;
    int xKey;
    int yKey;
    int upKey;
    int downKey;
    int leftKey;
    int rightKey;
    int lKey;
    int rKey;
    int startKey;
    int selectKey;
    int guiKey;

    // Joystick keys
    int joyAxisDeadzone;
    int aJoyKey;
    int bJoyKey;
    int xJoyKey;
    int yJoyKey;
    int upJoyKey;
    int downJoyKey;
    int leftJoyKey;
    int rightJoyKey;
    int lJoyKey;
    int rJoyKey;
    int startJoyKey;
    int selectJoyKey;
} SIM_config_pad_settings_type;

typedef struct {
    char serverIp[16]; /* IP of the last server connected to through Net */
} SIM_config_net_settings_type;

typedef struct {
    u32 internalResolutionScale; /* 3D rendering internal resolution scale factor */
    u32 windowWidth; /* Initial width of the window. If set to 0, will use the default width for the screen layout */
    u32 windowHeight; /* Initial height of the window. If set to 0, will use default height for the screen layout */
    SIM_config_screen_layout_type screenLayout; /* DS screen layout */
    BOOL swapScreens; /* Swap screen positions */
    u8 vsyncInterval; /* Vertical sync interval. Set to 0 to disable vsync, 1 every vblank, 2 every second vblank etc*/
    u8 capFrameRate; /* Should the frame rate be capped to 60 fps? */
    u8 macAddr[6]; /* MAC address */
    SIM_config_pad_settings_type padSettings; /* Input pad settings */
    NVRAMConfig osNvramConfig; /* OS NVRAM config */
    SIM_config_net_settings_type netSettings; /* Net Settings */
} SIM_config_type;

BOOL SIM_Config_LoadConfigFile(SIM_config_type * aConfig);
void SIM_Config_LoadDefaults(SIM_config_type * aConfig);
void SIM_Config_SaveConfigFile(SIM_config_type * aConfig);
void SIM_Config_SetOSNVRAMConfig(SIM_config_type * aConfig);

#endif
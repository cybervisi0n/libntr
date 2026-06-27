#include <SDL2/SDL_keycode.h>
#include <nitro.h>
#include <simulator/sim.h>
#include <simulator/config/ini.h>
#include <simulator/config/sim_config.h>

#include <string.h>
#include <wchar.h>

#include <SDL2/SDL.h>

#include <stdlib.h>

#define SIM_CONFIG_FILE_PATH "./sim_config.ini"

static int IniHandler(void* user, const char* section, const char* name,
                   const char* value);
static const char * BoolToString(BOOL arg);
static BOOL StringToBool(const char * str);
static const char * ScreenLayoutTypeToString(SIM_config_screen_layout_type layout);
static SIM_config_screen_layout_type StringToScreenLayoutType(const char * str);


static int IniHandler(void* user, const char* section, const char* name,
                   const char* value)
{
    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

    SIM_config_type * config = (SIM_config_type *)user;

    // General section
    if(MATCH("General", "InternalResolutionScale")) {
        config->internalResolutionScale = atoi(value);
    }
    if(MATCH("General", "WindowHeight")) {
        config->windowHeight = atoi(value);
    }
    if(MATCH("General", "WindowWidth")) {
        config->windowWidth = atoi(value);
    }
    if(MATCH("General", "ScreenLayout")) {
        config->screenLayout = StringToScreenLayoutType(value);
    }
    if(MATCH("General", "SwapScreens")) {
        config->swapScreens = StringToBool(value);
    }
    if(MATCH("General", "VsyncInterval")) {
        config->vsyncInterval = atoi(value);
    }
    if(MATCH("General", "CapFrameRate")) {
        config->capFrameRate = StringToBool(value);
    }

    if(MATCH("General", "MacAddr")) {
        char * pos = value;
        if(strlen(value) >= 12) {
            for (size_t count = 0; count < 6; count++) {
                sscanf(pos, "%2hhx", &config->macAddr[count]);
                pos += 2;
            }
        }
    }

    // PAD
    if(MATCH("PAD", "JoyAxisDeadzone")) {
        config->padSettings.joyAxisDeadzone = atoi(value);
    }
    if(MATCH("PAD", "AKey")) {
        config->padSettings.aKey = atoi(value);
    }
    if(MATCH("PAD", "BKey")) {
        config->padSettings.bKey = atoi(value);
    }
    if(MATCH("PAD", "XKey")) {
        config->padSettings.xKey = atoi(value);
    }
    if(MATCH("PAD", "YKey")) {
        config->padSettings.yKey = atoi(value);
    }
    if(MATCH("PAD", "UpKey")) {
        config->padSettings.upKey = atoi(value);
    }
    if(MATCH("PAD", "DownKey")) {
        config->padSettings.downKey = atoi(value);
    }
    if(MATCH("PAD", "LeftKey")) {
        config->padSettings.leftKey = atoi(value);
    }
    if(MATCH("PAD", "RightKey")) {
        config->padSettings.rightKey = atoi(value);
    }
    if(MATCH("PAD", "LKey")) {
        config->padSettings.lKey = atoi(value);
    }
    if(MATCH("PAD", "RKey")) {
        config->padSettings.rKey = atoi(value);
    }
    if(MATCH("PAD", "StartKey")) {
        config->padSettings.startKey = atoi(value);
    }
    if(MATCH("PAD", "SelectKey")) {
        config->padSettings.selectKey = atoi(value);
    }
    if(MATCH("PAD", "GUIKey")) {
        config->padSettings.guiKey = atoi(value);
    }
    if(MATCH("PAD", "AJoyKey")) {
        config->padSettings.aJoyKey = atoi(value);
    }
    if(MATCH("PAD", "BJoyKey")) {
        config->padSettings.bJoyKey = atoi(value);
    }
    if(MATCH("PAD", "XJoyKey")) {
        config->padSettings.xJoyKey = atoi(value);
    }
    if(MATCH("PAD", "YJoyKey")) {
        config->padSettings.yJoyKey = atoi(value);
    }
    if(MATCH("PAD", "UpJoyKey")) {
        config->padSettings.upJoyKey = atoi(value);
    }
    if(MATCH("PAD", "DownJoyKey")) {
        config->padSettings.downJoyKey = atoi(value);
    }
    if(MATCH("PAD", "LeftJoyKey")) {
        config->padSettings.leftJoyKey = atoi(value);
    }
    if(MATCH("PAD", "RightJoyKey")) {
        config->padSettings.rightJoyKey = atoi(value);
    }
    if(MATCH("PAD", "LJoyKey")) {
        config->padSettings.lJoyKey = atoi(value);
    }
    if(MATCH("PAD", "RJoyKey")) {
        config->padSettings.rJoyKey = atoi(value);
    }
    if(MATCH("PAD", "StartJoyKey")) {
        config->padSettings.startJoyKey = atoi(value);
    }
    if(MATCH("PAD", "SelectJoyKey")) {
        config->padSettings.selectJoyKey = atoi(value);
    }

    //OS NVRAM
    //Owner Info
    if(MATCH("OSNVRAM", "Owner_FavoriteColor")) {
        config->osNvramConfig.ncd.owner.favoriteColor = atoi(value);
    }
    if(MATCH("OSNVRAM", "Owner_BirthdayMonth")) {
        config->osNvramConfig.ncd.owner.birthday.month = atoi(value);
    }
    if(MATCH("OSNVRAM", "Owner_BirthdayDay")) {
        config->osNvramConfig.ncd.owner.birthday.day = atoi(value);
    }
    if(MATCH("OSNVRAM", "Owner_Nickname")) {
        memset(config->osNvramConfig.ncd.owner.nickname.str, 0, NVRAM_CONFIG_NICKNAME_LENGTH*2);

        int valueLen = strlen(value);
        for(int i=0; i < valueLen; i++) {
            u16 outChar = value[i];
            config->osNvramConfig.ncd.owner.nickname.str[i] = outChar;
        }
        config->osNvramConfig.ncd.owner.nickname.length = (u8)wcslen((wchar_t*)config->osNvramConfig.ncd.owner.nickname.str);
    }
    if(MATCH("OSNVRAM", "Owner_Comment")) {
        memset(config->osNvramConfig.ncd.owner.comment.str, 0, NVRAM_CONFIG_COMMENT_LENGTH*2);
        swprintf((wchar_t*)config->osNvramConfig.ncd.owner.comment.str, (NVRAM_CONFIG_COMMENT_LENGTH)-1, L"%s", value);
        config->osNvramConfig.ncd.owner.comment.length = wcslen((wchar_t*)config->osNvramConfig.ncd.owner.comment.str);
    }
    //Alarm
    if(MATCH("OSNVRAM", "Alarm_Hour")) {
        config->osNvramConfig.ncd.alarm.hour = atoi(value);
    }
    if(MATCH("OSNVRAM", "Alarm_Minute")) {
        config->osNvramConfig.ncd.alarm.minute = atoi(value);
    }
    if(MATCH("OSNVRAM", "Alarm_Second")) {
        config->osNvramConfig.ncd.alarm.minute = atoi(value);
    }
    if(MATCH("OSNVRAM", "Alarm_DaysOfWeek")) {
        config->osNvramConfig.ncd.alarm.enableWeek = atoi(value);
    }
    if(MATCH("OSNVRAM", "Alarm_Enabled")) {
        config->osNvramConfig.ncd.alarm.alarmOn = StringToBool(value);
    }
    //Touch panel calibration
    if(MATCH("OSNVRAM", "TP_RawX1")) {
        config->osNvramConfig.ncd.tp.raw_x1 = atoi(value);
    }
    if(MATCH("OSNVRAM", "TP_RawY1")) {
        config->osNvramConfig.ncd.tp.raw_y1 = atoi(value);
    }
    if(MATCH("OSNVRAM", "TP_dx1")) {
        config->osNvramConfig.ncd.tp.dx1 = atoi(value);
    }
    if(MATCH("OSNVRAM", "TP_dy1")) {
        config->osNvramConfig.ncd.tp.dy1 = atoi(value);
    }
    if(MATCH("OSNVRAM", "TP_RawX2")) {
        config->osNvramConfig.ncd.tp.raw_x2 = atoi(value);
    }
    if(MATCH("OSNVRAM", "TP_RawY2")) {
        config->osNvramConfig.ncd.tp.raw_y2 = atoi(value);
    }
    if(MATCH("OSNVRAM", "TP_dx2")) {
        config->osNvramConfig.ncd.tp.dx2 = atoi(value);
    }
    if(MATCH("OSNVRAM", "TP_dy2")) {
        config->osNvramConfig.ncd.tp.dy2 = atoi(value);
    }
    //Options
    if(MATCH("OSNVRAM", "Option_Language")) {
        config->osNvramConfig.ncd.option.language = atoi(value);
    }
    if(MATCH("OSNVRAM", "Option_AGBLCD")) {
        config->osNvramConfig.ncd.option.agbLcd = atoi(value);
    }
    if(MATCH("OSNVRAM", "Option_PullOutCardFlag")) {
        config->osNvramConfig.ncd.option.detectPullOutCardFlag = atoi(value);
    }
    if(MATCH("OSNVRAM", "Option_PullOutCtrdgFlag")) {
        config->osNvramConfig.ncd.option.detectPullOutCtrdgFlag = atoi(value);
    }
    if(MATCH("OSNVRAM", "Option_AutoBootFlag")) {
        config->osNvramConfig.ncd.option.autoBootFlag = StringToBool(value);
    }
    if(MATCH("OSNVRAM", "Option_FavoriteColorSet")) {
        config->osNvramConfig.ncd.option.input_favoriteColor = StringToBool(value);
    }
    if(MATCH("OSNVRAM", "Option_TouchPanelCalibrated")) {
        config->osNvramConfig.ncd.option.input_tp = StringToBool(value);
    }
    if(MATCH("OSNVRAM", "Option_LanguageSet")) {
        config->osNvramConfig.ncd.option.input_language = StringToBool(value);
    }
    if(MATCH("OSNVRAM", "Option_RTCSet")) {
        config->osNvramConfig.ncd.option.input_rtc = StringToBool(value);
    }
    if(MATCH("OSNVRAM", "Option_NicknameSet")) {
        config->osNvramConfig.ncd.option.input_nickname = StringToBool(value);
    }
    if(MATCH("OSNVRAM", "Option_Timezone")) {
        config->osNvramConfig.ncd.option.timezone = atoi(value);
    }
    if(MATCH("OSNVRAM", "Option_RTCClockAdjust")) {
        config->osNvramConfig.ncd.option.rtcClockAdjust = atoi(value);
    }
    if(MATCH("OSNVRAM", "Option_RTCOffset")) {
        config->osNvramConfig.ncd.option.rtcOffset = atoi(value);
    }

    // Net Settings
    if(MATCH("Net", "ServerIp")) {
        memset(config->netSettings.serverIp, 0, sizeof(config->netSettings.serverIp));
        strncpy(config->netSettings.serverIp, value, sizeof(config->netSettings.serverIp)-1);
    }
}

static const char * BoolToString(BOOL arg)
{
    const char * ret = "false";
    if(arg) {
        ret = "true";
    }

    return ret;
}

static BOOL StringToBool(const char * str)
{
    BOOL ret = FALSE;

    if(strcmp(str, "false") == 0) {
        ret = FALSE;
    } else if(strcmp(str, "true") == 0) {
        ret = TRUE;
    }
    return ret;
}

static const char * ScreenLayoutTypeToString(SIM_config_screen_layout_type layout)
{
    const char * ret = "";

    switch(layout) {
        default:
        case SIM_CONFIG_SCREEN_LAYOUT_VERTICAL:
            ret = "vertical";
            break;
        case SIM_CONFIG_SCREEN_LAYOUT_HORIZONTAL:
            ret = "horizontal";
            break;
        case SIM_CONFIG_SCREEN_LAYOUT_LARGE:
            ret = "large";
            break;
    }
    return ret;
}

static SIM_config_screen_layout_type StringToScreenLayoutType(const char * str)
{
    SIM_config_screen_layout_type ret = SIM_CONFIG_SCREEN_LAYOUT_VERTICAL;

    if(strcmp(str, "vertical") == 0) {
        ret = SIM_CONFIG_SCREEN_LAYOUT_VERTICAL;
    } else if (strcmp(str, "horizontal") == 0) {
        ret = SIM_CONFIG_SCREEN_LAYOUT_HORIZONTAL;
    } else if (strcmp(str, "large") == 0) {
        ret = SIM_CONFIG_SCREEN_LAYOUT_LARGE;
    }
    
    return ret;
}



// Loads the configuration file. If it does not exist, return false
BOOL SIM_Config_LoadConfigFile(SIM_config_type * aConfig)
{
    return (ini_parse(SIM_CONFIG_FILE_PATH, IniHandler, aConfig) >= 0);
}

// Initialize a SIM_config_type to default values
void SIM_Config_LoadDefaults(SIM_config_type * aConfig)
{
#ifdef SDK_BUILD_NX
    aConfig->internalResolutionScale = 2;
    aConfig->windowHeight = 1080;
    aConfig->windowWidth = 1920;
#else
    aConfig->internalResolutionScale = 10;
    aConfig->windowHeight = 0;
    aConfig->windowWidth = 0;
#endif
    aConfig->screenLayout = SIM_CONFIG_SCREEN_LAYOUT_VERTICAL;
    aConfig->swapScreens = FALSE;
    aConfig->vsyncInterval = 1;
    aConfig->capFrameRate = TRUE;

    aConfig->padSettings.joyAxisDeadzone = 32767 / 4;
    aConfig->padSettings.aKey = SDLK_z;
    aConfig->padSettings.bKey = SDLK_x;
    aConfig->padSettings.xKey = SDLK_c;
    aConfig->padSettings.yKey = SDLK_v;
    aConfig->padSettings.upKey = SDLK_UP;
    aConfig->padSettings.downKey = SDLK_DOWN;
    aConfig->padSettings.leftKey = SDLK_LEFT;
    aConfig->padSettings.rightKey = SDLK_RIGHT;
    aConfig->padSettings.lKey = SDLK_q;
    aConfig->padSettings.rKey = SDLK_e;
    aConfig->padSettings.startKey = SDLK_RETURN;
    aConfig->padSettings.selectKey = SDLK_RSHIFT;
    aConfig->padSettings.guiKey = SDLK_TAB;
    aConfig->padSettings.aJoyKey = 0;
    aConfig->padSettings.bJoyKey = 1;
    aConfig->padSettings.xJoyKey = 2;
    aConfig->padSettings.yJoyKey = 3;
#ifdef SDK_BUILD_NX
    aConfig->padSettings.upJoyKey = 13;
    aConfig->padSettings.downJoyKey = 15;
    aConfig->padSettings.leftJoyKey = 12;
    aConfig->padSettings.rightJoyKey = 14;
    aConfig->padSettings.lJoyKey = 6;
    aConfig->padSettings.rJoyKey = 7;
    aConfig->padSettings.startJoyKey = 10;
    aConfig->padSettings.selectJoyKey = 11;
#else
    aConfig->padSettings.upJoyKey = SDL_HAT_UP | SIM_CONFIG_JOY_HAT_MASK;
    aConfig->padSettings.downJoyKey = SDL_HAT_DOWN | SIM_CONFIG_JOY_HAT_MASK;
    aConfig->padSettings.leftJoyKey = SDL_HAT_LEFT | SIM_CONFIG_JOY_HAT_MASK;
    aConfig->padSettings.rightJoyKey = SDL_HAT_RIGHT | SIM_CONFIG_JOY_HAT_MASK;
    aConfig->padSettings.lJoyKey = 9;
    aConfig->padSettings.rJoyKey = 10;
    aConfig->padSettings.startJoyKey = 6;
    aConfig->padSettings.selectJoyKey = 4;
#endif

    // Set a random MAC address (with a nintendo prefix)
    aConfig->macAddr[0] = 0x98;
    aConfig->macAddr[1] = 0xE2;
    aConfig->macAddr[2] = 0x55;

    srand(time(NULL));
    aConfig->macAddr[3] = rand();
    aConfig->macAddr[4] = rand();
    aConfig->macAddr[5] = rand();

    memset(&aConfig->osNvramConfig, 0, sizeof(NVRAMConfig));
    wcscpy((wchar_t*)aConfig->osNvramConfig.ncd.owner.nickname.str, L"Player");
    aConfig->osNvramConfig.ncd.option.input_nickname = TRUE;
    aConfig->osNvramConfig.ncd.option.input_favoriteColor = TRUE;
    aConfig->osNvramConfig.ncd.option.input_language = TRUE;
    aConfig->osNvramConfig.ncd.option.input_tp = TRUE;
    aConfig->osNvramConfig.ncd.option.input_rtc = TRUE;
    
    aConfig->netSettings.serverIp[15] = 0;
    strcpy(aConfig->netSettings.serverIp, "127.0.0.1");
}

// Save a SIM_config_type to the configuration file
void SIM_Config_SaveConfigFile(SIM_config_type * aConfig)
{
    FILE * configFile = fopen(SIM_CONFIG_FILE_PATH, "w");
    
    fprintf(configFile, "[General]\n");
    fprintf(configFile, "InternalResolutionScale=%d\n", aConfig->internalResolutionScale);
    fprintf(configFile, "WindowHeight=%d\n", aConfig->windowHeight);
    fprintf(configFile, "WindowWidth=%d\n", aConfig->windowWidth);

    const char * screenLayoutString = ScreenLayoutTypeToString(aConfig->screenLayout);
    fprintf(configFile, "ScreenLayout=%s\n", screenLayoutString);
    fprintf(configFile, "SwapScreens=%s\n", BoolToString(aConfig->swapScreens));
    fprintf(configFile, "VsyncInterval=%d\n", aConfig->vsyncInterval);
    fprintf(configFile, "CapFrameRate=%s\n", BoolToString(aConfig->capFrameRate));
    fprintf(configFile, "MacAddr=%02x%02x%02x%02x%02x%02x\n", aConfig->macAddr[0],
                                                              aConfig->macAddr[1],
                                                              aConfig->macAddr[2],
                                                              aConfig->macAddr[3],
                                                              aConfig->macAddr[4],
                                                              aConfig->macAddr[5]);
    fprintf(configFile, "\n");

    fprintf(configFile, "[PAD]\n");
    fprintf(configFile, "JoyAxisDeadzone=%d\n", aConfig->padSettings.joyAxisDeadzone);
    fprintf(configFile, "AKey=%d\n", aConfig->padSettings.aKey);
    fprintf(configFile, "BKey=%d\n", aConfig->padSettings.bKey);
    fprintf(configFile, "XKey=%d\n", aConfig->padSettings.xKey);
    fprintf(configFile, "YKey=%d\n", aConfig->padSettings.yKey);
    fprintf(configFile, "UpKey=%d\n", aConfig->padSettings.upKey);
    fprintf(configFile, "DownKey=%d\n", aConfig->padSettings.downKey);
    fprintf(configFile, "LeftKey=%d\n", aConfig->padSettings.leftKey);
    fprintf(configFile, "RightKey=%d\n", aConfig->padSettings.rightKey);
    fprintf(configFile, "LKey=%d\n", aConfig->padSettings.lKey);
    fprintf(configFile, "RKey=%d\n", aConfig->padSettings.rKey);
    fprintf(configFile, "StartKey=%d\n", aConfig->padSettings.startKey);
    fprintf(configFile, "SelectKey=%d\n", aConfig->padSettings.selectKey);
    fprintf(configFile, "GUIKey=%d\n", aConfig->padSettings.guiKey);
    fprintf(configFile, "AJoyKey=%d\n", aConfig->padSettings.aJoyKey);
    fprintf(configFile, "BJoyKey=%d\n", aConfig->padSettings.bJoyKey);
    fprintf(configFile, "XJoyKey=%d\n", aConfig->padSettings.xJoyKey);
    fprintf(configFile, "YJoyKey=%d\n", aConfig->padSettings.yJoyKey);
    fprintf(configFile, "UpJoyKey=%d\n", aConfig->padSettings.upJoyKey);
    fprintf(configFile, "DownJoyKey=%d\n", aConfig->padSettings.downJoyKey);
    fprintf(configFile, "LeftJoyKey=%d\n", aConfig->padSettings.leftJoyKey);
    fprintf(configFile, "RightJoyKey=%d\n", aConfig->padSettings.rightJoyKey);
    fprintf(configFile, "LJoyKey=%d\n", aConfig->padSettings.lJoyKey);
    fprintf(configFile, "RJoyKey=%d\n", aConfig->padSettings.rJoyKey);
    fprintf(configFile, "StartJoyKey=%d\n", aConfig->padSettings.startJoyKey);
    fprintf(configFile, "SelectJoyKey=%d\n", aConfig->padSettings.selectJoyKey);
    fprintf(configFile, "\n");

    fprintf(configFile, "[OSNVRAM]\n");
    fprintf(configFile, "Owner_FavoriteColor=%d\n", aConfig->osNvramConfig.ncd.owner.favoriteColor);
    fprintf(configFile, "Owner_BirthdayMonth=%d\n", aConfig->osNvramConfig.ncd.owner.birthday.month);
    fprintf(configFile, "Owner_BirthdayDay=%d\n", aConfig->osNvramConfig.ncd.owner.birthday.day);
    fprintf(configFile, "Owner_Nickname=%ls\n", aConfig->osNvramConfig.ncd.owner.nickname.str);
    fprintf(configFile, "Owner_Comment=%ls\n", aConfig->osNvramConfig.ncd.owner.comment.str);
    fprintf(configFile, "Alarm_Hour=%d\n", aConfig->osNvramConfig.ncd.alarm.hour);
    fprintf(configFile, "Alarm_Minute=%d\n", aConfig->osNvramConfig.ncd.alarm.minute);
    fprintf(configFile, "Alarm_Second=%d\n", aConfig->osNvramConfig.ncd.alarm.second);
    fprintf(configFile, "Alarm_DaysOfWeek=%d\n", aConfig->osNvramConfig.ncd.alarm.enableWeek);
    fprintf(configFile, "Alarm_Enabled=%s\n", BoolToString(aConfig->osNvramConfig.ncd.alarm.alarmOn));
    fprintf(configFile, "TP_RawX1=%d\n", aConfig->osNvramConfig.ncd.tp.raw_x1);
    fprintf(configFile, "TP_RawY1=%d\n", aConfig->osNvramConfig.ncd.tp.raw_y1);
    fprintf(configFile, "TP_dx1=%d\n", aConfig->osNvramConfig.ncd.tp.dx1);
    fprintf(configFile, "TP_dy1=%d\n", aConfig->osNvramConfig.ncd.tp.dy1);
    fprintf(configFile, "TP_RawX2=%d\n", aConfig->osNvramConfig.ncd.tp.raw_x2);
    fprintf(configFile, "TP_RawY2=%d\n", aConfig->osNvramConfig.ncd.tp.raw_y2);
    fprintf(configFile, "TP_dx2=%d\n", aConfig->osNvramConfig.ncd.tp.dx2);
    fprintf(configFile, "TP_dy2=%d\n", aConfig->osNvramConfig.ncd.tp.dy2);
    fprintf(configFile, "Option_Language=%d\n", aConfig->osNvramConfig.ncd.option.language);
    fprintf(configFile, "Option_AGBLCD=%d\n", aConfig->osNvramConfig.ncd.option.agbLcd);
    fprintf(configFile, "Option_PullOutCardFlag=%d\n", aConfig->osNvramConfig.ncd.option.detectPullOutCardFlag);
    fprintf(configFile, "Option_PullOutCtrdgFlag=%d\n", aConfig->osNvramConfig.ncd.option.detectPullOutCtrdgFlag);
    fprintf(configFile, "Option_AutoBootFlag=%s\n", BoolToString(aConfig->osNvramConfig.ncd.option.autoBootFlag));
    fprintf(configFile, "Option_FavoriteColorSet=%s\n", BoolToString(aConfig->osNvramConfig.ncd.option.input_favoriteColor));
    fprintf(configFile, "Option_TouchPanelCalibrated=%s\n", BoolToString(aConfig->osNvramConfig.ncd.option.input_tp));
    fprintf(configFile, "Option_LanguageSet=%s\n", BoolToString(aConfig->osNvramConfig.ncd.option.input_language));
    fprintf(configFile, "Option_RTCSet=%s\n", BoolToString(aConfig->osNvramConfig.ncd.option.input_rtc));
    fprintf(configFile, "Option_NicknameSet=%s\n", BoolToString(aConfig->osNvramConfig.ncd.option.input_nickname));
    fprintf(configFile, "Option_Timezone=%d\n", aConfig->osNvramConfig.ncd.option.timezone);
    fprintf(configFile, "Option_RTCClockAdjust=%d\n", aConfig->osNvramConfig.ncd.option.rtcClockAdjust);
    fprintf(configFile, "Option_RTCOffset=%d\n", aConfig->osNvramConfig.ncd.option.rtcOffset);
    fprintf(configFile, "\n");
    fprintf(configFile, "[LAN]\n");
    fprintf(configFile, "ServerIp=%s\n", aConfig->netSettings.serverIp);
    fprintf(configFile, "\n");

    
    fclose(configFile);
}

void SIM_Config_SetOSNVRAMConfig(SIM_config_type * aConfig)
{
    MI_CpuCopy8(&aConfig->osNvramConfig, (void*)OS_GetSystemWork()->nvramUserInfo, sizeof(NVRAMConfig));
    
}
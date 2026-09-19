#ifndef NITRO_PM_ARM9_H_
#define NITRO_PM_ARM9_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SDK_TWL
#include <nitro/misc.h>
#include <nitro/types.h>
#else
#include <twl/misc.h>
#include <twl/types.h>
#endif
#include <nitro/spi/common/pm_common.h>
#include <nitro/spi/common/type.h>
#include <nitro/pxi/common/fifo.h>

typedef void (*PMCallback) (u32 result, void * arg);

#if SDK_VERSION_MAJOR == 4
typedef struct {
	u16 flag;
	u16 padding;
	u16 * buffer;
} PMData16;
#endif

typedef enum {
	PM_LCD_TOP = 0,
	PM_LCD_BOTTOM = 1,
	PM_LCD_ALL = 2
} PMLCDTarget;

typedef enum {
	PM_BACKLIGHT_OFF = 0,
	PM_BACKLIGHT_ON = 1
} PMBackLightSwitch;

typedef enum {
	PM_BATTERY_HIGH = 0,
	PM_BATTERY_LOW = 1
} PMBattery;

typedef enum {
	PM_AMP_OFF = 0,
	PM_AMP_ON = 1
} PMAmpSwitch;

typedef enum {
	PM_AMPGAIN_20 = 0,
	PM_AMPGAIN_40 = 1,
	PM_AMPGAIN_80 = 2,
	PM_AMPGAIN_160 = 3,
	PM_AMPGAIN_DEFAULT = PM_AMPGAIN_40
} PMAmpGain;

#define PM_GAINAMP_DEFAULT  PM_AMPGAIN_DEFAULT

typedef enum {
	PM_LCD_POWER_OFF = 0,
	PM_LCD_POWER_ON = 1
} PMLCDPower;

#if SDK_VERSION_MAJOR == 5
#ifdef SDK_TWL
typedef enum {
  PM_EXIT_FACTOR_NONE = 0,
  PM_EXIT_FACTOR_PWSW = 1,
  PM_EXIT_FACTOR_BATTERY = 2,
  PM_EXIT_FACTOR_USER = 3
} PMExitFactor;
#endif
#endif

#if SDK_VERSION_MAJOR == 4
typedef void (*PMSleepCallback) (void *);

typedef struct PMiSleepCallbackInfo PMSleepCallbackInfo;
struct PMiSleepCallbackInfo {
	PMSleepCallback callback;
	void * arg;
	PMSleepCallbackInfo * next;
};
#endif

#if SDK_VERSION_MAJOR == 5
typedef void (*PMGenCallback)(void *);

#define PMSleepCallback PMGenCallback
#ifdef SDK_TWL

#define PMExitCallback PMGenCallback

#define PMBatteryLowCallback PMGenCallback
#endif

typedef struct PMiGenCallbackInfo PMGenCallbackInfo;
struct PMiGenCallbackInfo {
  PMGenCallback callback;
  void *arg;
  int priority;
  PMGenCallbackInfo *next;
};

#define PMSleepCallbackInfo PMGenCallbackInfo
#ifdef SDK_TWL

#define PMExitCallbackInfo PMGenCallbackInfo

#define PMBatteryLowCallbackInfo PMGenCallbackInfo
#endif

#define PM_CALLBACK_PRIORITY_SYSMIN (-65535)
#define PM_CALLBACK_PRIORITY_MIN (-255)
#define PM_CALLBACK_PRIORITY_MAX 255
#define PM_CALLBACK_PRIORITY_SYSMAX 65535

#define PM_CALLBACK_PRIORITY_WM 1000
#define PM_CALLBACK_PRIORITY_DSP 1010
#define PM_CALLBACK_PRIORITY_CAMERA 1020
#define PM_CALLBACK_PRIORITY_NWM PM_CALLBACK_PRIORITY_SYSMAX
#define PM_CALLBACK_PRIORITY_FS PM_CALLBACK_PRIORITY_SYSMAX - 1
#define PM_CALLBACK_PRIORITY_SNDEX PM_CALLBACK_PRIORITY_SYSMAX - 2
#endif

void PM_Init(void);

#if SDK_VERSION_MAJOR == 4
u32 PM_SendUtilityCommandAsync(u32 number, PMCallback callback, void * arg);
u32 PM_SendUtilityCommand(u32 number);
#endif
#if SDK_VERSION_MAJOR == 5
u32 PM_SendUtilityCommandAsync(u32 number, u16 parameter, u16 *retValue,
                               PMCallback callback, void *arg);
u32 PM_SendUtilityCommand(u32 number, u16 parameter, u16 *retValue);
#endif

u32 PM_SetBackLightAsync(PMLCDTarget target, PMBackLightSwitch sw, PMCallback callback, void * arg);
u32 PM_SetBackLight(PMLCDTarget target, PMBackLightSwitch status);

u32 PM_ForceToPowerOffAsync(PMCallback callback, void * arg);
u32 PM_ForceToPowerOff(void);

u32 PM_SetAmpAsync(PMAmpSwitch sw, PMCallback callback, void * arg);
u32 PM_SetAmp(PMAmpSwitch sw);

u32 PM_SetAmpGainAsync(PMAmpGain gain, PMCallback callback, void * arg);
u32 PM_SetAmpGain(PMAmpGain gain);

#if SDK_VERSION_MAJOR == 5
#ifdef SDK_TWL
u32 PM_SetAmpGainLevelAsync(u8 level, PMCallback callback, void *arg);
u32 PM_SetAmpGainLevel(u8 level);
#endif
#endif

u32 PM_GetBattery(PMBattery * batteryBuf);
#if SDK_VERSION_MAJOR == 5
#ifdef SDK_TWL
u32 PM_GetBatteryLevel(PMBatteryLevel *levelBuf);
#else
static inline u32 PM_GetBatteryLevel(PMBatteryLevel *levelBuf) {
#pragma unused(levelBuf)
  return PM_RESULT_ERROR;
}
#endif

#ifdef SDK_TWL
u32 PM_GetACAdapter(BOOL *isConnectedBuf);
#else
static inline u32 PM_GetACAdapter(BOOL *isConnectedBuf) {
#pragma unused(isConnectedBuf)
  return PM_RESULT_ERROR;
}
#endif
#endif
u32 PM_GetBackLight(PMBackLightSwitch * top, PMBackLightSwitch * bottom);

u32 PM_GetAmp(PMAmpSwitch * swBuf);
u32 PM_GetAmpGain(PMAmpGain * gainBuf);
#if SDK_VERSION_MAJOR == 5
#ifdef SDK_TWL
u32 PM_GetAmpGainLevel(u8 *levelBuf);
u32 PMi_SetWirelessLED(PMWirelessLEDStatus sw);
#endif
#endif

void PM_GoSleepMode(PMWakeUpTrigger trigger, PMLogic logic, u16 keyPattern);

static inline void PM_SetSleepCallbackInfo (PMSleepCallbackInfo * info, PMSleepCallback callback, void * arg)
{
	info->callback = callback;
	info->arg = arg;
}

void PM_AppendPreSleepCallback(PMSleepCallbackInfo * info);
void PM_PrependPreSleepCallback(PMSleepCallbackInfo * info);

void PM_AppendPostSleepCallback(PMSleepCallbackInfo * info);
void PM_PrependPostSleepCallback(PMSleepCallbackInfo * info);

void PM_DeletePreSleepCallback(PMSleepCallbackInfo * info);
void PM_DeletePostSleepCallback(PMSleepCallbackInfo * info);

#if SDK_VERSION_MAJOR == 5
void PM_ClearPreSleepCallback(void);
void PM_ClearPostSleepCallback(void);
#endif

BOOL PM_SetLCDPower(PMLCDPower sw);
PMLCDPower PM_GetLCDPower(void);

u32 PMi_SendLEDPatternCommandAsync(PMLEDPattern pattern, PMCallback callback, void * arg);
u32 PMi_SendLEDPatternCommand(PMLEDPattern pattern);

inline u32 PM_SetLEDPatternAsync (PMLEDPattern pattern, PMCallback callback, void * arg)
{
	return PMi_SendLEDPatternCommandAsync(pattern, callback, arg);
}

inline u32 PM_SetLEDPattern (PMLEDPattern pattern)
{
	return PMi_SendLEDPatternCommand(pattern);
}

u32 PM_GetLEDPatternAsync(PMLEDPattern * patternBuf, PMCallback callback, void * arg);
u32 PM_GetLEDPattern(PMLEDPattern * patternBuf);

#if SDK_VERSION_MAJOR == 5
#ifdef SDK_TWL
static inline void PM_SetExitCallbackInfo(PMExitCallbackInfo *info,
                                          PMExitCallback callback, void *arg) {
  info->callback = callback;
  info->arg = arg;
}

void PM_ReadyToExit(void);
PMExitFactor PM_GetExitFactor(void);
void PM_AppendPreExitCallback(PMExitCallbackInfo *info);
void PM_AppendPostExitCallback(PMExitCallbackInfo *info);
void PM_PrependPreExitCallback(PMExitCallbackInfo *info);
void PM_PrependPostExitCallback(PMExitCallbackInfo *info);
void PM_InsertPreExitCallback(PMExitCallbackInfo *info, int priority);
void PM_InsertPostExitCallback(PMExitCallbackInfo *info, int priority);
void PM_DeletePreExitCallback(PMExitCallbackInfo *info);
void PM_DeletePostExitCallback(PMExitCallbackInfo *info);
void PM_ClearPreExitCallback(void);
void PM_ClearPostExitCallback(void);
void PM_SetAutoExit(BOOL sw);
BOOL PM_GetAutoExit(void);
void PM_SetBatteryLowCallback(PMBatteryLowCallback callback, void *arg);
void PM_ForceToResetHardware(void);
#endif
#endif

typedef enum {
	PM_SOUND_POWER_OFF = 0,
	PM_SOUND_POWER_ON = 1
} PMSoundPowerSwitch;

typedef enum {
	PM_SOUND_VOLUME_OFF = 0,
	PM_SOUND_VOLUME_ON = 1
} PMSoundVolumeSwitch;

BOOL PMi_SetLCDPower(PMLCDPower sw, PMLEDStatus led, BOOL skip, BOOL isSync);

u32 PMi_SetSoundPowerAsync(PMSoundPowerSwitch sw, PMCallback callback, void * arg);
u32 PMi_SetSoundPower(PMSoundPowerSwitch sw);

u32 PMi_SetSoundVolumeAsync(PMSoundVolumeSwitch sw, PMCallback callback, void * arg);
u32 PMi_SetSoundVolume(PMSoundVolumeSwitch sw);

u32 PMi_GetSoundPower(PMSoundPowerSwitch * swBuf);
u32 PMi_GetSoundVolume(PMSoundVolumeSwitch * swBuf);

void PMi_SendPxiData(u32 data);

#ifdef SDK_PORT
void PMi_CommonCallback(PXIFifoTag tag, u64 data, BOOL err);
#else
void PMi_CommonCallback(PXIFifoTag tag, u32 data, BOOL err);
#endif

u32 PMi_SendSleepStart(u16 trigger, u16 keyIntrData);
u32 PMi_DisposeSleepEnd(void);

u32 PMi_ReadRegisterAsync(u16 registerAddr, u16 * buffer, PMCallback callback, void * arg);
u32 PMi_ReadRegister(u16 registerAddr, u16 * buffer);

u32 PMi_WriteRegisterAsync(u16 registerAddr, u16 data, PMCallback callback, void * arg);
u32 PMi_WriteRegister(u16 registerAddr, u16 data);

u32 PMi_SetLEDAsync(PMLEDStatus status, PMCallback callback, void * arg);
u32 PMi_SetLED(PMLEDStatus status);

u32 PMi_GetLCDOffCount(void);

#if SDK_VERSION_MAJOR == 5
#ifdef SDK_TWL
void PMi_ExecuteAllListsOfExitCallback(void);
void PMi_ExecutePreExitCallbackList(void);
void PMi_ExecutePostExitCallbackList(void);
#endif

void PMi_InsertPreSleepCallbackEx(PMSleepCallbackInfo *info, int priority);
void PMi_InsertPostSleepCallbackEx(PMSleepCallbackInfo *info, int priority);

#ifdef SDK_TWL
void PMi_InsertPreExitCallbackEx(PMExitCallbackInfo *info, int priority);
void PMi_InsertPostExitCallbackEx(PMExitCallbackInfo *info, int priority);
#endif

void PMi_SetDispOffCount(void);

#ifdef SDK_TWL
BOOL PMi_TryLockForReset(void);
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif

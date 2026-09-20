#include <nitro/spi/ARM9/pm.h>
#include <nitro/pxi.h>
#include <nitro/gx.h>
#include <nitro/spi/common/config.h>
#include <nitro/ctrdg.h>
#include <nitro/mb.h>
#if SDK_VERSION_MAJOR == 5
#ifdef SDK_TWL
#include "../../os/include/application_jump_private.h"
#endif

#ifdef SDK_LINK_ISTD
#pragma warn_extracomma off
#include <istdbglib.h> // Has extra comma in enum
#pragma warn_extracomma reset
#endif
#endif

typedef struct {
    BOOL lock;
    PMCallback callback;
    void * callbackArg;
    void * work;
} PMiWork;

#if SDK_VERSION_MAJOR == 4
#define PMi_LCD_POWER_WAIT_MSEC  150
#define PMi_LCD_POWER_WAIT_TICK  (OS_MilliSecondsToTicks(PMi_LCD_POWER_WAIT_MSEC) * (64 * 2))
#define PMi_LCD_SLEEP_WAIT_MSEC  110
#define PMi_LCD_SLEEP_WAIT_TICK  (OS_MilliSecondsToTicks(PMi_LCD_SLEEP_WAIT_MSEC) * (64 * 2))

#define PMi_STAT_BATTERY_MASK    1

#define PMi_SetCallback(callback, arg)     \
    do {                                   \
        PMi_Work.callback = (callback);    \
        PMi_Work.callbackArg = (arg);      \
    } while (0)
#endif

#if SDK_VERSION_MAJOR == 5
#define PMi_LCD_WAIT_SYS_CYCLES 0x360000
#define PMi_PXI_WAIT_TICK 10

#define PMi_COMPARE_GT 0
#define PMi_COMPARE_GE 1
#endif

#ifdef SDK_PORT
static
#endif
inline u32 PMi_MakeData1 (u32 bit, u32 seq, u32 command, u32 data)
{
    return (bit) | ((seq) << SPI_PXI_INDEX_SHIFT) | ((command) << 8) | ((data) & 0xff);
}

#ifdef SDK_PORT
static
#endif
inline u32 PMi_MakeData2 (u32 bit, u32 seq, u32 data)
{
    return (bit) | ((seq) << SPI_PXI_INDEX_SHIFT) | ((data) & 0xffff);
}

#if SDK_VERSION_MAJOR == 4
BOOL PMi_Lock(void);
#endif

#if SDK_VERSION_MAJOR == 5
static u32 PMi_TryToSendPxiData(u32 *sendData, int num, u16 *retValue,
                                PMCallback callback, void *arg);
static void PMi_TryToSendPxiDataTillSuccess(u32 *sendData, int num);
static u32 PMi_ForceToPowerOff(void);
static void PMi_CallPostExitCallbackAndReset(BOOL isExit);
#endif

void PMi_WaitBusy(void);
void PMi_DummyCallback(u32 result, void * arg);
#if SDK_VERSION_MAJOR == 4
void PMi_PrependList(PMSleepCallbackInfo ** listp, PMSleepCallbackInfo * info);
void PMi_AppendList(PMSleepCallbackInfo ** listp, PMSleepCallbackInfo * info);
#endif
#if SDK_VERSION_MAJOR == 5
static void PMi_InsertList(PMGenCallbackInfo **listp, PMGenCallbackInfo *info,
                           int priority, int method);
static void PMi_ClearList(PMGenCallbackInfo **listp);
#endif
void PMi_DeleteList(PMSleepCallbackInfo ** listp, PMSleepCallbackInfo * info);
void PMi_ExecuteList(PMSleepCallbackInfo * listp);

#if SDK_VERSION_MAJOR == 5
#ifdef SDK_TWL
static void PMi_FinalizeDebugger(void);
#include <twl/ltdmain_begin.h>
static void PMi_ProceedToExit(PMExitFactor factor);
static void PMi_ClearPreExitCallback(void);
static void PMi_ClearPostExitCallback(void);
#include <twl/ltdmain_end.h>
#endif

static void PMi_LCDOnAvoidReset(void);
static void PMi_WaitVBlank(void);
#endif

#if SDK_VERSION_MAJOR == 4
static PMCallback PMi_Callback;
static u16 PMi_IsInit = FALSE;
#endif
static PMiWork PMi_Work;
#if SDK_VERSION_MAJOR == 4
static PMData16 PMi_RegisterBuffer[PMIC_REG_NUMS];
static volatile BOOL PMi_SyncFlag;
#endif
static volatile BOOL PMi_SleepEndFlag;

#if SDK_VERSION_MAJOR == 4
static OSMutex PMi_Mutex;
#endif
static u32 PMi_LCDCount;
#if SDK_VERSION_MAJOR == 5
static u32 PMi_DispOffCount;
#endif

static PMSleepCallbackInfo * PMi_PreSleepCallbackList = NULL;
static PMSleepCallbackInfo * PMi_PostSleepCallbackList = NULL;
#if SDK_VERSION_MAJOR == 5
#ifdef SDK_TWL
#include <twl/ltdmain_begin.h>
static PMExitCallbackInfo *PMi_PreExitCallbackList = NULL;
static PMExitCallbackInfo *PMi_PostExitCallbackList = NULL;
#ifdef SDK_PORT
static PMBatteryLowCallbackInfo PMi_BatteryLowCallbackInfo;
#else
static PMBatteryLowCallbackInfo PMi_BatteryLowCallbackInfo = {NULL, NULL, NULL};
#endif
#include <twl/ltdmain_end.h>
#endif
#endif

static u32 PMi_SetAmp(PMAmpSwitch status);
static PMAmpSwitch sAmpSwitch = PM_AMP_OFF;

#if SDK_VERSION_MAJOR == 4
#ifndef SDK_PORT
static 
#endif
BOOL PMi_Lock (void)
{
    OSIntrMode enabled = OS_DisableInterrupts();

    if (PMi_Work.lock) {
        (void)OS_RestoreInterrupts(enabled);
        return FALSE;
    }

    PMi_Work.lock = TRUE;
    (void)OS_RestoreInterrupts(enabled);

    return TRUE;
}
#endif

#if SDK_VERSION_MAJOR == 5
#ifdef SDK_TWL
static BOOL PMi_AutoExitFlag = TRUE;
#ifndef SDK_FINALROM
static BOOL PMi_ExitSequenceFlag = FALSE;
#endif
static PMExitFactor PMi_ExitFactor = PM_EXIT_FACTOR_NONE;
#endif
static u32 PMi_PreDmaCnt[4];

#define PMi_WAITBUSY_METHOD_CPUMODE (1 << 1)
#define PMi_WAITBUSY_METHOD_CPSR (1 << 2)
#define PMi_WAITBUSY_METHOD_IME (1 << 3)
static BOOL PMi_WaitBusyMethod = PMi_WAITBUSY_METHOD_CPUMODE;
#endif

extern void PXIi_HandlerRecvFifoNotEmpty(void);

#ifndef SDK_PORT
static
#endif
void PMi_WaitBusy (void)
{
    volatile BOOL * p = &PMi_Work.lock;

    #ifndef SDK_PORT
    while (*p) {
        if (OS_GetCpsrIrq() == OS_INTRMODE_IRQ_DISABLE) {
            PXIi_HandlerRecvFifoNotEmpty();
        }
    }
    #endif
}

#ifndef SDK_PORT
static
#endif
void PMi_DummyCallback (u32 result, void * arg)
{
    #if SDK_VERSION_MAJOR == 5
    if (arg) {
    #endif
      *(u32 *)arg = result;
    #if SDK_VERSION_MAJOR == 5
    }
    #endif
}

static void PMi_CallCallbackAndUnlock (u32 result)
{
    PMCallback callback;
    void * arg;

    callback = PMi_Work.callback;
    arg = PMi_Work.callbackArg;

    if (PMi_Work.lock) {
        PMi_Work.lock = FALSE;
    }

    if (callback) {
        PMi_Work.callback = NULL;
        (callback) (result, arg);
    }
}

#if SDK_VERSION_MAJOR == 5
static void PMi_WaitVBlank(void) {
  vu32 vcount = OS_GetVBlankCount();
  while (vcount == OS_GetVBlankCount()) {
  }
}
#endif

void PM_Init (void)
{
    #if SDK_VERSION_MAJOR == 4
    int i;
    #endif
    #if SDK_VERSION_MAJOR == 5
    static u16 PMi_IsInit = FALSE;
    #endif

    if (PMi_IsInit) {
        return;
    }

    PMi_IsInit = TRUE;
    PMi_Work.lock = FALSE;
    PMi_Work.callback = NULL;

    #if SDK_VERSION_MAJOR == 5
    #ifdef SDK_TWL
    *(u32 *)HW_RESET_LOCK_FLAG_BUF = PM_RESET_FLAG_NONE;
    #endif
    #endif

    PXI_Init();
    #ifndef SDK_PORT
    while (!PXI_IsCallbackReady(PXI_FIFO_TAG_PM, PXI_PROC_ARM7)) {
    }
    #endif

    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_PM, PMi_CommonCallback);

    #if SDK_VERSION_MAJOR == 4
    for (i = 0; i < PMIC_REG_NUMS; i++) {
        PMi_RegisterBuffer[i].flag = FALSE;
    }

    OS_InitMutex(&PMi_Mutex);

    PMi_LCDCount = OS_GetVBlankCount();
    #endif
    #if SDK_VERSION_MAJOR == 5
    PMi_LCDCount = PMi_DispOffCount = OS_GetVBlankCount();
    #endif
}

void PMi_CommonCallback (PXIFifoTag tag, uPtr data, BOOL err)
{
#pragma unused(tag)

    u16 command;
    u16 pxiResult;
    #if SDK_VERSION_MAJOR == 5
    BOOL callCallback = TRUE;
    #endif

    #if SDK_VERSION_MAJOR == 4
    if (err) {
        PMi_CallCallbackAndUnlock(PM_RESULT_ERROR);
        return;
    }
    #endif

    command = (u16)((data & SPI_PXI_RESULT_COMMAND_MASK) >> SPI_PXI_RESULT_COMMAND_SHIFT);
    pxiResult = (u16)((data & SPI_PXI_RESULT_DATA_MASK) >> SPI_PXI_RESULT_DATA_SHIFT);

    #if SDK_VERSION_MAJOR == 4
    if (SPI_PXI_COMMAND_PM_REG0VALUE <= command && command <= SPI_PXI_COMMAND_PM_REG4VALUE) {
        int num = (int)(command - SPI_PXI_COMMAND_PM_REG0VALUE);
        u16 value = (u16)(pxiResult & 0xff);
        u16 * buffer = PMi_RegisterBuffer[num].buffer;

        if (buffer) {
            *buffer = value;
        }
        PMi_RegisterBuffer[num].flag = TRUE;
        pxiResult = PM_RESULT_SUCCESS;
    } else if (command == SPI_PXI_COMMAND_PM_SYNC)   {
        PMi_SyncFlag = TRUE;
    } else if (command == SPI_PXI_COMMAND_PM_SLEEP_END)   {
        PMi_SleepEndFlag = TRUE;
    } else if (command == SPI_PXI_COMMAND_PM_GET_BLINK)   {
        if (PMi_Work.work) {
            *(u32 *)PMi_Work.work = pxiResult;
        }
        pxiResult = PM_RESULT_SUCCESS;
    }

    PMi_CallCallbackAndUnlock(pxiResult);
    #endif

    #if SDK_VERSION_MAJOR == 5
  if (err) {
    switch (command) {
    case SPI_PXI_COMMAND_PM_SLEEP_START:
    case SPI_PXI_COMMAND_PM_UTILITY:
      pxiResult = PM_RESULT_BUSY;
      break;

    default:
      pxiResult = PM_RESULT_ERROR;
    }

    PMi_CallCallbackAndUnlock(pxiResult);
    return;
  }

  switch (command) {
  case SPI_PXI_COMMAND_PM_SLEEP_START:

    break;

  case SPI_PXI_COMMAND_PM_UTILITY:
    if (PMi_Work.work) {
      *(u16 *)PMi_Work.work = (u16)pxiResult;
    }
    pxiResult = (u16)PM_RESULT_SUCCESS;
    break;

  case SPI_PXI_COMMAND_PM_SYNC:
    pxiResult = (u16)PM_RESULT_SUCCESS;
    break;

  case SPI_PXI_COMMAND_PM_SLEEP_END:
    PMi_SleepEndFlag = TRUE;
    break;

#ifdef SDK_TWL

  case SPI_PXI_COMMAND_PM_NOTIFY:
    switch (pxiResult) {
    case PM_NOTIFY_POWER_SWITCH:
      OS_TPrintf("[ARM9] Pushed power button.\n");
      PMi_ProceedToExit(PM_EXIT_FACTOR_PWSW);
      *(u32 *)HW_RESET_LOCK_FLAG_BUF = PM_RESET_FLAG_FORCED;
      break;

    case PM_NOTIFY_SHUTDOWN:
      OS_TPrintf("[ARM9] Shutdown\n");

      break;

    case PM_NOTIFY_RESET_HARDWARE:
      OS_TPrintf("[ARM9] Reset Hardware\n");

      break;
    case PM_NOTIFY_BATTERY_LOW:
      OS_TPrintf("[ARM9] Battery low\n");
      if (PMi_BatteryLowCallbackInfo.callback) {
        (PMi_BatteryLowCallbackInfo.callback)(PMi_BatteryLowCallbackInfo.arg);
      }
      break;
    case PM_NOTIFY_BATTERY_EMPTY:
      OS_TPrintf("[ARM9] Battery empty\n");
      PMi_ProceedToExit(PM_EXIT_FACTOR_BATTERY);
      *(u32 *)HW_RESET_LOCK_FLAG_BUF = PM_RESET_FLAG_FORCED;
      break;
    default:
      OS_TPrintf("[ARM9] unknown %x\n", pxiResult);
      break;
    }

    callCallback = FALSE;
    break;

#endif /* SDK_TWL */
  }

  if (callCallback) {
    PMi_CallCallbackAndUnlock(pxiResult);
  }
    #endif /* SDK_VERSION_MAJOR */
}

#if SDK_VERSION_MAJOR == 5
static u32 PMi_TryToSendPxiData(u32 *sendData, int num, u16 *retValue,
                                PMCallback callback, void *arg) {
  int n;
  OSIntrMode enabled = OS_DisableInterrupts();

  if (PMi_Work.lock) {
    (void)OS_RestoreInterrupts(enabled);
    return PM_BUSY;
  }
  PMi_Work.lock = TRUE;

  PMi_Work.work = (void *)retValue;
  PMi_Work.callback = callback;
  PMi_Work.callbackArg = arg;

  for (n = 0; n < num; n++) {
    PMi_SendPxiData(sendData[n]);
  }

  (void)OS_RestoreInterrupts(enabled);
  return PM_SUCCESS;
}

#define PMi_UNUSED_RESULT 0xffff0000 // Value that should never be returned
void PMi_TryToSendPxiDataTillSuccess(u32 *sendData, int num) {
  volatile u32 result;
  while (1) {
    result = PMi_UNUSED_RESULT;
    while (PMi_TryToSendPxiData(sendData, num, NULL, PMi_DummyCallback,
                                (void *)&result) != PM_SUCCESS) {
      OS_SpinWait(HW_CPU_CLOCK_ARM9 / 100);
    }

    while (result == PMi_UNUSED_RESULT) {
      OS_SpinWait(HW_CPU_CLOCK_ARM9 / 100);
    }
    if (result == SPI_PXI_RESULT_SUCCESS) {
      break;
    }

    OS_SpinWait(HW_CPU_CLOCK_ARM9 / 100);
  }
}
#endif

u32 PMi_SendSleepStart (u16 trigger, u16 keyIntrData)
{
    #if SDK_VERSION_MAJOR == 4
    u32 pxi_send_data;

    if (!PMi_Lock()) {
        return PM_BUSY;
    }

    PMi_SyncFlag = 0;
    pxi_send_data = PMi_MakeData1(SPI_PXI_START_BIT | SPI_PXI_END_BIT, 0, SPI_PXI_COMMAND_PM_SYNC, 0);
    PMi_SendPxiData(pxi_send_data);

    while (PMi_SyncFlag == 0) {
    }

    PMi_SyncFlag = 0;
    PMi_SleepEndFlag = 0;

    (void)PMi_SetLCDPower(PM_LCD_POWER_OFF, PM_LED_BLINK_LOW, FALSE, TRUE);

    pxi_send_data = PMi_MakeData1(SPI_PXI_START_BIT, 0, SPI_PXI_COMMAND_PM_SLEEP_START, trigger);
    PMi_SendPxiData(pxi_send_data);

    pxi_send_data = PMi_MakeData2(SPI_PXI_END_BIT, 1, keyIntrData);
    PMi_SendPxiData(pxi_send_data);
    #endif
    #if SDK_VERSION_MAJOR == 5
    u32 sendData[2];

    sendData[0] = PMi_MakeData1(SPI_PXI_START_BIT | SPI_PXI_END_BIT, 0,
                                SPI_PXI_COMMAND_PM_SYNC, 0);
    PMi_TryToSendPxiDataTillSuccess(sendData, 1);

    while (PMi_SetLCDPower(PM_LCD_POWER_OFF, PM_LED_BLINK_LOW, FALSE, TRUE) !=
           TRUE) {
    }

    sendData[0] = PMi_MakeData1(SPI_PXI_START_BIT, 0,
                                SPI_PXI_COMMAND_PM_SLEEP_START, trigger);
    sendData[1] = PMi_MakeData2(SPI_PXI_END_BIT, 1, keyIntrData);
    PMi_TryToSendPxiDataTillSuccess(sendData, 2);
    #endif

    return PM_SUCCESS;
}

u32 PM_SendUtilityCommandAsync (
    u32 number, 
#if SDK_VERSION_MAJOR == 5
    u16 parameter,
    u16 * retValue,
#endif
    PMCallback callback, 
    void * arg)
{
    #if SDK_VERSION_MAJOR == 4
    u32 pxi_send_data;

    if (!PMi_Lock()) {
        return PM_BUSY;
    }

    PMi_SetCallback(callback, arg);

    pxi_send_data = PMi_MakeData1(SPI_PXI_START_BIT, 0, SPI_PXI_COMMAND_PM_UTILITY, number >> 16);
    PMi_SendPxiData(pxi_send_data);

    pxi_send_data = PMi_MakeData2(SPI_PXI_END_BIT, 1, number);
    PMi_SendPxiData(pxi_send_data);

    return PM_SUCCESS;
    #endif
    #if SDK_VERSION_MAJOR == 5
    u32 sendData[2];

    sendData[0] =
        PMi_MakeData1(SPI_PXI_START_BIT, 0, SPI_PXI_COMMAND_PM_UTILITY, number);
    sendData[1] = PMi_MakeData2(SPI_PXI_END_BIT, 1, parameter);

    return PMi_TryToSendPxiData(sendData, 2, retValue, callback, arg);
    #endif
}

u32 PM_SendUtilityCommand (
    u32 number
    #if SDK_VERSION_MAJOR == 5
    ,
    u16 parameter,
    u16 * retValue
    #endif
)
{
    u32 commandResult;
    #if SDK_VERSION_MAJOR == 4
    u32 sendResult = PM_SendUtilityCommandAsync(number, PMi_DummyCallback, &commandResult);
    #endif
    #if SDK_VERSION_MAJOR == 5
    u32 sendResult = PM_SendUtilityCommandAsync(number, parameter, retValue, PMi_DummyCallback, &commandResult);
    #endif

    if (sendResult == PM_SUCCESS) {
        PMi_WaitBusy();
        return commandResult;
    }

    return sendResult;
}

u32 PMi_ReadRegisterAsync (u16 registerAddr, u16 * buffer, PMCallback callback, void * arg)
{
    #if SDK_VERSION_MAJOR == 4
    u32 pxi_send_data;

    if (!PMi_Lock()) {
        return PM_BUSY;
    }

    PMi_SetCallback(callback, arg);

    PMi_RegisterBuffer[registerAddr].flag = FALSE;
    PMi_RegisterBuffer[registerAddr].buffer = buffer;

    pxi_send_data = PMi_MakeData1(SPI_PXI_START_BIT | SPI_PXI_END_BIT, 0, SPI_PXI_COMMAND_PM_REG_READ, registerAddr);
    PMi_SendPxiData(pxi_send_data);

    return PM_SUCCESS;
    #endif
    #if SDK_VERSION_MAJOR == 5
    return PM_SendUtilityCommandAsync(PMi_UTIL_READREG, registerAddr, buffer,
                                  callback, arg);
    #endif
}

u32 PMi_ReadRegister (u16 registerAddr, u16 * buffer)
{
    #if SDK_VERSION_MAJOR == 4
    u32 commandResult;
    u32 sendResult =
        PMi_ReadRegisterAsync(registerAddr, buffer, PMi_DummyCallback, &commandResult);
    if (sendResult == PM_SUCCESS) {
        PMi_WaitBusy();
        return commandResult;
    }
    return sendResult;
    #endif
    #if SDK_VERSION_MAJOR == 5
    return PM_SendUtilityCommand(PMi_UTIL_READREG, registerAddr, buffer);
    #endif
}

u32 PMi_WriteRegisterAsync (u16 registerAddr, u16 data, PMCallback callback, void * arg)
{
    #if SDK_VERSION_MAJOR == 4
    u32 pxi_send_data;

    if (!PMi_Lock()) {
        return PM_BUSY;
    }

    PMi_SetCallback(callback, arg);

    pxi_send_data = PMi_MakeData1(SPI_PXI_START_BIT, 0, SPI_PXI_COMMAND_PM_REG_WRITE, registerAddr);
    PMi_SendPxiData(pxi_send_data);

    pxi_send_data = PMi_MakeData2(SPI_PXI_END_BIT, 1, data);
    PMi_SendPxiData(pxi_send_data);

    return PM_SUCCESS;
    #endif
    #if SDK_VERSION_MAJOR == 5
    return PM_SendUtilityCommandAsync(PMi_UTIL_WRITEREG,
                                  (u16)((registerAddr << 8) | (data & 0xff)),
                                  NULL, callback, arg);
    #endif
}

u32 PMi_WriteRegister (u16 registerAddr, u16 data)
{
    #if SDK_VERSION_MAJOR == 4
    u32 commandResult;
    u32 sendResult =
        PMi_WriteRegisterAsync(registerAddr, data, PMi_DummyCallback, &commandResult);
    if (sendResult == PM_SUCCESS) {
        PMi_WaitBusy();
        return commandResult;
    }
    return sendResult;
    #endif
    #if SDK_VERSION_MAJOR == 5
    return PM_SendUtilityCommand(
    PMi_UTIL_WRITEREG, (u16)((registerAddr << 8) | (data & 0xff)), NULL);
    #endif
}

u32 PMi_SetLEDAsync (PMLEDStatus status, PMCallback callback, void * arg)
{
    u32 command;

    switch (status) {
    case PM_LED_ON:
        command = PM_UTIL_LED_ON;
        break;
    case PM_LED_BLINK_HIGH:
        command = PM_UTIL_LED_BLINK_HIGH_SPEED;
        break;
    case PM_LED_BLINK_LOW:
        command = PM_UTIL_LED_BLINK_LOW_SPEED;
        break;
    default:
        command = 0;
    }

    #if SDK_VERSION_MAJOR == 4
    return (command) ? PM_SendUtilityCommandAsync(command, callback, arg) : PM_INVALID_COMMAND;
    #endif
    #if SDK_VERSION_MAJOR == 5
    return (command) ? PM_SendUtilityCommandAsync(command, 0, NULL, callback, arg)
                     : PM_INVALID_COMMAND;
    #endif
}

u32 PMi_SetLED (PMLEDStatus status)
{
    u32 commandResult;
    u32 sendResult = PMi_SetLEDAsync(status, PMi_DummyCallback, &commandResult);

    if (sendResult == PM_SUCCESS) {
        PMi_WaitBusy();
        return commandResult;
    }

    return sendResult;
}

u32 PM_SetBackLightAsync (PMLCDTarget target, PMBackLightSwitch sw, PMCallback callback, void * arg)
{
    u32 command = 0;

    if (target == PM_LCD_TOP) {
        if (sw == PM_BACKLIGHT_ON) {
            command = PM_UTIL_LCD2_BACKLIGHT_ON;
        }
        if (sw == PM_BACKLIGHT_OFF) {
            command = PM_UTIL_LCD2_BACKLIGHT_OFF;
        }
    } else if (target == PM_LCD_BOTTOM)   {
        if (sw == PM_BACKLIGHT_ON) {
            command = PM_UTIL_LCD1_BACKLIGHT_ON;
        }
        if (sw == PM_BACKLIGHT_OFF) {
            command = PM_UTIL_LCD1_BACKLIGHT_OFF;
        }
    } else if (target == PM_LCD_ALL)   {
        if (sw == PM_BACKLIGHT_ON) {
            command = PM_UTIL_LCD12_BACKLIGHT_ON;
        }
        if (sw == PM_BACKLIGHT_OFF) {
            command = PM_UTIL_LCD12_BACKLIGHT_OFF;
        }
    }

    #if SDK_VERSION_MAJOR == 4
    return (command) ? PM_SendUtilityCommandAsync(command, callback, arg) : PM_INVALID_COMMAND;
    #endif
    #if SDK_VERSION_MAJOR == 5
    return (command) ? PM_SendUtilityCommandAsync(command, 0, NULL, callback, arg)
                     : PM_INVALID_COMMAND;
    #endif
}

u32 PM_SetBackLight (PMLCDTarget target, PMBackLightSwitch sw)
{
    u32 commandResult;
    u32 sendResult = PM_SetBackLightAsync(target, sw, PMi_DummyCallback, &commandResult);

    if (sendResult == PM_SUCCESS) {
        PMi_WaitBusy();
        return commandResult;
    }

    return sendResult;
}

u32 PMi_SetSoundPowerAsync (PMSoundPowerSwitch sw, PMCallback callback, void * arg)
{
    u32 command;

    switch (sw) {
    case PM_SOUND_POWER_ON:
        command = PM_UTIL_SOUND_POWER_ON;
        break;
    case PM_SOUND_POWER_OFF:
        command = PM_UTIL_SOUND_POWER_OFF;
        break;
    default:
        command = 0;
    }

    #if SDK_VERSION_MAJOR == 4
    return (command) ? PM_SendUtilityCommandAsync(command, callback, arg) : PM_INVALID_COMMAND;
    #endif
    #if SDK_VERSION_MAJOR == 5
    return (command) ? PM_SendUtilityCommandAsync(command, 0, NULL, callback, arg)
                     : PM_INVALID_COMMAND;
    #endif
}

u32 PMi_SetSoundPower (PMSoundPowerSwitch sw)
{
    u32 commandResult;
    u32 sendResult = PMi_SetSoundPowerAsync(sw, PMi_DummyCallback, &commandResult);

    if (sendResult == PM_SUCCESS) {
        PMi_WaitBusy();
        return commandResult;
    }

    return sendResult;
}

u32 PMi_SetSoundVolumeAsync (PMSoundVolumeSwitch sw, PMCallback callback, void * arg)
{
    u32 command;

    switch (sw) {
    case PM_SOUND_VOLUME_ON:
        command = PM_UTIL_SOUND_VOL_CTRL_ON;
        break;
    case PM_SOUND_VOLUME_OFF:
        command = PM_UTIL_SOUND_VOL_CTRL_OFF;
        break;
    default:
        command = 0;
    }

    #if SDK_VERSION_MAJOR == 4
    return (command) ? PM_SendUtilityCommandAsync(command, callback, arg) : PM_INVALID_COMMAND;
    #endif
    #if SDK_VERSION_MAJOR == 5
    return (command) ? PM_SendUtilityCommandAsync(command, 0, NULL, callback, arg)
                     : PM_INVALID_COMMAND;
    #endif
}

u32 PMi_SetSoundVolume (PMSoundVolumeSwitch sw)
{
    u32 commandResult;
    u32 sendResult = PMi_SetSoundVolumeAsync(sw, PMi_DummyCallback, &commandResult);

    if (sendResult == PM_SUCCESS) {
        PMi_WaitBusy();
        return commandResult;
    }

    return sendResult;
}

u32 PM_ForceToPowerOffAsync (PMCallback callback, void * arg)
{
    #if SDK_VERSION_MAJOR == 4
    PMLCDPower LCDResult;
    PMBackLightSwitch top;
    PMBackLightSwitch bottom;

    OS_SpinWait(PMi_LCD_POWER_WAIT_TICK);
    LCDResult = PM_GetLCDPower();

    if (LCDResult != PM_LCD_POWER_ON) {
        (void)PM_GetBackLight(&top, &bottom);
        if (top != PM_BACKLIGHT_OFF)
            (void)PM_SetBackLight(PM_LCD_TOP, PM_BACKLIGHT_OFF);
        if (bottom != PM_BACKLIGHT_OFF)
            (void)PM_SetBackLight(PM_LCD_BOTTOM, PM_BACKLIGHT_OFF);

        while (!PM_SetLCDPower(PM_LCD_POWER_ON)) {
            OS_SpinWait(PMi_LCD_POWER_WAIT_TICK);
        }
    }

    return PM_SendUtilityCommandAsync(PM_UTIL_FORCE_POWER_OFF, callback, arg);
    #endif
    #if SDK_VERSION_MAJOR == 5
    #ifdef SDK_TWL
    PMi_ExitFactor = PM_EXIT_FACTOR_USER;
    #endif

    PMi_LCDOnAvoidReset();

    #ifdef SDK_TWL
    if (OS_IsRunOnTwl()) {
      PMi_ExecuteList(PMi_PostExitCallbackList);
    }
    #endif
    return PM_SendUtilityCommandAsync(PM_UTIL_FORCE_POWER_OFF, 0, NULL, callback,
                                      arg);
    #endif
}

u32 PM_ForceToPowerOff (void)
{
    u32 commandResult;
    u32 sendResult = PM_ForceToPowerOffAsync(PMi_DummyCallback, &commandResult);

    if (sendResult == PM_SUCCESS) {
        PMi_WaitBusy();
        return commandResult;
    }

    return sendResult;
}

u32 PM_SetAmpAsync (PMAmpSwitch status, PMCallback callback, void * arg)
{
    #if SDK_VERSION_MAJOR == 4
    return PMi_WriteRegisterAsync(REG_PMIC_OP_CTL_ADDR, (u16)status, callback, arg);
    #endif
    #if SDK_VERSION_MAJOR == 5
    return PM_SendUtilityCommandAsync(PM_UTIL_SET_AMP, (u16)status, NULL,
                                      callback, arg);
    #endif
}

u32 PM_SetAmp (PMAmpSwitch status)
{
    sAmpSwitch = status;
    return PMi_SetAmp(status);
}

static u32 PMi_SetAmp (PMAmpSwitch status)
{
    if (PM_GetLCDPower()) {
        #if SDK_VERSION_MAJOR == 4
        return PMi_WriteRegister(REG_PMIC_OP_CTL_ADDR, (u16)status);
        #endif
        #if SDK_VERSION_MAJOR == 5
        return PM_SendUtilityCommand(PM_UTIL_SET_AMP, (u16)status, NULL);
        #endif
    } else {
        return PM_RESULT_SUCCESS;
    }
}

u32 PM_SetAmpGainAsync (PMAmpGain status, PMCallback callback, void * arg)
{
    #if SDK_VERSION_MAJOR == 4
    return PMi_WriteRegisterAsync(REG_PMIC_PGA_GAIN_ADDR, (u16)status, callback, arg);
    #endif
    #if SDK_VERSION_MAJOR == 5
    return PM_SendUtilityCommandAsync(PM_UTIL_SET_AMPGAIN, (u16)status, NULL,
                                      callback, arg);
    #endif
}

u32 PM_SetAmpGain (PMAmpGain status)
{
    #if SDK_VERSION_MAJOR == 4
    return PMi_WriteRegister(REG_PMIC_PGA_GAIN_ADDR, (u16)status);
    #endif
    #if SDK_VERSION_MAJOR == 5
    return PM_SendUtilityCommand(PM_UTIL_SET_AMPGAIN, (u16)status, NULL);
    #endif
}

#if SDK_VERSION_MAJOR == 5
#ifdef SDK_TWL
u32 PM_SetAmpGainLevelAsync(u8 level, PMCallback callback, void *arg) {
  SDK_ASSERT(level <= PM_AMPGAIN_LEVEL_MAX);
  return PM_SendUtilityCommandAsync(PM_UTIL_SET_AMPGAIN_LEVEL, (u16)level, NULL,
                                    callback, arg);
}

u32 PM_SetAmpGainLevel(u8 level) {
  SDK_ASSERT(level <= PM_AMPGAIN_LEVEL_MAX);
  return PM_SendUtilityCommand(PM_UTIL_SET_AMPGAIN_LEVEL, (u16)level, NULL);
}
#endif
#endif

u32 PM_GetBattery (PMBattery * batteryBuf)
{
    #if SDK_VERSION_MAJOR == 4
    u16 reg;
    u32 result;

    if ((result = PMi_ReadRegister(REG_PMIC_STAT_ADDR, &reg)) == PM_SUCCESS) {
        if (batteryBuf) {
            *batteryBuf =
                (PMBattery)(reg & PMi_STAT_BATTERY_MASK) ? PM_BATTERY_LOW : PM_BATTERY_HIGH;
        }
    }
    #endif
    #if SDK_VERSION_MAJOR == 5
    u16 status;
    u32 result =
        PM_SendUtilityCommand(PM_UTIL_GET_STATUS, PM_UTIL_PARAM_BATTERY, &status);

    if (result == PM_RESULT_SUCCESS) {
      if (batteryBuf) {
        *batteryBuf = status ? PM_BATTERY_LOW : PM_BATTERY_HIGH;
      }
    }
    #endif

    return result;
}

#if SDK_VERSION_MAJOR == 5
#ifdef SDK_TWL
#include <twl/ltdmain_begin.h>
static u32 PMi_GetBatteryLevelCore(PMBatteryLevel *levelBuf) {
  u16 status;
  u32 result = PM_SendUtilityCommand(PM_UTIL_GET_STATUS,
                                     PM_UTIL_PARAM_BATTERY_LEVEL, &status);

  if (result == PM_RESULT_SUCCESS) {
    if (levelBuf) {
      *levelBuf = (PMBatteryLevel)status;
    }
  }
  return result;
}
#include <twl/ltdmain_end.h>

u32 PM_GetBatteryLevel(PMBatteryLevel *levelBuf) {
  if (OS_IsRunOnTwl()) {
    return PMi_GetBatteryLevelCore(levelBuf);
  } else {
    return PM_RESULT_ERROR;
  }
}
#endif

#ifdef SDK_TWL
#include <twl/ltdmain_begin.h>
static u32 PMi_GetACAdapterCore(BOOL *isConnectedBuf) {
  u16 status;
  u32 result = PM_SendUtilityCommand(PM_UTIL_GET_STATUS,
                                     PM_UTIL_PARAM_AC_ADAPTER, &status);

  if (result == PM_RESULT_SUCCESS) {
    if (isConnectedBuf) {
      *isConnectedBuf = status ? TRUE : FALSE;
    }
  }
  return result;
}
#include <twl/ltdmain_end.h>

u32 PM_GetACAdapter(BOOL *isConnectedBuf) {
  if (OS_IsRunOnTwl()) {
    return PMi_GetACAdapterCore(isConnectedBuf);
  } else {
    return PM_RESULT_ERROR;
  }
}
#endif
#endif

u32 PM_GetBackLight (PMBackLightSwitch * top, PMBackLightSwitch * bottom)
{
    u16 reg;
    u32 result;

    if ((result = PMi_ReadRegister(REG_PMIC_CTL_ADDR, &reg)) == PM_SUCCESS) {
        if (top) {
            *top = (reg & PMIC_CTL_BKLT2) ? PM_BACKLIGHT_ON : PM_BACKLIGHT_OFF;
        }
        if (bottom) {
            *bottom = (reg & PMIC_CTL_BKLT1) ? PM_BACKLIGHT_ON : PM_BACKLIGHT_OFF;
        }
    }

    return result;
}

u32 PMi_GetSoundPower (PMSoundPowerSwitch * swBuf)
{
    u16 reg;
    u32 result;

    if ((result = PMi_ReadRegister(REG_PMIC_CTL_ADDR, &reg)) == PM_SUCCESS) {
        if (swBuf) {
            *swBuf = (reg & PMIC_CTL_SND_PWR) ? PM_SOUND_POWER_ON : PM_SOUND_POWER_OFF;
        }
    }

    return result;
}

u32 PMi_GetSoundVolume (PMSoundVolumeSwitch * swBuf)
{
    u16 reg;
    u32 result;

    if ((result = PMi_ReadRegister(REG_PMIC_CTL_ADDR, &reg)) == PM_SUCCESS) {
        if (swBuf) {
            *swBuf = (reg & PMIC_CTL_SND_VOLCTRL) ? PM_SOUND_VOLUME_ON : PM_SOUND_VOLUME_OFF;
        }
    }

    return result;
}

u32 PM_GetAmp (PMAmpSwitch * swBuf)
{
    u16 reg;
    u32 result;

    if ((result = PMi_ReadRegister(REG_PMIC_OP_CTL_ADDR, &reg)) == PM_SUCCESS) {
        if (swBuf) {
            *swBuf = (PMAmpSwitch)reg;
        }
    }

    return result;
}

u32 PM_GetAmpGain (PMAmpGain * gainBuf)
{
    u16 reg;
    u32 result;

    if ((result = PMi_ReadRegister(REG_PMIC_PGA_GAIN_ADDR, &reg)) == PM_SUCCESS) {
        if (gainBuf) {
            *gainBuf = (PMAmpGain)reg;
        }
    }

    return result;
}

void PMi_SendPxiData (u32 data)
{
    while (PXI_SendWordByFifo(PXI_FIFO_TAG_PM, data, FALSE) != PXI_FIFO_SUCCESS) {

    }
}

void PM_GoSleepMode (PMWakeUpTrigger trigger, PMLogic logic, u16 keyPattern)
{
    BOOL prepIrq;
    OSIntrMode prepIntrMode;
    OSIrqMask prepIntrMask;
    BOOL powerOffFlag = FALSE;
    PMBackLightSwitch preTop;
    PMBackLightSwitch preBottom;
    u32 preGX;
    u32 preGXS;
    PMLCDPower preLCDPower;

    PMi_ExecuteList(PMi_PreSleepCallbackList);

    prepIrq = OS_DisableIrq();
    prepIntrMode = OS_DisableInterrupts();
    prepIntrMask = OS_DisableIrqMask((1 << OS_IRQ_TABLE_MAX) - 1);

    {

        OSIntrMode intr = OS_IE_FIFO_RECV | (OS_IsTickAvailable()? OS_IE_TIMER0: 0);
        (void)OS_SetIrqMask(intr);
    }

    (void)OS_RestoreInterrupts(prepIntrMode);
    (void)OS_EnableIrq();

    if (trigger & PM_TRIGGER_CARD) {
        if (MB_IsMultiBootChild()) {
            trigger &= ~PM_TRIGGER_CARD;
        }
    }

    if (trigger & PM_TRIGGER_CARTRIDGE) {
        if (!CTRDG_IsExisting()) {
            trigger &= ~PM_TRIGGER_CARTRIDGE;
        }
    }

    preGX = reg_GX_DISPCNT;
    preGXS = reg_GXS_DB_DISPCNT;
    preLCDPower = PM_GetLCDPower();

    (void)PM_GetBackLight(&preTop, &preBottom);
    (void)PM_SetBackLight(PM_LCD_ALL, PM_BACKLIGHT_OFF);

    {
        vu32 vcount = OS_GetVBlankCount();
        while (vcount == OS_GetVBlankCount()) {
        }
        vcount = OS_GetVBlankCount();

        reg_GX_DISPCNT = reg_GX_DISPCNT & ~REG_GX_DISPCNT_MODE_MASK;
        GXS_DispOff();

        while (vcount == OS_GetVBlankCount()) {
        }
        vcount = OS_GetVBlankCount();
        while (vcount == OS_GetVBlankCount()) {
        }
    }

    {
        u16 param;
        param = (u16)(trigger
                      | preTop << PM_BACKLIGHT_RECOVER_TOP_SHIFT
                      | preBottom << PM_BACKLIGHT_RECOVER_BOTTOM_SHIFT);
        while (PMi_SendSleepStart(param, (u16)(logic | keyPattern)) != PM_SUCCESS) {
        }
    }

    #ifndef SDK_PORT
    OS_Halt();
    #endif

    if ((trigger & PM_TRIGGER_CARD) && (OS_GetRequestIrqMask() & OS_IE_CARD_IREQ)) {
        powerOffFlag = TRUE;
    }

    if (!powerOffFlag) {
        if (preLCDPower == PM_LCD_POWER_ON) {
            (void)PMi_SetLCDPower(PM_LCD_POWER_ON, PM_LED_ON, TRUE, TRUE);
        } else {
            (void)PMi_SetLED(PM_LED_ON);
        }

        reg_GX_DISPCNT = preGX;
        reg_GXS_DB_DISPCNT = preGXS;
    }

    #if SDK_VERSION_MAJOR == 4
    OS_SpinWait(PMi_LCD_SLEEP_WAIT_TICK);
    #endif
    #if SDK_VERSION_MAJOR == 5
    OS_SpinWaitSysCycles(PMi_LCD_WAIT_SYS_CYCLES);
    #endif

    (void)OS_DisableInterrupts();
    (void)OS_SetIrqMask(prepIntrMask);
    (void)OS_RestoreInterrupts(prepIntrMode);
    (void)OS_RestoreIrq(prepIrq);

    if (powerOffFlag) {
        (void)PM_ForceToPowerOff();
    }

    PMi_ExecuteList(PMi_PostSleepCallbackList);
}

#define PMi_WAIT_FRAME  7

BOOL PMi_SetLCDPower (PMLCDPower sw, PMLEDStatus led, BOOL skip, BOOL isSync)
{
    switch (sw) {
    case PM_LCD_POWER_ON:
        if (!skip && OS_GetVBlankCount() - PMi_LCDCount <= PMi_WAIT_FRAME) {
            return FALSE;
        }

        if (led != PM_LED_NONE) {
            if (isSync) {
                (void)PMi_SetLED(led);
            } else {
                (void)PMi_SetLEDAsync(led, NULL, NULL);
            }
        }

        (void)GXi_PowerLCD(TRUE);
        (void)PMi_SetAmp(sAmpSwitch);
        break;
    case PM_LCD_POWER_OFF:
        (void)PMi_SetAmp(PM_AMP_OFF);
        (void)GXi_PowerLCD(FALSE);

        PMi_LCDCount = OS_GetVBlankCount();

        if (led != PM_LED_NONE) {
            if (isSync) {
                (void)PMi_SetLED(led);
            } else {
                (void)PMi_SetLEDAsync(led, NULL, NULL);
            }
        }
        break;
    default:
        break;
    }

    return TRUE;
}

BOOL PM_SetLCDPower (PMLCDPower sw)
{
    if (sw != PM_LCD_POWER_ON) {
        sw = PM_LCD_POWER_OFF;
    }

    return PMi_SetLCDPower(sw, PM_LED_NONE, FALSE, TRUE);
}

PMLCDPower PM_GetLCDPower (void)
{
    return (reg_GX_POWCNT & REG_GX_POWCNT_LCD_MASK) ? PM_LCD_POWER_ON : PM_LCD_POWER_OFF;
}

u32 PMi_GetLCDOffCount (void)
{
    return PMi_LCDCount;
}

u32 PMi_SendLEDPatternCommandAsync (PMLEDPattern pattern, PMCallback callback, void * arg)
{
    #if SDK_VERSION_MAJOR == 4
    u32 pxi_send_data;

    if (!PMi_Lock()) {
        return PM_BUSY;
    }

    PMi_SetCallback(callback, arg);

    pxi_send_data = PMi_MakeData1(SPI_PXI_START_BIT | SPI_PXI_END_BIT, 0, SPI_PXI_COMMAND_PM_SELF_BLINK, pattern);
    PMi_SendPxiData(pxi_send_data);

    return PM_SUCCESS;
    #endif
    #if SDK_VERSION_MAJOR == 5
    return PM_SendUtilityCommandAsync(PM_UTIL_SET_BLINK, pattern, NULL, callback, arg);
    #endif
}

u32 PMi_SendLEDPatternCommand (PMLEDPattern pattern)
{
    #if SDK_VERSION_MAJOR == 4
    u32 commandResult;
    u32 sendResult = PMi_SendLEDPatternCommandAsync(pattern, PMi_DummyCallback, &commandResult);

    if (sendResult == PM_SUCCESS) {
        PMi_WaitBusy();
        return commandResult;
    }

    return sendResult;
    #endif
    #if SDK_VERSION_MAJOR == 5
    return PM_SendUtilityCommand(PM_UTIL_SET_BLINK, pattern, NULL);
    #endif
}

u32 PM_GetLEDPatternAsync (PMLEDPattern * patternBuf, PMCallback callback, void * arg)
{
    #if SDK_VERSION_MAJOR == 4
    u32 pxi_send_data;

    if (!PMi_Lock()) {
        return PM_BUSY;
    }

    PMi_SetCallback(callback, arg);
    PMi_Work.work = (void *)patternBuf;

    pxi_send_data = PMi_MakeData1(SPI_PXI_START_BIT | SPI_PXI_END_BIT, 0, SPI_PXI_COMMAND_PM_GET_BLINK, 0);
    PMi_SendPxiData(pxi_send_data);

    return PM_SUCCESS;
    #endif
    #if SDK_VERSION_MAJOR == 5
    return PM_SendUtilityCommandAsync(PM_UTIL_GET_STATUS, PM_UTIL_PARAM_BLINK,
                                      (u16 *)&patternBuf, callback, arg);
    #endif
}

u32 PM_GetLEDPattern (PMLEDPattern * patternBuf)
{
    #if SDK_VERSION_MAJOR == 4
    u32 commandResult;
    u32 sendResult = PM_GetLEDPatternAsync(patternBuf, PMi_DummyCallback, &commandResult);

    if (sendResult == PM_SUCCESS) {
        PMi_WaitBusy();
        return commandResult;
    }

    return sendResult;
    #endif
    #if SDK_VERSION_MAJOR == 5
    u16 status;
    u32 result =
        PM_SendUtilityCommand(PM_UTIL_GET_STATUS, PM_UTIL_PARAM_BLINK, &status);

    if (result == PM_RESULT_SUCCESS) {
      if (patternBuf) {
        *patternBuf = (PMLEDPattern)status;
      }
    }
    return result;
    #endif
}

void PMi_PrependList (PMSleepCallbackInfo ** listp, PMSleepCallbackInfo * info)
{
    if (!listp) {
        return;
    }

    info->next = *listp;
    *listp = info;
}

void PMi_AppendList (PMSleepCallbackInfo ** listp, PMSleepCallbackInfo * info)
{
    if (!listp) {
        return;
    }

    if (!*listp) {
        info->next = NULL;
        *listp = info;
    } else {
        PMSleepCallbackInfo * p = *listp;
        while (p->next) {
            p = p->next;
        }

        info->next = p->next;
        p->next = info;
    }
}

void PMi_DeleteList (PMSleepCallbackInfo ** listp, PMSleepCallbackInfo * info)
{
    PMSleepCallbackInfo * p = *listp;
    PMSleepCallbackInfo * pre;

    if (!listp) {
        return;
    }

    pre = p = *listp;
    while (p) {
        if (p == info) {
            if (p == pre) {
                *listp = p->next;
            } else {
                pre->next = p->next;
            }
            break;
        }

        pre = p;
        p = p->next;
    }
}

void PMi_ExecuteList (PMSleepCallbackInfo * listp)
{
    while (listp) {
        (listp->callback)(listp->arg);
        listp = listp->next;
    }
}

#if SDK_VERSION_MAJOR == 5
static void PMi_InsertList(PMGenCallbackInfo **listp, PMGenCallbackInfo *info,
                           int priority, int method) {
  OSIntrMode intr;
  PMGenCallbackInfo *p;
  PMGenCallbackInfo *pre;

  if (!listp) {
    return;
  }

  info->priority = priority;

  intr = OS_DisableInterrupts();
  p = *listp;
  pre = NULL;

  while (p) {

    if (method == PMi_COMPARE_GT && p->priority > priority) {
      break;
    }
    if (method == PMi_COMPARE_GE && p->priority >= priority) {
      break;
    }

    pre = p;
    p = p->next;
  }

  if (p) {
    info->next = p;
  } else {
    info->next = NULL;
  }

  if (pre) {
    pre->next = info;
  } else {

    *listp = info;
  }

  (void)OS_RestoreInterrupts(intr);
}

static void PMi_ClearList(PMGenCallbackInfo **listp) { listp = NULL; }

void PMi_InsertPreSleepCallbackEx(PMSleepCallbackInfo *info, int priority) {
  SDK_ASSERT(PM_CALLBACK_PRIORITY_SYSMIN <= priority &&
             priority <= PM_CALLBACK_PRIORITY_SYSMAX);
  PMi_InsertList(&PMi_PreSleepCallbackList, info, priority, PMi_COMPARE_GT);
}
void PM_InsertPreSleepCallback(PMSleepCallbackInfo *info, int priority) {
  SDK_ASSERT(PM_CALLBACK_PRIORITY_MIN <= priority &&
             priority <= PM_CALLBACK_PRIORITY_MAX);
  PMi_InsertPreSleepCallbackEx(info, priority);
}
#endif

void PM_AppendPreSleepCallback (PMSleepCallbackInfo * info)
{
    PMi_AppendList(&PMi_PreSleepCallbackList, info);
}

void PM_PrependPreSleepCallback (PMSleepCallbackInfo * info)
{
    PMi_PrependList(&PMi_PreSleepCallbackList, info);
}

void PM_AppendPostSleepCallback (PMSleepCallbackInfo * info)
{
    PMi_AppendList(&PMi_PostSleepCallbackList, info);
}

void PM_PrependPostSleepCallback (PMSleepCallbackInfo * info)
{
    PMi_PrependList(&PMi_PostSleepCallbackList, info);
}

void PM_DeletePreSleepCallback (PMSleepCallbackInfo * info)
{
    PMi_DeleteList(&PMi_PreSleepCallbackList, info);
}

void PM_DeletePostSleepCallback (PMSleepCallbackInfo * info)
{
    PMi_DeleteList(&PMi_PostSleepCallbackList, info);
}

#if SDK_VERSION_MAJOR == 5
void PM_ClearPreSleepCallback(void) {
  PMi_ClearList(&PMi_PreSleepCallbackList);
}

void PM_ClearPostSleepCallback(void) {
  PMi_ClearList(&PMi_PostSleepCallbackList);
}

#ifdef SDK_TWL
#include <twl/ltdmain_begin.h>

static void PMi_ProceedToExit(PMExitFactor factor) {

  if (!PMi_TryLockForReset()) {
    return;
  }

  PMi_ExitFactor = factor;

#ifndef SDK_FINALROM

  PMi_ExitSequenceFlag = TRUE;
#endif

  PMi_ExecuteList(PMi_PreExitCallbackList);

  if (PMi_AutoExitFlag) {

    PM_ReadyToExit();
  }
}

void PM_ReadyToExit(void) {
#ifndef SDK_FINALROM
  SDK_ASSERT(PMi_ExitSequenceFlag == TRUE);
#endif

  PMi_CallPostExitCallbackAndReset(TRUE);
}

static void PMi_FinalizeDebugger(void) {

  OSi_SetSyncValue(OSi_SYNCVAL_NOT_READY);
  OSi_SyncWithOtherProc(OSi_SYNCTYPE_SENDER, (void *)HW_INIT_LOCK_BUF);
  OSi_SyncWithOtherProc(OSi_SYNCTYPE_RECVER, (void *)HW_INIT_LOCK_BUF);

  (void)OS_DisableInterrupts();

#ifndef SDK_FINALROM

  if (OSi_DetectDebugger() & OS_CONSOLE_TWLDEBUGGER) {
    _ISTDbgLib_OnBeforeResetHard();
  }
#endif

  OSi_SetSyncValue(OSi_SYNCVAL_READY);
}

PMExitFactor PM_GetExitFactor(void) { return PMi_ExitFactor; }

void PM_AppendPreExitCallback(PMExitCallbackInfo *info) {
  PMi_InsertList(&PMi_PreExitCallbackList, info, PM_CALLBACK_PRIORITY_MAX,
                 PMi_COMPARE_GT);
}

void PM_AppendPostExitCallback(PMExitCallbackInfo *info) {
  PMi_InsertList(&PMi_PostExitCallbackList, info, PM_CALLBACK_PRIORITY_MAX,
                 PMi_COMPARE_GT);
}

void PM_PrependPreExitCallback(PMExitCallbackInfo *info) {
  PMi_InsertList(&PMi_PreExitCallbackList, info, PM_CALLBACK_PRIORITY_MIN,
                 PMi_COMPARE_GE);
}

void PM_PrependPostExitCallback(PMExitCallbackInfo *info) {
  PMi_InsertList(&PMi_PostExitCallbackList, info, PM_CALLBACK_PRIORITY_MIN,
                 PMi_COMPARE_GE);
}

void PMi_InsertPreExitCallbackEx(PMExitCallbackInfo *info, int priority) {
  SDK_ASSERT(PM_CALLBACK_PRIORITY_SYSMIN <= priority &&
             priority <= PM_CALLBACK_PRIORITY_SYSMAX);
  PMi_InsertList(&PMi_PreExitCallbackList, info, priority, PMi_COMPARE_GT);
}
void PM_InsertPreExitCallback(PMExitCallbackInfo *info, int priority) {
  SDK_ASSERT(PM_CALLBACK_PRIORITY_MIN <= priority &&
             priority <= PM_CALLBACK_PRIORITY_MAX);
  PMi_InsertPreExitCallbackEx(info, priority);
}

void PMi_InsertPostExitCallbackEx(PMExitCallbackInfo *info, int priority) {
  SDK_ASSERT(PM_CALLBACK_PRIORITY_SYSMIN <= priority &&
             priority <= PM_CALLBACK_PRIORITY_SYSMAX);
  PMi_InsertList(&PMi_PostExitCallbackList, info, priority, PMi_COMPARE_GT);
}
void PM_InsertPostExitCallback(PMExitCallbackInfo *info, int priority) {
  SDK_ASSERT(PM_CALLBACK_PRIORITY_MIN <= priority &&
             priority <= PM_CALLBACK_PRIORITY_MAX);
  PMi_InsertPostExitCallbackEx(info, priority);
}

void PM_DeletePreExitCallback(PMExitCallbackInfo *info) {
  PMi_DeleteList(&PMi_PreExitCallbackList, info);
}

void PM_DeletePostExitCallback(PMExitCallbackInfo *info) {
  PMi_DeleteList(&PMi_PostExitCallbackList, info);
}

static void PMi_ClearPreExitCallback(void) {
  PMi_ClearList(&PMi_PreExitCallbackList);
}

static void PMi_ClearPostExitCallback(void) {
  PMi_ClearList(&PMi_PostExitCallbackList);
}

void PMi_ExecutePreExitCallbackList(void) {
  PMi_ExecuteList(PMi_PreExitCallbackList);
}

void PMi_ExecutePostExitCallbackList(void) {
  PMi_ExecuteList(PMi_PostExitCallbackList);
}

void PMi_ExecuteAllListsOfExitCallback(void) {
  PMi_ExecuteList(PMi_PreExitCallbackList);
  PMi_ExecuteList(PMi_PostExitCallbackList);
}

void PM_SetAutoExit(BOOL sw) { PMi_AutoExitFlag = sw; }

BOOL PM_GetAutoExit(void) { return PMi_AutoExitFlag; }

void PM_SetBatteryLowCallback(PMBatteryLowCallback callback, void *arg) {
  PMi_BatteryLowCallbackInfo.callback = callback;
  PMi_BatteryLowCallbackInfo.arg = arg;
}

void PM_ForceToResetHardware(void) {

  PMi_ExitFactor = PM_EXIT_FACTOR_USER;

  PMi_CallPostExitCallbackAndReset(FALSE);
}

static void PMi_CallPostExitCallbackAndReset(BOOL isExit) {

  PMi_ExecuteList(PMi_PostExitCallbackList);

  GX_DispOff();
  GXS_DispOff();

  MI_SetMainMemoryPriority(MI_PROCESSOR_ARM7);
  {
    int n;
    for (n = 0; n < 3; n++) {
      u32 count = OS_GetVBlankCount();
      while (count == OS_GetVBlankCount()) {
        OS_SpinWait(100);
      }
    }
  }

  while (1) {
    u16 result;
    u32 command = isExit ? PM_UTIL_FORCE_EXIT : PM_UTIL_FORCE_RESET_HARDWARE;

    if (*(u32 *)HW_RESET_LOCK_FLAG_BUF == PM_RESET_FLAG_FORCED) {
      command = PM_UTIL_FORCE_EXIT;

      ((LauncherParam *)HW_PARAM_LAUNCH_PARAM)->header.magicCode = 0;
    }

    PMi_WaitBusyMethod = PMi_WAITBUSY_METHOD_CPUMODE |
                         PMi_WAITBUSY_METHOD_CPSR | PMi_WAITBUSY_METHOD_IME;
    while (PM_SendUtilityCommand(command, 0, &result) != PM_SUCCESS) {

      OS_SpinWait(HW_CPU_CLOCK_ARM9 / 100);
      PMi_WaitBusy();
    }
    if (result == SPI_PXI_RESULT_SUCCESS) {
      break;
    }

    OS_SpinWait(HW_CPU_CLOCK_ARM9 / 100);
  }

  (void)OS_DisableInterrupts();

  MI_StopAllDma();
  MI_StopAllNDma();

  PMi_FinalizeDebugger();

  OSi_TerminateCore();
}
#include <twl/ltdmain_end.h>
#endif

static void PMi_LCDOnAvoidReset(void) {
  BOOL preMethod;

  OS_SpinWaitSysCycles(PMi_LCD_WAIT_SYS_CYCLES);

  preMethod = PMi_WaitBusyMethod;
  PMi_WaitBusyMethod = PMi_WAITBUSY_METHOD_CPUMODE | PMi_WAITBUSY_METHOD_CPSR |
                       PMi_WAITBUSY_METHOD_IME;
  if (PM_GetLCDPower() != PM_LCD_POWER_ON) {

    while (PM_SetBackLight(PM_LCD_ALL, PM_BACKLIGHT_OFF) != PM_RESULT_SUCCESS) {
      OS_SpinWait(HW_CPU_CLOCK_ARM9 / 100);
    }

    while (!PM_SetLCDPower(PM_LCD_POWER_ON)) {
      OS_SpinWait(PMi_PXI_WAIT_TICK);
    }
  }
  PMi_WaitBusyMethod = preMethod;
}

#ifndef SDK_FINALROM

u32 PMi_ReadRegisterAsync(u16 registerAddr, u16 *buffer, PMCallback callback,
                          void *arg) {
  return PM_SendUtilityCommandAsync(PMi_UTIL_READREG, registerAddr, buffer,
                                    callback, arg);
}
u32 PMi_ReadRegister(u16 registerAddr, u16 *buffer) {
  return PM_SendUtilityCommand(PMi_UTIL_READREG, registerAddr, buffer);
}

u32 PMi_WriteRegisterAsync(u16 registerAddr, u16 data, PMCallback callback,
                           void *arg) {
  return PM_SendUtilityCommandAsync(PMi_UTIL_WRITEREG,
                                    (u16)((registerAddr << 8) | (data & 0xff)),
                                    NULL, callback, arg);
}

u32 PMi_WriteRegister(u16 registerAddr, u16 data) {
  return PM_SendUtilityCommand(
      PMi_UTIL_WRITEREG, (u16)((registerAddr << 8) | (data & 0xff)), NULL);
}
#endif

void PMi_SetDispOffCount(void) { PMi_DispOffCount = OS_GetVBlankCount(); }

#ifndef SDK_FINALROM

void PM_DumpSleepCallback(void) {
  PMGenCallbackInfo *p;

  p = PMi_PreSleepCallbackList;
  OS_TPrintf("----PreSleep Callback\n");
  while (p) {
    OS_TPrintf("[%08x] (prio=%d) (arg=%x)\n", p->callback, p->priority, p->arg);
    p = p->next;
  }

  p = PMi_PostSleepCallbackList;
  OS_TPrintf("----PostSleep Callback\n");
  while (p) {
    OS_TPrintf("[%08x] (prio=%d) (arg=%x)\n", p->callback, p->priority, p->arg);
    p = p->next;
  }
}

#ifdef SDK_TWL
#include <twl/ltdmain_begin.h>

void PM_DumpExitCallback(void) {
  PMGenCallbackInfo *p;

  p = PMi_PreExitCallbackList;
  OS_TPrintf("----PreExit Callback\n");
  while (p) {
    OS_TPrintf("[%08x] (prio=%d) (arg=%x)\n", p->callback, p->priority, p->arg);
    p = p->next;
  }

  p = PMi_PostExitCallbackList;
  OS_TPrintf("----PostExit Callback\n");
  while (p) {
    OS_TPrintf("[%08x] (prio=%d) (arg=%x)\n", p->callback, p->priority, p->arg);
    p = p->next;
  }
}
#include <twl/ltdmain_end.h>
#endif // ifdef SDK_TWL
#endif // ifndef SDK_FINALROM

#ifdef SDK_TWL
#include <twl/ltdmain_begin.h>

static volatile BOOL isLockedReset = FALSE;
BOOL PMi_TryLockForReset(void) {
  OSIntrMode e = OS_DisableInterrupts();

  if (isLockedReset) {
    (void)OS_RestoreInterrupts(e);
    return FALSE;
  }
  isLockedReset = TRUE;

  (void)OS_RestoreInterrupts(e);
  return TRUE;
}
#include <twl/ltdmain_end.h>
#endif // ifdef SDK_TWL
#endif /* SDK_VERSION_MAJOR */
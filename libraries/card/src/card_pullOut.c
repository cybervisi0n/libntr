#if SDK_VERSION_MAJOR == 4
#include <nitro.h>
#elif SDK_VERSION_MAJOR == 5
#include <nitro/card/rom.h>
#include <nitro/card/pullOut.h>
#endif

#include <card_rom.h>

static CARDPulledOutCallback CARD_UserCallback;

#if SDK_VERSION_MAJOR == 5
static u32 CARDiSlotResetCount;
#endif
static BOOL CARDi_IsPulledOutFlag = FALSE;

#ifdef SDK_PORT
static void CARDi_PulledOutCallback(PXIFifoTag tag, u64 data, BOOL err);
#else
static void CARDi_PulledOutCallback(PXIFifoTag tag, u32 data, BOOL err);
#endif
static void CARDi_SendtoPxi(u32 data, u32 wait);

void CARD_InitPulledOutCallback (void)
{
	PXI_Init();
	
	#if SDK_VERSION_MAJOR == 5
	CARDiSlotResetCount = 0;
	CARDi_IsPulledOutFlag = FALSE;
	#endif

	PXI_SetFifoRecvCallback(PXI_FIFO_TAG_CARD, CARDi_PulledOutCallback);

	CARD_UserCallback = NULL;
}

#ifdef SDK_PORT
static void CARDi_PulledOutCallback (PXIFifoTag tag, u64 data, BOOL err)
#else
static void CARDi_PulledOutCallback (PXIFifoTag tag, u32 data, BOOL err)
#endif
{
#pragma unused(tag, err)

	u32 command = data & CARD_PXI_COMMAND_MASK;

	if (command == CARD_PXI_COMMAND_PULLED_OUT) {
		if (CARDi_IsPulledOutFlag == FALSE) {
			BOOL isTerminateImm = TRUE;
			CARDi_IsPulledOutFlag = TRUE;

			if (CARD_UserCallback) {
				isTerminateImm = CARD_UserCallback();
			}

			if (isTerminateImm) {
				CARD_TerminateForPulledOut();
			}
		}
	} 
#if SDK_VERSION_MAJOR == 5
	else if (command == CARD_PXI_COMMAND_RESET_SLOT) {
    	CARDiSlotResetCount += 1;
    	CARDi_IsPulledOutFlag = FALSE;
    	CARDi_NotifyEvent(CARD_EVENT_SLOTRESET, NULL);
	}
#endif
	else {
#ifndef SDK_FINALROM
		OS_Panic("illegal card pxi command.");
#else
		OS_Panic("");
#endif
	}
}

void CARD_SetPulledOutCallback (CARDPulledOutCallback callback)
{
	CARD_UserCallback = callback;
}

BOOL CARD_IsPulledOut (void)
{
	return CARDi_IsPulledOutFlag;
}

void CARD_TerminateForPulledOut (void)
{
#if SDK_VERSION_MAJOR == 4
#ifndef SDK_TEG
	BOOL should_be_halt = TRUE;

	MI_StopDma(0);
	MI_StopDma(1);
	MI_StopDma(2);
	MI_StopDma(3);

	if (PAD_DetectFold()) {
		u32 res;
		while ((res = PM_ForceToPowerOff()) == SPI_PXI_RESULT_EXCLUSIVE) {
			OS_SpinWait(HW_CPU_CLOCK_ARM9 / 100);
		}
		if (res == PM_RESULT_SUCCESS) {
			should_be_halt = FALSE;
		}
	}

	if (should_be_halt) {
		CARDi_SendtoPxi(CARD_PXI_COMMAND_TERMINATE, 1);
	}
#endif
#endif
#if SDK_VERSION_MAJOR == 5
  if (PAD_DetectFold()) {
    (void)PM_ForceToPowerOff();
  }

#ifdef SDK_TWL
  if (OS_IsRunOnTwl()) {

    PMi_ExecutePostExitCallbackList();
  }
#endif // SDK_TWL

  CARDi_SendtoPxi(CARD_PXI_COMMAND_TERMINATE, 1);

  MI_StopAllDma();
#ifdef SDK_TWL
  if (OS_IsRunOnTwl()) {
    MI_StopAllNDma();
  }
#endif
#endif

	OS_Terminate();
}

void CARDi_CheckPulledOutCore (u32 id)
{
	#if SDK_VERSION_MAJOR == 4
	vu32 iplCardID = *(vu32 *)((*(u16 *)HW_CHECK_DEBUGGER_SW ==
	                            0) ? HW_RED_RESERVED : HW_BOOT_CHECK_INFO_BUF);
	#elif SDK_VERSION_MAJOR == 5
	vu32 iplCardID = *(vu32 *)(HW_BOOT_CHECK_INFO_BUF);
	#endif

	if (id != (u32)iplCardID) {
		OSIntrMode bak_cpsr = OS_DisableInterrupts();
		CARDi_PulledOutCallback(PXI_FIFO_TAG_CARD, CARD_PXI_COMMAND_PULLED_OUT, FALSE);
		(void)OS_RestoreInterrupts(bak_cpsr);
	}
}

void CARD_CheckPulledOut (void)
{
	CARDi_CheckPulledOutCore(CARDi_ReadRomID());
}

static void CARDi_SendtoPxi (u32 data, u32 wait)
{
	while (PXI_SendWordByFifo(PXI_FIFO_TAG_CARD, data, FALSE) != PXI_FIFO_SUCCESS) {
		SVC_WaitByLoop((s32)wait);
	}
}

#if SDK_VERSION_MAJOR == 5
u32 CARDi_GetSlotResetCount(void) { return CARDiSlotResetCount; }

BOOL CARDi_IsPulledOutEx(u32 count) {
  BOOL result = FALSE;
  OSIntrMode bak = OS_DisableInterrupts();
  {
    result = ((count == CARDi_GetSlotResetCount()) && !CARD_IsPulledOut());
  }
  (void)OS_RestoreInterrupts(bak);
  return result;
}
#endif
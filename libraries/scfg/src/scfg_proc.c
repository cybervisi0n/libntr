#include <nitro/pxi/common/fifo.h>
#include <twl/scfg.h>

typedef struct {
  BOOL lock;
  u64 fuseData;
  u32 readFlag;
  SCFGCallback callback;
  void *callbackArg;
} SCFGFuseInfo;

SCFGFuseInfo SCFGi_FuseInfo;

static void SCFGi_SwitchCpuSpeed(SCFGCpuSpeed cpuSpeed);
#ifdef SDK_PORT
static void SCFGi_CommonCallback(PXIFifoTag tag, u64 data, BOOL err);
#else
static void SCFGi_CommonCallback(PXIFifoTag tag, u32 data, BOOL err);
#endif
static void SCFGi_SendPxiData(u32 command, u16 ordinal, u16 data);
static void SCFGi_Sync(u64 fuseData, void *arg);

void SCFG_Init(void) {
  PXI_SetFifoRecvCallback(PXI_FIFO_TAG_SCFG, SCFGi_CommonCallback);

  SCFGi_FuseInfo.lock = FALSE;
}

#define SCFGi_SWITCH_CPU_WAIT                                                  \
  10 // 10 cycle for safety (actually 8 cycle is needed)
#define SCFGi_SWITCH_CPU_WAIT2 (SCFGi_SWITCH_CPU_WAIT * 2)

#ifdef SDK_PORT
void SCFGi_SwitchCpuSpeed(SCFGCpuSpeed cpuSpeed) {}
#else
#include <twl/itcm_begin.h>
#include <nitro/code32.h>
asm void SCFGi_SwitchCpuSpeed(SCFGCpuSpeed cpuSpeed) {

  ldr r2, = REG_CLK_ADDR ldrh r1,
      [r2]

      bic r1,
      r1,
      #REG_SCFG_CLK_CPUSPD_MASK // #REG_SCFG_CLK_CPUSPD_MASK = #1
      orr r1,
      r0, r1 strh r1,
      [r2]

      cmp r0,
      #SCFG_CPU_SPEED_1X moveq r0, #SCFGi_SWITCH_CPU_WAIT movne r0,
      #SCFGi_SWITCH_CPU_WAIT2

      _1 : subs r0,
      r0,
      #4 bcs _1 bx lr
}
#include <nitro/codereset.h>
#include <twl/itcm_end.h>
#endif

void SCFG_SetCpuSpeed(SCFGCpuSpeed cpuSpeed) {
  OSIntrMode enable;

  SDK_ASSERT(cpuSpeed == SCFG_CPU_SPEED_1X || cpuSpeed == SCFG_CPU_SPEED_2X);

  enable = OS_DisableInterrupts();

  if (cpuSpeed != SCFG_GetCpuSpeed()) {
    SCFGi_SwitchCpuSpeed(cpuSpeed);
  }

  (void)OS_RestoreInterrupts(enable);
}

#define SCFGi_READ_FUSE_DONE 0xf

#ifdef SDK_PORT
static void SCFGi_CommonCallback(PXIFifoTag tag, u64 pxiData, BOOL err)
#else
static void SCFGi_CommonCallback(PXIFifoTag tag, u32 pxiData, BOOL err)
#endif
{
#pragma unused(tag, err)
  u16 command =
      (u16)((pxiData & SCFG_PXI_COMMAND_MASK) >> SCFG_PXI_COMMAND_SHIFT);
  u16 ordinal =
      (u16)((pxiData & SCFG_PXI_ORDINAL_MASK) >> SCFG_PXI_ORDINAL_SHIFT);
  u16 data = (u16)((pxiData & SCFG_PXI_DATA_MASK) >> SCFG_PXI_DATA_SHIFT);

  switch (command) {
  case SCFGi_PXI_COMMAND_READ:

    SCFGi_FuseInfo.fuseData |= (((u64)data) << (ordinal << 4));
    SCFGi_FuseInfo.readFlag |= (1 << ordinal);

    if (SCFGi_FuseInfo.readFlag == SCFGi_READ_FUSE_DONE) {
      if (SCFGi_FuseInfo.callback) {
        (SCFGi_FuseInfo.callback)(SCFGi_FuseInfo.fuseData,
                                  SCFGi_FuseInfo.callbackArg);
        SCFGi_FuseInfo.callback = NULL;
      }
      SCFGi_FuseInfo.lock = FALSE;
    }
    break;
  case SCFGi_PXI_COMMAND_READ_OP: {
    u64 tempData;

    *((u16 *)&tempData) = data;
    if (SCFGi_FuseInfo.callback) {
      (SCFGi_FuseInfo.callback)(tempData, SCFGi_FuseInfo.callbackArg);
      SCFGi_FuseInfo.callback = NULL;
    }
    SCFGi_FuseInfo.lock = FALSE;
  } break;
  default:

    OS_TPanic("illegal SCFG Pxi.");
  }
}

static void SCFGi_SendPxiData(u32 command, u16 ordinal, u16 data) {
  u32 pxiData =
      (u32)(((command << SCFG_PXI_COMMAND_SHIFT) & SCFG_PXI_COMMAND_MASK) |
            ((ordinal << SCFG_PXI_ORDINAL_SHIFT) & SCFG_PXI_ORDINAL_MASK) |
            ((data << SCFG_PXI_DATA_SHIFT) & SCFG_PXI_DATA_MASK));

  while (PXI_SendWordByFifo(PXI_FIFO_TAG_SCFG, pxiData, FALSE) !=
         PXI_FIFO_SUCCESS) {
  }
}

BOOL SCFG_ReadFuseDataAsync(SCFGCallback callback, void *arg) {
  OSIntrMode enable = OS_DisableInterrupts();

  if (SCFGi_FuseInfo.lock) {
    (void)OS_RestoreInterrupts(enable);
    return FALSE;
  }

  SCFGi_FuseInfo.lock = TRUE;
  SCFGi_FuseInfo.callback = callback;
  SCFGi_FuseInfo.callbackArg = arg;
  SCFGi_FuseInfo.readFlag = 0;
  SCFGi_FuseInfo.fuseData = 0;

  SCFGi_SendPxiData(SCFGi_PXI_COMMAND_READ, 0, 0);

  (void)OS_RestoreInterrupts(enable);
  return TRUE;
}

typedef struct {
  BOOL flag;
  u64 data;
} SCFGiDataForSync;

u64 SCFG_ReadFuseData(void) {
  volatile SCFGiDataForSync info;

  info.flag = FALSE;
  if (SCFG_ReadFuseDataAsync(SCFGi_Sync, (void *)&info)) {
    while (info.flag == FALSE) {
    }
    return info.data;
  } else {
    return 0;
  }
}

BOOL SCFG_ReadBondingOptionAsync(SCFGCallback callback, void *arg) {
  OSIntrMode enable = OS_DisableInterrupts();

  if (SCFGi_FuseInfo.lock) {
    (void)OS_RestoreInterrupts(enable);
    return FALSE;
  }

  SCFGi_FuseInfo.lock = TRUE;
  SCFGi_FuseInfo.callback = callback;
  SCFGi_FuseInfo.callbackArg = arg;

  SCFGi_SendPxiData(SCFGi_PXI_COMMAND_READ_OP, 0, 0);

  (void)OS_RestoreInterrupts(enable);
  return TRUE;
}

u16 SCFG_ReadBondingOption(void) {
  volatile SCFGiDataForSync info;

  info.flag = FALSE;
  if (SCFG_ReadBondingOptionAsync(SCFGi_Sync, (void *)&info)) {
    while (info.flag == FALSE) {
    }
    return *(u16 *)(&info.data);
  } else {
    return 0xffff;
  }
}

static void SCFGi_Sync(u64 fuseData, void *arg) {
  ((SCFGiDataForSync *)arg)->flag = TRUE;
  ((SCFGiDataForSync *)arg)->data = fuseData;
}

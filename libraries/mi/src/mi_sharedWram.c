#ifdef SDK_ARM9
#include <twl.h>
#else
#include <twl_sp.h>
#endif

#include <nitro/os/common/system.h>

static void MIi_InitWramManager(void);
void MI_InitWramManager(void) {
  if (OS_IsRunOnTwl() == FALSE) {
    return;
  }
  MIi_InitWramManager();
}

#ifdef SDK_ARM9
#include <twl/ltdmain_begin.h>
#endif

#ifdef SDK_ARM9

void MIi_SetWramBank_A(int num, MIWramProc proc, MIWramOffset offset,
                       MIWramEnable enable) {
  SDK_ASSERT(0 <= num && num < MI_WRAM_A_MAX_NUM);
  SDK_ASSERT((offset & 1) == 0);
  SDK_ASSERT(proc == MI_WRAM_ARM9 || proc == MI_WRAM_ARM7);

  *(vu8 *)(REG_MBK_A0_ADDR + num) =
      (vu8)((enable ? REG_MI_MBK_A0_E_MASK : 0) | proc |
            ((offset >> 1) << REG_MI_MBK_A0_OF_SHIFT));
}

void MIi_SetWramBank_B(int num, MIWramProc proc, MIWramOffset offset,
                       MIWramEnable enable) {
  SDK_ASSERT(0 <= num && num < MI_WRAM_B_MAX_NUM);

  *(vu8 *)(REG_MBK_B0_ADDR + num) =
      (vu8)((enable ? REG_MI_MBK_B0_E_MASK : 0) | proc |
            (offset << REG_MI_MBK_B0_OF_SHIFT));
}

void MIi_SetWramBank_C(int num, MIWramProc proc, MIWramOffset offset,
                       MIWramEnable enable) {
  SDK_ASSERT(0 <= num && num < MI_WRAM_C_MAX_NUM);

  *(vu8 *)(REG_MBK_C0_ADDR + num) =
      (vu8)((enable ? REG_MI_MBK_C0_E_MASK : 0) | proc |
            (offset << REG_MI_MBK_C0_OF_SHIFT));
}

void MIi_SetWramBank(MIWramPos wram, int num, MIWramProc proc,
                     MIWramOffset offset, MIWramEnable enable) {
  static void (*f[3])(int, MIWramProc, MIWramOffset, MIWramEnable) = {
      MIi_SetWramBank_A, MIi_SetWramBank_B, MIi_SetWramBank_C};

  SDK_ASSERT(0 <= wram && wram < 3);
  (f[wram])(num, proc, offset, enable);
}

vu8 MIi_GetWramBank_A(int num) {
  SDK_ASSERT(0 <= num && num < MI_WRAM_A_MAX_NUM);
  return *(vu8 *)(REG_MBK_A0_ADDR + num);
}

vu8 MIi_GetWramBank_B(int num) {
  SDK_ASSERT(0 <= num && num < MI_WRAM_B_MAX_NUM);
  return *(vu8 *)(REG_MBK_B0_ADDR + num);
}

vu8 MIi_GetWramBank_C(int num) {
  SDK_ASSERT(0 <= num && num < MI_WRAM_C_MAX_NUM);
  return *(vu8 *)(REG_MBK_C0_ADDR + num);
}

static vu8 MIi_GetWramBank(MIWramPos wram, int num) {
  static vu8 (*f[3])(int) = {MIi_GetWramBank_A, MIi_GetWramBank_B,
                             MIi_GetWramBank_C};

  SDK_ASSERT(0 <= wram && wram < 3);
  return (f[wram])(num);
}

void MIi_SetWramBankMaster_A(int num, MIWramProc proc) {
  vu8 *p = (vu8 *)(REG_MBK_A0_ADDR + num);

  SDK_ASSERT(0 <= num && num < MI_WRAM_A_MAX_NUM);
  *p = (u8)(*p & ~MI_WRAM_MASTER_MASK_A | proc);
}

void MIi_SetWramBankMaster_B(int num, MIWramProc proc) {
  vu8 *p = (vu8 *)(REG_MBK_B0_ADDR + num);

  SDK_ASSERT(0 <= num && num < MI_WRAM_B_MAX_NUM);
  *p = (u8)(*p & ~MI_WRAM_MASTER_MASK_B | proc);
}

void MIi_SetWramBankMaster_C(int num, MIWramProc proc) {
  vu8 *p = (vu8 *)(REG_MBK_C0_ADDR + num);

  SDK_ASSERT(0 <= num && num < MI_WRAM_C_MAX_NUM);
  *p = (u8)(*p & ~MI_WRAM_MASTER_MASK_C | proc);
}

void MIi_SetWramBankMaster(MIWramPos wram, int num, MIWramProc proc) {
  static void (*f[3])(int, MIWramProc) = {MIi_SetWramBankMaster_A,
                                          MIi_SetWramBankMaster_B,
                                          MIi_SetWramBankMaster_C};
  SDK_ASSERT(0 <= wram && wram < 3);
  (f[wram])(num, proc);
}

void MIi_SetWramBankEnable_A(int num, MIWramEnable enable) {
  vu8 *p = (vu8 *)(REG_MBK_A0_ADDR + num);

  SDK_ASSERT(0 <= num && num < MI_WRAM_A_MAX_NUM);
  *p = (u8)(*p & ~MI_WRAM_ENABLE_MASK_A | (enable ? MI_WRAM_ENABLE_MASK_A : 0));
}

void MIi_SetWramBankEnable_B(int num, MIWramEnable enable) {
  vu8 *p = (vu8 *)(REG_MBK_B0_ADDR + num);

  SDK_ASSERT(0 <= num && num < MI_WRAM_B_MAX_NUM);
  *p = (u8)(*p & ~MI_WRAM_ENABLE_MASK_B | (enable ? MI_WRAM_ENABLE_MASK_B : 0));
}

void MIi_SetWramBankEnable_C(int num, MIWramEnable enable) {
  vu8 *p = (vu8 *)(REG_MBK_C0_ADDR + num);

  SDK_ASSERT(0 <= num && num < MI_WRAM_C_MAX_NUM);
  *p = (u8)(*p & ~MI_WRAM_ENABLE_MASK_C | (enable ? MI_WRAM_ENABLE_MASK_C : 0));
}

void MIi_SetWramBankEnable(MIWramPos wram, int num, MIWramEnable enable) {
  static void (*f[3])(int, MIWramEnable) = {MIi_SetWramBankEnable_A,
                                            MIi_SetWramBankEnable_B,
                                            MIi_SetWramBankEnable_C};

  SDK_ASSERT(0 <= wram && wram < 3);
  (f[wram])(num, enable);
}
#endif

MIWramProc MI_GetWramBankMaster_A(int num) {
  SDK_ASSERT(0 <= num && num < MI_WRAM_A_MAX_NUM);
  return (MIWramProc)(*(REGType8v *)(REG_MBK_A0_ADDR + num) &
                      MI_WRAM_MASTER_MASK_A);
}
MIWramProc MI_GetWramBankMaster_B(int num) {
  SDK_ASSERT(0 <= num && num < MI_WRAM_B_MAX_NUM);
  return (MIWramProc)(*(REGType8v *)(REG_MBK_B0_ADDR + num) &
                      MI_WRAM_MASTER_MASK_B);
}
MIWramProc MI_GetWramBankMaster_C(int num) {
  SDK_ASSERT(0 <= num && num < MI_WRAM_C_MAX_NUM);
  return (MIWramProc)(*(REGType8v *)(REG_MBK_C0_ADDR + num) &
                      MI_WRAM_MASTER_MASK_C);
}
MIWramProc MI_GetWramBankMaster(MIWramPos wram, int num) {
  static MIWramProc (*f[3])(int) = {
      MI_GetWramBankMaster_A, MI_GetWramBankMaster_B, MI_GetWramBankMaster_C};

  SDK_ASSERT(0 <= wram && wram < 3);
  return (f[wram])(num);
}

MIWramOffset MI_GetWramBankOffset_A(int num) {
  SDK_ASSERT(0 <= num && num < MI_WRAM_A_MAX_NUM);
  return (MIWramOffset)((*(REGType8v *)(REG_MBK_A0_ADDR + num) &
                         MI_WRAM_OFFSET_MASK_A) >>
                        MI_WRAM_OFFSET_SHIFT_A << 1);
}
MIWramOffset MI_GetWramBankOffset_B(int num) {
  SDK_ASSERT(0 <= num && num < MI_WRAM_B_MAX_NUM);
  return (MIWramOffset)((*(REGType8v *)(REG_MBK_B0_ADDR + num) &
                         MI_WRAM_OFFSET_MASK_B) >>
                        MI_WRAM_OFFSET_SHIFT_B);
}
MIWramOffset MI_GetWramBankOffset_C(int num) {
  SDK_ASSERT(0 <= num && num < MI_WRAM_C_MAX_NUM);
  return (MIWramOffset)((*(REGType8v *)(REG_MBK_C0_ADDR + num) &
                         MI_WRAM_OFFSET_MASK_C) >>
                        MI_WRAM_OFFSET_SHIFT_C);
}
MIWramOffset MI_GetWramBankOffset(MIWramPos wram, int num) {
  static MIWramOffset (*f[3])(int) = {
      MI_GetWramBankOffset_A, MI_GetWramBankOffset_B, MI_GetWramBankOffset_C};

  SDK_ASSERT(0 <= wram && wram < 3);
  return (f[wram])(num);
}

MIWramEnable MI_GetWramBankEnable_A(int num) {
  SDK_ASSERT(0 <= num && num < MI_WRAM_A_MAX_NUM);
  return (*(REGType8v *)(REG_MBK_A0_ADDR + num) & MI_WRAM_ENABLE_MASK_A)
             ? MI_WRAM_ENABLE
             : MI_WRAM_DISABLE;
}
MIWramEnable MI_GetWramBankEnable_B(int num) {
  SDK_ASSERT(0 <= num && num < MI_WRAM_B_MAX_NUM);
  return (*(REGType8v *)(REG_MBK_B0_ADDR + num) & MI_WRAM_ENABLE_MASK_B)
             ? MI_WRAM_ENABLE
             : MI_WRAM_DISABLE;
}
MIWramEnable MI_GetWramBankEnable_C(int num) {
  SDK_ASSERT(0 <= num && num < MI_WRAM_C_MAX_NUM);
  return (*(REGType8v *)(REG_MBK_C0_ADDR + num) & MI_WRAM_ENABLE_MASK_C)
             ? MI_WRAM_ENABLE
             : MI_WRAM_DISABLE;
}
MIWramEnable MI_GetWramBankEnable(MIWramPos wram, int num) {
  static MIWramEnable (*f[3])(int) = {
      MI_GetWramBankEnable_A, MI_GetWramBankEnable_B, MI_GetWramBankEnable_C};

  SDK_ASSERT(0 <= wram && wram < 3);
  return (f[wram])(num);
}

void MIi_SetWramMap_A(u32 start, u32 end, MIWramImage image) {
  SDK_ASSERT(image == MI_WRAM_IMAGE_64KB || image == MI_WRAM_IMAGE_128KB ||
             image == MI_WRAM_IMAGE_256KB);
  SDK_ASSERT((start & 0xffff) == 0 && 0x03000000 <= start &&
             start <= 0x03ff0000);
  SDK_ASSERT((end & 0xffff) == 0xffff && 0x02ffffff <= end &&
             end <= 0x03ffffff);

  reg_MI_MBK6 = (image << REG_MI_MBK6_WA_ISIZE_SHIFT) |
                ((start - 0x03000000) >> (16 - REG_MI_MBK6_WA_SADDR_SHIFT)) |
                ((end - 0x02ffffff) << (REG_MI_MBK6_WA_EADDR_SHIFT - 16));
}

void MIi_SetWramMap_B(u32 start, u32 end, MIWramImage image) {
  SDK_ASSERT((start & 0x7fff) == 0 && 0x03000000 <= start &&
             start <= 0x03ff8000);
  SDK_ASSERT((end & 0x7fff) == 0x7fff && 0x02ffffff <= end &&
             end <= 0x03ffffff);

  reg_MI_MBK7 = (image << REG_MI_MBK7_WB_ISIZE_SHIFT) |
                ((start - 0x03000000) >> (16 - REG_MI_MBK7_WB_SADDR_SHIFT)) |
                ((end - 0x02ffffff) << (REG_MI_MBK7_WB_EADDR_SHIFT - 16));
}

void MIi_SetWramMap_C(u32 start, u32 end, MIWramImage image) {
  SDK_ASSERT((start & 0x7fff) == 0 && 0x03000000 <= start &&
             start <= 0x03ff8000);
  SDK_ASSERT((end & 0x7fff) == 0x7fff && 0x02ffffff <= end &&
             end <= 0x03ffffff);

  reg_MI_MBK8 = (image << REG_MI_MBK8_WC_ISIZE_SHIFT) |
                ((start - 0x03000000) >> (16 - REG_MI_MBK8_WC_SADDR_SHIFT)) |
                ((end - 0x02ffffff) << (REG_MI_MBK8_WC_EADDR_SHIFT - 16));
}

u32 MI_GetWramMapStart_A(void) {
#ifdef SDK_ARM9
  u32 wramReg = reg_MI_MBK6;
#else
  u32 wramReg = *(u32 *)(HW_TWL_ROM_HEADER_BUF + 0x0194);
#endif
  return HW_WRAM_BASE +
         (((wramReg & REG_MI_MBK6_WA_SADDR_MASK) >> REG_MI_MBK6_WA_SADDR_SHIFT)
          << 16);
}
u32 MI_GetWramMapStart_B(void) {
#ifdef SDK_ARM9
  u32 wramReg = reg_MI_MBK7;
#else
  u32 wramReg = *(u32 *)(HW_TWL_ROM_HEADER_BUF + 0x0198);
#endif
  return HW_WRAM_BASE +
         (((wramReg & REG_MI_MBK7_WB_SADDR_MASK) >> REG_MI_MBK7_WB_SADDR_SHIFT)
          << 15);
}
u32 MI_GetWramMapStart_C(void) {
#ifdef SDK_ARM9
  u32 wramReg = reg_MI_MBK8;
#else
  u32 wramReg = *(u32 *)(HW_TWL_ROM_HEADER_BUF + 0x019C);
#endif
  return HW_WRAM_BASE +
         (((wramReg & REG_MI_MBK8_WC_SADDR_MASK) >> REG_MI_MBK8_WC_SADDR_SHIFT)
          << 15);
}
u32 MI_GetWramMapStart(MIWramPos wram) {
  static u32 (*f[3])(void) = {MI_GetWramMapStart_A, MI_GetWramMapStart_B,
                              MI_GetWramMapStart_C};
  SDK_ASSERT(0 <= wram && wram < 3);
  return (f[wram])();
}

u32 MI_GetWramMapEnd_A(void) {
#ifdef SDK_ARM9
  u32 wramReg = reg_MI_MBK6;
#else
  u32 wramReg = *(u32 *)(HW_TWL_ROM_HEADER_BUF + 0x0194);
#endif
  return (HW_WRAM_BASE - 1) +
         (((wramReg & REG_MI_MBK6_WA_EADDR_MASK) >> REG_MI_MBK6_WA_EADDR_SHIFT)
          << 16);
}
u32 MI_GetWramMapEnd_B(void) {
#ifdef SDK_ARM9
  u32 wramReg = reg_MI_MBK7;
#else
  u32 wramReg = *(u32 *)(HW_TWL_ROM_HEADER_BUF + 0x0198);
#endif
  return (HW_WRAM_BASE - 1) +
         (((wramReg & REG_MI_MBK7_WB_EADDR_MASK) >> REG_MI_MBK7_WB_EADDR_SHIFT)
          << 15);
}
u32 MI_GetWramMapEnd_C(void) {
#ifdef SDK_ARM9
  u32 wramReg = reg_MI_MBK8;
#else
  u32 wramReg = *(u32 *)(HW_TWL_ROM_HEADER_BUF + 0x019C);
#endif
  return (HW_WRAM_BASE - 1) +
         (((wramReg & REG_MI_MBK8_WC_EADDR_MASK) >> REG_MI_MBK8_WC_EADDR_SHIFT)
          << 15);
}
u32 MI_GetWramMapEnd(MIWramPos wram) {
  static u32 (*f[3])(void) = {MI_GetWramMapEnd_A, MI_GetWramMapEnd_B,
                              MI_GetWramMapEnd_C};
  SDK_ASSERT(0 <= wram && wram < 3);
  return (f[wram])();
}

MIWramImage MI_GetWramMapImage_A(void) {
#ifdef SDK_ARM9
  u32 wramReg = reg_MI_MBK6;
#else
  u32 wramReg = *(u32 *)(HW_TWL_ROM_HEADER_BUF + 0x0194);
#endif
  MIWramImage image = (MIWramImage)((wramReg & REG_MI_MBK6_WA_ISIZE_MASK) >>
                                    REG_MI_MBK6_WA_ISIZE_SHIFT);
  if (image == MI_WRAM_IMAGE_32KB) {
    image = MI_WRAM_IMAGE_64KB;
  }
  return image;
}
MIWramImage MI_GetWramMapImage_B(void) {
#ifdef SDK_ARM9
  u32 wramReg = reg_MI_MBK7;
#else
  u32 wramReg = *(u32 *)(HW_TWL_ROM_HEADER_BUF + 0x0198);
#endif
  return (MIWramImage)((wramReg & REG_MI_MBK7_WB_ISIZE_MASK) >>
                       REG_MI_MBK7_WB_ISIZE_SHIFT);
}
MIWramImage MI_GetWramMapImage_C(void) {
#ifdef SDK_ARM9
  u32 wramReg = reg_MI_MBK8;
#else
  u32 wramReg = *(u32 *)(HW_TWL_ROM_HEADER_BUF + 0x019C);
#endif
  return (MIWramImage)((wramReg & REG_MI_MBK8_WC_ISIZE_MASK) >>
                       REG_MI_MBK8_WC_ISIZE_SHIFT);
}
MIWramImage MI_GetWramMapImage(MIWramPos wram) {
  static MIWramImage (*f[3])(void) = {
      MI_GetWramMapImage_A, MI_GetWramMapImage_B, MI_GetWramMapImage_C};
  SDK_ASSERT(0 <= wram && wram < 3);
  return (f[wram])();
}

BOOL MI_IsWramSlotLocked_A(int num) {
  SDK_ASSERT(0 <= num && num < MI_WRAM_A_MAX_NUM);

  return (reg_MI_MBK_A_LOCK & (1 << (num + REG_MI_MBK_A_LOCK_A0_SHIFT)))
             ? TRUE
             : FALSE;
}
BOOL MI_IsWramSlotLocked_B(int num) {
  SDK_ASSERT(0 <= num && num < MI_WRAM_B_MAX_NUM);

  return (reg_MI_MBK_B_LOCK & (1 << (num + REG_MI_MBK_B_LOCK_B0_SHIFT)))
             ? TRUE
             : FALSE;
}
BOOL MI_IsWramSlotLocked_C(int num) {
  SDK_ASSERT(0 <= num && num < MI_WRAM_C_MAX_NUM);

  return (reg_MI_MBK_C_LOCK & (1 << (num + REG_MI_MBK_C_LOCK_C0_SHIFT)))
             ? TRUE
             : FALSE;
}
BOOL MI_IsWramSlotLocked(MIWramPos wram, int num) {
  static BOOL (*f[3])(int) = {MI_IsWramSlotLocked_A, MI_IsWramSlotLocked_B,
                              MI_IsWramSlotLocked_C};
  SDK_ASSERT(0 <= wram && wram < 3);
  return (f[wram])(num);
}

#define MIi_PXI_COMMAND_ALLOC 1
#define MIi_PXI_COMMAND_ALLOC_SLOT 2

#define MIi_PXI_COMMAND_FREE 3
#define MIi_PXI_COMMAND_FREE_SLOT 4

#define MIi_PXI_COMMAND_SWITCH 5
#define MIi_PXI_COMMAND_SWITCH_SLOT 6

#define MIi_PXI_COMMAND_RESERVE 7
#define MIi_PXI_COMMAND_RESERVE_SLOT 8

#define MIi_PXI_COMMAND_CANCEL 9
#define MIi_PXI_COMMAND_CANCEL_SLOT 10

#define MIi_PXI_COMMAND_GET_RESERVE 11

#define MIi_PXI_COMMAND_LOCKWRAM 12   // arm9->arm7
#define MIi_PXI_COMMAND_UNLOCKWRAM 13 // arm9->arm7

#define MIi_PXI_COMMAND_GET_ALLOCATABLE 14

#define MIi_SENDTYPE_COMMAND (FALSE)
#define MIi_SENDTYPE_RESULT (TRUE)

#ifdef SDK_PORT
static void MIi_CallbackForPxi(PXIFifoTag tag, u64 data, BOOL sendType);
#else
static void MIi_CallbackForPxi(PXIFifoTag tag, u32 data, BOOL sendType);
#endif

static int MIi_LockForPxi(volatile BOOL *finishFlag, u32 *ptr);
static void MIi_WaitWhileBusy(volatile BOOL *finished);
static void MIi_SendToPxi(BOOL sendType, u32 data);

static u32 MIi_MakeData(u32 command, MIWramPos wram, MIWramProc proc, u32 data);
static void MIi_TakeApartData(u32 pxiData, u32 *command, MIWramPos *wram,
                              MIWramProc *proc, u32 *data);

static BOOL lock = FALSE;
static volatile BOOL *finishPtr = FALSE;
static u32 *resultPtr = NULL;

static u32 MIi_DoAllocWram(MIWramPos wram, MIWramSize size, MIWramProc proc);
static u32 MIi_DoAllocWramSlot(MIWramPos wram, int slot, MIWramSize size,
                               MIWramProc proc);
static int MIi_DoFreeWram(MIWramPos wram, MIWramProc proc);
static int MIi_DoFreeWramSlot(MIWramPos wram, int slot, MIWramSize size,
                              MIWramProc proc);
static int MIi_DoSwitchWram(MIWramPos wram, MIWramProc proc,
                            MIWramProc newProc);
static int MIi_DoSwitchWramSlot(MIWramPos wram, int slot, MIWramSize size,
                                MIWramProc proc, MIWramProc newProc);
static u32 MIi_DoReserveWram(MIWramPos wram, MIWramSize size, MIWramProc proc);
static u32 MIi_DoReserveWramSlot(MIWramPos wram, int slot, MIWramSize size,
                                 MIWramProc proc);
static int MIi_DoCancelWram(MIWramPos wram, MIWramProc proc);
static int MIi_DoCancelWramSlot(MIWramPos wram, int slot, MIWramSize size,
                                MIWramProc proc);
static int MIi_DoLockWramSlots(MIWramPos wram, u32 slots);
static int MIi_DoUnlockWramSlots(MIWramPos wram, u32 slots);
static int MIi_DoGetAllocatableWramSlot(MIWramPos wram, MIWramProc proc);

static OSIntrMode MIi_LockWramManagerDI(u16 *lockid);
static void MIi_UnlockWramManagerRI(u16 lockid, OSIntrMode e);

#ifdef SDK_ARM9

static struct {
  u16 wram[3][MI_WRAM_B_MAX_NUM];
} MIi_Info;

#define MI_MAN_WRAM_RESERVE_SHIFT 8
#define MI_MAN_WRAM_MASTER_SHIFT 0

#define MI_MAN_WRAM_RESERVE_MASK (3 << MI_MAN_WRAM_RESERVE_SHIFT)
#define MI_MAN_WRAM_MASTER_MASK (3 << MI_MAN_WRAM_MASTER_SHIFT)

static inline int MIi_GetReservation(MIWramPos wram, int num) {
  return (MIi_Info.wram[wram][num] & MI_MAN_WRAM_RESERVE_MASK) >>
         MI_MAN_WRAM_RESERVE_SHIFT;
}
#define MIi_GetReservation_A(num) (MIi_GetReservation(MI_WRAM_A, (num)))
#define MIi_GetReservation_B(num) (MIi_GetReservation(MI_WRAM_B, (num)))
#define MIi_GetReservation_C(num) (MIi_GetReservation(MI_WRAM_C, (num)))

static inline void MIi_SetReservation(MIWramPos wram, int num, int val) {
  MIi_Info.wram[wram][num] =
      (u16)(MIi_Info.wram[wram][num] & ~MI_MAN_WRAM_RESERVE_MASK |
            (val << MI_MAN_WRAM_RESERVE_SHIFT));
}
#define MIi_SetReservation_A(num, val)                                         \
  do {                                                                         \
    MIi_SetReservation(MI_WRAM_A, (num), (val));                               \
  } while (0)
#define MIi_SetReservation_B(num, val)                                         \
  do {                                                                         \
    MIi_SetReservation(MI_WRAM_B, (num), (val));                               \
  } while (0)
#define MIi_SetReservation_C(num, val)                                         \
  do {                                                                         \
    MIi_SetReservation(MI_WRAM_C, (num), (val));                               \
  } while (0)

static inline int MIi_GetMaster(MIWramPos wram, int num) {
  return (MIi_Info.wram[wram][num] & MI_MAN_WRAM_MASTER_MASK) >>
         MI_MAN_WRAM_MASTER_SHIFT;
}
#define MIi_GetMaster_A(num) (MIi_GetMaster(MI_WRAM_A, (num)))
#define MIi_GetMaster_B(num) (MIi_GetMaster(MI_WRAM_B, (num)))
#define MIi_GetMaster_C(num) (MIi_GetMaster(MI_WRAM_C, (num)))

static inline void MIi_SetMaster(MIWramPos wram, int num, int val) {
  MIi_Info.wram[wram][num] =
      (u16)(MIi_Info.wram[wram][num] & ~MI_MAN_WRAM_MASTER_MASK |
            (val << MI_MAN_WRAM_MASTER_SHIFT));
}
#define MIi_SetMaster_A(num, val)                                              \
  do {                                                                         \
    MIi_SetMaster(MI_WRAM_A, (num), (val));                                    \
  } while (0)
#define MIi_SetMaster_B(num, val)                                              \
  do {                                                                         \
    MIi_SetMaster(MI_WRAM_B, (num), (val));                                    \
  } while (0)
#define MIi_SetMaster_C(num, val)                                              \
  do {                                                                         \
    MIi_SetMaster(MI_WRAM_C, (num), (val));                                    \
  } while (0)
#endif

static u32 MIi_GetAddress_A(int num) {
  return (u32)(MI_GetWramMapStart_A() + MI_WRAM_A_SLOT_SIZE * num);
}
static u32 MIi_GetAddress_B(int num) {
  return (u32)(MI_GetWramMapStart_B() + MI_WRAM_B_SLOT_SIZE * num);
}
static u32 MIi_GetAddress_C(int num) {
  return (u32)(MI_GetWramMapStart_C() + MI_WRAM_C_SLOT_SIZE * num);
}
static inline u32 MIi_GetAddress(MIWramPos wram, int num) {
  if (wram == MI_WRAM_A) {
    return MIi_GetAddress_A(num);
  }
  if (wram == MI_WRAM_B) {
    return MIi_GetAddress_B(num);
  }
  if (wram == MI_WRAM_C) {
    return MIi_GetAddress_C(num);
  }
  return 0;
}

static void MIi_InitWramManager(void) {
  static BOOL sInitialized = FALSE;

#ifdef SDK_ARM9
  int n;
#endif

  if (sInitialized) {
    return;
  }
  sInitialized = TRUE;

#ifdef SDK_ARM9

  for (n = 0; n < MI_WRAM_A_MAX_NUM; n++) {
    u8 val = MIi_GetWramBank_A(n);
    switch (val & 1) {
    case 0:
      MIi_SetReservation_A(n, MI_WRAM_ARM9);
      MIi_SetMaster_A(n, MI_WRAM_ARM9);
      break;
    case 1:
      MIi_SetReservation_A(n, MI_WRAM_ARM7);
      MIi_SetMaster_A(n, MI_WRAM_ARM7);
      break;
    }
  }

  for (n = 0; n < MI_WRAM_B_MAX_NUM; n++) {
    u8 val = MIi_GetWramBank_B(n);
    switch (val & 3) {
    case 0:
      MIi_SetReservation_B(n, MI_WRAM_ARM9);
      MIi_SetMaster_B(n, MI_WRAM_ARM9);
      break;
    case 1:
      MIi_SetReservation_B(n, MI_WRAM_ARM7);
      MIi_SetMaster_B(n, MI_WRAM_ARM7);
      break;
    default:
      MIi_SetReservation_B(n, MI_WRAM_DSP);
      MIi_SetMaster_B(n, MI_WRAM_DSP);
      break;
    }
  }

  for (n = 0; n < MI_WRAM_C_MAX_NUM; n++) {
    u8 val = MIi_GetWramBank_C(n);
    switch (val & 3) {
    case 0:
      MIi_SetReservation_C(n, MI_WRAM_ARM9);
      MIi_SetMaster_C(n, MI_WRAM_ARM9);
      break;
    case 1:
      MIi_SetReservation_C(n, MI_WRAM_ARM7);
      MIi_SetMaster_C(n, MI_WRAM_ARM7);
      break;
    default:
      MIi_SetReservation_C(n, MI_WRAM_DSP);
      MIi_SetMaster_C(n, MI_WRAM_DSP);
      break;
    }
  }

  PXI_SetFifoRecvCallback(PXI_FIFO_TAG_MI, MIi_CallbackForPxi);

#else

  while (!PXI_IsCallbackReady(PXI_FIFO_TAG_MI, PXI_PROC_ARM9)) {
    SVC_WaitByLoop(1);
  }

  PXI_SetFifoRecvCallback(PXI_FIFO_TAG_MI, MIi_CallbackForPxi);
#endif
}

#ifdef SDK_PORT
static void MIi_CallbackForPxi(PXIFifoTag tag, u64 data, BOOL sendType)
#else
static void MIi_CallbackForPxi(PXIFifoTag tag, u32 data, BOOL sendType)
#endif
{
#pragma unused(tag)

  if (sendType == MIi_SENDTYPE_COMMAND) {
    u32 command, param;
    MIWramPos wram;
    MIWramProc proc;
    u32 retval = 0;

    MIi_TakeApartData(data, &command, &wram, &proc, &param);

#ifdef SDK_ARM9
    switch (command) {
    case MIi_PXI_COMMAND_ALLOC: {
      u32 size = (param >> 8) & 0xff;
      retval = MIi_DoAllocWram(wram, (MIWramSize)size, proc);
    } break;
    case MIi_PXI_COMMAND_ALLOC_SLOT: {
      int slot = (int)(param >> 8) & 0xff;
      MIWramSize size = (MIWramSize)(param & 0xff);
      retval = MIi_DoAllocWramSlot(wram, slot, size, proc);
    } break;
    case MIi_PXI_COMMAND_FREE: {
      retval = (u32)MIi_DoFreeWram(wram, proc);
    } break;
    case MIi_PXI_COMMAND_FREE_SLOT: {
      int slot = (int)((param >> 8) & 0xff);
      MIWramSize size = (MIWramSize)(param & 0xff);
      retval = (u32)MIi_DoFreeWramSlot(wram, slot, size, proc);
    } break;
    case MIi_PXI_COMMAND_SWITCH: {
      MIWramProc newProc = (MIWramProc)((param >> 12) & 3);
      retval = (u32)MIi_DoSwitchWram(wram, proc, newProc);
    } break;
    case MIi_PXI_COMMAND_SWITCH_SLOT: {
      int slot = (int)((param >> 8) & 0xf);
      MIWramSize size = (MIWramSize)(param & 0xff);
      MIWramProc newProc = (MIWramProc)((param >> 12) & 3);
      retval = (u32)MIi_DoSwitchWramSlot(wram, slot, size, proc, newProc);
    } break;
    case MIi_PXI_COMMAND_RESERVE: {
      u32 size = (param & 0xff);
      retval = MIi_DoReserveWram(wram, (MIWramSize)size, proc);
    } break;
    case MIi_PXI_COMMAND_RESERVE_SLOT: {
      int slot = (int)((param >> 8) & 0xff);
      MIWramSize size = (MIWramSize)(param & 0xff);
      retval = MIi_DoReserveWramSlot(wram, slot, size, proc);
    } break;
    case MIi_PXI_COMMAND_CANCEL: {
      retval = (u32)MIi_DoCancelWram(wram, proc);
    } break;
    case MIi_PXI_COMMAND_CANCEL_SLOT: {
      int slot = (int)((param >> 8) & 0xff);
      MIWramSize size = (MIWramSize)(param & 0xff);
      retval = (u32)MIi_DoCancelWramSlot(wram, slot, size, proc);
    } break;
    case MIi_PXI_COMMAND_GET_RESERVE: {
      retval = (u32)MIi_GetReservation(wram, (int)param);
    } break;
    case MIi_PXI_COMMAND_GET_ALLOCATABLE: {
      retval = (u8)MIi_DoGetAllocatableWramSlot(wram, proc);
    } break;
    }
#else
    switch (command) {
    case MIi_PXI_COMMAND_LOCKWRAM: {
      retval = (u32)MIi_DoLockWramSlots(wram, param);
    } break;
    case MIi_PXI_COMMAND_UNLOCKWRAM: {
      retval = (u32)MIi_DoUnlockWramSlots(wram, param);
    } break;
    }
#endif
    MIi_SendToPxi(MIi_SENDTYPE_RESULT, retval);
  } else {
    if (resultPtr) {
      *resultPtr = data;
    }
    if (finishPtr) {
      *finishPtr = TRUE;
    }

    lock = FALSE;
  }
}

u32 MI_AllocWram(MIWramPos wram, MIWramSize size, MIWramProc proc) {
  u32 result;
  u16 lockid;

  OSIntrMode e = MIi_LockWramManagerDI(&lockid);
  result = MIi_DoAllocWram(wram, size, proc);
  MIi_UnlockWramManagerRI(lockid, e);

  return result;
}

static u32 MIi_DoAllocWram(MIWramPos wram, MIWramSize size, MIWramProc proc) {
#ifdef SDK_ARM9
  int blocks = (int)((wram == MI_WRAM_A) ? (size / 2) : size);
  int allocated = 0;
  int firstBlock;
  int n;
  int maxNum = (wram == MI_WRAM_A) ? MI_WRAM_A_MAX_NUM : MI_WRAM_B_MAX_NUM;

  for (n = 0; n < maxNum; n++) {
    int master = MIi_GetMaster(wram, n);
    int reserve = MIi_GetReservation(wram, n);

    if (master == MI_WRAM_FREE &&
        (reserve == proc || reserve == MI_WRAM_FREE)) {
      if (allocated == 0) {
        firstBlock = n;
      }

      if (++allocated >= blocks) {
        break;
      }
    } else {
      allocated = 0;
    }
  }

  if (allocated < blocks) {
    return 0;
  }

  for (n = 0; n < blocks; n++) {
    MIi_SetMaster(wram, firstBlock + n, proc);
    MIi_SetWramBankMaster(wram, firstBlock + n, proc);
    MIi_SetWramBankEnable(wram, firstBlock + n, MI_WRAM_ENABLE);
  }

  return MIi_GetAddress(wram, firstBlock);

#else
  volatile BOOL finished = FALSE;
  u32 result;

  if (!MIi_LockForPxi(&finished, &result)) {
    result = 0;
  } else {
    u32 data =
        MIi_MakeData(MIi_PXI_COMMAND_ALLOC, wram, proc, (u32)(size << 8));
    MIi_SendToPxi(MIi_SENDTYPE_COMMAND, data);
    MIi_WaitWhileBusy(&finished);
  }
  return result;
#endif
}

u32 MI_AllocWramSlot(MIWramPos wram, int slot, MIWramSize size,
                     MIWramProc proc) {
  u32 result;
  u16 lockid;

  OSIntrMode e = MIi_LockWramManagerDI(&lockid);
  result = MIi_DoAllocWramSlot(wram, slot, size, proc);
  MIi_UnlockWramManagerRI(lockid, e);

  return result;
}

static u32 MIi_DoAllocWramSlot(MIWramPos wram, int slot, MIWramSize size,
                               MIWramProc proc) {
#ifdef SDK_ARM9
  int blocks = (int)((wram == MI_WRAM_A) ? (size / 2) : size);
  int n;
  int maxNum = (wram == MI_WRAM_A) ? MI_WRAM_A_MAX_NUM : MI_WRAM_B_MAX_NUM;

  SDK_ASSERT(0 <= slot && slot < maxNum);

  for (n = slot; n < slot + blocks; n++) {
    int reserve = MIi_GetReservation(wram, n);
    int master = MIi_GetMaster(wram, n);

    if (master == MI_WRAM_FREE &&
        (reserve == proc || reserve == MI_WRAM_FREE)) {

    } else {
      return 0;
    }
  }

  for (n = slot; n < slot + blocks; n++) {
    MIi_SetMaster(wram, n, proc);
    MIi_SetWramBankMaster(wram, n, proc);
    MIi_SetWramBankEnable(wram, n, MI_WRAM_ENABLE);
  }

  return MIi_GetAddress(wram, slot);

#else
  volatile BOOL finished = FALSE;
  u32 result;

  if (!MIi_LockForPxi(&finished, &result)) {
    result = 0;
  } else {
    u32 data = MIi_MakeData(MIi_PXI_COMMAND_ALLOC_SLOT, wram, proc,
                            (u32)(slot << 8) | size);
    MIi_SendToPxi(MIi_SENDTYPE_COMMAND, data);
    MIi_WaitWhileBusy(&finished);
  }
  return result;
#endif
}

int MI_FreeWram(MIWramPos wram, MIWramProc proc) {
  int result;
  u16 lockid;

  OSIntrMode e = MIi_LockWramManagerDI(&lockid);
  result = MIi_DoFreeWram(wram, proc);
  MIi_UnlockWramManagerRI(lockid, e);

  return result;
}

static int MIi_DoFreeWram(MIWramPos wram, MIWramProc proc) {
#ifdef SDK_ARM9
  int n;
  int freeCount = 0;
  int maxNum = (wram == MI_WRAM_A) ? MI_WRAM_A_MAX_NUM : MI_WRAM_B_MAX_NUM;

  for (n = 0; n < maxNum; n++) {
    int master = MIi_GetMaster(wram, n);
    if (master == proc) {
      MIi_SetMaster(wram, n, MI_WRAM_FREE);
      MIi_SetWramBankMaster(wram, n, MI_WRAM_ARM7 /*dummy*/);
      MIi_SetWramBankEnable(wram, n, MI_WRAM_DISABLE);
      freeCount++;
    }
  }

  return freeCount;

#else
  volatile BOOL finished = FALSE;
  u32 result;

  if (!MIi_LockForPxi(&finished, &result)) {
    return -1;
  } else {
    u32 data = MIi_MakeData(MIi_PXI_COMMAND_FREE, wram, proc, 0);
    MIi_SendToPxi(MIi_SENDTYPE_COMMAND, data);
    MIi_WaitWhileBusy(&finished);
  }
  return (int)result;
#endif
}

int MI_FreeWramSlot(MIWramPos wram, int slot, MIWramSize size,
                    MIWramProc proc) {
  int result;
  u16 lockid;

  OSIntrMode e = MIi_LockWramManagerDI(&lockid);
  result = MIi_DoFreeWramSlot(wram, slot, size, proc);
  MIi_UnlockWramManagerRI(lockid, e);

  return result;
}

static int MIi_DoFreeWramSlot(MIWramPos wram, int slot, MIWramSize size,
                              MIWramProc proc) {
#ifdef SDK_ARM9
  int blocks = (int)((wram == MI_WRAM_A) ? (size / 2) : size);
  int n;
  int freeCount = 0;

  for (n = slot; n < slot + blocks; n++) {
    int master = MIi_GetMaster(wram, n);
    if (master == proc) {
      MIi_SetMaster(wram, n, MI_WRAM_FREE);
      MIi_SetWramBankMaster(wram, n, MI_WRAM_ARM7 /*dummy*/);
      MIi_SetWramBankEnable(wram, n, MI_WRAM_DISABLE);
      freeCount++;
    }
  }

  return freeCount;

#else
  volatile BOOL finished = FALSE;
  u32 result;

  if (!MIi_LockForPxi(&finished, &result)) {
    return -1;
  } else {
    u32 data = MIi_MakeData(MIi_PXI_COMMAND_FREE_SLOT, wram, proc,
                            (u32)(slot << 8) | size);
    MIi_SendToPxi(MIi_SENDTYPE_COMMAND, data);
    MIi_WaitWhileBusy(&finished);
  }
  return (int)result;
#endif
}

int MI_SwitchWram(MIWramPos wram, MIWramProc proc, MIWramProc newProc) {
  int result;
  u16 lockid;

  OSIntrMode e = MIi_LockWramManagerDI(&lockid);
  result = MIi_DoSwitchWram(wram, proc, newProc);
  MIi_UnlockWramManagerRI(lockid, e);

  return result;
}

static int MIi_DoSwitchWram(MIWramPos wram, MIWramProc proc,
                            MIWramProc newProc) {
#ifdef SDK_ARM9
  int n;
  int retval = 0;
  int maxNum = (wram == MI_WRAM_A) ? MI_WRAM_A_MAX_NUM : MI_WRAM_B_MAX_NUM;

  for (n = 0; n < maxNum; n++) {
    int master = MIi_GetMaster(wram, n);
    if (master == proc) {
      MIi_SetMaster(wram, n, newProc);
      MIi_SetWramBankMaster(wram, n, newProc);
      MIi_SetWramBankEnable(wram, n, MI_WRAM_ENABLE);
      retval++;
    }
  }
  return retval;

#else
  volatile BOOL finished = FALSE;
  u32 result;

  if (!MIi_LockForPxi(&finished, &result)) {
    return -1;
  } else {
    u32 data =
        MIi_MakeData(MIi_PXI_COMMAND_SWITCH, wram, proc, (u32)(newProc << 12));
    MIi_SendToPxi(MIi_SENDTYPE_COMMAND, data);
    MIi_WaitWhileBusy(&finished);
  }
  return (int)result;
#endif
}

int MI_SwitchWramSlot(MIWramPos wram, int slot, MIWramSize size,
                      MIWramProc proc, MIWramProc newProc) {
  int result;
  u16 lockid;

  OSIntrMode e = MIi_LockWramManagerDI(&lockid);
  result = MIi_DoSwitchWramSlot(wram, slot, size, proc, newProc);
  MIi_UnlockWramManagerRI(lockid, e);

  return result;
}

static int MIi_DoSwitchWramSlot(MIWramPos wram, int slot, MIWramSize size,
                                MIWramProc proc, MIWramProc newProc) {
#ifdef SDK_ARM9
  int blocks = (int)((wram == MI_WRAM_A) ? (size / 2) : size);
  int retval = 0;
  int n;

  for (n = slot; n < slot + blocks; n++) {
    int master = MIi_GetMaster(wram, n);
    if (master == proc) {
      MIi_SetMaster(wram, n, newProc);
      MIi_SetWramBankMaster(wram, n, newProc);
      MIi_SetWramBankEnable(wram, n, MI_WRAM_ENABLE);
      retval++;
    }
  }
  return retval;

#else
  volatile BOOL finished = FALSE;
  u32 result;

  if (!MIi_LockForPxi(&finished, &result)) {
    return -1;
  } else {
    u32 data = MIi_MakeData(MIi_PXI_COMMAND_SWITCH_SLOT, wram, proc,
                            (u32)((newProc << 12) | (slot << 8) | (size)));
    MIi_SendToPxi(MIi_SENDTYPE_COMMAND, data);
    MIi_WaitWhileBusy(&finished);
  }
  return (int)result;
#endif
}

u32 MI_ReserveWram(MIWramPos wram, MIWramSize size, MIWramProc proc) {
  u32 result;
  u16 lockid;

  OSIntrMode e = MIi_LockWramManagerDI(&lockid);
  result = MIi_DoReserveWram(wram, size, proc);
  MIi_UnlockWramManagerRI(lockid, e);

  return result;
}

static u32 MIi_DoReserveWram(MIWramPos wram, MIWramSize size, MIWramProc proc) {
#ifdef SDK_ARM9
  int blocks = (int)((wram == MI_WRAM_A) ? (size / 2) : size);
  int allocated = 0;
  int firstBlock;
  int n;
  int maxNum = (wram == MI_WRAM_A) ? MI_WRAM_A_MAX_NUM : MI_WRAM_B_MAX_NUM;

  for (n = 0; n < maxNum; n++) {
    int master = MIi_GetMaster(wram, n);
    int reserve = MIi_GetReservation(wram, n);

    if (master == MI_WRAM_FREE && reserve == MI_WRAM_FREE) {
      if (allocated == 0) {
        firstBlock = n;
      }

      if (++allocated >= blocks) {
        break;
      }
    } else {
      allocated = 0;
    }
  }

  if (allocated < blocks) {
    return 0;
  }

  for (n = 0; n < blocks; n++) {
    MIi_SetReservation(wram, firstBlock + n, proc);
  }

  return MIi_GetAddress(wram, firstBlock);

#else
  u32 result;
  volatile BOOL finished = FALSE;

  if (!MIi_LockForPxi(&finished, &result)) {
    result = 0;
  } else {
    u32 data = MIi_MakeData(MIi_PXI_COMMAND_RESERVE, wram, proc, size);
    MIi_SendToPxi(MIi_SENDTYPE_COMMAND, data);
    MIi_WaitWhileBusy(&finished);
  }
  return result;
#endif
}

u32 MI_ReserveWramSlot(MIWramPos wram, int slot, MIWramSize size,
                       MIWramProc proc) {
  u32 result;
  u16 lockid;

  OSIntrMode e = MIi_LockWramManagerDI(&lockid);
  result = MIi_DoReserveWramSlot(wram, slot, size, proc);
  MIi_UnlockWramManagerRI(lockid, e);

  return result;
}

static u32 MIi_DoReserveWramSlot(MIWramPos wram, int slot, MIWramSize size,
                                 MIWramProc proc) {
#ifdef SDK_ARM9
  int blocks = (int)((wram == MI_WRAM_A) ? (size / 2) : size);
  int n;
  int maxNum = (wram == MI_WRAM_A) ? MI_WRAM_A_MAX_NUM : MI_WRAM_B_MAX_NUM;

  SDK_ASSERT(0 <= slot && slot < maxNum);

  for (n = slot; n < slot + blocks; n++) {
    int master = MIi_GetMaster(wram, slot);
    int reserve = MIi_GetReservation(wram, slot);

    if (master == MI_WRAM_FREE && reserve == MI_WRAM_FREE) {

    } else {
      return 0;
    }
  }

  for (n = slot; n < slot + blocks; n++) {
    MIi_SetReservation(wram, n, proc);
  }

  return MIi_GetAddress(wram, slot);

#else
  u32 result;
  volatile BOOL finished = FALSE;

  if (!MIi_LockForPxi(&finished, &result)) {
    result = 0;
  } else {
    u32 data = MIi_MakeData(MIi_PXI_COMMAND_RESERVE_SLOT, wram, proc,
                            (u32)(slot << 8) | size);
    MIi_SendToPxi(MIi_SENDTYPE_COMMAND, data);
    MIi_WaitWhileBusy(&finished);
  }
  return result;
#endif
}

int MI_CancelWram(MIWramPos wram, MIWramProc proc) {
  int result;
  u16 lockid;

  OSIntrMode e = MIi_LockWramManagerDI(&lockid);
  result = MIi_DoCancelWram(wram, proc);
  MIi_UnlockWramManagerRI(lockid, e);

  return result;
}

static int MIi_DoCancelWram(MIWramPos wram, MIWramProc proc) {
#ifdef SDK_ARM9
  int retval = 0;
  int maxNum = (wram == MI_WRAM_A) ? MI_WRAM_A_MAX_NUM : MI_WRAM_B_MAX_NUM;
  int n;

  for (n = 0; n < maxNum; n++) {
    if (MIi_GetReservation(wram, n) == proc) {
      MIi_SetReservation(wram, n, MI_WRAM_FREE);
      retval++;
    }
  }
  return retval;

#else
  volatile BOOL finished = FALSE;
  u32 result;

  if (!MIi_LockForPxi(&finished, &result)) {
    return -1;
  } else {
    u32 data = MIi_MakeData(MIi_PXI_COMMAND_CANCEL, wram, proc, 0);
    MIi_SendToPxi(MIi_SENDTYPE_COMMAND, data);
    MIi_WaitWhileBusy(&finished);
  }
  return (int)result;
#endif
}

int MI_CancelWramSlot(MIWramPos wram, int slot, MIWramSize size,
                      MIWramProc proc) {
  int result;
  u16 lockid;

  OSIntrMode e = MIi_LockWramManagerDI(&lockid);
  result = MIi_DoCancelWramSlot(wram, slot, size, proc);
  MIi_UnlockWramManagerRI(lockid, e);

  return result;
}

static int MIi_DoCancelWramSlot(MIWramPos wram, int slot, MIWramSize size,
                                MIWramProc proc) {
#ifdef SDK_ARM9
  int blocks = (int)((wram == MI_WRAM_A) ? (size / 2) : size);
  int retval = 0;
  int n;
  int maxNum = (wram == MI_WRAM_A) ? MI_WRAM_A_MAX_NUM : MI_WRAM_B_MAX_NUM;

  for (n = slot; n < slot + blocks; n++) {
    if (MIi_GetReservation(wram, n) == proc) {
      MIi_SetReservation(wram, n, MI_WRAM_FREE);
      retval++;
    }
  }
  return retval;

#else
  volatile BOOL finished = FALSE;
  u32 result;

  if (!MIi_LockForPxi(&finished, &result)) {
    return -1;
  } else {
    u32 data = MIi_MakeData(MIi_PXI_COMMAND_CANCEL_SLOT, wram, proc,
                            (u32)(slot << 8) | size);
    MIi_SendToPxi(MIi_SENDTYPE_COMMAND, data);
    MIi_WaitWhileBusy(&finished);
  }
  return (int)result;
#endif
}

int MI_LockWramSlots(MIWramPos wram, u32 slots) {
  int result;
  u16 lockid;

  OSIntrMode e = MIi_LockWramManagerDI(&lockid);
  result = MIi_DoLockWramSlots(wram, slots);
  MIi_UnlockWramManagerRI(lockid, e);

  return result;
}

static int MIi_DoLockWramSlots(MIWramPos wram, u32 slots) {
#ifdef SDK_ARM9
  u32 result;
  volatile BOOL finished = FALSE;

  if (!MIi_LockForPxi(&finished, &result)) {
    return -1;
  } else {
    u32 data =
        MIi_MakeData(MIi_PXI_COMMAND_LOCKWRAM, wram, (MIWramProc)0, slots);
    MIi_SendToPxi(MIi_SENDTYPE_COMMAND, data);
    MIi_WaitWhileBusy(&finished);
  }
  return (int)result;

#else
  switch (wram) {
  case MI_WRAM_A:
    reg_MI_MBK_A_LOCK |= slots;
    break;
  case MI_WRAM_B:
    reg_MI_MBK_B_LOCK |= slots;
    break;
  case MI_WRAM_C:
    reg_MI_MBK_C_LOCK |= slots;
    break;
  }

  return 0;
#endif
}

int MI_UnlockWramSlots(MIWramPos wram, u32 slots) {
  int result;
  u16 lockid;

  OSIntrMode e = MIi_LockWramManagerDI(&lockid);
  result = MIi_DoUnlockWramSlots(wram, slots);
  MIi_UnlockWramManagerRI(lockid, e);

  return result;
}

static int MIi_DoUnlockWramSlots(MIWramPos wram, u32 slots) {
#ifdef SDK_ARM9
  volatile BOOL finished = FALSE;
  u32 result;

  if (!MIi_LockForPxi(&finished, &result)) {
    return -1;
  } else {
    u32 data =
        MIi_MakeData(MIi_PXI_COMMAND_UNLOCKWRAM, wram, (MIWramProc)0, slots);
    MIi_SendToPxi(MIi_SENDTYPE_COMMAND, data);
    MIi_WaitWhileBusy(&finished);
  }
  return (int)result;

#else
  switch (wram) {
  case MI_WRAM_A:
    reg_MI_MBK_A_LOCK &= ~slots;
    break;
  case MI_WRAM_B:
    reg_MI_MBK_B_LOCK &= ~slots;
    break;
  case MI_WRAM_C:
    reg_MI_MBK_C_LOCK &= ~slots;
    break;
  }
  return 0;
#endif
}

MIWramProc MI_GetWramReservation(MIWramPos wram, int slot) {
#ifdef SDK_ARM9
  return (MIWramProc)MIi_GetReservation(wram, slot);

#else
  volatile BOOL finished = FALSE;
  u32 result;

  if (!MIi_LockForPxi(&finished, &result)) {

  } else {
    u32 data = MIi_MakeData(MIi_PXI_COMMAND_GET_RESERVE, wram, (MIWramProc)0,
                            (u32)slot);
    MIi_SendToPxi(MIi_SENDTYPE_COMMAND, data);
    MIi_WaitWhileBusy(&finished);
  }
  return (MIWramProc)result;
#endif
}

u8 MI_GetAllocatableWramSlot(MIWramPos wram, MIWramProc proc) {
  u8 result;
  u16 lockid;

  OSIntrMode e = MIi_LockWramManagerDI(&lockid);
  result = (u8)MIi_DoGetAllocatableWramSlot(wram, proc);
  MIi_UnlockWramManagerRI(lockid, e);

  return result;
}

static int MIi_DoGetAllocatableWramSlot(MIWramPos wram, MIWramProc proc) {
#ifdef SDK_ARM9
  int retval = 0;
  int n;
  int maxNum = (wram == MI_WRAM_A) ? MI_WRAM_A_MAX_NUM : MI_WRAM_B_MAX_NUM;

  for (n = 0; n < maxNum; n++) {
    int master = MIi_GetMaster(wram, n);
    int reserve = MIi_GetReservation(wram, n);

    if (master == MI_WRAM_FREE &&
        (reserve == proc || reserve == MI_WRAM_FREE)) {
      retval |= (1 << n);
    }
  }
  return retval;

#else
  volatile BOOL finished = FALSE;
  u32 result;

  if (!MIi_LockForPxi(&finished, &result)) {
    return -1;
  } else {
    u32 data = MIi_MakeData(MIi_PXI_COMMAND_GET_ALLOCATABLE, wram, proc, 0);
    MIi_SendToPxi(MIi_SENDTYPE_COMMAND, data);
    MIi_WaitWhileBusy(&finished);
  }
  return (int)result;
#endif
}

#ifdef SDK_ARM9

u8 MI_GetUsedWramSlot(MIWramPos wram) {
  u16 lockid;
  OSIntrMode e = MIi_LockWramManagerDI(&lockid);
  int retval = 0;
  int n;
  int maxNum = (wram == MI_WRAM_A) ? MI_WRAM_A_MAX_NUM : MI_WRAM_B_MAX_NUM;

  for (n = 0; n < maxNum; n++) {
    if (MIi_GetMaster(wram, n) != MI_WRAM_FREE) {
      retval |= (1 << n);
    }
  }
  MIi_UnlockWramManagerRI(lockid, e);
  return (u8)retval;
}

BOOL MI_IsWramSlotUsed(MIWramPos wram, int slot) {
  return (MIi_GetMaster(wram, slot) == MI_WRAM_FREE) ? FALSE : TRUE;
}
#endif

static int MIi_LockForPxi(volatile BOOL *finishFlag, u32 *ptr) {
  OSIntrMode enabled = OS_DisableInterrupts();
  if (lock) {
    (void)OS_RestoreInterrupts(enabled);
    return FALSE;
  }

  lock = TRUE;
  finishPtr = finishFlag;
  resultPtr = ptr;

  (void)OS_RestoreInterrupts(enabled);

  return TRUE;
}

static void MIi_WaitWhileBusy(volatile BOOL *finished) {
  while (*finished == FALSE) {
    if (OS_GetCpsrIrq() == OS_INTRMODE_IRQ_DISABLE) {
      PXIi_HandlerRecvFifoNotEmpty();
    }
    SVC_WaitByLoop(1);
  }
}

static void MIi_SendToPxi(BOOL sendType, u32 data) {
  while (PXI_SendWordByFifo(PXI_FIFO_TAG_MI, data, sendType) !=
         PXI_FIFO_SUCCESS) {
    SVC_WaitByLoop(1);
  }
}

static u32 MIi_MakeData(u32 command, MIWramPos wram, MIWramProc proc,
                        u32 data) {
  return (command << 20) | (wram << 18) | (proc << 16) | data;
}

static void MIi_TakeApartData(u32 pxiData, u32 *command, MIWramPos *wram,
                              MIWramProc *proc, u32 *data) {
  if (command) {
    *command = (pxiData >> 20) & 0xf;
  }
  if (wram) {
    *wram = (MIWramPos)((pxiData >> 18) & 3);
  }
  if (proc) {
    *proc = (MIWramProc)((pxiData >> 16) & 3);
  }
  if (data) {
    *data = (pxiData) & 0xffff;
  }
}

void MI_LockWramManager(u16 *lockid) {
  *lockid = (u16)OS_GetLockID();
  (void)OS_LockByWord(*lockid, (OSLockWord *)(HW_WRAM_EX_LOCK_BUF), NULL);
}

void MI_UnlockWramManager(u16 lockid) {
  (void)OS_UnlockByWord(lockid, (OSLockWord *)(HW_WRAM_EX_LOCK_BUF), NULL);
  OS_ReleaseLockID(lockid);
}

s32 MI_TryLockWramManager(u16 *lockid) {
  *lockid = (u16)OS_GetLockID();
  return OS_TryLockByWord(*lockid, (OSLockWord *)(HW_WRAM_EX_LOCK_BUF), NULL);
}

static OSIntrMode MIi_LockWramManagerDI(u16 *lockid) {
  OSIntrMode e = OS_DisableInterrupts();
  MI_LockWramManager(lockid);
  return e;
}

static void MIi_UnlockWramManagerRI(u16 lockid, OSIntrMode e) {
  MI_UnlockWramManager(lockid);
  (void)OS_RestoreInterrupts(e);
}

static char *getMasterStr(MIWramProc m);
static char *getMasterStr(MIWramProc m) {
  static char strARM9[] = "ARM9";
  static char strARM7[] = "ARM7";
  static char strDSP[] = "DSP ";
  static char strNA[] = "----";

  switch (m) {
  case MI_WRAM_ARM9:
    return strARM9;
  case MI_WRAM_ARM7:
    return strARM7;
  case MI_WRAM_DSP:
    return strDSP;
  }
  return strNA;
}

void MI_DumpWramList(MIWramPos wram) {
#ifdef SDK_FINALROM
#pragma unused(wram)
#else
  int n;
  int maxNum = (wram == MI_WRAM_A) ? MI_WRAM_A_MAX_NUM : MI_WRAM_B_MAX_NUM;

  OS_TPrintf("----WRAM-%c (%x-%x)\n", 'A' + (int)wram, MI_GetWramMapStart(wram),
             MI_GetWramMapEnd(wram));

  OS_TPrintf("      ALLOC  RESERVE\n");

  for (n = 0; n < maxNum; n++) {

    OS_TPrintf("slot%d  ", n);

    if (MI_GetWramBankEnable(wram, n)) {
      OS_TPrintf("%s   ", getMasterStr(MI_GetWramBankMaster(wram, n)));
    } else {
      OS_TPrintf("----   ");
    }

    OS_TPrintf("%s\n", getMasterStr(MI_GetWramReservation(wram, n)));
  }
#endif
}

void MI_DumpWramListAll(void) {
#ifndef SDK_FINALROM
  MI_DumpWramList(MI_WRAM_A);
  MI_DumpWramList(MI_WRAM_B);
  MI_DumpWramList(MI_WRAM_C);
  OS_TPrintf("\n");
#endif // ifndef SDK_FINALROM
}

#ifdef SDK_ARM9
#include <twl/ltdmain_end.h>
#endif

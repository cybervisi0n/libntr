#ifndef TWL_MI_SHAREDWRAM_H_
#define TWL_MI_SHAREDWRAM_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum { MI_WRAM_A = 0, MI_WRAM_B = 1, MI_WRAM_C = 2 } MIWramPos;

#define MI_WRAM_A_MAX_NUM 4
#define MI_WRAM_B_MAX_NUM 8
#define MI_WRAM_C_MAX_NUM 8

#define MI_WRAM_A_SIZE HW_WRAM_A_SIZE
#define MI_WRAM_B_SIZE HW_WRAM_B_SIZE
#define MI_WRAM_C_SIZE HW_WRAM_C_SIZE

#ifdef SDK_TWLHYB
#define MI_WRAM_ADDRESS_A0 0x03000000
#else
#define MI_WRAM_ADDRESS_A0 0x037c0000
#endif
#define MI_WRAM_ADDRESS_B0 HW_WRAM_B
#define MI_WRAM_ADDRESS_C0 HW_WRAM_C

#define MI_WRAM_A_SLOT_SIZE 0x10000
#define MI_WRAM_B_SLOT_SIZE 0x8000
#define MI_WRAM_C_SLOT_SIZE 0x8000

typedef enum {
  MI_WRAM_ARM9 = 0,
  MI_WRAM_ARM7 = 1,
  MI_WRAM_DSP = 2, // only for WRAM_B and WRAM_C

  MI_WRAM_FREE = 3
} MIWramProc;

#define MI_WRAM_MASTER_MASK_A REG_MI_MBK_A0_M_MASK
#define MI_WRAM_MASTER_MASK_B REG_MI_MBK_B0_M_MASK
#define MI_WRAM_MASTER_MASK_C REG_MI_MBK_C0_M_MASK

typedef enum {
  MI_WRAM_OFFSET_0KB = 0,
  MI_WRAM_OFFSET_32KB = 1,
  MI_WRAM_OFFSET_64KB = 2,
  MI_WRAM_OFFSET_96KB = 3,
  MI_WRAM_OFFSET_128KB = 4,
  MI_WRAM_OFFSET_160KB = 5,
  MI_WRAM_OFFSET_192KB = 6,
  MI_WRAM_OFFSET_224KB = 7
} MIWramOffset;

#define MI_WRAM_OFFSET_MASK_A REG_MI_MBK_A0_OF_MASK
#define MI_WRAM_OFFSET_MASK_B REG_MI_MBK_B0_OF_MASK
#define MI_WRAM_OFFSET_MASK_C REG_MI_MBK_C0_OF_MASK
#define MI_WRAM_OFFSET_SHIFT_A REG_MI_MBK_A0_OF_SHIFT
#define MI_WRAM_OFFSET_SHIFT_B REG_MI_MBK_B0_OF_SHIFT
#define MI_WRAM_OFFSET_SHIFT_C REG_MI_MBK_C0_OF_SHIFT

typedef enum { MI_WRAM_ENABLE = TRUE, MI_WRAM_DISABLE = FALSE } MIWramEnable;

#define MI_WRAM_ENABLE_MASK_A REG_MI_MBK_A0_E_MASK
#define MI_WRAM_ENABLE_MASK_B REG_MI_MBK_B0_E_MASK
#define MI_WRAM_ENABLE_MASK_C REG_MI_MBK_C0_E_MASK

typedef enum {
  MI_WRAM_IMAGE_32KB = 0, // only for WRAM_B and WRAM_C
  MI_WRAM_IMAGE_64KB = 1,
  MI_WRAM_IMAGE_128KB = 2,
  MI_WRAM_IMAGE_256KB = 3
} MIWramImage;

typedef enum {
  MI_WRAM_SIZE_0KB = 0,
  MI_WRAM_SIZE_32KB = 1,
  MI_WRAM_SIZE_64KB = 2,
  MI_WRAM_SIZE_96KB = 3,
  MI_WRAM_SIZE_128KB = 4,
  MI_WRAM_SIZE_160KB = 5,
  MI_WRAM_SIZE_192KB = 6,
  MI_WRAM_SIZE_224KB = 7,
  MI_WRAM_SIZE_256KB = 8
} MIWramSize;

#define MI_WRAM_SIZE_TO_ENUM(size) ((MIWramSize)((int)(size) >> 5))
#define MI_WRAM_ENUM_TO_SIZE(n) ((int)((n) << 5))

#define MI_WRAM_ERROR_LOCKED -1

#if defined(SDK_ARM9) || defined(SDK_PORT)

void MIi_SetWramBank_A(int num, MIWramProc proc, MIWramOffset offset,
                       MIWramEnable enable);
void MIi_SetWramBank_B(int num, MIWramProc proc, MIWramOffset offset,
                       MIWramEnable enable);
void MIi_SetWramBank_C(int num, MIWramProc proc, MIWramOffset offset,
                       MIWramEnable enable);
void MIi_SetWramBank(MIWramPos wram, int num, MIWramProc proc,
                     MIWramOffset offset, MIWramEnable enable);

void MIi_SetWramBankMaster_A(int num, MIWramProc proc);
void MIi_SetWramBankMaster_B(int num, MIWramProc proc);
void MIi_SetWramBankMaster_C(int num, MIWramProc proc);
void MIi_SetWramBankMaster(MIWramPos wram, int num, MIWramProc proc);

void MIi_SetWramBankEnable_A(int num, MIWramEnable enable);
void MIi_SetWramBankEnable_B(int num, MIWramEnable enable);
void MIi_SetWramBankEnable_C(int num, MIWramEnable enable);
void MIi_SetWramBankEnable(MIWramPos wram, int num, MIWramEnable enable);
#endif

vu8 MIi_GetWramBank_A(int num);
vu8 MIi_GetWramBank_B(int num);
vu8 MIi_GetWramBank_C(int num);
vu8 MIi_GetWramBank(MIWramPos wram, int num);

MIWramProc MI_GetWramBankMaster_A(int num);
MIWramProc MI_GetWramBankMaster_B(int num);
MIWramProc MI_GetWramBankMaster_C(int num);
MIWramProc MI_GetWramBankMaster(MIWramPos wram, int num);

MIWramOffset MI_GetWramBankOffset_A(int num);
MIWramOffset MI_GetWramBankOffset_B(int num);
MIWramOffset MI_GetWramBankOffset_C(int num);
MIWramOffset MI_GetWramBankOffset(MIWramPos wram, int num);

MIWramEnable MI_GetWramBankEnable_A(int num);
MIWramEnable MI_GetWramBankEnable_B(int num);
MIWramEnable MI_GetWramBankEnable_C(int num);
MIWramEnable MI_GetWramBankEnable(MIWramPos wram, int num);

void MIi_SetWramMap_A(u32 start, u32 end, MIWramImage image);
void MIi_SetWramMap_B(u32 start, u32 end, MIWramImage image);
void MIi_SetWramMap_C(u32 start, u32 end, MIWramImage image);

u32 MI_GetWramMapStart_A(void);
u32 MI_GetWramMapStart_B(void);
u32 MI_GetWramMapStart_C(void);
u32 MI_GetWramMapStart(MIWramPos wram);

u32 MI_GetWramMapEnd_A(void);
u32 MI_GetWramMapEnd_B(void);
u32 MI_GetWramMapEnd_C(void);
u32 MI_GetWramMapEnd(MIWramPos wram);

MIWramImage MI_GetWramMapImage_A(void);
MIWramImage MI_GetWramMapImage_B(void);
MIWramImage MI_GetWramMapImage_C(void);
MIWramImage MI_GetWramMapImage(MIWramPos wram);

BOOL MI_IsWramSlotLocked_A(int num);
BOOL MI_IsWramSlotLocked_B(int num);
BOOL MI_IsWramSlotLocked_C(int num);
BOOL MI_IsWramSlotLocked(MIWramPos wram, int num);

SDK_INLINE int MIi_AddressToWramSlot(const void *address, MIWramPos *type) {
  int retval = -1;
  static const u32 wramBCtop = HW_WRAM_C;
  static const u32 wramBCbototm = HW_WRAM_B_END;
  SDK_COMPILER_ASSERT(HW_WRAM_C < HW_WRAM_B);
  SDK_COMPILER_ASSERT(wramBCbototm - wramBCtop ==
                      HW_WRAM_B_SIZE + HW_WRAM_C_SIZE);
  #ifdef SDK_BUILD_ARM
  u32 slot = (u32)(((u32)address - wramBCtop) / MI_WRAM_B_SLOT_SIZE);
  if (slot < MI_WRAM_C_MAX_NUM) {
    if (type) {
      *type = MI_WRAM_C;
    }
    retval = (int)slot;
  } else if (slot < MI_WRAM_B_MAX_NUM + MI_WRAM_C_MAX_NUM) {
    if (type) {
      *type = MI_WRAM_B;
    }
    retval = (int)(slot - MI_WRAM_C_MAX_NUM);
  }
  return retval;
  #else
  return 0;
  #endif
}

void MI_InitWramManager(void);

u32 MI_AllocWram(MIWramPos wram, MIWramSize size, MIWramProc proc);
#define MI_AllocWram_A(size, proc) MI_AllocWram(MI_WRAM_A, size, proc)
#define MI_AllocWram_B(size, proc) MI_AllocWram(MI_WRAM_B, size, proc)
#define MI_AllocWram_C(size, proc) MI_AllocWram(MI_WRAM_C, size, proc)

u32 MI_AllocWramSlot(MIWramPos wram, int slot, MIWramSize size,
                     MIWramProc proc);
#define MI_AllocWramSlot_A(slot, size, proc)                                   \
  MI_AllocWramSlot(MI_WRAM_A, slot, size, proc)
#define MI_AllocWramSlot_B(slot, size, proc)                                   \
  MI_AllocWramSlot(MI_WRAM_B, slot, size, proc)
#define MI_AllocWramSlot_C(slot, size, proc)                                   \
  MI_AllocWramSlot(MI_WRAM_C, slot, size, proc)

int MI_FreeWram(MIWramPos wram, MIWramProc proc);
#define MI_FreeWram_A(proc) MI_FreeWram(MI_WRAM_A, proc)
#define MI_FreeWram_B(proc) MI_FreeWram(MI_WRAM_B, proc)
#define MI_FreeWram_C(proc) MI_FreeWram(MI_WRAM_C, proc)

int MI_FreeWramSlot(MIWramPos wram, int slot, MIWramSize size, MIWramProc proc);
#define MI_FreeWramSlot_A(slot, size, proc)                                    \
  MI_FreeWramSlot(MI_WRAM_A, slot, size, proc)
#define MI_FreeWramSlot_B(slot, size, proc)                                    \
  MI_FreeWramSlot(MI_WRAM_B, slot, size, proc)
#define MI_FreeWramSlot_C(slot, size, proc)                                    \
  MI_FreeWramSlot(MI_WRAM_C, slot, size, proc)

int MI_SwitchWram(MIWramPos wram, MIWramProc proc, MIWramProc newProc);
#define MI_SwitchWram_A(proc, newProc) MI_SwitchWram(MI_WRAM_A, proc, newProc)
#define MI_SwitchWram_B(proc, newProc) MI_SwitchWram(MI_WRAM_B, proc, newProc)
#define MI_SwitchWram_C(proc, newProc) MI_SwitchWram(MI_WRAM_C, proc, newProc)

int MI_SwitchWramSlot(MIWramPos wram, int slot, MIWramSize size,
                      MIWramProc proc, MIWramProc newProc);
#define MI_SwitchWramSlot_A(slot, size, proc, newProc)                         \
  MI_SwitchWramSlot(MI_WRAM_A, slot, size, proc, newProc)
#define MI_SwitchWramSlot_B(slot, size, proc, newProc)                         \
  MI_SwitchWramSlot(MI_WRAM_B, slot, size, proc, newProc)
#define MI_SwitchWramSlot_C(slot, size, proc, newProc)                         \
  MI_SwitchWramSlot(MI_WRAM_C, slot, size, proc, newProc)

u32 MI_ReserveWram(MIWramPos wram, MIWramSize size, MIWramProc proc);
#define MI_ReserveWram_A(size, proc) MI_ReserveWram(MI_WRAM_A, size, proc)
#define MI_ReserveWram_B(size, proc) MI_ReserveWram(MI_WRAM_B, size, proc)
#define MI_ReserveWram_C(size, proc) MI_ReserveWram(MI_WRAM_C, size, proc)

u32 MI_ReserveWramSlot(MIWramPos wram, int slot, MIWramSize size,
                       MIWramProc proc);
#define MI_ReserveWramSlot_A(slot, size, proc)                                 \
  MI_ReserveWramSlot(MI_WRAM_A, slot, size, proc)
#define MI_ReserveWramSlot_B(slot, size, proc)                                 \
  MI_ReserveWramSlot(MI_WRAM_B, slot, size, proc)
#define MI_ReserveWramSlot_C(slot, size, proc)                                 \
  MI_ReserveWramSlot(MI_WRAM_C, slot, size, proc)

int MI_CancelWram(MIWramPos wram, MIWramProc proc);
#define MI_CancelWram_A(proc) MI_CancelWram(MI_WRAM_A, proc)
#define MI_CancelWram_B(proc) MI_CancelWram(MI_WRAM_B, proc)
#define MI_CancelWram_C(proc) MI_CancelWram(MI_WRAM_C, proc)

int MI_CancelWramSlot(MIWramPos wram, int slot, MIWramSize size,
                      MIWramProc proc);
#define MI_CancelWramSlot_A(slot, size, proc)                                  \
  MI_CancelWramSlot(MI_WRAM_A, slot, size, proc)
#define MI_CancelWramSlot_B(slot, size, proc)                                  \
  MI_CancelWramSlot(MI_WRAM_B, slot, size, proc)
#define MI_CancelWramSlot_C(slot, size, proc)                                  \
  MI_CancelWramSlot(MI_WRAM_C, slot, size, proc)

int MI_LockWramSlots(MIWramPos wram, u32 slots);
#define MI_LockWramSlots_A(slots) MI_LockWramSlots(MI_WRAM_A, slots)
#define MI_LockWramSlots_B(slots) MI_LockWramSlots(MI_WRAM_B, slots)
#define MI_LockWramSlots_C(slots) MI_LockWramSlots(MI_WRAM_C, slots)

int MI_UnlockWramSlots(MIWramPos wram, u32 slots);
#define MI_UnlockWramSlots_A(slots) MI_UnlockWramSlots(MI_WRAM_A, slots)
#define MI_UnlockWramSlots_B(slots) MI_UnlockWramSlots(MI_WRAM_B, slots)
#define MI_UnlockWramSlots_C(slots) MI_UnlockWramSlots(MI_WRAM_C, slots)

MIWramProc MI_GetWramReservation(MIWramPos wram, int slot);
#define MI_GetWramReservation_A(slot) MI_GetWramReservation(MI_WRAM_A, slot)
#define MI_GetWramReservation_B(slot) MI_GetWramReservation(MI_WRAM_B, slot)
#define MI_GetWramReservation_C(slot) MI_GetWramReservation(MI_WRAM_C, slot)

u8 MI_GetAllocatableWramSlot(MIWramPos wram, MIWramProc proc);
#define MI_GetAllocatableWramSlot_A(proc)                                      \
  MI_GetAllocatableWramSlot(MI_WRAM_A, proc)
#define MI_GetAllocatableWramSlot_B(proc)                                      \
  MI_GetAllocatableWramSlot(MI_WRAM_B, proc)
#define MI_GetAllocatableWramSlot_C(proc)                                      \
  MI_GetAllocatableWramSlot(MI_WRAM_C, proc)

#define MI_GetFreeWramSlot(wram) MI_GetAllocatableWramSlot(wram, MI_WRAM_FREE)
#define MI_GetFreeWramSlot_A() MI_GetFreeWramSlot(MI_WRAM_A)
#define MI_GetFreeWramSlot_B() MI_GetFreeWramSlot(MI_WRAM_B)
#define MI_GetFreeWramSlot_C() MI_GetFreeWramSlot(MI_WRAM_C)

#if defined(SDK_ARM9) || defined(SDK_PORT)
u8 MI_GetUsedWramSlot(MIWramPos wram);
#define MI_GetUsedWramSlot_A() MI_GetUsedWramSlot(MI_WRAM_A)
#define MI_GetUsedWramSlot_B() MI_GetUsedWramSlot(MI_WRAM_B)
#define MI_GetUsedWramSlot_C() MI_GetUsedWramSlot(MI_WRAM_C)
#endif

#if defined(SDK_ARM9) || defined(SDK_PORT)
BOOL MI_IsWramSlotUsed(MIWramPos wram, int slot);
#define MI_IsWramSlotUsed_A(slot) MI_IsWramSlotUsed(MI_WRAM_A, slot)
#define MI_IsWramSlotUsed_B(slot) MI_IsWramSlotUsed(MI_WRAM_B, slot)
#define MI_IsWramSlotUsed_C(slot) MI_IsWramSlotUsed(MI_WRAM_C, slot)
#endif

void MI_LockWramManager(u16 *lockid);

void MI_UnlockWramManager(u16 lockid);

s32 MI_TryLockWramManager(u16 *lockid);

void MI_DumpWramList(MIWramPos wram);
#define MI_DumpWramList_A() MI_DumpWramList(MI_WRAM_A)
#define MI_DumpWramList_B() MI_DumpWramList(MI_WRAM_B)
#define MI_DumpWramList_C() MI_DumpWramList(MI_WRAM_C)

void MI_DumpWramListAll(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

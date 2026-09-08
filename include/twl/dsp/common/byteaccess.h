#ifndef TWL_DSP_BYTEACCESS_H_
#define TWL_DSP_BYTEACCESS_H_

#ifdef SDK_TWL
#include <twl/types.h>
#include <twl/os.h>
#else
#include <dsp/types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef u16
    DSPAddr; // This type expresses an address in the DSP (2 bytes, 1 word)
typedef u16 DSPWord; // This type expresses a size in the DSP (2 bytes, 1 word)
typedef u16 DSPByte; // This type expresses a single-byte unit in the DSP (1
                     // byte, 1 word)
typedef u32
    DSPWord32; // This type expresses a size in the DSP (2 bytes, 1 word)
typedef u32 DSPByte32; // This type expresses a single-byte unit in the DSP (1
                       // byte, 1 word)
typedef u32 DSPAddrInARM; // This type has converted DSP addresses into bytes

#define DSP_ADDR_TO_ARM(address) (u32)((address) << 1)
#define DSP_ADDR_TO_DSP(address) (u16)((u32)(address) >> 1)
#define DSP_WORD_TO_ARM(word) (u16)((word) << 1)
#define DSP_WORD_TO_DSP(word) (u16)((word) >> 1)
#define DSP_WORD_TO_ARM32(word) (u32)((word) << 1)
#define DSP_WORD_TO_DSP32(word) (u32)((word) >> 1)
#define DSP_32BIT_TO_ARM(value)                                                \
  (u32)(((u32)(value) >> 16) | ((u32)(value) << 16))
#define DSP_32BIT_TO_DSP(value)                                                \
  (u32)(((u32)(value) >> 16) | ((u32)(value) << 16))
#ifdef SDK_TWL
#define DSP_BYTE_TO_UNIT(byte) (u16)(byte)
#define DSP_UNIT_TO_BYTE(unit) (u16)(unit)
#else
#define DSP_BYTE_TO_UNIT(byte) (u16)((byte) >> 1)
#define DSP_UNIT_TO_BYTE(unit) (u16)((unit) << 1)
#endif

#define DSP_WORD_UNIT (3 - sizeof(DSPWord))

#ifdef SDK_TWL

SDK_INLINE u16 DSP_LoadWord(DSPAddr offset) {
  u16 value;
  OSIntrMode cpsr = OS_DisableInterrupts();
  DSP_RecvFifo(DSP_FIFO_MEMSEL_DATA, &value, offset,
               DSP_WORD_TO_DSP(sizeof(u16)));
  reg_DSP_PCFG &= ~(REG_DSP_PCFG_DRS_MASK | REG_DSP_PCFG_AIM_MASK);
  (void)OS_RestoreInterrupts(cpsr);
  return value;
}

SDK_INLINE void DSP_StoreWord(DSPAddr offset, u16 value) {
  OSIntrMode cpsr = OS_DisableInterrupts();
  DSP_SendFifo(DSP_FIFO_MEMSEL_DATA, offset, &value,
               DSP_WORD_TO_DSP(sizeof(u16)));

  (void)OS_RestoreInterrupts(cpsr);
}

SDK_INLINE u8 DSP_Load8(DSPAddrInARM offset) {
  return (u8)(DSP_LoadWord(DSP_WORD_TO_DSP(offset)) >> ((offset & 1) << 3));
}

u16 DSP_Load16(DSPAddrInARM offset);

u32 DSP_Load32(DSPAddrInARM offset);

void DSP_LoadData(DSPAddrInARM offset, void *buffer, u32 length);

void DSP_Store8(DSPAddrInARM offset, u8 value);

void DSP_Store16(DSPAddrInARM offset, u16 value);

void DSP_Store32(DSPAddrInARM offset, u32 value);

void DSP_StoreData(DSPAddrInARM offset, const void *buffer, u32 length);

#endif // SDK_TWL

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TWL_DSP_BYTEACCESS_H_ */

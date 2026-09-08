#ifndef TWL_DSP_COFF_H_
#define TWL_DSP_COFF_H_

#include <twl/dsp.h>

#ifdef __cplusplus
extern "C" {
#endif

#define COFF_SECTION_ATTR_CODE_SECTION 0x00000001
#define COFF_SECTION_ATTR_MAPPED_IN_CODE_MEMORY 0x00000008
#define COFF_SECTION_ATTR_MAPPED_IN_DATA_MEMORY 0x00000010
#define COFF_SECTION_ATTR_NOLOAD_FOR_CODE_MEMORY 0x00000020
#define COFF_SECTION_ATTR_NOLOAD_FOR_DATA_MEMORY 0x00000040
#define COFF_SECTION_ATTR_BLOCK_HEADER 0x00000080
#define COFF_SECTION_ATTR_CONTAINS_C_C ++_LINE_INFO 0x00000200

#define COFF_SYMBOL_NUMBER_UNDEFINEND 0x0000
#define COFF_SYMBOL_NUMBER_ABSOLUTE 0xFFFF
#define COFF_SYMBOL_NUMBER_DEBUG 0xFFFE

#define COFF_SYMBOL_TYPE_NULL 0x00
#define COFF_SYMBOL_TYPE_VOID 0x01
#define COFF_SYMBOL_TYPE_CHAR 0x02
#define COFF_SYMBOL_TYPE_SHORT 0x03
#define COFF_SYMBOL_TYPE_INT 0x04
#define COFF_SYMBOL_TYPE_LONG 0x05
#define COFF_SYMBOL_TYPE_FLOAT 0x06
#define COFF_SYMBOL_TYPE_DOUBLE 0x07
#define COFF_SYMBOL_TYPE_STRUCT 0x08
#define COFF_SYMBOL_TYPE_UNION 0x09
#define COFF_SYMBOL_TYPE_ENUM 0x0A
#define COFF_SYMBOL_TYPE_MOE 0x0B
#define COFF_SYMBOL_TYPE_BYTE 0x0C
#define COFF_SYMBOL_TYPE_WORD 0x0D
#define COFF_SYMBOL_TYPE_UINT 0x0E
#define COFF_SYMBOL_TYPE_DWORD 0x0F

#define COFF_SYMBOL_DTYPE_NULL 0x00
#define COFF_SYMBOL_DTYPE_POINTER 0x01
#define COFF_SYMBOL_DTYPE_FUNCTION 0x02
#define COFF_SYMBOL_DTYPE_ARRAY 0x03

typedef struct COFFFileHeader {
  u16 Machine;
  u16 NumberOfSections;
  u32 TimeDateStamp;
  u32 PointerToSymbolTable;
  u32 NumberOfSymbols;
  u16 SizeOfOptionalHeader;
  u16 Characteristics;
} COFFFileHeader;

typedef struct COFFSectionTable {
  u8 Name[8];
  u32 s_paddr;
  u32 s_vaddr;
  u32 s_size;
  u32 s_scnptr;
  u32 s_relptr;
  u32 s_lnnoptr;
  u16 s_nreloc;
  u16 s_nlnno;
  u32 s_flags;
} COFFSectionTable;

typedef struct COFFSymbolTable {
  union {
    u8 shortname[8];
    struct {
      u32 zero;
      u32 offset;
    } longname;
  } namefield;
  u32 Value;
  u16 SectionNumber;
  u16 Type;
  u8 StorageClass;
  u8 NumberOfFollowingEntries;
  u8 padding[2];
} COFFSymbolTable;

#define SIZEOF_COFFSymbolTable 18

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TWL_DSP_COFF_H_ */

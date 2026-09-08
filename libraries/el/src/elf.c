#include "el_config.h"

#ifdef SDK_TWL
#include <twl.h>
#else
#include <nitro.h>
#endif

#include "elf.h"

static int endian;
static BOOL load_strndx_done = FALSE;

static u8 *load_elf32_byte(u8 *dest, u8 *org, int lsb) {
#pragma unused(lsb)
  u8 *temp_ptr;

  temp_ptr = (u8 *)org;
  *dest = *temp_ptr;
  temp_ptr++;

  return temp_ptr;
}

static u8 *load_elf32_half(Elf32_Half *dest, u8 *org, int lsb) {
  u8 *temp_ptr;

  temp_ptr = (u8 *)org;
  if (lsb == ELFDATA2LSB) {
    *dest = (u16)((u16)(*temp_ptr) & 0x00ff);
    temp_ptr++;
    *dest |= ((u16)(*temp_ptr) << 8) & 0xff00;
    temp_ptr++;
  } else /* ELFDATA2MSB */ {
    *dest = (u16)(((u16)(*temp_ptr) << 8) & 0xff00);
    temp_ptr++;
    *dest |= ((u16)(*temp_ptr) & 0x00ff);
    temp_ptr++;
  }
  return (void *)temp_ptr;
}

static u8 *load_elf32_sword(Elf32_Sword *dest, u8 *org, int lsb) {
  u8 *temp_ptr;
  u32 temp;

  temp_ptr = (u8 *)org;
  if (lsb == ELFDATA2LSB) {
    temp = ((u32)(*temp_ptr) & 0x000000ff);
    temp_ptr++;
    temp |= (((u32)(*temp_ptr) << 8) & 0x0000ff00);
    temp_ptr++;
    temp |= (((u32)(*temp_ptr) << 16) & 0x00ff0000);
    temp_ptr++;
    temp |= (((u32)(*temp_ptr) << 24) & 0xff000000);
    temp_ptr++;
  } else /* ELFDATA2MSB */ {
    temp = (((u32)(*temp_ptr) << 24) & 0xff000000);
    temp_ptr++;
    temp |= (((u32)(*temp_ptr) << 16) & 0x00ff0000);
    temp_ptr++;
    temp |= (((u32)(*temp_ptr) << 8) & 0x0000ff00);
    temp_ptr++;
    temp |= ((u32)(*temp_ptr) & 0x000000ff);
    temp_ptr++;
  }
  *dest = *((Elf32_Sword *)&temp);
  return (void *)temp_ptr;
}

static u8 *load_elf32_word(Elf32_Word *dest, u8 *org, int lsb) {
  u8 *temp_ptr;
  temp_ptr = (u8 *)org;
  if (lsb == ELFDATA2LSB) {
    *dest = ((u32)(*temp_ptr) & 0x000000ff);
    temp_ptr++;
    *dest |= (((u32)(*temp_ptr) << 8) & 0x0000ff00);
    temp_ptr++;
    *dest |= (((u32)(*temp_ptr) << 16) & 0x00ff0000);
    temp_ptr++;
    *dest |= (((u32)(*temp_ptr) << 24) & 0xff000000);
    temp_ptr++;
  } else /* ELFDATA2MSB */ {
    *dest = (((u32)(*temp_ptr) << 24) & 0xff000000);
    temp_ptr++;
    *dest |= (((u32)(*temp_ptr) << 16) & 0x00ff0000);
    temp_ptr++;
    *dest |= (((u32)(*temp_ptr) << 8) & 0x0000ff00);
    temp_ptr++;
    *dest |= ((u32)(*temp_ptr) & 0x000000ff);
    temp_ptr++;
  }
  return (void *)temp_ptr;
}

void *ELF_LoadELFHeader(const void *buf, Elf32_Ehdr *ehdr) {
  u8 *file_ptr;

  if (!buf) {
    return NULL;
  }

  file_ptr = (u8 *)buf;

  OSAPI_CPUCOPY8((void *)file_ptr, ehdr->e_ident, EI_NIDENT);
  file_ptr += EI_NIDENT;

  if (ehdr->e_ident[EI_MAG0] != ELFMAG0) {
    return NULL;
  }
  if (ehdr->e_ident[EI_MAG1] != ELFMAG1) {
    return NULL;
  }
  if (ehdr->e_ident[EI_MAG2] != ELFMAG2) {
    return NULL;
  }
  if (ehdr->e_ident[EI_MAG3] != ELFMAG3) {
    return NULL;
  }

  switch (ehdr->e_ident[EI_CLASS]) {
  case ELFCLASSNONE:
    break;
  case ELFCLASS32:
    break;
  case ELFCLASS64:
    break;
  default:
    break;
  }

  switch (ehdr->e_ident[EI_DATA]) {
  case ELFDATANONE:
    break;
  case ELFDATA2LSB:
    endian = ELFDATA2LSB;
    break;
  case ELFDATA2MSB:
    endian = ELFDATA2MSB;
    break;
  default:
    break;
  }

  file_ptr = load_elf32_half(&(ehdr->e_type), file_ptr, endian);
  switch (ehdr->e_type) {
  case ET_NONE:
    break;
  case ET_REL:
    break;
  case ET_EXEC:
    break;
  case ET_DYN:
    break;
  case ET_CORE:
    break;
  case ET_LOPROC:
    break;
  case ET_HIPROC:
    break;
  default:
    break;
  }

  PRINTDEBUG("\nELF Header:\n");
  file_ptr = load_elf32_half(&(ehdr->e_machine), file_ptr, endian);
  PRINTDEBUG("e_machine = %d\n", ehdr->e_machine);

  file_ptr = load_elf32_word(&(ehdr->e_version), file_ptr, endian);
  PRINTDEBUG("e_version = %d\n", ehdr->e_version);

  file_ptr = load_elf32_word(&(ehdr->e_entry), file_ptr, endian);
  PRINTDEBUG("e_entry(entry point) = 0x%08x\n", ehdr->e_entry);

  file_ptr = load_elf32_word(&(ehdr->e_phoff), file_ptr, endian);
  PRINTDEBUG("e_phoff(program header offset) = 0x%08x\n", ehdr->e_phoff);

  file_ptr = load_elf32_word(&(ehdr->e_shoff), file_ptr, endian);
  PRINTDEBUG("e_shoff(section header offset) = 0x%08x\n", ehdr->e_shoff);

  file_ptr = load_elf32_word(&(ehdr->e_flags), file_ptr, endian);
  PRINTDEBUG("e_flags = 0x%08x\n", ehdr->e_flags);
  if (ehdr->e_flags & EF_ARM_HASENTRY) {
    PRINTDEBUG("has entry\n");
  }
  if (ehdr->e_flags & EF_ARM_SYMSARESORTED) {
    PRINTDEBUG("symbols are sorted\n");
  }
  if (ehdr->e_flags & EF_ARM_DYNSYMSUSESEGIDX) {
    PRINTDEBUG("dynamic symbols use segmnet index\n");
  }
  if (ehdr->e_flags & EF_ARM_MAPSYMSFIRST) {
    PRINTDEBUG("map symbols first\n");
  }
  PRINTDEBUG("EABI version %x \n", (ehdr->e_flags & EF_ARM_EABIMASK) >> 24);

  file_ptr = load_elf32_half(&(ehdr->e_ehsize), file_ptr, endian);
  PRINTDEBUG("e_ehsize = %d\n", ehdr->e_ehsize);

  file_ptr = load_elf32_half(&(ehdr->e_phentsize), file_ptr, endian);
  PRINTDEBUG("e_phentsize = %d\n", ehdr->e_phentsize);

  file_ptr = load_elf32_half(&(ehdr->e_phnum), file_ptr, endian);
  PRINTDEBUG("e_phnum = %d\n", ehdr->e_phnum);

  file_ptr = load_elf32_half(&(ehdr->e_shentsize), file_ptr, endian);
  PRINTDEBUG("e_shentsize = %d\n", ehdr->e_shentsize);

  file_ptr = load_elf32_half(&(ehdr->e_shnum), file_ptr, endian);
  PRINTDEBUG("e_shnum = %d\n", ehdr->e_shnum);

  file_ptr = load_elf32_half(&(ehdr->e_shstrndx), file_ptr, endian);
  PRINTDEBUG("e_shstrndx(section index no. of the section header string table "
             "section = %d\n",
             ehdr->e_shstrndx);
  PRINTDEBUG("\n");

  return file_ptr;
}

static void *ELF_LoadRel(const void *buf, Elf32_Rel *rel) {
  u8 *file_ptr;

  if (!buf) {
    return NULL;
  }

  file_ptr = (u8 *)buf;
  file_ptr = load_elf32_word(&(rel->r_offset), file_ptr, endian);
  file_ptr = load_elf32_word(&(rel->r_info), file_ptr, endian);

  return file_ptr;
}

static void *ELF_LoadRela(const void *buf, Elf32_Rela *rela) {
  u8 *file_ptr;

  if (!buf) {
    return NULL;
  }

  file_ptr = (u8 *)buf;
  file_ptr = load_elf32_word(&(rela->r_offset), file_ptr, endian);
  file_ptr = load_elf32_word(&(rela->r_info), file_ptr, endian);
  file_ptr = load_elf32_sword(&(rela->r_addend), file_ptr, endian);

  return file_ptr;
}

static void *ELF_LoadSymbol(const void *buf, Elf32_Sym *sym) {
  u8 *file_ptr;

  if (!buf) {
    return NULL;
  }

  file_ptr = (u8 *)buf;
  file_ptr = load_elf32_word(&(sym->st_name), file_ptr, endian);
  file_ptr = load_elf32_word(&(sym->st_value), file_ptr, endian);
  file_ptr = load_elf32_word(&(sym->st_size), file_ptr, endian);
  file_ptr = load_elf32_byte(&(sym->st_info), file_ptr, endian);
  file_ptr = load_elf32_byte(&(sym->st_other), file_ptr, endian);
  file_ptr = load_elf32_half(&(sym->st_shndx), file_ptr, endian);

  return file_ptr;
}

static void *ELF_LoadSectionHeader(const void *buf, Elf32_Shdr *shdr) {
  u8 *file_ptr;

  if (!buf) {
    return NULL;
  }

  file_ptr = (u8 *)buf;
  file_ptr = load_elf32_word(&(shdr->sh_name), file_ptr, endian);
  file_ptr = load_elf32_word(&(shdr->sh_type), file_ptr, endian);
  file_ptr = load_elf32_word(&(shdr->sh_flags), file_ptr, endian);
  file_ptr = load_elf32_word(&(shdr->sh_addr), file_ptr, endian);
  file_ptr = load_elf32_word(&(shdr->sh_offset), file_ptr, endian);
  file_ptr = load_elf32_word(&(shdr->sh_size), file_ptr, endian);
  file_ptr = load_elf32_word(&(shdr->sh_link), file_ptr, endian);
  file_ptr = load_elf32_word(&(shdr->sh_info), file_ptr, endian);
  file_ptr = load_elf32_word(&(shdr->sh_addralign), file_ptr, endian);
  file_ptr = load_elf32_word(&(shdr->sh_entsize), file_ptr, endian);

  return file_ptr;
}

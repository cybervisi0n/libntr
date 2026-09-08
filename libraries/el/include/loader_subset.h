#ifndef _LOADER_SUBSET_H_
#define _LOADER_SUBSET_H_

#include "elf.h"
#include "elf_loader.h"

void *ELi_CopyVeneerToBuffer(ELDesc *elElfDesc, ELObject *MYObject, u32 start,
                             u32 data, s32 threshold);

void *ELi_CopyV4tVeneerToBuffer(ELDesc *elElfDesc, ELObject *MYObject,
                                u32 start, u32 data, s32 threshold);

void *ELi_CopySegmentToBuffer(ELDesc *elElfDesc, ELObject *MYObject,
                              Elf32_Phdr *Phdr);

void *ELi_CopySectionToBuffer(ELDesc *elElfDesc, ELObject *MYObject,
                              Elf32_Shdr *Shdr);

void *ELi_AllocSectionToBuffer(ELDesc *elElfDesc, ELObject *MYObject,
                               Elf32_Shdr *Shdr);

void ELi_GetPhdr(ELDesc *elElfDesc, u32 index, Elf32_Phdr *Phdr);

void ELi_GetShdr(ELDesc *elElfDesc, u32 index, Elf32_Shdr *Shdr);

void ELi_GetSent(ELDesc *elElfDesc, u32 index, void *entry_buf, u32 offset,
                 u32 size);

void ELi_GetEntry(ELDesc *elElfDesc, Elf32_Shdr *Shdr, u32 index,
                  void *entry_buf);

void ELi_GetStrAdr(ELDesc *elElfDesc, u32 strsh_index, u32 ent_index, char *str,
                   u32 len);

BOOL ELi_RelocateSym(ELDesc *elElfDesc, ELObject *MYObject, u32 relsh_index);

BOOL ELi_GoPublicGlobalSym(ELDesc *elElfDesc, ELObject *MYObject,
                           u32 symtblsh_index);

void ELi_FreeSymList(ELDesc *elElfDesc);

BOOL ELi_DoRelocate(ELDesc *elElfDesc, ELObject *MYObject,
                    ELImportEntry *UnresolvedInfo);

ELShdrEx *ELi_GetShdrExfromList(ELShdrEx *ShdrExStart, u32 index);

BOOL ELi_ShdrIsDebug(ELDesc *elElfDesc, u32 index);

u32 ELi_CodeIsThumb(ELDesc *elElfDesc, u16 sh_index, u32 offset);

static void ELi_InitImport(ELImportEntry *ImportInfo);

BOOL ELi_ExtractImportEntry(ELImportEntry **StartEnt, ELImportEntry *ImpEnt);

void ELi_AddImportEntry(ELImportEntry **ELUnrEntStart, ELImportEntry *UnrEnt);

void ELi_FreeImportTbl(ELImportEntry **ELImpEntStart);

void *ELi_FreeVenTbl(ELDesc *elElfDesc, ELObject *MYObject);

#endif /*_LOADER_SUBSET_H_*/

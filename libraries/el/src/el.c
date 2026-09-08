#include "el_config.h"

#include <twl/el.h>

#include "elf.h"
#include "arch.h"
#include "elf_loader.h"
#include "loader_subset.h"

extern ELAlloc i_elAlloc;
extern ELFree i_elFree;

extern BOOL elRemoveObjEntry(ELObject **StartEnt, ELObject *ObjEnt);

ELDesc *i_eldesc = NULL;     // For link
ELDesc *i_eldesc_sim = NULL; // For simulation link

s32 EL_Init(ELAlloc alloc, ELFree free) {
  if (i_eldesc != NULL) {
    return (-1); // Initialized
  }

  ELi_Init(alloc, free);

  i_eldesc = ELi_Malloc(NULL, NULL,
                        ((sizeof(ELDesc)) * 2)); // For link and simulation-link
  if (i_eldesc == NULL) {
    return (-1);
  }

  (void)ELi_InitDesc(i_eldesc);
  i_eldesc_sim = &(i_eldesc[1]);
  (void)ELi_InitDesc(i_eldesc_sim);

  return (0);
}

#if 1

s32 EL_CalcEnoughBufferSizeforLinkFile(const char *FilePath, const void *buf,
                                       ELLinkMode link_mode) {
  ELDlld dlld;
  u32 size;

  if (link_mode != EL_LINKMODE_ONESHOT) {
    return (-1);
  }
  dlld =
      EL_LoadLibraryfromFile(i_eldesc_sim, FilePath, (void *)buf, 0xFFFFFFFF);
  if (dlld) {
    if (ELi_ResolveAllLibrary(i_eldesc_sim) != EL_PROC_RELOCATED) {
      return (-1);
    }
    size = ((ELObject *)dlld)->lib_size; // Veneer-included size because after
                                         // ELi_ResolveAllLibrary

    (void)ELi_Unlink(i_eldesc, dlld);
    (void)elRemoveObjEntry(&(i_eldesc_sim->ELObjectStart), (ELObject *)dlld);
    return ((s32)size);
  }
  return (-1);
}

s32 EL_CalcEnoughBufferSizeforLink(ELReadImage readfunc, u32 len,
                                   const void *buf, ELLinkMode link_mode) {
  ELDlld dlld;
  u32 size;

  if (link_mode != EL_LINKMODE_ONESHOT) {
    return (-1);
  }
  dlld = EL_LoadLibrary(i_eldesc_sim, readfunc, len, (void *)buf, 0xFFFFFFFF);
  if (dlld) {
    if (ELi_ResolveAllLibrary(i_eldesc_sim) != EL_PROC_RELOCATED) {
      return (-1);
    }
    size = ((ELObject *)dlld)->lib_size; // Veneer-included size because after
                                         // ELi_ResolveAllLibrary

    (void)ELi_Unlink(i_eldesc, dlld);
    (void)elRemoveObjEntry(&(i_eldesc_sim->ELObjectStart), (ELObject *)dlld);
    return ((s32)size);
  }
  return (-1);
}

s32 EL_CalcEnoughBufferSizeforLinkImage(void *obj_image, u32 obj_len,
                                        const void *buf, ELLinkMode link_mode) {
  ELDlld dlld;
  u32 size;

  if (link_mode != EL_LINKMODE_ONESHOT) {
    return (-1);
  }
  dlld = EL_LoadLibraryfromMem(i_eldesc_sim, obj_image, obj_len, (void *)buf,
                               0xFFFFFFFF);
  if (dlld) {
    if (ELi_ResolveAllLibrary(i_eldesc_sim) != EL_PROC_RELOCATED) {
      return (-1);
    }
    size = ((ELObject *)dlld)->lib_size; // Veneer-included size because after
                                         // ELi_ResolveAllLibrary

    (void)ELi_Unlink(i_eldesc, dlld);
    (void)elRemoveObjEntry(&(i_eldesc_sim->ELObjectStart), (ELObject *)dlld);
    return ((s32)size);
  }
  return (-1);
}
#endif

ELDlld EL_LinkFileEx(const char *FilePath, void *buf, u32 buf_size) {
  return (EL_LoadLibraryfromFile(i_eldesc, FilePath, buf, buf_size));
}

ELDlld EL_LinkEx(ELReadImage readfunc, u32 len, void *buf, u32 buf_size) {
  return (EL_LoadLibrary(i_eldesc, readfunc, len, buf, buf_size));
}

ELDlld EL_LinkImageEx(void *obj_image, u32 obj_len, void *buf, u32 buf_size) {
  return (EL_LoadLibraryfromMem(i_eldesc, obj_image, obj_len, buf, buf_size));
}

ELDlld EL_LinkFile(const char *FilePath, void *buf) {
  return (EL_LoadLibraryfromFile(i_eldesc, FilePath, buf, 0xFFFFFFFF));
}

ELDlld EL_Link(ELReadImage readfunc, u32 len, void *buf) {
  return (EL_LoadLibrary(i_eldesc, readfunc, len, buf, 0xFFFFFFFF));
}

ELDlld EL_LinkImage(void *obj_image, u32 obj_len, void *buf) {
  return (EL_LoadLibraryfromMem(i_eldesc, obj_image, obj_len, buf, 0xFFFFFFFF));
}

u16 EL_ResolveAll(void) {
  if (ELi_ResolveAllLibrary(i_eldesc) == EL_PROC_RELOCATED) {
    return (EL_RELOCATED);
  }
  return (EL_FAILED);
}

BOOL EL_Export(ELAdrEntry *AdrEnt) { return ELi_Export(i_eldesc, AdrEnt); }

void *EL_GetGlobalAdr(ELDlld my_dlld, const char *ent_name) {
  return (ELi_GetGlobalAdr(i_eldesc, my_dlld, ent_name));
}

u16 EL_Unlink(ELDlld my_dlld) {
  if (ELi_Unlink(i_eldesc, my_dlld) == TRUE) {

    (void)elRemoveObjEntry(&(i_eldesc->ELObjectStart),
                           (ELObject *)my_dlld); // No problem if it fails
    return (EL_SUCCESS);
  }
  return (EL_FAILED);
}

ELResult EL_GetResultCode(void) {
  if (i_eldesc == NULL) {
    return (EL_RESULT_FAILURE);
  }
  return ((ELResult)(i_eldesc->result));
}

#if 0
ELResult EL_GetResultCode( ELDlld my_dlld)
{
    ELObject*      MYObject;

    if( my_dlld == 0) {
        return( EL_RESULT_INVALID_PARAMETER);
    }


    MYObject = (ELObject*)my_dlld;

    return( MYObject->result);
}
#endif

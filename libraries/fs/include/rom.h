#if !defined(NITRO_FS_ROM_H_)
#define NITRO_FS_ROM_H_

#include <nitro/misc.h>
#include <nitro/types.h>
#include <nitro/fs/file.h>
#include <nitro/fs/archive.h>
#if SDK_VERSION_MAJOR == 5
#include <nitro/card/hash.h>
#endif


#if SDK_VERSION_MAJOR == 4
extern s32 fsi_card_lock_id;

extern CARDRomRegion fsi_ovt9;
extern CARDRomRegion fsi_ovt7;

#if defined(FS_IMPLEMENT)
    extern FSArchive fsi_arc_rom;
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SDK_VERSION_MAJOR == 4
void FSi_InitRom(u32 default_dma_no);

BOOL FSi_LoadOverlayInfoCore(FSOverlayInfo * p_ovi, MIProcessor target, FSOverlayID id,
                             FSArchive * p_arc,
                             u32 offset_arm9, u32 len_arm9, u32 offset_arm7, u32 len_arm7);
#elif SDK_VERSION_MAJOR == 5
#if defined(FS_IMPLEMENT)

void FSi_InitRomArchive(u32 default_dma_no);

void FSi_EndRomArchive(void);

BOOL FSi_MountSRLFile(FSArchive *arc, FSFile *file, CARDRomHashContext *hash);

void FSi_ConvertPathToFATFS(char *dst, const char *src, BOOL ignorePermission);

FSResult FSi_ConvertError(u32 error);

BOOL FSi_MountFATFS(u32 index, const char *arcname, const char *drivename);

void FSi_MountDefaultArchives(void);

#else

void FSi_ReadRomDirect(const void *src, void *dst, u32 len);

#endif
#endif

#ifdef __cplusplus
}
#endif

#endif

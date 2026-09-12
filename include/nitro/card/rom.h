#if !defined(NITRO_CARD_ROM_H_)
#define NITRO_CARD_ROM_H_

#ifdef __cplusplus
extern "C"
{
#endif

#if SDK_VERSION_MAJOR == 4
#include <nitro/misc.h>
#include <nitro/types.h>
#include <nitro/memorymap.h>
#elif SDK_VERSION_MAJOR == 5
#include <nitro/card/types.h>
#endif
#include <nitro/mi/dma.h>
#include <nitro/mi/exMemory.h>
#if SDK_VERSION_MAJOR == 4
#include <nitro/os.h>

#include <nitro/card/common.h>
#endif

#if SDK_VERSION_MAJOR == 4
typedef struct {
	u32 offset;
	u32 length;
} CARDRomRegion;

typedef struct {
	char game_name[12];
	u32 game_code;
	u16 maker_code;
	u8 product_id;
	u8 device_type;
	u8 device_size;
	u8 reserved_A[9];
	u8 game_version;
	u8 property;
	#ifdef SDK_PORT
	u32 main_rom_offset;
	u32 main_entry_address;
	u32 main_ram_address;
	u32 main_size;
	u32 sub_rom_offset;
	u32 sub_entry_address;
	u32 sub_ram_address;
	u32 sub_size;
	#else
	void * main_rom_offset;
	void * main_entry_address;
	void * main_ram_address;
	u32 main_size;
	void * sub_rom_offset;
	void * sub_entry_address;
	void * sub_ram_address;
	u32 sub_size;
	#endif
	CARDRomRegion fnt;
	CARDRomRegion fat;
	CARDRomRegion main_ovt;
	CARDRomRegion sub_ovt;
	u8 rom_param_A[8];
	u32 banner_offset;
	u16 secure_crc;
	u8 rom_param_B[2];
	void * main_autoload_done;
	void * sub_autoload_done;
	u8 rom_param_C[8];
	u32 rom_size;
	u32 header_size;
	u8 reserved_B[0x38];
	u8 logo_data[0x9C];
	u16 logo_crc;
	u16 header_crc;
} CARDRomHeader;

#define CARD_ROM_PAGE_SIZE    512
#endif

#if SDK_VERSION_MAJOR == 4
static inline const CARDRomRegion * CARD_GetRomRegionFNT (void)
{
	return (const CARDRomRegion *)((const u8 *)HW_ROM_HEADER_BUF + 0x40);
}

static inline const CARDRomRegion * CARD_GetRomRegionFAT (void)
{
	return (const CARDRomRegion *)((const u8 *)HW_ROM_HEADER_BUF + 0x48);
}

static inline const CARDRomRegion * CARD_GetRomRegionOVT (MIProcessor target)
{
	return (target == MI_PROCESSOR_ARM9) ?
	       (const CARDRomRegion *)((const u8 *)HW_ROM_HEADER_BUF + 0x50) :
	       (const CARDRomRegion *)((const u8 *)HW_ROM_HEADER_BUF + 0x58);
}
#elif SDK_VERSION_MAJOR == 5
const u8 *CARD_GetRomHeader(void);

const CARDRomHeader *CARD_GetOwnRomHeader(void);

#ifdef SDK_TWL

const CARDRomHeaderTWL *CARD_GetOwnRomHeaderTWL(void);

#endif // SDK_TWL

SDK_INLINE const CARDRomRegion *CARD_GetRomRegionFNT(void) {
  const CARDRomHeader *header = CARD_GetOwnRomHeader();
  return &header->fnt;
}

SDK_INLINE const CARDRomRegion *CARD_GetRomRegionFAT(void) {
  const CARDRomHeader *header = CARD_GetOwnRomHeader();
  return &header->fat;
}

SDK_INLINE const CARDRomRegion *CARD_GetRomRegionOVT(MIProcessor target) {
  const CARDRomHeader *header = CARD_GetOwnRomHeader();
  return (target == MI_PROCESSOR_ARM9) ? &header->main_ovt : &header->sub_ovt;
}
#endif

#if defined(SDK_TEG)
    static inline BOOL CARDi_IsTrueRom (void)
    {
        return (OS_GetConsoleType() & OS_CONSOLE_DEV_CARD) != 0;
    }
#endif

void CARD_LockRom(u16 lock_id);
void CARD_UnlockRom(u16 lock_id);

void CARDi_ReadRom(u32 dma,
                   const void * src, void * dst, u32 len,
                   MIDmaCallback callback, void * arg, BOOL is_async);

BOOL CARD_TryWaitRomAsync(void);
void CARD_WaitRomAsync(void);

static inline void CARD_ReadRomAsync (u32 dma,
                                      const void * src, void * dst, u32 len,
                                      MIDmaCallback callback, void * arg)
{
	CARDi_ReadRom(dma, src, dst, len, callback, arg, TRUE);
}

static inline void CARD_ReadRom (u32 dma, const void * src, void * dst, u32 len)
{
	CARDi_ReadRom(dma, src, dst, len, NULL, NULL, FALSE);
}

u32 CARDi_ReadRomID(void);

#if defined(SDK_TEG) && defined(SDK_ARM7)
    void CARDi_CreatePxiRecvThread(u32 priority);
#endif

#if SDK_VERSION_MAJOR == 5
void CARD_GetCacheFlushThreshold(u32 *icache, u32 *dcache);
void CARD_SetCacheFlushThreshold(u32 icache, u32 dcache);
void CARD_GetCacheFlushFlag(BOOL *icache, BOOL *dcache);
void CARD_SetCacheFlushFlag(BOOL icache, BOOL dcache);
void CARDi_RefreshRom(u32 warn_mask);
BOOL CARDi_IsTwlRom(void);
const u8 *CARDi_GetOwnSignature(void);
void CARDi_SetOwnSignature(const void *signature);
#endif

#ifdef __cplusplus
}
#endif

#endif

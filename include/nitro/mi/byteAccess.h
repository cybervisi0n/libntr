#ifndef NITRO_MI_BYTEACCESS_H_
#define NITRO_MI_BYTEACCESS_H_

#ifdef __cplusplus
extern "C" {
#endif

#if SDK_VERSION_MAJOR == 4
#include <nitro/ioreg.h>
#elif SDK_VERSION_MAJOR == 5
#ifndef SDK_TWL
#include <nitro/types.h>
#else
#include <twl/types.h>
#endif
#endif

#ifdef  SDK_TEG
    u8 MI_ReadByte(const void * address);
#else
    static inline u8 MI_ReadByte (const void * address)
    {
        return *(u8 *)address;
    }
#endif

#ifdef  SDK_TEG
    void MI_WriteByte(void * address, u8 value);
#else
    static inline void MI_WriteByte (void * address, u8 value)
    {
        *(u8 *)address = value;
    }
#endif

#ifdef __cplusplus
}
#endif

#endif

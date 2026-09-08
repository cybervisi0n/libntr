#include "el_config.h"

#ifdef SDK_TWL
#include <twl.h>
#else
#include <nitro.h>
#endif

#include "arch.h"

u32 AR_GetEntrySize(ArchHdr *ArHdr) {
  u16 i;
  u32 digit = 1;
  u32 size = 0;

  for (i = 0; i < 10; i++) {
    if (ArHdr->ar_size[i] == 0x20) {
      break;
    } else {
      digit *= 10;
    }
  }
  digit /= 10;

  for (i = 0; i < 10; i++) {
    size +=
        (*(((u8 *)(ArHdr->ar_size)) + i) - 0x30) * digit; // Convert char to u8
    if (digit == 1) {
      break;
    } else {
      digit /= 10;
    }
  }

  return size;
}

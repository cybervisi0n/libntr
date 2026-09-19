#pragma once

#ifdef __cplusplus
extern "C" {
#endif

enum ISTDHIODEVMASK {
  ISTDHIODEVMASK_AUTOSELECT = (0), // Auto-select
  ISTDHIODEVMASK_TWLDBG = (1 << 0) // IS-TWL-DEBUGGER
};

void ISTDHIOInit(void);

u32 ISTDHIOGetDevMask(void);

BOOL ISTDHIOOpen(u32 fDevMask);
BOOL ISTDHIOClose(void);

#ifdef __cplusplus
}
#endif

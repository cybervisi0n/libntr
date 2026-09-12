#ifndef DEMO_UTILITY_H_
#define DEMO_UTILITY_H_
#if SDK_VERSION_MAJOR == 4
#include <nitro.h>
#elif SDK_VERSION_MAJOR == 5
#ifdef SDK_TWL
#include <twl.h>
#else
#include <nitro.h>
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

void DEMO_Set3DDefaultMaterial(BOOL bUsediffuseAsVtxCol, BOOL bUseShininessTbl);
void DEMO_Set3DDefaultShininessTable();

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* DEMO_UTILITY_H_ */

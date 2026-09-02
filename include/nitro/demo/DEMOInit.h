#ifndef DEMO_INIT_H_
#define DEMO_INIT_H_

#ifdef __cplusplus
extern "C" {
#endif

extern void VBlankIntr(void);

void DEMOInitCommon(void);
void DEMOInitVRAM(void);
void DEMOInitDisplayBG0Only(void);
void DEMOInitDisplaySubBG0Only(void);
void DEMOInitDisplayBG2Only(void);
void DEMOInitDisplaySubBG2Only(void);
void DEMOInitDisplayOBJOnly(void);
void DEMOInitDisplaySubOBJOnly(void);
void DEMOInitDisplay3D(void);
void DEMOInit(void);

void DEMOStartDisplay(void);

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif

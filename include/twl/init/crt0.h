#ifndef TWL_INIT_CRT0_H_
#define TWL_INIT_CRT0_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SDK_ARM9
void TwlMain(void);
#define NitroMain TwlMain

void NitroStartUp(void);
#define TwlStartUp NitroStartUp
#else
void TwlSpMain(void);
#define NitroSpMain TwlSpMain

void NitroSpStartUp(void);
#define TwlStartUp NitroSpStartUp
#define TwlSpStartUp NitroSpStartUp
#endif

#ifdef __cplusplus
}
#endif

#endif

#ifndef NITRO_OS_ARM9_CHINA_H_
#define NITRO_OS_ARM9_CHINA_H_

#ifdef  __cplusplus
extern "C" {
#endif

#define OS_BURY_STRING_FORCHINA         "[SDK+NINTENDO:FORCHINA]"
#define OS_BUSY_STRING_LEN_FORCHINA     23

#if SDK_VERSION_MAJOR == 5
typedef enum {
  OS_CHINA_ISBN_DISP,
  OS_CHINA_ISBN_NO_DISP,
  OS_CHINA_ISBN_CHECK_ROM
} OSChinaIsbn;
#endif

void OS_InitChina(const char ** isbn);
void OS_ShowAttentionChina(const char ** isbn);

#ifdef  __cplusplus
}
#endif

#endif

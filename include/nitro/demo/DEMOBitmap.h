#ifndef DEMO_BITMAP_H_
#define DEMO_BITMAP_H_

#if SDK_VERSION_MAJOR < 5
#error This file is for SDK version 5 only!
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define DEMO_RGB_NONE GX_RGBA(31, 31, 31, 0)
#define DEMO_RGB_CLEAR GX_RGBA(0, 0, 0, 0)

extern const u32 DEMOAsciiChr[8 * 0x100];

#define DEMOSetBitmapTextColor(color)                                          \
  DEMOiSetBitmapTextColor(DEMOVerifyGXRgb(color))
#define DEMOSetBitmapGroundColor(color)                                        \
  DEMOiSetBitmapGroundColor(DEMOVerifyGXRgb(color))
#define DEMOFillRect(x, y, wx, wy, color)                                      \
  DEMOiFillRect(x, y, wx, wy, DEMOVerifyGXRgb(color))
#define DEMODrawLine(sx, sy, tx, ty, color)                                    \
  DEMOiDrawLine(sx, sy, tx, ty, DEMOVerifyGXRgb(color))
#define DEMODrawFrame(x, y, wx, wy, color)                                     \
  DEMOiDrawFrame(x, y, wx, wy, DEMOVerifyGXRgb(color))

void DEMOInitDisplayBitmap(void);
void DEMO_DrawFlip();
void DEMOHookConsole(void);
GXRgb DEMOVerifyGXRgb(int color);
void DEMOiSetBitmapTextColor(GXRgb color);
void DEMOiSetBitmapGroundColor(GXRgb color);
void DEMOiFillRect(int x, int y, int wx, int wy, GXRgb color);
void DEMOBlitRect(int x, int y, int wx, int wy, const GXRgb *image, int stroke);
void DEMOBlitTex16(int x, int y, int wx, int wy, const void *chr,
                   const GXRgb *plt);
void DEMOiDrawLine(int sx, int sy, int tx, int ty, GXRgb color);
void DEMOiDrawFrame(int x, int y, int wx, int wy, GXRgb color);
void DEMODrawText(int x, int y, const char *format, ...);
void DEMOClearString(void);
void DEMOPutString(int x, int y, const char *format, ...);
void DEMOPutLog(const char *format, ...);
void DEMOSetViewPort(int x, int y, int wx, int wy);

#ifdef __cplusplus
}
#endif

#endif

#ifdef SDK_TWL
#include <twl.h>
#else
#include <nitro.h>
#endif
#include <nitro/demo/DEMOBitmap.h>

typedef struct DEMOBitmapContext {
  u32 require_flip : 1;
  u32 bitmap_modified : 1;
  u32 main_bg_modified : 1;
  u32 main_oam_modified : 1;
  u32 sub_bg_modified : 1;
  u32 sub_oam_modified : 1;
  u32 : 26;
  OSThreadQueue flip_wait_q[1];
  void (*HookedConsole)(int console, const char *string);
  GXRgb bitmap_ground_color;
  GXRgb bitmap_text_color;
  u8 padding[12];
  GXRgb bitmap_frame[GX_LCD_SIZE_X * GX_LCD_SIZE_Y] ATTRIBUTE_ALIGN(32);
  u16 main_bg[32 * 24] ATTRIBUTE_ALIGN(32);
  GXOamAttr main_oam[128] ATTRIBUTE_ALIGN(32);
  u16 sub_bg[32 * 24] ATTRIBUTE_ALIGN(32);
  GXOamAttr sub_oam[128] ATTRIBUTE_ALIGN(32);
  char sub_log[24][32 + 1];
  int sub_log_count;
} DEMOBitmapContext;

static DEMOBitmapContext bitmap ATTRIBUTE_ALIGN(32);

extern void VBlankIntr(void);

SDK_WEAK_SYMBOL void VBlankIntr(void) { OS_SetIrqCheckFlag(OS_IE_V_BLANK); }

static void LogConsole(int console, const char *string) {
  DEMOBitmapContext *const context = &bitmap;
  DEMOPutLog("%s", string);
  (*context->HookedConsole)(console, string);
}

static void OnVBlank(void) {
  DEMOBitmapContext *const context = &bitmap;
  if (context->require_flip) {
    if (context->bitmap_modified) {
      context->bitmap_modified = 0;
      GX_LoadBG3Bmp(context->bitmap_frame, 0, sizeof(context->bitmap_frame));
    }
    if (context->main_bg_modified) {
      context->main_bg_modified = 0;
      GX_LoadBG1Scr(context->main_bg, 0, sizeof(context->main_bg));
    }
    if (context->main_oam_modified) {
      context->main_oam_modified = 0;
      GX_LoadOAM(context->main_oam, 0, sizeof(context->main_oam));
    }

    if (context->sub_bg_modified) {
      context->sub_bg_modified = 0;
      GXS_LoadBG0Scr(context->sub_bg, 0, sizeof(context->sub_bg));
    }
    if (context->sub_oam_modified) {
      context->sub_oam_modified = 0;
      GXS_LoadOAM(context->sub_oam, 0, sizeof(context->sub_oam));
    }
    context->require_flip = 0;
    OS_WakeupThread(context->flip_wait_q);
  }
  VBlankIntr();
}

void DEMOInitDisplayBitmap(void) {

  G3X_Init();
  G3X_InitMtxStack();
  GX_SetBankForTex(GX_VRAM_TEX_0_A);
  GX_SetBankForBG(GX_VRAM_BG_128_D);
  GX_SetBankForTexPltt(GX_VRAM_TEXPLTT_0123_E);
  GX_SetBankForOBJ(GX_VRAM_OBJ_32_FG);
  GX_SetBankForSubBG(GX_VRAM_SUB_BG_32_H);
  GX_SetBankForSubOBJ(GX_VRAM_SUB_OBJ_16_I);
  GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_4, GX_BG0_AS_3D);
  GX_SetVisiblePlane(GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG3 |
                     GX_PLANEMASK_OBJ);
  GXS_SetGraphicsMode(GX_BGMODE_0);
  GXS_SetVisiblePlane(GX_PLANEMASK_BG0 | GX_PLANEMASK_OBJ);
  G2_BlendNone();
  G2S_BlendNone();
  GX_Power2DSub(TRUE);

  {
    G2_SetBG0Priority(2);
    G3X_SetShading(GX_SHADING_TOON);
    G3X_AntiAlias(TRUE);
  }

  {
    G2_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16,
                     GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x18000,
                     GX_BG_EXTPLTT_01);
    GX_SetBGScrOffset(GX_BGSCROFFSET_0x10000);
    G2_SetBG1Priority(0);
    G2_BG1Mosaic(FALSE);
  }

  {
    G2_SetBG3ControlDCBmp(GX_BG_SCRSIZE_DCBMP_256x256, GX_BG_AREAOVER_XLU,
                          GX_BG_BMPSCRBASE_0x00000);
    G2_SetBG3Priority(1);
  }

  {
    G2S_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16,
                      GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x00000,
                      GX_BG_EXTPLTT_01);
    G2S_SetBG0Priority(1);
    G2S_BG0Mosaic(FALSE);
  }
  G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);
  G3X_SetClearColor(GX_RGB(0, 0, 0), 31, 0x7fff, 63, FALSE);
  DEMOSetViewPort(0, 0, GX_LCD_SIZE_X, GX_LCD_SIZE_Y);

  {
    static const GXRgb pal[16] = {
        GX_RGB(0, 0, 0),
        GX_RGB(31, 31, 31),
    };
    MI_CpuFill32(&bitmap, 0x00000000, sizeof(bitmap));
    GX_LoadBGPltt(pal, 0x0000, sizeof(pal));
    GX_LoadBG1Char(DEMOAsciiChr, 0x00000, sizeof(DEMOAsciiChr));
    GXS_LoadBGPltt(pal, 0x0000, sizeof(pal));
    GXS_LoadBG0Char(DEMOAsciiChr, 0x00000, sizeof(DEMOAsciiChr));
    OS_InitThreadQueue(bitmap.flip_wait_q);
    OS_SetIrqFunction(OS_IE_V_BLANK, OnVBlank);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
  }
}

void DEMO_DrawFlip() {
  DEMOBitmapContext *const context = &bitmap;

  if (context->bitmap_modified) {
    DC_StoreRange(context->bitmap_frame, sizeof(context->bitmap_frame));
  }
  if (context->main_bg_modified) {
    DC_StoreRange(context->main_bg, sizeof(context->main_bg));
  }
  if (context->main_oam_modified) {
    DC_StoreRange(context->main_oam, sizeof(context->main_oam));
  }
  if (context->sub_bg_modified) {
    const int visible_lines = 20;
    const int palette = 0;
    int offset = (context->sub_log_count + 1 > visible_lines)
                     ? (context->sub_log_count + 1 - visible_lines)
                     : 0;
    int x, y;
    for (y = 0; y < visible_lines; ++y) {
      u16 *dst = &context->sub_bg[y * 32];
      const char *src = context->sub_log[(offset + y) % 24];
      for (x = 0; (x < 32) && src[x] && (src[x] != '\n'); ++x) {
        dst[x] = (u16)((u8)src[x] | (palette << 12));
      }
      for (; x < 32; ++x) {
        dst[x] = (u16)(0 | (palette << 12));
      }
    }
    DC_StoreRange(context->sub_bg, sizeof(context->sub_bg));
  }
  if (context->sub_oam_modified) {
    DC_StoreRange(context->sub_oam, sizeof(context->sub_oam));
  }
  DC_WaitWriteBufferEmpty();
  {
    OSIntrMode bak_intr = OS_DisableInterrupts();
    context->require_flip = 1;

    (void)OS_RestoreInterrupts(bak_intr);
  }
}

GXRgb DEMOVerifyGXRgb(int color) {
  if ((color < 0) || (color > 0xFFFF)) {
    static BOOL once = FALSE;
    if (!once) {
      OS_TWarning("color(%04X) exceeds GXRgb range.\n", color);
      once = TRUE;
    }
    color = 0xFFFF;
  }
  return (GXRgb)color;
}

void DEMOiSetBitmapTextColor(GXRgb color) {
  DEMOBitmapContext *context = &bitmap;
  context->bitmap_text_color = color;
}

void DEMOiSetBitmapGroundColor(GXRgb color) {
  DEMOBitmapContext *context = &bitmap;
  context->bitmap_ground_color = color;
}

void DEMOiFillRect(int x, int y, int wx, int wy, GXRgb color) {
  if ((x < GX_LCD_SIZE_X) && (y < GX_LCD_SIZE_Y)) {
    if (wx > GX_LCD_SIZE_X - x) {
      wx = GX_LCD_SIZE_X - x;
    }
    if (wy > GX_LCD_SIZE_Y - y) {
      wy = GX_LCD_SIZE_Y - y;
    }
    if ((wx > 0) && (wy > 0)) {
      DEMOBitmapContext *context = &bitmap;
      GXRgb *dst = &context->bitmap_frame[x + y * GX_LCD_SIZE_X];
      while (--wy >= 0) {
        MI_CpuFill16(dst, color, wx * sizeof(GXRgb));
        dst += GX_LCD_SIZE_X;
      }
      context->bitmap_modified = 1;
    }
  }
}

void DEMOBlitRect(int x, int y, int wx, int wy, const GXRgb *image,
                  int stroke) {
  if ((x < GX_LCD_SIZE_X) && (y < GX_LCD_SIZE_Y)) {
    if (wx > GX_LCD_SIZE_X - x) {
      wx = GX_LCD_SIZE_X - x;
    }
    if (wy > GX_LCD_SIZE_Y - y) {
      wy = GX_LCD_SIZE_Y - y;
    }
    if ((wx > 0) && (wy > 0)) {
      DEMOBitmapContext *context = &bitmap;
      GXRgb *dst = &context->bitmap_frame[x + y * GX_LCD_SIZE_X];
      while (--wy >= 0) {
        MI_CpuCopy16(image, dst, wx * sizeof(GXRgb));
        image += stroke;
        dst += GX_LCD_SIZE_X;
      }
      context->bitmap_modified = 1;
    }
  }
}

void DEMOBlitTex16(int x, int y, int wx, int wy, const void *chr,
                   const GXRgb *plt) {
  if ((x < GX_LCD_SIZE_X) && (y < GX_LCD_SIZE_Y)) {
    int stroke = wx;
    if (wx > GX_LCD_SIZE_X - x) {
      wx = GX_LCD_SIZE_X - x;
    }
    if (wy > GX_LCD_SIZE_Y - y) {
      wy = GX_LCD_SIZE_Y - y;
    }
    if ((wx > 0) && (wy > 0)) {
      DEMOBitmapContext *context = &bitmap;
      const u8 *src = (const u8 *)chr;
      GXRgb *dst = &context->bitmap_frame[x + y * GX_LCD_SIZE_X];
      int tx, ty, dx, dy, nx, ny;
      for (ty = 0; ty < wy; ty += 8) {
        ny = MATH_MIN(wy - ty, 8);
        for (tx = 0; tx < wx; tx += 8) {
          nx = MATH_MIN(wx - tx, 8);
          for (dy = 0; dy < ny; ++dy) {
            for (dx = 0; dx < nx; ++dx) {
              u8 index =
                  (u8)((src[(dy * 8 + dx) / 2] >> ((dx & 1) * 4)) & 0x0F);
              if (index > 0) {
                dst[(ty + dy) * GX_LCD_SIZE_X + tx + dx] =
                    (GXRgb)(plt[index] | 0x8000);
              }
            }
          }
          src += 8 * 8 / 2;
        }
        src += (stroke - tx) * 8 / 2;
      }
      context->bitmap_modified = 1;
    }
  }
}

void DEMOiDrawLine(int sx, int sy, int tx, int ty, GXRgb color) {
  DEMOBitmapContext *context = &bitmap;
  int tmp;

  if (sx > tx) {
    tmp = sx + 1, sx = tx + 1, tx = tmp;
    tmp = sy, sy = ty, ty = tmp;
  }
  {
    GXRgb *dst = NULL;
    int wx = tx - sx;
    int wy = ty - sy;

    if (!wx) {
      if (wy < 0) {
        wy = -wy;
        tmp = sy + 1, sy = ty + 1, ty = tmp;
      }
      if ((sx >= 0) && (sx < GX_LCD_SIZE_X) && (sy < GX_LCD_SIZE_Y)) {
        if (sy < 0) {
          wy += sy, sy = 0;
        }
        if (wy > GX_LCD_SIZE_Y - sy) {
          wy = GX_LCD_SIZE_Y - sy;
        }
        dst = &context->bitmap_frame[sx + sy * GX_LCD_SIZE_X];
        while (--wy >= 0) {
          *dst = color;
          dst += GX_LCD_SIZE_X;
        }
      }
    }

    else if (!wy) {
      if ((sy >= 0) && (sy < GX_LCD_SIZE_Y) && (sx < GX_LCD_SIZE_X)) {
        if (sx < 0) {
          wx += sx, sx = 0;
        }
        if (wx > GX_LCD_SIZE_X - sx) {
          wx = GX_LCD_SIZE_X - sx;
        }
        dst = &context->bitmap_frame[sx + sy * GX_LCD_SIZE_X];
        MI_CpuFill16(dst, color, wx * sizeof(GXRgb));
      }
    }

    else {
      int n, dx, dy;
      int y_delta = +1;
      int y_ofs = GX_LCD_SIZE_X;
      dst = &context->bitmap_frame[sx + sy * GX_LCD_SIZE_X];
      if (wy < 0) {
        wy = -wy;
        y_delta = -y_delta;
        y_ofs = -y_ofs;
      }
      dx = wy - 1, dy = wx - 1;
      --sx, sy -= y_delta;
      for (n = wx * wy; --n >= 0;) {
        BOOL moved = FALSE;
        if (++dx >= wy) {
          moved = TRUE, dx = 0, ++sx, dst += 1;
        }
        if (++dy >= wx) {
          moved = TRUE, dy = 0, sy += y_delta, dst += y_ofs;
        }
        if (moved && (sx >= 0) && (sx < GX_LCD_SIZE_X) && (sy >= 0) &&
            (sy < GX_LCD_SIZE_Y)) {
          *dst = color;
        }
      }
    }
  }
}

void DEMOiDrawFrame(int x, int y, int wx, int wy, GXRgb color) {
  DEMOiFillRect(x, y, wx, 1, color);
  DEMOiFillRect(x, y + wy - 1, wx, 1, color);
  DEMOiFillRect(x, y + 1, 1, wy - 1, color);
  DEMOiFillRect(x + wx - 1, y + 1, 1, wy - 1, color);
}

void DEMODrawText(int x, int y, const char *format, ...) {
  static char tmp[512];
  {
    va_list va;
    va_start(va, format);
    (void)OS_VSNPrintf(tmp, sizeof(tmp) - 2, format, va);
    va_end(va);
  }
  {
    DEMOBitmapContext *context = &bitmap;
    const char *s = tmp;
    int n = sizeof(tmp) - 1;
    GXRgb *p = &context->bitmap_frame[x + y * GX_LCD_SIZE_X];
    GXRgb txt = context->bitmap_text_color;
    GXRgb gnd = context->bitmap_ground_color;
    int px = 0;
    int rep = 0;
    for (; (n > 0) && *s; ++s, --n) {
      int c = (u8)*s;
      if (STD_IsSjisLeadByte(c)) {
        c = (c << 8) | (u8) * ++s;
      }
      switch (c) {
      case '\0':
        return;
      case '\r':
        c = s[1];
        if (c == '\n') {
          ++s, --n;
        }
      case '\n':
        y += 8;
        p += GX_LCD_SIZE_X * 8;
        px = 0;
        break;
      case '\t': {
        const int align = 4;
        rep = align - ((px / 8) & (align - 1));
        c = ' ';
      }
        goto put_char;
      default:
        rep = 1;
        goto put_char;

      put_char:
        while (--rep >= 0) {
          int tx = x + px;
          if ((tx > -8) && (tx < GX_LCD_SIZE_X) && (y > -8) &&
              (y < GX_LCD_SIZE_Y)) {
            int ox = 0, oy = 0;
            int wx = 8, wy = 8;
            if (tx < 0) {
              ox = -tx;
            } else {
              wx = MATH_MIN(wx, GX_LCD_SIZE_X - tx);
            }
            if (y < 0) {
              oy = -y;
            } else {
              wy = MATH_MIN(wy, GX_LCD_SIZE_Y - y);
            }
            {
              const u32 *DEMO_GetSjisFont(int code);
              const u32 *src = DEMO_GetSjisFont(c);
              GXRgb *dst = &p[GX_LCD_SIZE_X * oy + px];
              int stroke = GX_LCD_SIZE_X;
              for (; oy < wy; ++oy) {
                int i;
                const u32 bits = src[oy];
                for (i = ox; i < wx; ++i) {
                  if (((bits >> (i * 4)) & 0xF) != 0) {
                    dst[i] = txt;
                  } else if (gnd != DEMO_RGB_NONE) {
                    dst[i] = gnd;
                  }
                }
                dst += stroke;
              }
            }
          }
          px += 8;
        }
        break;
      }
    }
    context->bitmap_modified = 1;
  }
}

void DEMOClearString(void) {
  DEMOBitmapContext *context = &bitmap;
  MI_CpuFillFast(context->main_bg, 0x00000000, sizeof(context->main_bg));
  context->main_bg_modified = 1;
}

void DEMOPutString(int x, int y, const char *format, ...) {
  DEMOBitmapContext *context = &bitmap;
  u16 *dst = &context->main_bg[y * 32];
  const int palette = 0;
  char tmp[32 + 1];
  {
    va_list va;
    va_start(va, format);
    (void)OS_VSNPrintf(tmp + x, sizeof(tmp) - x, format, va);
    va_end(va);
  }
  for (; (x < 32) && tmp[x]; ++x) {
    dst[x] = (u16)((u8)tmp[x] | (palette << 12));
  }
  context->main_bg_modified = 1;
}

void DEMOPutLog(const char *format, ...) {
  DEMOBitmapContext *context = &bitmap;
  static char tmp[512];
  {
    va_list va;
    va_start(va, format);
    (void)OS_VSNPrintf(tmp, sizeof(tmp), format, va);
    va_end(va);
  }
  {
    const char *src = tmp;
    char *dst = context->sub_log[context->sub_log_count % 24];
    int x = STD_GetStringLength(dst);
    for (; *src; ++src) {
      switch (*src) {
      case '\r':
        if (src[1] == '\n') {
          break;
        }

      case '\n':
        dst[(x < 32) ? x : 32] = '\0';
        ++context->sub_log_count;
        dst = context->sub_log[context->sub_log_count % 24];
        x = 0;
        break;
      default:
        if (x < 32) {
          dst[x++] = *src;
        }
        break;
      }
    }
    dst[x] = '\0';
  }
  context->sub_bg_modified = 1;
}

void DEMOHookConsole(void) {
#ifndef SDK_FINALROM
  DEMOBitmapContext *context = &bitmap;

  OS_FPutString(0, "");

  context->HookedConsole = OS_FPutString;
  OS_FPutString = LogConsole;
#endif
}

void DEMOSetViewPort(int x, int y, int wx, int wy) {
  G3_ViewPort(x, (GX_LCD_SIZE_Y - (y + wy)), (x + wx) - 1,
              (GX_LCD_SIZE_Y - y) - 1);
  {
    fx32 right = FX32_ONE;
    fx32 top = FX32_ONE * wy / wx;
    fx32 near = FX32_ONE;
    fx32 far = FX32_ONE * 400;
    G3_Perspective(FX32_SIN30, FX32_COS30, FX32_ONE * wx / wy, near, far, NULL);
    G3_StoreMtx(0);
  }
}

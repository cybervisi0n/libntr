#include <nitro/math.h>

#define Y_R (short)0x4c8B // 19595    unsigned
#define Y_G (short)0x9645 // 38470    unsigned
#define Y_B (short)0x1d2f // 7471     signed
#define U_R (short)0xd4bc // -11075   signed
#define U_G (short)0xab44 // -21692   signed
#define U_B (short)0x7fff // 32767    signed
#define V_R (short)0x7fff // 32767    signed
#define V_G (short)0x94bd // -27459   signed
#define V_B (short)0xeb44 // -5308    signed

#define HYB1 (short)0x0e97 // 3735 signed
#define HYB2 (short)0x0e98 // 3736 signed
#define TYB1 (short)0x09BB // 2491 signed
#define TYB2 (short)0x09BA // 2490 signed
#define TYB3 (short)0x09BA // 2490 signed
#define QYB1 (short)0x074c // 1868 signed
#define QYB2 (short)0x074c // 1868 signed
#define QYB3 (short)0x074c // 1868 signed
#define QYB4 (short)0x074b // 1867 signed

#define HYR1 (short)0x2645 // 9797 signed
#define HYR2 (short)0x2646 // 9798 signed
#define TYR1 (short)0x1984 // 6532 signed
#define TYR2 (short)0x1984 // 6532 signed
#define TYR3 (short)0x1983 // 6531 signed
#define QYR1 (short)0x1323 // 4899 signed
#define QYR2 (short)0x1323 // 4899 signed
#define QYR3 (short)0x1323 // 4899 signed
#define QYR4 (short)0x1322 // 4898 signed

#define HYG1 (short)0x4B23 // 19235 signed
#define HYG2 (short)0x4B23 // 19235 signed
#define TYG1 (short)0x3218 // 12824 signed
#define TYG2 (short)0x3217 // 12823 signed
#define TYG3 (short)0x3217 // 12823 signed
#define QYG1 (short)0x2591 // 9617 signed
#define QYG2 (short)0x2592 // 9618 signed
#define QYG3 (short)0x2591 // 9617 signed
#define QYG4 (short)0x2592 // 9618 signed

#define HUG1 (short)0xd5a2 // -10846 signed
#define HUG2 (short)0xd5a2 // -10846 signed
#define HVG1 (short)0xca5f // -13729 signed
#define HVG2 (short)0xca5e // -13730 signed

#define Ymin -128
#define Ymax 127
#define Umin -128
#define Umax 127
#define Vmin -128
#define Vmax 127

#define sat2s(a, b, c) (short)MATH_IMin(MATH_IMax(a, b), c)

short TblRGBConst[16] = {Y_R, HYG1, HYG2, Y_B, U_R, HUG1, HUG2, U_B,
                         V_R, HVG1, HVG2, V_B, Y_R, HYG1, HYG2, Y_B};

void DSPi_rgb565_2_yuv(unsigned char in_rgb[4][3], unsigned char out_ycb_cr[6]);
void DSPi_rgb565_2_yuv(unsigned char in_rgb[4][3],
                       unsigned char out_ycb_cr[6]) {

  short Bx, By;
  short R, G, B;
  long Y, Cb, Cr;
  short ybuf_index = 0;

  for (By = 0; By < 2; By++) {

    for (Bx = 0; Bx < 2; Bx++) {

      R = (short)(in_rgb[By * 2 + Bx][2] - 128);
      G = (short)(in_rgb[By * 2 + Bx][1] - 128);
      B = (short)(in_rgb[By * 2 + Bx][0] - 128);

      Y = (long)TblRGBConst[0] * (long)R + (long)TblRGBConst[1] * (long)G +
          (long)TblRGBConst[2] * (long)G + (long)TblRGBConst[3] * (long)B;
      out_ycb_cr[ybuf_index++] =
          (unsigned char)(sat2s((short)(Y >> 16), Ymin, Ymax) + 128);
    }
  }

  R = (short)(in_rgb[0][2] - 128);
  G = (short)(in_rgb[0][1] - 128);
  B = (short)(in_rgb[0][0] - 128);

  Cb = U_R * (long)R + HUG1 * (long)G + HUG2 * (long)G + U_B * (long)B;
  Cr = V_R * (long)R + HVG1 * (long)G + HVG2 * (long)G + V_B * (long)B;
  out_ycb_cr[4] = (unsigned char)(sat2s((short)(Cb >> 16), Umin, Umax) + 128);
  out_ycb_cr[5] = (unsigned char)(sat2s((short)(Cr >> 16), Vmin, Vmax) + 128);
}

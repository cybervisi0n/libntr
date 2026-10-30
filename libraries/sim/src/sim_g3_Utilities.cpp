#include "simulator/sim_g3_Utilities.hpp"
#include "simulator/sim.h"

namespace SIM::G3::Util {

MtxFx33 MultiplyMtxFx33(MtxFx33 a, MtxFx33 b) {
    MtxFx33 result;

    fx32 curSum = 0;
    //A * B
    for( int i = 0; i < 3; i++ ) //Row of A
    {
        for( int j = 0; j < 3; j++ )
        {
            
            //Get the dot product of curRow * curCol
            curSum = 0;
            for(int k=0; k < 3; k++)
            {
                curSum = curSum + FX_Mul(a.m[i][k], a.m[k][j]);
            }
            result.m[i][j] = curSum;
        }
    }

    return result;
}

MtxFx44 MultiplyMtxFx44(MtxFx44 a, MtxFx44 b) {
    MtxFx44 result;

    fx32 curSum = 0;
    //A * B
    for( int i = 0; i < 4; i++ ) //Row of A
    {
        for( int j = 0; j < 4; j++ )
        {
            
            //Get the dot product of curRow * curCol
            curSum = 0;
            for(int k=0; k < 4; k++)
            {
                curSum = curSum + FX_Mul(a.m[i][k], b.m[k][j]);
            }
            result.m[i][j] = curSum;
        }
    }

    return result;
}

void DecodeTex4(u8* vramTex, u16* colorAddr, u8* out, u32 s, u32 t, u8 color0){
    u32 texOffset = 0;
    u8 color1;
    u8 color2;
    u8 color3;
    u8 color4;
    u16 palColor;

    u8 r;
    u8 g;
    u8 b;
    for(int i=0; i < s; i++)
    {
        for(int j=0; j < t / 4; j++)
        {
            color1 = (*vramTex) & 0b00000011;
			color2 = ((*vramTex) & 0b00001100) >> 2;
            color3 = ((*vramTex) & 0b00110000) >> 4;
            color4 = ((*vramTex) & 0b11000000) >> 6;

            palColor = colorAddr[color1];
            SIM_u16ToRGB( palColor, &r, &g, &b );
			out[texOffset] = r;
			texOffset++;
			out[texOffset] = g;
			texOffset++;
			out[texOffset] = b;
			texOffset++;
            if(color0 == 1 && color1 == 0)
            {
                out[texOffset] = 0;
            }else{
                out[texOffset] = 255;
            }
			texOffset++;

            palColor = colorAddr[color2];
            SIM_u16ToRGB( palColor, &r, &g, &b );
			out[texOffset] = r;
			texOffset++;
			out[texOffset] = g;
			texOffset++;
			out[texOffset] = b;
			texOffset++;
            if(color0 == 1 && color2 == 0)
            {
                out[texOffset] = 0;
            }else{
                out[texOffset] = 255;
            }
			texOffset++;

            palColor = colorAddr[color3];
            SIM_u16ToRGB( palColor, &r, &g, &b );
			out[texOffset] = r;
			texOffset++;
			out[texOffset] = g;
			texOffset++;
			out[texOffset] = b;
			texOffset++;
            if(color0 == 1 && color3 == 0)
            {
                out[texOffset] = 0;
            }else{
                out[texOffset] = 255;
            }
			texOffset++;

            palColor = colorAddr[color4];
            SIM_u16ToRGB( palColor, &r, &g, &b );
			out[texOffset] = r;
			texOffset++;
			out[texOffset] = g;
			texOffset++;
			out[texOffset] = b;
			texOffset++;
            if(color0 == 1 && color4 == 0)
            {
                out[texOffset] = 0;
            }else{
                out[texOffset] = 255;
            }
			texOffset++;

            vramTex++;
        }
    }
}

void DecodeTex16(u8* vramTex, u16* colorAddr, u8* out, u32 s, u32 t, u8 color0){
    //4 bit per pixel (16 colour)
	u32 texOffset = 0;
    if( vramTex == NULL )
    {
        //This probably should not happen
        return;
    }
	for( int i=0; i < s; i++ )
	{
		for( int j=0; j < t / 2; j++ )
		{
			u8 color1 = (*vramTex) & 0b00001111;
			u8 color2 = ((*vramTex) & 0b11110000) >> 4;
			u16 palColor1;
			u16 palColor2;
			palColor1 = colorAddr[color1];
			palColor2 = colorAddr[color2];

			u8 r;
			u8 g;
			u8 b;

			SIM_u16ToRGB( palColor1, &r, &g, &b );
			out[texOffset] = r;
			texOffset++;
			out[texOffset] = g;
			texOffset++;
			out[texOffset] = b;
			texOffset++;
            if(color0 == 1 && color1 == 0)
            {
                out[texOffset] = 0;
            }else{
                out[texOffset] = 255;
            }
			texOffset++;

			//Color 2
			SIM_u16ToRGB( palColor2, &r, &g, &b );
			out[texOffset] = r;
			texOffset++;
			out[texOffset] = g;
			texOffset++;
			out[texOffset] = b;
			texOffset++;
            if(color0 == 1 && color2 == 0)
            {
                out[texOffset] = 0;
            }else{
                out[texOffset] = 255;
            }
			texOffset++;
			vramTex++;
		}
	}
}

void DecodeTex256(u8* vramTex, u16* colorAddr, u8* out, u32 s, u32 t, u8 color0){
	u32 texOffset = 0;
	for( int i=0; i < s; i++ )
	{
		for( int j=0; j < t; j++ )
		{
			u8 color = (*vramTex);
			u16 palColor;
			palColor = colorAddr[color];

			u8 r;
			u8 g;
			u8 b;

			SIM_u16ToRGB( palColor, &r, &g, &b );
			out[texOffset] = r;
			texOffset++;
			out[texOffset] = g;
			texOffset++;
			out[texOffset] = b;
			texOffset++;
            if(color0 == 1 && color == 0)
            {
                out[texOffset] = 0;
            }else{
                out[texOffset] = 255;
            }
			texOffset++;
			vramTex++;
		}
	}
}

void DecodeTexA3I5(u8* vramTex, u16* colorAddr, u8* out, u32 s, u32 t)
{
    #ifdef SDK_TRACY_ENABLE
    TracyCZone(ctx, 1);
    #endif
    u32 texOffset = 0;
    u8 colorIdx;
    u8 alpha;

    u8 r;
    u8 g;
    u8 b;

    u16 color;
    for(int i=0; i < s; i++)
    {
        for(int j=0; j < t; j++)
        {
            colorIdx = (*vramTex) & 0b00011111;
            alpha = ((*vramTex) & 0b11100000) >> 5;
            //See GBATek A3I5
            alpha = (alpha*4) + (alpha/2);
            color = colorAddr[colorIdx];
            SIM_u16ToRGB( color, &r, &g, &b );
			out[texOffset] = r;
			texOffset++;
			out[texOffset] = g;
			texOffset++;
			out[texOffset] = b;
			texOffset++;
			out[texOffset] = alpha * 8;
			texOffset++;
            vramTex++;
        }
    }
    #ifdef SDK_TRACY_ENABLE
    TracyCZoneEnd(ctx);
    #endif
}

void DecodeTexA5I3(u8* vramTex, u16* colorAddr, u8* out, u32 s, u32 t)
{
    #ifdef SDK_TRACY_ENABLE
    TracyCZone(ctx, 1);
    #endif
    u32 texOffset = 0;
    u8 colorIdx;
    u8 alpha;

    u8 r;
    u8 g;
    u8 b;

    u16 color;
    for(int i=0; i < s; i++)
    {
        for(int j=0; j < t; j++)
        {
            colorIdx = (*vramTex) & 0b00000111;
            alpha = ((*vramTex) & 0b11111000) >> 3;
            color = colorAddr[colorIdx];
            SIM_u16ToRGB( color, &r, &g, &b );
			out[texOffset] = r;
			texOffset++;
			out[texOffset] = g;
			texOffset++;
			out[texOffset] = b;
			texOffset++;
			out[texOffset] = alpha*8;
			texOffset++;
            vramTex++;
        }
    }
    #ifdef SDK_TRACY_ENABLE
    TracyCZoneEnd(ctx);
    #endif
}

void DecodeTexDirect(u8* vramTex, u8* out, u32 s, u32 t)
{
    #ifdef SDK_TRACY_ENABLE
    TracyCZone(ctx, 1);
    #endif
    u16 * colorAddr = (u16*)vramTex;
    u8 r;
    u8 g;
    u8 b;
    u32 texOffset = 0;
    for(int i=0; i < s; i++)
    {
        for(int j = 0; j < t; j++ )
        {
            u16 color = *colorAddr;
            SIM_u16ToRGB( color, &r, &g, &b );
			out[texOffset] = r;
			texOffset++;
			out[texOffset] = g;
			texOffset++;
			out[texOffset] = b;
			texOffset++;
			out[texOffset] = 255;
			texOffset++;
            colorAddr++;
        }
    }
    #ifdef SDK_TRACY_ENABLE
    TracyCZoneEnd(ctx);
    #endif
}

}
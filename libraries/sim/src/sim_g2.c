#include <nitro.h>
#include <simulator/sim.h>
#ifdef SDK_BUILD_NX
#include <glad/glad.h>
#else
#include <simulator/glad/glad.h>
#endif

#include "tracy/TracyC.h"

//TODO: Move more of these in here as statics
extern u8 bgtex[4*SIM_NDS_SCREEN_WIDTH*SIM_NDS_SCREEN_HEIGHT*2];
extern u8 bg0tex[4*SIM_NDS_SCREEN_WIDTH*SIM_NDS_SCREEN_HEIGHT*2];
extern u8 bg1tex[4*SIM_NDS_SCREEN_WIDTH*SIM_NDS_SCREEN_HEIGHT*2];
extern u8 bg2tex[4*SIM_NDS_SCREEN_WIDTH*SIM_NDS_SCREEN_HEIGHT*2];
extern u8 bg3tex[4*SIM_NDS_SCREEN_WIDTH*SIM_NDS_SCREEN_HEIGHT*2];
extern u8 bg0stex[4*SIM_NDS_SCREEN_WIDTH*SIM_NDS_SCREEN_HEIGHT*2];
extern u8 bg1stex[4*SIM_NDS_SCREEN_WIDTH*SIM_NDS_SCREEN_HEIGHT*2];
extern u8 bg2stex[4*SIM_NDS_SCREEN_WIDTH*SIM_NDS_SCREEN_HEIGHT*2];
extern u8 bg3stex[4*SIM_NDS_SCREEN_WIDTH*SIM_NDS_SCREEN_HEIGHT*2];

extern u8 oam0Tex[4*SIM_NDS_SCREEN_WIDTH*SIM_NDS_SCREEN_HEIGHT*2];
extern u8 oam1Tex[4*SIM_NDS_SCREEN_WIDTH*SIM_NDS_SCREEN_HEIGHT*2];
extern u8 oam2Tex[4*SIM_NDS_SCREEN_WIDTH*SIM_NDS_SCREEN_HEIGHT*2];
extern u8 oam3Tex[4*SIM_NDS_SCREEN_WIDTH*SIM_NDS_SCREEN_HEIGHT*2];
extern u8 oam0sTex[4*SIM_NDS_SCREEN_WIDTH*SIM_NDS_SCREEN_HEIGHT*2];
extern u8 oam1sTex[4*SIM_NDS_SCREEN_WIDTH*SIM_NDS_SCREEN_HEIGHT*2];
extern u8 oam2sTex[4*SIM_NDS_SCREEN_WIDTH*SIM_NDS_SCREEN_HEIGHT*2];
extern u8 oam3sTex[4*SIM_NDS_SCREEN_WIDTH*SIM_NDS_SCREEN_HEIGHT*2];
extern u8 objWindowTex[4*SIM_NDS_SCREEN_WIDTH*SIM_NDS_SCREEN_HEIGHT*2];
extern u8 objWindowsTex[4*SIM_NDS_SCREEN_WIDTH*SIM_NDS_SCREEN_HEIGHT*2];

extern u8 s_SIM_DBG_BGenable[4];
extern u8 s_SIM_DBG_OAMenable;
extern u8 s_SIM_DBG_BGSenable[4];
extern u8 s_SIM_DBG_OAMSenable;

extern u8 s_mosaicTable[16][256];

extern GLuint s_bgTextureId[4];
extern GLuint s_objTextureId[5];
extern GLint s_bgTexUnits[];

extern u32 OBJLine[2][256];
extern u8 OBJWindow[2][256];

static u8 * bgTexTable[] = {bg0tex, bg1tex, bg2tex, bg3tex};
static u8 * bgsTexTable[] = {bg0stex, bg1stex, bg2stex, bg3stex};
static s32 bgAffineX[4];
static s32 bgAffineY[4];

static void DrawBGLine_Text(u32 line, u32 bgnum, u8 mosaic, u8 isSub);
static void DrawBGLine_Extended(u32 line, u32 bgnum, u8 mosaic, u8 isSub);
static void DrawBGLine_Affine(u32 line, u32 bgnum, u8 mosaic, u8 isSub);
static void DrawOBJ_RotScale(u32 num, u32 boundwidth, u32 boundheight, u32 width, u32 height, s32 xpos, s32 ypos, u8 isSub, u8 isWin);
static void DrawOBJ_Normal(u32 num, u32 width, u32 height, s32 xpos, s32 ypos, u8 isSub, u8 isWin);
static void* getBgExtPlttVramBank(BOOL isSub);

void G2SIM_DrawBG( u8 bgNum, u8 bgMode, u8 bg03D, u8 isSub )
{
    TracyCZone(ctx, 1);
    char zoneNameBuf[50] = {0};
    snprintf(zoneNameBuf, 49, 
             "G2SIM_DrawBG%s%d %s",
             isSub ? "S" : "",
             bgNum,
             (bgNum == 0) && !isSub && bg03D ? "(3D)" : "");
    TracyCZoneName(ctx, zoneNameBuf, strlen(zoneNameBuf));

    if(isSub){
        if(!s_SIM_DBG_BGSenable[bgNum]) {
            // Return early if the BgNum has been disabled
            // via debugging GUI
            return;
        }
    } else {
        if(!s_SIM_DBG_BGenable[bgNum]) {
            // Return early if the BgNum has been disabled
            // via debugging GUI
            return;
        }
    }

    switch( bgNum )
    {
        case 0:
            //BG0
            switch( bgMode )
            {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                    //Modes 0-5 on BG0 can be text or 3D
                    //On sub engine, this is always text mode
                    if( bg03D && (isSub == 0) )
                    {

                    }
                    else
                    {
                        //Text Mode
                        for( int j=0; j<SIM_NDS_SCREEN_HEIGHT; j++ )
                        {
                            DrawBGLine_Text(j, bgNum, 0, isSub);
                        }
                    }
                    break;
                case 6:
                    //Mode 6 on BG0 is always 3D
                    //On sub engine this mode is disabled
                    break;
                default:
                    //invalid mode
                    break;
            }
            break;
                case 1:
                    //BG1
                    switch( bgMode )
                    {
                        case 0:
                        case 1:
                        case 2:
                        case 3:
                        case 4:
                        case 5:
                            //Modes 0-5 on BG1 are always text
                            //Text Mode
                            for( int j=0; j<SIM_NDS_SCREEN_HEIGHT; j++ )
                            {
                                DrawBGLine_Text(j, bgNum, 0, isSub);
                            }
                            break;
                        case 6:
                            //Mode 6 disabled BG1
                            //Don't do anything
                            break;
                        default:
                            //invalid mode
                            break;
                    }
                    break;
                        case 2:
                            //BG2
                            switch( bgMode )
                            {
                                case 0:
                                case 1:
                                    //Modes 0 and 1 are text on BG2
                                    //Text Mode
                                    for( int j=0; j<SIM_NDS_SCREEN_HEIGHT; j++ )
                                    {
                                        DrawBGLine_Text(j, bgNum, 0, isSub);
                                    }
                                    break;
                                case 2:
                                    //Mode 2 is affine on BG2
                                    //Affine Mode
                                    for( int j=0; j<SIM_NDS_SCREEN_HEIGHT; j++ )
                                    {
                                        DrawBGLine_Affine(j, bgNum, 0, isSub);
                                    }
                                    break;
                                case 3:
                                    //Mode 3 is text on BG2
                                    //Text Mode
                                    for( int j=0; j<SIM_NDS_SCREEN_HEIGHT; j++ )
                                    {
                                        DrawBGLine_Text(j, bgNum, 0, isSub);
                                    }
                                    break;
                                case 4:
                                    //Mode 5 is affine on BG2
                                    //Affine Mode
                                    for( int j=0; j<SIM_NDS_SCREEN_HEIGHT; j++ )
                                    {
                                        DrawBGLine_Affine(j, bgNum, 0, isSub);
                                    }
                                    break;
                                case 5:
                                    //Mode 5 is "Extended" on BG2
                                    //Extended Mode
                                    for(int j=0; j < SIM_NDS_SCREEN_HEIGHT; j++) {
                                        DrawBGLine_Extended(j, bgNum, 0, isSub);
                                    }
                                    break;
                                case 6:
                                    //Mode 6 is "Large" on BG2
                                    //On the sub engine this mode is disabled
                                    if( isSub == 0 )
                                    {
                                        //Large Mode
                                        //PCPORT_TODO
                                    }
                                    break;
                                default:
                                    //invalid mode
                                    break;
                            }
                            break;
                                case 3:
                                    //BG3
                                    switch( bgMode )
                                    {
                                        case 0:
                                            //Mode 0 and is text on BG3
                                            //Text Mode
                                            for( int j=0; j<SIM_NDS_SCREEN_HEIGHT; j++ )
                                            {
                                                DrawBGLine_Text(j, bgNum, 0, isSub);
                                            }
                                            break;
                                        case 1:
                                        case 2:
                                            //Modes 1 and 2 are affine on BG3
                                            //Affine Mode
                                            for( int j=0; j<SIM_NDS_SCREEN_HEIGHT; j++ )
                                            {
                                                DrawBGLine_Affine(j, bgNum, 0, isSub);
                                            }
                                            break;
                                        case 3:
                                        case 4:
                                        case 5:
                                            //Modes 3-5 are "Extended" on BG3
                                            //Extended Mode
                                            for( int j=0; j<SIM_NDS_SCREEN_HEIGHT; j++ )
                                            {
                                                DrawBGLine_Extended(j, bgNum, 0, isSub);
                                            }
                                            break;
                                        case 6:
                                            //Mode 6 is disabled on BG3
                                            //Do nothing
                                            break;
                                        default:
                                            //invalid mode
                                            break;
                                    }
                                    break;
                                        default:
                                            //Invalid bg
                                            break;
    }

    u8* bgTexBuf;
    if(isSub)
    {
        bgTexBuf = bgsTexTable[bgNum];
    }
    else
    {
        bgTexBuf = bgTexTable[bgNum];
    }

    glActiveTexture(GL_TEXTURE0 + s_bgTexUnits[bgNum]);
    glBindTexture(GL_TEXTURE_2D, s_bgTextureId[bgNum]);

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SIM_NDS_SCREEN_WIDTH, SIM_NDS_SCREEN_HEIGHT*2, GL_RGBA, GL_UNSIGNED_BYTE, (void *)bgTexBuf);
    TracyCZoneEnd(ctx);
}

void G2SIM_DrawOBJ( u32 line, u8 isSub, int bgnum ) {
    u32 dispcnt;
    u16* oam;
    u8 * pixelBuf;
    if( isSub )
    {
        dispcnt = s_reg_GXS_DB_DISPCNT;
        oam = (void *)s_HW_DB_OAM;

        if(!s_SIM_DBG_OAMSenable) {
            // Return early when OAM is turned off
            // via debugging GUI
            return;
        }
    }
    else
    {
        dispcnt = s_reg_GX_DISPCNT;
        oam = (void *)s_HW_OAM;

        if(!s_SIM_DBG_OAMenable) {
            // Return early when OAM is turned off
            // via debugging GUI
            return;
        }
    }

    if( !(dispcnt & 0x1000) )
    {
        return;
    }

    const s32 spritewidth[16] =
    {
        8, 16, 8, 8,
        16, 32, 8, 8,
        32, 32, 16, 8,
        64, 64, 32, 8
    };
    const s32 spriteheight[16] =
    {
        8, 8, 16, 8,
        16, 8, 32, 8,
        32, 16, 32, 8,
        64, 32, 64, 8
    };

    TracyCZone(ctx, 1);
    for(int sprnum = 127; sprnum >= 0; sprnum-- )
    {
        u16* attrib = &oam[sprnum*4];

        if((attrib[2] & 0x0C00) != bgnum)
        {
            continue;
        }
        u8 iswin = (((attrib[0] >> 10) & 0x3) == 2);

        u32 sprline;
        if ((attrib[0] & 0x1000) && !iswin)
        {
            // apply Y mosaic
            //sprline = CurUnit->OBJMosaicY;
        }
        else
        {
            sprline = line;
        }

        if(attrib[0] & 0x0100)
        {
            u32 sizeparam = (attrib[0] >> 14) | ((attrib[1] & 0xC000) >> 12);
            s32 width = spritewidth[sizeparam];
            s32 height = spriteheight[sizeparam];
            s32 boundwidth = width;
            s32 boundheight = height;


            if (attrib[0] & 0x0200)
            {
                boundwidth <<= 1;
                boundheight <<= 1;
            }

            u32 ypos = attrib[0] & 0xFF;
            if (((line - ypos) & 0xFF) >= (u32)boundheight)
            {
                continue;
            }
            ypos = (sprline - ypos) & 0xFF;

            s32 xpos = (s32)(attrib[1] << 23) >> 23;
            if (xpos <= -boundwidth)
            {
                continue;
            }

            u32 rotparamgroup = (attrib[1] >> 9) & 0x1F;

            DrawOBJ_RotScale(sprnum, boundwidth, boundheight, width, height, xpos, ypos, isSub, iswin);
        }
        else
        {
            if( attrib[ 0 ] & 0x0200 )
            {
                continue;
            }

            u32 sizeparam = (attrib[0] >> 14) | ((attrib[1] & 0xC000) >> 12);
            s32 width = spritewidth[sizeparam];
            s32 height = spriteheight[sizeparam];

            u32 ypos = attrib[0] & 0xFF;
            if (((line - ypos) & 0xFF) >= (u32)height)
            {
                continue;
            }
            ypos = (sprline - ypos) & 0xFF;

            s32 xpos = (s32)(attrib[1] << 23) >> 23;
            if( xpos <= -width )
            {
                continue;
            }

            DrawOBJ_Normal(sprnum, width, height, xpos, ypos, isSub, iswin);
        }
    }
    TracyCZoneEnd(ctx);
}

// Draws a pixel to the screenBuf. X and Y coordinates are in local DS screen coordinates
// Pixel will be plotted on the correct screen based on dispsel and isSub
void G2SIM_PlotPixel(u8 * screenBuf, u8 r, u8 g, u8 b, u32 x, u32 y, BOOL isSub) {
    u32 dispSel = s_reg_GX_POWCNT >> 15;

    BOOL topScreen = dispSel ? !isSub : isSub;

    if( topScreen )
    {
        screenBuf[(4*x)+(4*SIM_NDS_SCREEN_WIDTH*((SIM_NDS_SCREEN_HEIGHT*2)-1-y))] = r;
        screenBuf[(4*x)+(4*SIM_NDS_SCREEN_WIDTH*((SIM_NDS_SCREEN_HEIGHT*2)-1-y))+1] = g;
        screenBuf[(4*x)+(4*SIM_NDS_SCREEN_WIDTH*((SIM_NDS_SCREEN_HEIGHT*2)-1-y))+2] = b;
        screenBuf[(4*x)+(4*SIM_NDS_SCREEN_WIDTH*((SIM_NDS_SCREEN_HEIGHT*2)-1-y))+3] = 255;
    }
    else
    {
        screenBuf[(4*x)+(4*SIM_NDS_SCREEN_WIDTH*((SIM_NDS_SCREEN_HEIGHT*2)-1-(y+SIM_NDS_SCREEN_HEIGHT)))] = r;
        screenBuf[(4*x)+(4*SIM_NDS_SCREEN_WIDTH*((SIM_NDS_SCREEN_HEIGHT*2)-1-(y+SIM_NDS_SCREEN_HEIGHT)))+1] = g;
        screenBuf[(4*x)+(4*SIM_NDS_SCREEN_WIDTH*((SIM_NDS_SCREEN_HEIGHT*2)-1-(y+SIM_NDS_SCREEN_HEIGHT)))+2] = b;
        screenBuf[(4*x)+(4*SIM_NDS_SCREEN_WIDTH*((SIM_NDS_SCREEN_HEIGHT*2)-1-(y+SIM_NDS_SCREEN_HEIGHT)))+3] = 255;							
    }
}

static void DrawBGLine_Text(u32 line, u32 bgnum, u8 mosaic, u8 isSub){
    TracyCZone(ctx, 1);
    u8 * bgvram = (u8 *)s_HW_BG_VRAM;
    u8 * pixelBuf;
    u32 tilesetaddr;
    u32 tilemapaddr;
    u32 bgvrammask = isSub ? 0x1FFFF : 0x7FFFF;

    u32 i;

    u32 lastxpos;

    u8 color;

    u16 xoff = 0;
    u16 yoff = line;

    u32 widexmask;

    REGType16v bgcnt;
    REGType32v dispcnt;
    u8 dispSel;
    dispSel = s_reg_GX_POWCNT >> 15;
    if( isSub )
    {
        switch( bgnum )
        {
            case 0:
                xoff += (s_reg_G2S_DB_BG0OFS & 0x1ff);
                yoff += ((s_reg_G2S_DB_BG0OFS >> 16) & 0x1ff);
                bgcnt = s_reg_G2S_DB_BG0CNT;
                pixelBuf = bg0stex;
                break;
            case 1:
                xoff += (s_reg_G2S_DB_BG1OFS & 0x1ff);
                yoff += ((s_reg_G2S_DB_BG1OFS >> 16) & 0x1ff);
                bgcnt = s_reg_G2S_DB_BG1CNT;
                pixelBuf = bg1stex;
                break;
            case 2:
                xoff += (s_reg_G2S_DB_BG2OFS & 0x1ff);
                yoff += ((s_reg_G2S_DB_BG2OFS >> 16) & 0x1ff);
                bgcnt = s_reg_G2S_DB_BG2CNT;
                pixelBuf = bg2stex;
                break;
            case 3:
                xoff += (s_reg_G2S_DB_BG3OFS & 0x1ff);
                yoff += ((s_reg_G2S_DB_BG3OFS >> 16) & 0x1ff);
                bgcnt = s_reg_G2S_DB_BG3CNT;
                pixelBuf = bg3stex;
                break;
            default:
                //Should not get here
                bgcnt = 0;
                break;
        }
    }
    else
    {
        switch( bgnum )
        {
            case 0:
                xoff += (s_reg_G2_BG0OFS & 0x1ff);
                yoff += ((s_reg_G2_BG0OFS >> 16) & 0x1ff);
                bgcnt = s_reg_G2_BG0CNT;
                pixelBuf = bg0tex;
                break;
            case 1:
                xoff += (s_reg_G2_BG1OFS & 0x1ff);
                yoff += ((s_reg_G2_BG1OFS >> 16) & 0x1ff);
                bgcnt = s_reg_G2_BG1CNT;
                pixelBuf = bg1tex;
                break;
            case 2:
                xoff += (s_reg_G2_BG2OFS & 0x1ff);
                yoff += ((s_reg_G2_BG2OFS >> 16) & 0x1ff);
                bgcnt = s_reg_G2_BG2CNT;
                pixelBuf = bg2tex;
                break;
            case 3:
                xoff += (s_reg_G2_BG3OFS & 0x1ff);
                yoff += ((s_reg_G2_BG3OFS >> 16) & 0x1ff);
                bgcnt = s_reg_G2_BG3CNT;
                pixelBuf = bg3tex;
                break;
            default:
                //Should not get here
                bgcnt = 0;
                break;
        }
    }


    if( isSub )
    {
        dispcnt = s_reg_GXS_DB_DISPCNT;
        bgvram = (u8*)s_HW_DB_BG_VRAM;
    }
    else
    {
        dispcnt = s_reg_GX_DISPCNT;
    }

    u16 * curpal;
    u16 * pal;
    u32 extpal, extpalslot;

    widexmask = (bgcnt & 0x4000) ? 0x100 : 0;

    extpal = dispcnt & 0x40000000;
    if( extpal )
    {
        extpalslot = ((bgnum<2) && (bgcnt & 0x2000)) ? (2+bgnum) : bgnum;
    }

    if( isSub )
    {
        tilesetaddr = ((bgcnt & 0x003C) << 12);
        tilemapaddr = ((bgcnt & 0x1F00) << 3);
        pal = (u16*)&s_HW_DB_BG_PLTT[0];
    }
    else
    {
        tilesetaddr = ((dispcnt & 0x07000000) >> 8) + ((bgcnt & 0x003C) << 12);
        tilemapaddr = ((dispcnt & 0x38000000) >> 11) + ((bgcnt & 0x1F00) << 3);
        pal = (u16*)&s_HW_BG_PLTT[0];
    }



     //PCPORT_TODO might be wrong

    if (bgcnt & 0x8000)
    {
        tilemapaddr += ((yoff & 0x1F8) << 3);
        if (bgcnt & 0x4000)
            tilemapaddr += ((yoff & 0x100) << 3);
    }
    else
    {
        tilemapaddr += ((yoff & 0xF8) << 3);
    }

    u16 curtile;



    u32 pixelsaddr;

    if( bgcnt & 0x0080)
    {
        //256 color
        //preload shit as needed
        if( (xoff & 0x7 ) || mosaic )
        {
            curtile = *(u16*)&bgvram[((tilemapaddr + ((xoff & 0xF8) >> 2) + ((xoff & widexmask) << 3))) & bgvrammask];
            if( extpal )
            {
                const u32 PaletteSize = 256 * 2;
                const u32 SlotSize = PaletteSize * 16;

                if( isSub )
                {
                    curpal = (u16*)(getBgExtPlttVramBank(isSub) + (extpalslot * SlotSize) + ((curtile>>12) * PaletteSize));
                }
                else
                {
                    curpal = (u16*)getBgExtPlttVramBank(isSub);
                }
            }
            else
            {
                curpal = pal;
            }
            pixelsaddr = tilesetaddr + ((curtile & 0x03FF) << 6)
                                     + (((curtile & 0x0800) ? (7-(yoff&0x7)) : (yoff&0x7)) << 3);
        }

        if( mosaic )
        {
            lastxpos = xoff;
        }

        for( i=0; i<256; i++ )
        {
            u32 xpos;
            if( mosaic )
            {
                xpos = xoff - s_mosaicTable[7][i];
            }
            else
            {
                xpos = xoff;
            }

            if ((!mosaic && (!(xpos & 0x7))) ||
                (mosaic && ((xpos >> 3) != (lastxpos >> 3))))
            {
                //load a new tile
                curtile = *(u16*)&bgvram[(tilemapaddr + ((xpos & 0xF8) >> 2) + ((xpos & widexmask) << 3)) & bgvrammask];
                
                //PCPORT_TODO extpal
                if( extpal )
                {
                    const u32 PaletteSize = 256 * 2;
                    const u32 SlotSize = PaletteSize * 16;

                    if( isSub )
                    {
                        curpal = (u16*)(getBgExtPlttVramBank(isSub) + (extpalslot * SlotSize) + ((curtile>>12) * PaletteSize));
                    }
                    else
                    {
                        curpal = (u16*)getBgExtPlttVramBank(isSub);
                    }
                }
                else
                {
                    curpal = pal;
                }

                pixelsaddr = tilesetaddr + ((curtile & 0x03FF) << 6)
                                     + (((curtile & 0x0800) ? (7-(yoff&0x7)) : (yoff&0x7)) << 3);
                
                if( mosaic )
                {
                    lastxpos = xpos;
                }
            }

            //draw pixel

            // Determine if we are inside/outside the window, and if the current bg should be drawn based on that
            u8 win0Enabled = (dispcnt >> 13) & 1;
            u8 win1Enabled = (dispcnt >> 14) & 1;

            u16 win0H = isSub ? s_reg_G2S_DB_WIN0H : s_reg_G2_WIN0H;
            u16 win1H = isSub ? s_reg_G2S_DB_WIN1H : s_reg_G2_WIN1H;

            u16 win0V = isSub ? s_reg_G2S_DB_WIN0V : s_reg_G2_WIN0V;
            u16 win1V = isSub ? s_reg_G2S_DB_WIN1V : s_reg_G2_WIN1V;

            u16 winIn = isSub ? s_reg_G2S_DB_WININ : s_reg_G2_WININ;
            u16 winOut = isSub ? s_reg_G2S_DB_WINOUT : s_reg_G2_WINOUT;

            u8 windowPass = TRUE;

            if(win0Enabled) {
                u8 windowRight = win0H & 0b11111111;
                u8 windowLeft = (win0H >> 8) & 0b11111111;
                u8 windowBottom = win0V & 0b11111111;
                u8 windowTop = (win0V >> 8) & 0b11111111;

                if(i >= windowLeft && i < windowRight
                && line >= windowTop && line < windowBottom) {
                    // Inside window
                    windowPass = ((winIn >> bgnum) & 1);
                } else {
                    windowPass = ((winOut >> bgnum) & 1);
                }
            }

            if(windowPass) {
                u32 tilexoff = (curtile & 0x0400) ? (7-(xpos&0x7)) : (xpos&0x7);
                color = bgvram[(pixelsaddr + tilexoff) & bgvrammask];
                if( color )
                {
                    u8 r;
                    u8 g;
                    u8 b;
                    SIM_u16ToRGB( curpal[color], &r, &g, &b );
                    //SDL_SetRenderDrawColor(renderer, r, g, b, 255);
                    G2SIM_PlotPixel(pixelBuf, r, g, b, i, line, isSub);
                }
            }

            xoff++;
        }
    }
    else
    {
        //16 color
        
        //preload shit as needed
        if( ( xoff & 0x7 ) || mosaic )
        {
            curtile = *(u16*)&bgvram[((tilemapaddr + ((xoff & 0xF8) >> 2) + ((xoff & widexmask) << 3))) & bgvrammask];
            //PCPORT_TODO: extpal
            curpal = pal + ((curtile & 0xF000) >> 8);
            pixelsaddr = tilesetaddr + ((curtile & 0x03FF) << 5)
                                     + (((curtile & 0x0800) ? (7-(yoff&0x7)) : (yoff&0x7)) << 2);
        }

        if( mosaic )
        {
            lastxpos = xoff;
        }

        for( i=0; i < 256; i++ )
        {
            u32 xpos;
            if( mosaic )
            {
                xpos = xoff - s_mosaicTable[7][i];
            }
            else
            {
                xpos = xoff;
            }


            if ((!mosaic && (!(xpos & 0x7))) 
             || (mosaic && ((xpos >> 3) != (lastxpos >> 3))))
            {
                //load new tile
                curtile = *(u16*)&bgvram[(tilemapaddr + ((xpos & 0xF8) >> 2) + ((xpos & widexmask) << 3)) & bgvrammask];
                curpal = pal + ((curtile & 0xF000) >> 8);
                pixelsaddr = tilesetaddr + ((curtile & 0x03FF) << 5)
                                             + (((curtile & 0x0800) ? (7-(yoff&0x7)) : (yoff&0x7)) << 2);
                if( mosaic )
                {
                    lastxpos = xpos;
                }
            }

            if( curtile != 0 )
            {
                u32 temp;
                temp = 1;
            }

            //draw pixel
            u32 tilexoff = (curtile & 0x0400) ? (7-(xpos&0x7)) : (xpos&0x7);
            if (tilexoff & 0x1)
            {
                color = bgvram[(pixelsaddr + (tilexoff >> 1)) & bgvrammask] >> 4;
            }
            else
            {
                color = bgvram[(pixelsaddr + (tilexoff >> 1)) & bgvrammask] & 0x0F;
            }

            // Determine if we are inside/outside the window, and if the current bg should be drawn based on that
            u8 win0Enabled = (dispcnt >> 13) & 1;
            u8 win1Enabled = (dispcnt >> 14) & 1;

            u16 win0H = isSub ? s_reg_G2S_DB_WIN0H : s_reg_G2_WIN0H;
            u16 win1H = isSub ? s_reg_G2S_DB_WIN1H : s_reg_G2_WIN1H;

            u16 win0V = isSub ? s_reg_G2S_DB_WIN0V : s_reg_G2_WIN0V;
            u16 win1V = isSub ? s_reg_G2S_DB_WIN1V : s_reg_G2_WIN1V;

            u16 winIn = isSub ? s_reg_G2S_DB_WININ : s_reg_G2_WININ;
            u16 winOut = isSub ? s_reg_G2S_DB_WINOUT : s_reg_G2_WINOUT;

            u8 windowPass = TRUE;

            if(win0Enabled) {
                u8 windowRight = win0H & 0b11111111;
                u8 windowLeft = (win0H >> 8) & 0b11111111;
                u8 windowBottom = win0V & 0b11111111;
                u8 windowTop = (win0V >> 8) & 0b11111111;

                if(i >= windowLeft && i < windowRight
                && line >= windowTop && line < windowBottom) {
                    // Inside window
                    windowPass = ((winIn >> bgnum) & 1);
                } else {
                    windowPass = ((winOut >> bgnum) & 1);
                }
            }

            if( color )
            {
                u8 r;
                u8 g;
                u8 b;
                if(windowPass != 0) {
                    SIM_u16ToRGB( curpal[color], &r, &g, &b );
                    G2SIM_PlotPixel(pixelBuf, r, g, b, i, line, isSub);
                }
            }

            xoff++;
        }

    }

    TracyCZoneEnd(ctx);
}

static void DrawBGLine_Affine(u32 line, u32 bgnum, u8 mosaic, u8 isSub)
{
    TracyCZone(ctx, 1);
    u32 tilesetaddr;
    u32 tilemapaddr;
    u16* pal;
    u8 * pixelBuf;

    u32 coordmask;
    u32 yshift;
    u8 dispSel;
    dispSel = s_reg_GX_POWCNT >> 15;

    REGType16v bgcnt;

    //PCPORT_TODO not sure if these are the right numbers
    s16 rotA;
    s16 rotB;
    s16 rotC;
    s16 rotD;

    s32 rotX;
    s32 rotY;

    if( isSub )
    {
        switch( bgnum )
        {
            case 0:
                //BG0 never has affine
                break;
            case 1:
                //BG1 never has affine
                break;
            case 2:
                bgcnt = s_reg_G2S_DB_BG2CNT;
                rotA = s_reg_G2S_DB_BG2PA;
                rotB = s_reg_G2S_DB_BG2PB;
                rotC = s_reg_G2S_DB_BG2PC;
                rotD = s_reg_G2S_DB_BG2PD;

                rotX = s_reg_G2S_DB_BG2X;
                rotY = s_reg_G2S_DB_BG2Y;
                pixelBuf = bg2stex;
                break;
            case 3:
                bgcnt = s_reg_G2S_DB_BG3CNT;
                rotA = s_reg_G2S_DB_BG3PA;
                rotB = s_reg_G2S_DB_BG3PB;
                rotC = s_reg_G2S_DB_BG3PC;
                rotD = s_reg_G2S_DB_BG3PD;

                rotX = s_reg_G2S_DB_BG3X;
                rotY = s_reg_G2S_DB_BG3Y;
                pixelBuf = bg3stex;
                break;
            default:
                //Should not get here
                bgcnt = 0;
                break;
        }
    }
    else
    {
        switch( bgnum )
        {
            case 0:
                //BG0 never has affine
                break;
            case 1:
                //BG1 never has affine
                break;
            case 2:
                bgcnt = s_reg_G2_BG2CNT;
                rotA = s_reg_G2_BG2PA;
                rotB = s_reg_G2_BG2PB;
                rotC = s_reg_G2_BG2PC;
                rotD = s_reg_G2_BG2PD;

                rotX = s_reg_G2_BG2X;
                rotY = s_reg_G2_BG2Y;
                pixelBuf = bg2tex;
                break;
            case 3:
                bgcnt = s_reg_G2_BG3CNT;
                rotA = s_reg_G2_BG3PA;
                rotB = s_reg_G2_BG3PB;
                rotC = s_reg_G2_BG3PC;
                rotD = s_reg_G2_BG3PD;

                rotX = s_reg_G2_BG3X;
                rotY = s_reg_G2_BG3Y;
                pixelBuf = bg3tex;
                break;
            default:
                //Should not get here
                bgcnt = 0;
                break;
        }
    }
    if(line == 0)
    {
        if(isSub)
        {
            bgAffineX[bgnum-2] = rotX;
            bgAffineY[bgnum-2] = rotY;
        }
        else
        {
            bgAffineX[bgnum] = rotX;
            bgAffineY[bgnum] = rotY;
        }
    }
    else
    {
        if( isSub )
        {
            rotX = bgAffineX[bgnum-2];
            rotY = bgAffineY[bgnum-2];
        }
        else
        {
            rotX = bgAffineX[bgnum];
            rotY = bgAffineY[bgnum];			
        }
    }

    switch (bgcnt & 0xC000)
    {
        case 0x0000: coordmask = 0x07800; yshift = 7; break;
        case 0x4000: coordmask = 0x0F800; yshift = 8; break;
        case 0x8000: coordmask = 0x1F800; yshift = 9; break;
        case 0xC000: coordmask = 0x3F800; yshift = 10; break;
    }

    u32 overflowmask;
    if( bgcnt & 0x2000 )
    {
        overflowmask = 0;
    }
    else
    {
        overflowmask = ~(coordmask | 0x7FF);
    }

    if( bgcnt & 0x0040 )
    {
        //vertical mosaic
        //PCPORT_TODO: Not sure what CurUnit->BGMosaicY would be
    }

    u8 * bgvram = isSub ? (u8*)s_HW_DB_BG_VRAM : (u8 *)s_HW_BG_VRAM;
    u32 bgvrammask = isSub ? 0x1FFFF : 0x7FFFF;

    if(isSub) {
        tilesetaddr = ((bgcnt & 0x003C) << 12);
        tilemapaddr = ((bgcnt & 0x1F00) << 3);
    } else {
        tilesetaddr = ((s_reg_GX_DISPCNT & 0x07000000) >> 8) + ((bgcnt & 0x003C) << 12);
        tilemapaddr = ((s_reg_GX_DISPCNT & 0x38000000) >> 11) + ((bgcnt & 0x1F00) << 3);
    }


    pal = isSub ? (u16*)&s_HW_DB_BG_PLTT[0] : (u16*)&s_HW_BG_PLTT[0];

    u16 curtile;
    u8 color;

    yshift -= 3;

    for( int i=0; i < 256; i++ )
    {
        //PCPORT_TODO assuming windowmask is true here
        if( 1 << bgnum )
        {
            s32 finalX;
            s32 finalY;
            if( mosaic )
            {
                int im;
                im = 7;
                finalX = rotX - (im * rotA);
                finalY = rotY - (im * rotC);
            }
            else
            {
                finalX = rotX;
                finalY = rotY;
            }
            if ((!((finalX|finalY) & overflowmask)))
            {
                curtile = bgvram[(tilemapaddr + ((((finalY & coordmask) >> 11) << yshift) + ((finalX & coordmask) >> 11))) & bgvrammask];

                // draw pixel
                u32 tilexoff = (finalX >> 8) & 0x7;
                u32 tileyoff = (finalY >> 8) & 0x7;

                color = bgvram[(tilesetaddr + (curtile << 6) + (tileyoff << 3) + tilexoff) & bgvrammask];

                if (color)
                {
                    u8 r;
                    u8 g;
                    u8 b;
                    SIM_u16ToRGB( pal[color], &r, &g, &b );
                    G2SIM_PlotPixel(pixelBuf, r, g, b, i, line, isSub);
                }
            }			
        }

        rotX += rotA;
        rotY += rotC;
    }

    //PCPORT_TODO:
    //s_reg_G2_BG2X += rotB;
    //s_reg_G2_BG2Y += rotD;
    if( isSub )
    {
        bgAffineX[bgnum-2] += rotB;
        bgAffineY[bgnum-2] += rotD;
    }
    else
    {
        bgAffineX[bgnum] += rotB;
        bgAffineY[bgnum] += rotD;	
    }


    TracyCZoneEnd(ctx);
}

static void DrawBGLine_Extended(u32 line, u32 bgnum, u8 mosaic, u8 isSub)
{
    TracyCZone(ctx, 1);
    u32 tilesetaddr;
    u32 tilemapaddr;
    u16* pal;
    u8 * pixelBuf;

    u32 coordmask;
    u32 yshift;
    u8 dispSel;
    dispSel = s_reg_GX_POWCNT >> 15;

    REGType16v bgcnt;
    REGType32v dispcnt = isSub ? s_reg_GXS_DB_DISPCNT : s_reg_GX_DISPCNT;
    u32 extpal = dispcnt & 0x40000000;
    u32 extpalslot;
    if( extpal )
    {
        extpalslot = ((bgnum<2) && (bgcnt & 0x2000)) ? (2+bgnum) : bgnum;
    }

    s16 rotA;
    s16 rotB;
    s16 rotC;
    s16 rotD;

    s32 rotX;
    s32 rotY;

    u8 * bgvram = isSub ? (u8*)s_HW_DB_BG_VRAM : (u8 *)s_HW_BG_VRAM;
    u32 bgvrammask = isSub ? 0x1FFFF : 0x7FFFF;

    if( isSub )
    {
        switch( bgnum )
        {
            case 0:
                //BG0 never has affine
                break;
            case 1:
                //BG1 never has affine
                break;
            case 2:
                bgcnt = s_reg_G2S_DB_BG2CNT;
                rotA = s_reg_G2S_DB_BG2PA;
                rotB = s_reg_G2S_DB_BG2PB;
                rotC = s_reg_G2S_DB_BG2PC;
                rotD = s_reg_G2S_DB_BG2PD;

                rotX = s_reg_G2S_DB_BG2X;
                rotY = s_reg_G2S_DB_BG2Y;
                pixelBuf = bg2stex;
                break;
            case 3:
                bgcnt = s_reg_G2S_DB_BG3CNT;
                rotA = s_reg_G2S_DB_BG3PA;
                rotB = s_reg_G2S_DB_BG3PB;
                rotC = s_reg_G2S_DB_BG3PC;
                rotD = s_reg_G2S_DB_BG3PD;

                rotX = s_reg_G2S_DB_BG3X;
                rotY = s_reg_G2S_DB_BG3Y;
                pixelBuf = bg3stex;
                break;
            default:
                //Should not get here
                bgcnt = 0;
                break;
        }
    }
    else
    {
        switch( bgnum )
        {
            case 0:
                //BG0 never has affine
                break;
            case 1:
                //BG1 never has affine
                break;
            case 2:
                bgcnt = s_reg_G2_BG2CNT;
                rotA = s_reg_G2_BG2PA;
                rotB = s_reg_G2_BG2PB;
                rotC = s_reg_G2_BG2PC;
                rotD = s_reg_G2_BG2PD;

                rotX = s_reg_G2_BG2X;
                rotY = s_reg_G2_BG2Y;
                pixelBuf = bg2tex;
                break;
            case 3:
                bgcnt = s_reg_G2_BG3CNT;
                rotA = s_reg_G2_BG3PA;
                rotB = s_reg_G2_BG3PB;
                rotC = s_reg_G2_BG3PC;
                rotD = s_reg_G2_BG3PD;

                rotX = s_reg_G2_BG3X;
                rotY = s_reg_G2_BG3Y;
                pixelBuf = bg3tex;
                break;
            default:
                //Should not get here
                bgcnt = 0;
                break;
        }
    }

    if(line == 0)
    {
        if(isSub)
        {
            bgAffineX[bgnum-2] = rotX;
            bgAffineY[bgnum-2] = rotY;
        }
        else
        {
            bgAffineX[bgnum] = rotX;
            bgAffineY[bgnum] = rotY;
        }
    }
    else
    {
        if( isSub )
        {
            rotX = bgAffineX[bgnum-2];
            rotY = bgAffineY[bgnum-2];
        }
        else
        {
            rotX = bgAffineX[bgnum];
            rotY = bgAffineY[bgnum];			
        }
    }

    if (bgcnt & 0x0040)
    {
        // vertical mosaic
        //rotX -= (CurUnit->BGMosaicY * rotB);
        //rotY -= (CurUnit->BGMosaicY * rotD);
    }

    if (bgcnt & 0x0080)
    {
        // bitmap modes

        u32 xmask, ymask;
        u32 yshift;
        switch (bgcnt & 0xC000)
        {
        case 0x0000: xmask = 0x07FFF; ymask = 0x07FFF; yshift = 7; break;
        case 0x4000: xmask = 0x0FFFF; ymask = 0x0FFFF; yshift = 8; break;
        case 0x8000: xmask = 0x1FFFF; ymask = 0x0FFFF; yshift = 9; break;
        case 0xC000: xmask = 0x1FFFF; ymask = 0x1FFFF; yshift = 9; break;
        }

        u32 ofxmask, ofymask;
        if (bgcnt & 0x2000)
        {
            ofxmask = 0;
            ofymask = 0;
        }
        else
        {
            ofxmask = ~xmask;
            ofymask = ~ymask;
        }

        if (isSub) tilemapaddr = ((bgcnt & 0x1F00) << 6);
        else              tilemapaddr = ((bgcnt & 0x1F00) << 6);

        if (bgcnt & 0x0004)
        {
            // direct color bitmap

            u16 color;

            for (int i = 0; i < 256; i++)
            {
                if (/*WindowMask[i] &*/ (1<<bgnum))
                {
                    s32 finalX, finalY;
                    if (mosaic)
                    {
                        //int im = CurBGXMosaicTable[i];
                        int im;
                        im = 7;
                        finalX = rotX - (im * rotA);
                        finalY = rotY - (im * rotC);
                    }
                    else
                    {
                        finalX = rotX;
                        finalY = rotY;
                    }

                    if (!(finalX & ofxmask) && !(finalY & ofymask))
                    {
                        color = *(u16*)&bgvram[(tilemapaddr + (((((finalY & ymask) >> 8) << yshift) + ((finalX & xmask) >> 8)) << 1)) & bgvrammask];

                        if (color & 0x8000)
                        {
                            u8 r;
                            u8 g;
                            u8 b;
                            SIM_u16ToRGB( pal[color], &r, &g, &b );
                            G2SIM_PlotPixel(pixelBuf, r, g, b, i, line, isSub);
                        }
                    }
                }

                rotX += rotA;
                rotY += rotC;
            }
        }
        else
        {
            // 256-color bitmap

            if (isSub) pal = (u16*)&s_HW_DB_BG_PLTT[0];
            else       pal = (u16*)&s_HW_BG_PLTT[0];

            u8 color;

            for (int i = 0; i < 256; i++)
            {
                if (/*WindowMask[i] &*/ (1<<bgnum))
                {
                    s32 finalX, finalY;
                    if (mosaic)
                    {
                        //int im = CurBGXMosaicTable[i];
                        int im;
                        im = 7;
                        finalX = rotX - (im * rotA);
                        finalY = rotY - (im * rotC);
                    }
                    else
                    {
                        finalX = rotX;
                        finalY = rotY;
                    }

                    if (!(finalX & ofxmask) && !(finalY & ofymask))
                    {
                        color = bgvram[(tilemapaddr + (((finalY & ymask) >> 8) << yshift) + ((finalX & xmask) >> 8)) & bgvrammask];

                        if (color)
                        {
                            u8 r;
                            u8 g;
                            u8 b;
                            SIM_u16ToRGB( pal[color], &r, &g, &b );
                            G2SIM_PlotPixel(pixelBuf, r, g, b, i, line, isSub);
                        }
                    }
                }

                rotX += rotA;
                rotY += rotC;
            }
        }
    }
    else
    {
        // mixed affine/text mode

        u32 coordmask;
        u32 yshift;
        switch (bgcnt & 0xC000)
        {
        case 0x0000: coordmask = 0x07800; yshift = 7; break;
        case 0x4000: coordmask = 0x0F800; yshift = 8; break;
        case 0x8000: coordmask = 0x1F800; yshift = 9; break;
        case 0xC000: coordmask = 0x3F800; yshift = 10; break;
        }

        u32 overflowmask;
        if (bgcnt & 0x2000) overflowmask = 0;
        else                overflowmask = ~(coordmask | 0x7FF);

        if (isSub)
        {
            tilesetaddr = ((bgcnt & 0x003C) << 12);
            tilemapaddr = ((bgcnt & 0x1F00) << 3);
            
            pal = (u16*)&s_HW_DB_BG_PLTT[0];
        }
        else
        {
            tilesetaddr = ((dispcnt & 0x07000000) >> 8) + ((bgcnt & 0x003C) << 12);
            tilemapaddr = ((dispcnt & 0x38000000) >> 11) + ((bgcnt & 0x1F00) << 3);
            
            pal = (u16*)&s_HW_BG_PLTT[0];
        }

        u16 curtile;
        u16* curpal;
        u8 color;

        yshift -= 3;

        for (int i = 0; i < 256; i++)
        {
            if (/*WindowMask[i] &*/ (1<<bgnum))
            {
                s32 finalX, finalY;
                if (mosaic)
                {
                    //int im = CurBGXMosaicTable[i];
                    int im = 7;
                    finalX = rotX - (im * rotA);
                    finalY = rotY - (im * rotC);
                }
                else
                {
                    finalX = rotX;
                    finalY = rotY;
                }

                if ((!((finalX|finalY) & overflowmask)))
                {
                    curtile = *(u16*)&bgvram[(tilemapaddr + (((((finalY & coordmask) >> 11) << yshift) + ((finalX & coordmask) >> 11)) << 1)) & bgvrammask];

                    if(extpal) {
                        const u32 PaletteSize = 256 * 2;
                        const u32 SlotSize = PaletteSize * 16;

                        if( isSub )
                        {
                            curpal = (u16*)(getBgExtPlttVramBank(isSub) + (extpalslot * SlotSize) + ((curtile>>12) * PaletteSize));
                        }
                        else
                        {
                            curpal = (u16*)(getBgExtPlttVramBank(isSub) + (bgnum * SlotSize) + ((curtile>>12) * PaletteSize));
                        }
                    } else {
                        curpal = pal;
                    }

                    // draw pixel
                    u32 tilexoff = (finalX >> 8) & 0x7;
                    u32 tileyoff = (finalY >> 8) & 0x7;

                    if (curtile & 0x0400) tilexoff = 7-tilexoff;
                    if (curtile & 0x0800) tileyoff = 7-tileyoff;

                    color = bgvram[(tilesetaddr + ((curtile & 0x03FF) << 6) + (tileyoff << 3) + tilexoff) & bgvrammask];

                    if (color)
                    {
                        u8 r;
                        u8 g;
                        u8 b;
                        SIM_u16ToRGB( curpal[color], &r, &g, &b );
                        G2SIM_PlotPixel(pixelBuf, r, g, b, i, line, isSub);
                    }
                }
            }

            rotX += rotA;
            rotY += rotC;
        }
    }

    if( isSub )
    {
        bgAffineX[bgnum-2] += rotB;
        bgAffineY[bgnum-2] += rotD;
    }
    else
    {
        bgAffineX[bgnum] += rotB;
        bgAffineY[bgnum] += rotD;	
    }
    TracyCZoneEnd(ctx);
}

static void DrawOBJ_RotScale(u32 num, u32 boundwidth, u32 boundheight, u32 width, u32 height, s32 xpos, s32 ypos, u8 isSub, u8 isWin)
{
    TracyCZone(ctx, 1);
    u32 dispcnt;
    u16 * oam;
    u16 * attrib;
    u8* objvram;
    u32 objvrammask;
    if( isSub )
    {
        dispcnt = s_reg_GXS_DB_DISPCNT;
        oam = (void *)s_HW_DB_OAM;
        objvram = (void *)s_HW_DB_OBJ_VRAM;
        objvrammask = 0x1FFFF;
    }
    else
    {
        dispcnt = s_reg_GX_DISPCNT;
        oam = (void *)s_HW_OAM;
        objvram = (void *)s_HW_OBJ_VRAM;
        objvrammask = 0x3FFFF;
    }

    BOOL window;
    window = isWin;

    attrib = &oam[num * 4];
    u16* rotparams = &oam[(((attrib[1] >> 9) & 0x1F) * 16) + 3];

    u32 pixelattr = ((attrib[2] & 0x0C00) << 6) | 0xC0000;
    u32 tilenum = attrib[2] & 0x03FF;
    u32 spritemode = window ? 0 : ((attrib[0] >> 10) & 0x3);

    u32 ytilefactor;

    u32 * objLine = OBJLine[isSub];
    u8* objWindow = OBJWindow[isSub];

    s32 centerX = boundwidth >> 1;
    s32 centerY = boundheight >> 1;

    if ((attrib[0] & 0x1000) && !window)
    {
        // apply Y mosaic
        pixelattr |= 0x100000;
    }

    u32 xoff;
    if (xpos >= 0)
    {
        xoff = 0;
        if ((xpos+boundwidth) > 256)
            boundwidth = 256-xpos;
    }
    else
    {
        xoff = -xpos;
        xpos = 0;
    }

    s16 rotA = (s16)rotparams[0];
    s16 rotB = (s16)rotparams[4];
    s16 rotC = (s16)rotparams[8];
    s16 rotD = (s16)rotparams[12];

    s32 rotX = ((xoff-centerX) * rotA) + ((ypos-centerY) * rotB) + (width << 7);
    s32 rotY = ((xoff-centerX) * rotC) + ((ypos-centerY) * rotD) + (height << 7);

    width <<= 8;
    height <<= 8;

    u16 color = 0; // transparent in all cases

    if (spritemode == 3)
    {
        u32 alpha = attrib[2] >> 12;
        if (!alpha) return;
        alpha++;

        pixelattr |= (0xC0000000 | (alpha << 24));

        u32 pixelsaddr;
        if (dispcnt & 0x40)
        {
            if (dispcnt & 0x20)
            {
                // 'reserved'
                // draws nothing

                TracyCZoneEnd(ctx);
                return;
            }
            else
            {
                pixelsaddr = tilenum << (7 + ((dispcnt >> 22) & 0x1));
                ytilefactor = ((width >> 8) * 2);
            }
        }
        else
        {
            if (dispcnt & 0x20)
            {
                pixelsaddr = ((tilenum & 0x01F) << 4) + ((tilenum & 0x3E0) << 7);
                ytilefactor = (256 * 2);
            }
            else
            {
                pixelsaddr = ((tilenum & 0x00F) << 4) + ((tilenum & 0x3F0) << 7);
                ytilefactor = (128 * 2);
            }
        }

        for (; xoff < boundwidth;)
        {
            if ((u32)rotX < width && (u32)rotY < height)
            {
                color = *(u16*)&objvram[(pixelsaddr + ((rotY >> 8) * ytilefactor) + ((rotX >> 8) << 1)) & objvrammask];

                if (color & 0x8000)
                {
                    if (window) objWindow[xpos] = 1;
                    else        objLine[xpos] = color | pixelattr;
                }
                else if (!window)
                {
                    if (objLine[xpos] == 0)
                        objLine[xpos] = pixelattr & 0x180000;
                }
            }

            rotX += rotA;
            rotY += rotC;
            xoff++;
            xpos++;
        }
    }
    else
    {
        u32 pixelsaddr = tilenum;
        if (dispcnt & 0x10)
        {
            pixelsaddr <<= ((dispcnt >> 20) & 0x3);
            ytilefactor = (width >> 11) << ((attrib[0] & 0x2000) ? 1:0);
        }
        else
        {
            ytilefactor = 0x20;
        }

        if (spritemode == 1) pixelattr |= 0x80000000;
        else                 pixelattr |= 0x10000000;

        ytilefactor <<= 5;
        pixelsaddr <<= 5;

        if (attrib[0] & 0x2000)
        {
            // 256-color

            if (!window)
            {
                if (!(dispcnt & 0x80000000))
                    pixelattr |= 0x1000;
                else
                    pixelattr |= ((attrib[2] & 0xF000) >> 4);
            }

            for (; xoff < boundwidth;)
            {
                if ((u32)rotX < width && (u32)rotY < height)
                {
                    color = objvram[(pixelsaddr + ((rotY>>11)*ytilefactor) + ((rotY&0x700)>>5) + ((rotX>>11)*64) + ((rotX&0x700)>>8)) & objvrammask];

                    if (color)
                    {
                        if (window) objWindow[xpos] = 1;
                        else        objLine[xpos] = color | pixelattr;
                    }
                    else if (!window)
                    {
                        if (objLine[xpos] == 0)
                            objLine[xpos] = pixelattr & 0x180000;
                    }
                }

                rotX += rotA;
                rotY += rotC;
                xoff++;
                xpos++;
            }
        }
        else
        {
            // 16-color
            if (!window)
            {
                pixelattr |= 0x1000;
                pixelattr |= ((attrib[2] & 0xF000) >> 8);
            }

            for (; xoff < boundwidth;)
            {
                if ((u32)rotX < width && (u32)rotY < height)
                {
                    color = objvram[(pixelsaddr + ((rotY>>11)*ytilefactor) + ((rotY&0x700)>>6) + ((rotX>>11)*32) + ((rotX&0x700)>>9)) & objvrammask];
                    if (rotX & 0x100)
                        color >>= 4;
                    else
                        color &= 0x0F;

                    if (color)
                    {
                        if (window) objWindow[xpos] = 1;
                        else        objLine[xpos] = color | pixelattr;
                    }
                    else if (!window)
                    {
                        if (objLine[xpos] == 0)
                            objLine[xpos] = pixelattr & 0x180000;
                    }
                }

                rotX += rotA;
                rotY += rotC;
                xoff++;
                xpos++;
            }
        }
    }
    TracyCZoneEnd(ctx);
}

static void DrawOBJ_Normal(u32 num, u32 width, u32 height, s32 xpos, s32 ypos, u8 isSub, u8 isWin)
{
    TracyCZone(ctx, 1);
    u32 dispcnt;
    u16 * oam;
    u16 * attrib;
    u8* objvram;
    u32 objvrammask;
    if( isSub )
    {
        dispcnt = s_reg_GXS_DB_DISPCNT;
        oam = (void *)s_HW_DB_OAM;
        objvram = (void *)s_HW_DB_OBJ_VRAM;
        objvrammask = 0x1FFFF;
    }
    else
    {
        dispcnt = s_reg_GX_DISPCNT;
        oam = (void *)s_HW_OAM;
        objvram = (void *)s_HW_OBJ_VRAM;
        objvrammask = 0x3FFFF;
    }

    attrib = &oam[num * 4];

    u32 pixelattr = ((attrib[2] & 0x0C00) << 6) | 0xC0000;
    u32 tilenum = attrib[2] & 0x03FF;

    BOOL window = isWin;

    u32 spritemode = window ? 0 : ((attrib[0] >> 10) & 0x3);

    u32 wmask = width - 8; // really ((width - 1) & ~0x7)

    if ((attrib[0] & 0x1000) && !window)
    {
        // apply Y mosaic
        pixelattr |= 0x100000;
    }

    u32 * objLine = OBJLine[isSub];
    u8* objWindow = OBJWindow[isSub];

    // yflip
    if (attrib[1] & 0x2000)
    {
        ypos = height-1 - ypos;
    }

    u32 xoff;
    u32 xend = width;
    if (xpos >= 0)
    {
        xoff = 0;
        if ((xpos+xend) > 256)
            xend = 256-xpos;
    }
    else
    {
        xoff = -xpos;
        xpos = 0;
    }

    u16 color = 0; // transparent in all cases

    if (spritemode == 3)
    {
        // bitmap sprite

        u32 alpha = attrib[2] >> 12;
        if (!alpha) {
            TracyCZoneEnd(ctx);
            return;
        }
        alpha++;

        pixelattr |= (0xC0000000 | (alpha << 24));

        u32 pixelsaddr = tilenum;
        if (dispcnt & 0x40)
        {
            if (dispcnt & 0x20)
            {
                // 'reserved'
                // draws nothing

                TracyCZoneEnd(ctx);
                return;
            }
            else
            {
                pixelsaddr <<= (7 + ((dispcnt >> 22) & 0x1));
                pixelsaddr += (ypos * width * 2);
            }
        }
        else
        {
            if (dispcnt & 0x20)
            {
                pixelsaddr = ((tilenum & 0x01F) << 4) + ((tilenum & 0x3E0) << 7);
                pixelsaddr += (ypos * 256 * 2);
            }
            else
            {
                pixelsaddr = ((tilenum & 0x00F) << 4) + ((tilenum & 0x3F0) << 7);
                pixelsaddr += (ypos * 128 * 2);
            }
        }

        s32 pixelstride;

        if (attrib[1] & 0x1000) // xflip
        {
            pixelsaddr += ((width-1) << 1);
            pixelsaddr -= (xoff << 1);
            pixelstride = -2;
        }
        else
        {
            pixelsaddr += (xoff << 1);
            pixelstride = 2;
        }

        for (; xoff < xend;)
        {
            color = *(u16*)&objvram[pixelsaddr & objvrammask];

            pixelsaddr += pixelstride;

            if (color & 0x8000)
            {
                if(window) {
                    objWindow[xpos] = 1;
                } else {
                    objLine[xpos] = color | pixelattr;
                }
            }
            else if (!window)
            {
                if (objLine[xpos] == 0)
                    objLine[xpos] = pixelattr & 0x180000;
            }

            xoff++;
            xpos++;
        }
    }
    else
    {
        u32 pixelsaddr = tilenum;
        if (dispcnt & 0x10)
        {
            pixelsaddr <<= ((dispcnt >> 20) & 0x3);
            pixelsaddr += ((ypos >> 3) * (width >> 3)) << ((attrib[0] & 0x2000) ? 1:0);
        }
        else
        {
            pixelsaddr += ((ypos >> 3) * 0x20);
        }

        if (spritemode == 1) pixelattr |= 0x80000000;
        else                 pixelattr |= 0x10000000;

        if (attrib[0] & 0x2000)
        {
            // 256-color
            pixelsaddr <<= 5;
            pixelsaddr += ((ypos & 0x7) << 3);
            s32 pixelstride;

            if (!window)
            {
                if (!(dispcnt & 0x80000000))
                    pixelattr |= 0x1000;
                else
                    pixelattr |= ((attrib[2] & 0xF000) >> 4);
            }

            if (attrib[1] & 0x1000) // xflip
            {
                pixelsaddr += (((width-1) & wmask) << 3);
                pixelsaddr += ((width-1) & 0x7);
                pixelsaddr -= ((xoff & wmask) << 3);
                pixelsaddr -= (xoff & 0x7);
                pixelstride = -1;
            }
            else
            {
                pixelsaddr += ((xoff & wmask) << 3);
                pixelsaddr += (xoff & 0x7);
                pixelstride = 1;
            }

            for (; xoff < xend;)
            {
                color = objvram[pixelsaddr & objvrammask];

                pixelsaddr += pixelstride;

                if (color)
                {
                    if(window) {
                        objWindow[xpos] = 1;
                    } else {
                        objLine[xpos] = color | pixelattr;
                    }
                }
                else if(!window)
                {
                    if (objLine[xpos] == 0)
                        objLine[xpos] = pixelattr & 0x180000;
                }

                xoff++;
                xpos++;
                if (!(xoff & 0x7)) pixelsaddr += (56 * pixelstride);
            }
        }
        else
        {
            // 16-color
            pixelsaddr <<= 5;
            pixelsaddr += ((ypos & 0x7) << 2);
            s32 pixelstride;

            if(!window)
            {
                pixelattr |= 0x1000;
                pixelattr |= ((attrib[2] & 0xF000) >> 8);
            }

            // TODO: optimize VRAM access!!
            // TODO: do xflip better? the 'two pixels per byte' thing makes it a bit shitty

            if (attrib[1] & 0x1000) // xflip
            {
                pixelsaddr += (((width-1) & wmask) << 2);
                pixelsaddr += (((width-1) & 0x7) >> 1);
                pixelsaddr -= ((xoff & wmask) << 2);
                pixelsaddr -= ((xoff & 0x7) >> 1);
                pixelstride = -1;
            }
            else
            {
                pixelsaddr += ((xoff & wmask) << 2);
                pixelsaddr += ((xoff & 0x7) >> 1);
                pixelstride = 1;
            }

            for (; xoff < xend;)
            {
                if (attrib[1] & 0x1000)
                {
                    if (xoff & 0x1) { color = objvram[pixelsaddr & objvrammask] & 0x0F; pixelsaddr--; }
                    else              color = objvram[pixelsaddr & objvrammask] >> 4;
                }
                else
                {
                    if (xoff & 0x1) { color = objvram[pixelsaddr & objvrammask] >> 4; pixelsaddr++; }
                    else              color = objvram[pixelsaddr & objvrammask] & 0x0F;
                }

                if (color)
                {
                    if(window) {
                        objWindow[xpos] = 1;
                    } else {
                        objLine[xpos] = color | pixelattr;
                    }
                }
                else if (!window)
                {
                    if (objLine[xpos] == 0)
                        objLine[xpos] = pixelattr & 0x180000;
                }

                xoff++;
                xpos++;
                if (!(xoff & 0x7)) pixelsaddr += ((attrib[1] & 0x1000) ? -28 : 28);
            }
        }
    }

    TracyCZoneEnd(ctx);
}

static void* getBgExtPlttVramBank(BOOL isSub)
{
    void * vramBank = NULL;

    if(isSub) {
        switch( s_SIM_GXSubBgExtPltt ){
            default:
            case GX_VRAM_SUB_BGEXTPLTT_0123_H:
                //If the sub vram bank is set to none we probably shouldnt do this, 
                //but I dont want to return a NULL pointer either.
                vramBank = (void * )HW_LCDC_VRAM_H;
                break;
        }
    } else {
        switch( s_SIM_GXBgExtPltt ){
            case GX_VRAM_BGEXTPLTT_NONE:
            case GX_VRAM_BGEXTPLTT_0123_E:
                vramBank = (void *)HW_LCDC_VRAM_E;
                break;
            case GX_VRAM_BGEXTPLTT_01_F:
            case GX_VRAM_BGEXTPLTT_0123_FG:
                vramBank = (void *)HW_LCDC_VRAM_F;
                break;
            case GX_VRAM_BGEXTPLTT_23_G:
                vramBank = (void *)HW_LCDC_VRAM_G;
                break;
        }
    }

    return vramBank;
}

#include <nitro.h>
#include <simulator/sim.h>

#include "simulator/gui.h"
#include "simulator/glad/glad.h"

#include "gui_internal.h"

#define TILE_SIZE8BIT (64)
#define TILE_SIZE4BIT (32)
#define NUM_TILES_8BIT HW_BG_VRAM_SIZE / TILE_SIZE8BIT;
#define NUM_TILES_4BIT HW_BG_VRAM_SIZE / TILE_SIZE4BIT
#define MAX_NUM_TILES 16*16

enum {
    TILEVIEWER_MODE_BGTILES_1,
    TILEVIEWER_MODE_BGTILES_2,
    TILEVIEWER_MODE_OBJTILES
};

enum {
    TILEVIEWER_ENGINE_MAIN,
    TILEVIEWER_ENGINE_SUB
};

enum {
    TILEVIEWER_BIT_DEPTH_4,
    TILEVIEWER_BIT_DEPTH_8
};

extern u8 s_SIM_DBG_BGenable[4];
extern u8 s_SIM_DBG_OAMenable;
extern u8 s_SIM_DBG_BGSenable[4];
extern u8 s_SIM_DBG_OAMSenable;

static ImVec2 s_btnSize = {50, 20};

static bool s_showWindowTileViewer = 0;
static bool s_showWindowLayers = 0;
static GLuint tileViewerTextures[MAX_NUM_TILES] = {0};

static u8 tileViewerMode = TILEVIEWER_MODE_BGTILES_1;
static u8 tileViewerEngine = TILEVIEWER_ENGINE_MAIN;
static u8 tileViewerBitDepth = TILEVIEWER_BIT_DEPTH_4;


static void GUI_AppG2TileViewerWindow(bool * p_open);

static void GUI_AppG2TileViewerWindow(bool * p_open) {
    u8 tileTex[8*8*4];
    if(tileViewerTextures[0] == 0) {
        glGenTextures(MAX_NUM_TILES, tileViewerTextures);
        for(int i=0; i<MAX_NUM_TILES; i++) {
            glBindTexture(GL_TEXTURE_2D, tileViewerTextures[i]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 8, 8, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        }
    }

    igBegin("Tile Viewer", p_open, 0);

    if(igButton("Tiles 1", s_btnSize)) {
        tileViewerMode = TILEVIEWER_MODE_BGTILES_1;
    }
    igSameLine(0.0f, 2.0f);
    if(igButton("Tiles 2", s_btnSize)) {
        tileViewerMode = TILEVIEWER_MODE_BGTILES_2;
    }
    igSameLine(0.0f, 2.0f);
    if(igButton("OBJ Tiles", s_btnSize)) {
        tileViewerMode = TILEVIEWER_MODE_OBJTILES;
    }

    if(igButton("Main Engine", s_btnSize)) {
        tileViewerEngine = TILEVIEWER_ENGINE_MAIN;
    }
    igSameLine(0.0f, 2.0f);
    if(igButton("Sub Engine", s_btnSize)) {
        tileViewerEngine = TILEVIEWER_ENGINE_SUB;
    }

    if(igButton("4Bit", s_btnSize)) {
        tileViewerBitDepth = TILEVIEWER_BIT_DEPTH_4;
    }
    igSameLine(0.0f, 2.0f);
    if(igButton("8Bit", s_btnSize)) {
        tileViewerBitDepth = TILEVIEWER_BIT_DEPTH_8;
    }

    // Decode and display a tile
    u8 * bgVramPtr;
    u16 * plttPtr;
    u32 tileVramSize;
    u32 tileVramOffset;
    if(tileViewerEngine == TILEVIEWER_ENGINE_MAIN) {
        if(tileViewerMode == TILEVIEWER_MODE_OBJTILES) {
            bgVramPtr = (u8*)HW_OBJ_VRAM;
            plttPtr = (u16*)HW_OBJ_PLTT;
            tileVramSize = HW_OBJ_VRAM_SIZE;
        } else {
            bgVramPtr = (u8*)HW_BG_VRAM;
            plttPtr = (u16*)HW_BG_PLTT;
            tileVramSize = HW_BG_VRAM_SIZE;
        }
    } else {
        if(tileViewerMode == TILEVIEWER_MODE_OBJTILES) {
            bgVramPtr = (u8*)HW_DB_OBJ_VRAM;
            plttPtr = (u16*)HW_DB_OBJ_PLTT;
            tileVramSize = HW_DB_OBJ_VRAM_SIZE;
        } else {
            bgVramPtr = (u8*)HW_DB_BG_VRAM;
            plttPtr = (u16*)HW_DB_BG_PLTT;
            tileVramSize = HW_DB_BG_VRAM_SIZE;
        }
    }

    if(tileViewerBitDepth == TILEVIEWER_BIT_DEPTH_4) {
        if(tileViewerMode == TILEVIEWER_MODE_BGTILES_2) {
            tileVramOffset = TILE_SIZE4BIT * MAX_NUM_TILES;
        } else {
            tileVramOffset = 0;
        }
    } else {
        if(tileViewerMode == TILEVIEWER_MODE_BGTILES_2) {
            tileVramOffset = TILE_SIZE8BIT * MAX_NUM_TILES;
        } else {
            tileVramOffset = 0;
        }
    }

    if(tileVramOffset > tileVramSize) {
        tileVramOffset = 0;
    }

    u32 numTilesToDisplay;
    if(tileViewerBitDepth == TILEVIEWER_BIT_DEPTH_4) {
        numTilesToDisplay = (tileVramSize - tileVramOffset) / TILE_SIZE4BIT;
    } else {
        numTilesToDisplay = (tileVramSize - tileVramOffset) / TILE_SIZE8BIT;
    }
    if(numTilesToDisplay > MAX_NUM_TILES) {
        numTilesToDisplay = MAX_NUM_TILES;
    }

    bgVramPtr += tileVramOffset;
    
    u8 * outTexPtr;
    for(int tileNum = 0; tileNum < numTilesToDisplay; tileNum ++) {
        outTexPtr = tileTex;

        memset(tileTex, 0, sizeof(tileTex));
        if(tileViewerBitDepth == TILEVIEWER_BIT_DEPTH_4) {
            for(int i=0; i < (8*8)/2; i++) {
                u8 colorIdx;
                u8 r;
                u8 g;
                u8 b;
            
            
            
                //Get the first color
                colorIdx = *bgVramPtr & 0xF;
                SIM_u16ToRGB(plttPtr[colorIdx], &r, &g, &b);
                *outTexPtr = r;
                outTexPtr++;
                *outTexPtr = g;
                outTexPtr++;
                *outTexPtr = b;
                outTexPtr++;
                *outTexPtr = 0xFF;
                outTexPtr++;
            
                //Get the second color
                colorIdx = ((*bgVramPtr) >> 4) & 0xF;
                SIM_u16ToRGB(plttPtr[colorIdx], &r, &g, &b);
                *outTexPtr = r;
                outTexPtr++;
                *outTexPtr = g;
                outTexPtr++;
                *outTexPtr = b;
                outTexPtr++;
                *outTexPtr = 0xFF;
                outTexPtr++;
                bgVramPtr++;
            }
        } else {
            for(int i=0; i < (8*8); i++) {
                u8 colorIdx;
                u8 r;
                u8 g;
                u8 b;
                colorIdx = *bgVramPtr;
                SIM_u16ToRGB(plttPtr[colorIdx], &r, &g, &b);
                *outTexPtr = r;
                outTexPtr++;
                *outTexPtr = g;
                outTexPtr++;
                *outTexPtr = b;
                outTexPtr++;
                *outTexPtr = 0xFF;
                outTexPtr++;
                bgVramPtr++;
            }
        }


        glBindTexture(GL_TEXTURE_2D, tileViewerTextures[tileNum]);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 8, 8, GL_RGBA, GL_UNSIGNED_BYTE, (void *)tileTex);
        ImVec2 tileSize = {32.0f, 32.0f};
        ImVec2 uv0 = {0.0f, 0.0f};
        ImVec2 uv1 = {1.0f, 1.0f};

        if(tileNum % 16 != 0) {
            igSameLine(0.0f, 1.0f);
        }

        igPushID_Int(tileNum);
        igImage(tileViewerTextures[tileNum], tileSize, uv0, uv1);
        igPopID();
    }
    
    igEnd();
}

static void GUI_AppG2LayersWindow(bool * p_open)
{
    char bgName[4];
    ImVec2 btnSize = {50, 20};

    u8 bgEnabled[4];

    //Get the enabled BGs (main engine)
    bgEnabled[0] = ( s_reg_GX_DISPCNT >> 8 ) & 0x01;
    bgEnabled[1] = ( s_reg_GX_DISPCNT >> 8 ) & 0x02;
    bgEnabled[2] = ( s_reg_GX_DISPCNT >> 8 ) & 0x04;
    bgEnabled[3] = ( s_reg_GX_DISPCNT >> 8 ) & 0x08;

    igBegin("Layers", p_open, 0);

    igColumns(2, "layers", true);

    igText("Main Engine:");

    // BG Enable/Disable buttons
    for(int i=0; i < 4; i++) {
        igPushID_Int(i);

        if(s_SIM_DBG_BGenable[i]) {
            if(bgEnabled[i]){
                igPushStyleColor_U32(ImGuiCol_Button, 0xFFFF0000);
            } else {
                igPushStyleColor_U32(ImGuiCol_Button, 0xFF000000);
            }
        } else {
            igPushStyleColor_U32(ImGuiCol_Button, 0xFF0000FF);
        }

        snprintf(bgName, 4, "BG%d", i);
        if(igButton(bgName, btnSize)){
            if(s_SIM_DBG_BGenable[i]){
                s_SIM_DBG_BGenable[i] = 0;
            } else {
                s_SIM_DBG_BGenable[i] = 1;
            }
        }

        igPopID();
        igPopStyleColor(1);
    }

    // OAM Enable/Disable button
    igPushID_Int(4);
    if(s_SIM_DBG_OAMenable) {
        igPushStyleColor_U32(ImGuiCol_Button, 0xFFFF0000);
    } else {
        igPushStyleColor_U32(ImGuiCol_Button, 0xFF0000FF);
    }

    if(igButton("OAM", btnSize)){
        if(s_SIM_DBG_OAMenable){
            s_SIM_DBG_OAMenable = 0;
        } else {
            s_SIM_DBG_OAMenable = 1;
        }
    }
    igPopID();
    igPopStyleColor(1);

    igNextColumn();

    //Get the enabled BGs (sub engine)
    bgEnabled[0] = ( s_reg_GXS_DB_DISPCNT >> 8 ) & 0x01;
    bgEnabled[1] = ( s_reg_GXS_DB_DISPCNT >> 8 ) & 0x02;
    bgEnabled[2] = ( s_reg_GXS_DB_DISPCNT >> 8 ) & 0x04;
    bgEnabled[3] = ( s_reg_GXS_DB_DISPCNT >> 8 ) & 0x08;

    igText("Sub Engine:");

    // BG Enable/Disable buttons
    for(int i=0; i < 4; i++) {
        igPushID_Int(i + 5);

        if(s_SIM_DBG_BGSenable[i]) {
            if(bgEnabled[i]){
                igPushStyleColor_U32(ImGuiCol_Button, 0xFFFF0000);
            } else {
                igPushStyleColor_U32(ImGuiCol_Button, 0xFF000000);
            }
        } else {
            igPushStyleColor_U32(ImGuiCol_Button, 0xFF0000FF);
        }

        snprintf(bgName, 4, "BG%d", i);
        if(igButton(bgName, btnSize)){
            if(s_SIM_DBG_BGSenable[i]){
                s_SIM_DBG_BGSenable[i] = 0;
            } else {
                s_SIM_DBG_BGSenable[i] = 1;
            }
        }

        igPopID();
        igPopStyleColor(1);
    }

    // OAM Enable/Disable button
    igPushID_Int(9);
    if(s_SIM_DBG_OAMSenable) {
        igPushStyleColor_U32(ImGuiCol_Button, 0xFFFF0000);
    } else {
        igPushStyleColor_U32(ImGuiCol_Button, 0xFF0000FF);
    }

    if(igButton("OAM", btnSize)){
        if(s_SIM_DBG_OAMSenable){
            s_SIM_DBG_OAMSenable = 0;
        } else {
            s_SIM_DBG_OAMSenable = 1;
        }
    }
    igPopID();
    igPopStyleColor(1);

    igEnd();
    return;
}

void GUI_AppG2Main(bool * p_open)
{
    igBegin("G2", p_open, 0);
    if(igButton("Tile Viewer", s_btnSize)) {
        if(s_showWindowTileViewer){
            s_showWindowTileViewer = 0;
        } else {
            s_showWindowTileViewer = 1;
        }
    }

    if(igButton("Layers", s_btnSize)) {
        if(s_showWindowLayers){
            s_showWindowLayers = 0;
        } else {
            s_showWindowLayers = 1;
        }
    }

    if(s_showWindowTileViewer) {
        GUI_AppG2TileViewerWindow(&s_showWindowTileViewer);
    }

    if(s_showWindowLayers){
        GUI_AppG2LayersWindow(&s_showWindowLayers);
    }

    igEnd();
    return;
}

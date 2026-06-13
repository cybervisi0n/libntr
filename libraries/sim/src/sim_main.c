extern void NitroMain();
extern int NitroSpMain(void * arg);
#define SDL_MAIN_HANDLED
#define _POSIX_C_SOURCE 199309L

#include <pthread.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>

#include <simulator/glad/glad.h>
#include <simulator/sim.h>
#include <simulator/sim_snd.h>
#include <simulator/sim_audio.h>
#include <simulator/sim_net.h>

#include <stdio.h>
#include <stdlib.h>

#include <nitro/gx/g3.h>
#include <nitro/gx/gx_load.h>
#include <nitro/gx/gx_vramcnt.h>
#include <nitro/hw/X86/ioreg.h>
#include <nitro/hw/X86/mmap_shared.h>
#include <nitro/os.h>
#include <nitro/pxi/common/fifo.h>

#include <simulator/config/sim_config.h>
#include <simulator/drawmsg.h>
#include <simulator/queue.h>
#include <simulator/simvariables.h>
#include <simulator/g3_draw.h>
#include <simulator/g3_handler.h>
#include <simulator/gui.h>

#include <math.h>
#include <time.h>

#include "gui/gui_internal.h"
#include "screenquads.h"

#ifdef SDK_BUILD_WIN64
#include <windows.h>
#include <winsock2.h>
#endif

#ifdef SDK_BUILD_LINUX
#include <signal.h>
#endif

#include "sim_g2.h"

//Simulator configuration
SIM_config_type s_SIM_config = {};

//Thread
extern OSThreadInfo OSi_ThreadInfo;

static char s_SIM_game_name[13] = {}; // The name of the game from the ROM header (if present)

static u32 s_TouchPanelCoords[4] = {}; // The bounding box of the touch screen

//SND
#include <nitro/snd/common/work.h>
#include <nitro/snd/common/command.h>

#ifndef SDK_BUILD_LINUX
#define NANOTIME_IMPLEMENTATION
#include "nanotime.h"
#endif

#include <nitro/wm/ARM7/wm_sp.h>

//Graphics Debug Variables
u8 s_SIM_DBG_BGenable[4] = {1, 1, 1, 1};
u8 s_SIM_DBG_OAMenable = 1;
u8 s_SIM_DBG_BGSenable[4] = {1, 1, 1, 1};
u8 s_SIM_DBG_OAMSenable = 1;

u8 s_SIM_useWBuffer = 0;

static struct timespec s_SIM_lastFrameEnd;
u64 s_SIM_frameTime;

u8 bgtex[4*SIM_NDS_SCREEN_WIDTH*SIM_NDS_SCREEN_HEIGHT*2];
u8 bg0tex[4*SIM_NDS_SCREEN_WIDTH*SIM_NDS_SCREEN_HEIGHT*2];
u8 bg1tex[4*SIM_NDS_SCREEN_WIDTH*SIM_NDS_SCREEN_HEIGHT*2];
u8 bg2tex[4*SIM_NDS_SCREEN_WIDTH*SIM_NDS_SCREEN_HEIGHT*2];
u8 bg3tex[4*SIM_NDS_SCREEN_WIDTH*SIM_NDS_SCREEN_HEIGHT*2];
u8 bg0stex[4*SIM_NDS_SCREEN_WIDTH*SIM_NDS_SCREEN_HEIGHT*2];
u8 bg1stex[4*SIM_NDS_SCREEN_WIDTH*SIM_NDS_SCREEN_HEIGHT*2];
u8 bg2stex[4*SIM_NDS_SCREEN_WIDTH*SIM_NDS_SCREEN_HEIGHT*2];
u8 bg3stex[4*SIM_NDS_SCREEN_WIDTH*SIM_NDS_SCREEN_HEIGHT*2];

u8 oam0Tex[4*SIM_NDS_SCREEN_WIDTH*SIM_NDS_SCREEN_HEIGHT*2];
u8 oam1Tex[4*SIM_NDS_SCREEN_WIDTH*SIM_NDS_SCREEN_HEIGHT*2];
u8 oam2Tex[4*SIM_NDS_SCREEN_WIDTH*SIM_NDS_SCREEN_HEIGHT*2];
u8 oam3Tex[4*SIM_NDS_SCREEN_WIDTH*SIM_NDS_SCREEN_HEIGHT*2];
u8 oam0sTex[4*SIM_NDS_SCREEN_WIDTH*SIM_NDS_SCREEN_HEIGHT*2];
u8 oam1sTex[4*SIM_NDS_SCREEN_WIDTH*SIM_NDS_SCREEN_HEIGHT*2];
u8 oam2sTex[4*SIM_NDS_SCREEN_WIDTH*SIM_NDS_SCREEN_HEIGHT*2];
u8 oam3sTex[4*SIM_NDS_SCREEN_WIDTH*SIM_NDS_SCREEN_HEIGHT*2];
u8 objWindowTex[4*SIM_NDS_SCREEN_WIDTH*SIM_NDS_SCREEN_HEIGHT*2];
u8 objWindowsTex[4*SIM_NDS_SCREEN_WIDTH*SIM_NDS_SCREEN_HEIGHT*2];


u32 OBJLine[2][256];
u8 OBJWindow[2][256];

GXVRamTex s_SIM_GXVRamTex;
GXVRamTexPltt s_SIM_GXVRamTexPltt;
GXVRamBGExtPltt s_SIM_GXBgExtPltt;
GXVRamSubBGExtPltt s_SIM_GXSubBgExtPltt;

//4MB! can we reduce this?
u8 s_SIM_g3tex[4*1024*1024];

//FBOs
static GLuint g3FrameBuffer;
static GLuint fboId;
static GLuint g2FrameBuffer;

//Texture Objects
GLuint s_bgTextureId[4];
GLuint s_objTextureId[5];
static GLuint g2TextureId;
static GLuint g3TextureId1024;
static GLuint g3TextureId1024x512;
static GLuint g3TextureId512;
static GLuint g3TextureId256;
static GLuint g3TextureId128;
static GLuint g3TextureId128x64;
static GLuint g3TextureId128x256;
static GLuint g3TextureId64;
static GLuint g3TextureId64x16;
static GLuint g3TextureId64x32;
static GLuint g3TextureId64x128;
static GLuint g3TextureId32;
static GLuint g3TextureId32x16;
static GLuint g3TextureId32x64;
static GLuint g3TextureId32x128;
static GLuint g3TextureId16;
static GLuint g3TextureId16x8;
static GLuint g3TextureId16x32;
static GLuint g3TextureId16x64;
static GLuint g3TextureId8;
static GLuint g3TextureId8x16;
static GLuint g3RenderTextureId;
static GLuint dummyScreenTextureId;
static GLuint objWindowTextureId;

GLint s_bgTexUnits[] = {0, 1, 2, 3};
static GLint s_objTexUnits[] = {4, 5, 6, 7, 8};
static GLint s_objWindowTexUnit = 9;

//Shader Programs
GLuint g3shaderProgramID;
static GLuint g2shaderProgramID;

//VAOs
static GLuint g2vertexArray;

//VBOs
static GLuint g2vertexBuffer;

//RBOs
static GLuint g3RenderBufferId;

//Shaders
static GLuint g2VertexShader;
static GLuint g2FragmentShader;
static GLuint myVertexShader;
static GLuint myFragmentShader;

//GL Context
static SDL_GLContext context;

SDL_Window * window;

u8 s_SIM_dbg_dumpSound = 0;
static u8 s_SIM_dbg_soundFileOpen = 0;
static FILE * s_SIM_dbg_soundFile = NULL;
u8 s_SIM_dbg_soundSoloChannel = 0xFF;
void * win_snd_channel_bufs[32] = {0};
WIN_snd_channel_info win_snd_channels[32];
WIN_snd_channel_info win_sndi_channels[16] = {0};
WIN_snd_alarm_info win_snd_alarms[32] = {0};
SNDSharedWork * win_SNDi_SharedWork;

OSIrqFunction OS_IRQTable[OS_IRQ_TABLE_MAX] = {
    OS_IrqDummy,                       // VBlank (for ARM9)
    OS_IrqDummy,                       // HBlank
    OS_IrqDummy,                       // VCounter
    OS_IrqDummy,                     // timer0
    OS_IrqDummy,                     // timer1
    OS_IrqDummy,                     // timer2
    OS_IrqDummy,                     // timer3
    OS_IrqDummy,                       // serial communication (will not occur)
    OS_IrqDummy,                       // DMA0
    OS_IrqDummy,                       // DMA1
    OS_IrqDummy,                       // DMA2
    OS_IrqDummy,                       // DMA3
    OS_IrqDummy,                       // key
    OS_IrqDummy,                       // cartridge
    OS_IrqDummy,                       // (not used)
    OS_IrqDummy,                       // (not used)
    OS_IrqDummy,                       // sub processor
    OS_IrqDummy,                       // sub processor send FIFO empty
    OS_IrqDummy,                       // sub processor receive FIFO not empty
    OS_IrqDummy,                       // card data transfer finish
    OS_IrqDummy,                       // card IREQ
    OS_IrqDummy,                       // geometry command FIFO
    OS_IrqDummy,                       // Power Management IC
    OS_IrqDummy,                       // SPI data transfer
    OS_IrqDummy                        // Wireless module
};

u8 s_mosaicTable[16][256];

static u8 isMouseDown = 0;

//Local Procedure Declarations
static void ConvertMsgParams(draw_msg_t * msg);
static u8 GetNumParamsFromCommandType(draw_command_type_t type);
static u8 GetParamSizeFromCommandType(draw_command_type_t type);
static u8 isParamSigned(draw_command_type_t type);
static u64 map(u64 x, u64 in_min, u64 in_max, u64 out_min, u64 out_max);
static void process_nibble(unsigned char data4bit, int * Index, int * Pcm16bit);
static void DrawScreenQuad();
static void DrawEngine(BOOL isSub);

static u8 s_numG3DrawsThisFrame = 0;
static u8 s_numG3CommandsThisFrame = 0;



//Vcount stuff
static SDL_sem * vcountResetSemaphore;
static SDL_sem * vcountVblankSemaphore;

SDL_Joystick* s_SIM_SDLJoystick = NULL;



GLuint SIM_GetTextureID()
{
    if(s_texImageParam.textureSSize == 8 && s_texImageParam.textureTSize == 16) {
        return g3TextureId8x16;
    }
    if(s_texImageParam.textureSSize == 16) {
        switch(s_texImageParam.textureTSize) {
            case 8:
                return g3TextureId16x8;
            default:
            case 16:
                return g3TextureId16;
            case 32:
                return g3TextureId16x32;
            case 64:
                return g3TextureId16x64;
        }
    }
    if(s_texImageParam.textureSSize == 32) {
        switch(s_texImageParam.textureTSize) {
            case 16:
                return g3TextureId32x16;
            default:
            case 32:
                return g3TextureId32;
            case 64:
                return g3TextureId32x64;
            case 128:
                return g3TextureId32x128;
        }
    }
    if(s_texImageParam.textureSSize == 64 && s_texImageParam.textureTSize == 16) {
        return g3TextureId64x16;
    }
    if(s_texImageParam.textureSSize == 64 && s_texImageParam.textureTSize == 32) {
        return g3TextureId64x32;
    }
    if(s_texImageParam.textureSSize == 64 && s_texImageParam.textureTSize == 128) {
        return g3TextureId64x128;
    }
    if(s_texImageParam.textureSSize == 128 && s_texImageParam.textureTSize == 64) {
        return g3TextureId128x64;
    }
    if(s_texImageParam.textureSSize == 128 && s_texImageParam.textureTSize == 256) {
        return g3TextureId128x256;
    }
    if(s_texImageParam.textureSSize == 1024 && s_texImageParam.textureTSize == 512) {
        return g3TextureId1024x512;
    }
    switch( s_texImageParam.textureSSize )
    {
        case 8:
            return g3TextureId8;
        case 32:
            return g3TextureId32;
        case 64:
            return g3TextureId64;
        case 128:
            return g3TextureId128;
        case 256:
            return g3TextureId256;
        case 512:
            return g3TextureId512;
        case 1024:
            return g3TextureId1024;
        default:
            return 0;
    }
}

static void DrawScreenQuad()
{
    glBindVertexArray(g2vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, g2vertexBuffer);
    G3SIM_Vertex_t * quadArray = sim_GetScreenQuadArray(s_SIM_config.screenLayout, s_SIM_config.swapScreens);
    int arrayCount = sim_GetScreenQuadVertexCount(s_SIM_config.screenLayout, s_SIM_config.swapScreens);

    glBufferData(GL_ARRAY_BUFFER, sizeof(G3SIM_Vertex_t) * arrayCount, quadArray, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(G3SIM_Vertex_t), (void*)offsetof(G3SIM_Vertex_t, x));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(G3SIM_Vertex_t), (void*)offsetof(G3SIM_Vertex_t, s));
    
    glDrawArrays(GL_TRIANGLES,0,arrayCount);
}



SIM_config_type * SIM_GetConfigPtr()
{
    return &s_SIM_config;
}


static u64 map(u64 x, u64 in_min, u64 in_max, u64 out_min, u64 out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void SIM_u16ToRGB( u16 in, u8 * r, u8 *g, u8 * b )
{
    *r = in & 0b0000000000011111;
    *r = *r * 7;
    *g = (in >> 5) & 0b0000000000011111;
    *g = *g * 7;
    *b = (in >> 10) & 0b0000000000011111;
    *b = *b * 7;
}

static draw_command_type_t DecodeG3Op(u32 op)
{
    switch( op )
        {
        case G3OP_NOP:
            return DRAW_CMD_G3_NOP;
        case G3OP_MTX_MODE:
            return DRAW_CMD_G3_MTXMODE;
        case G3OP_MTX_PUSH:
            return DRAW_CMD_G3_PUSHMTX;
        case G3OP_MTX_POP:
            return DRAW_CMD_G3_POPMTX;
        case G3OP_MTX_STORE:
            return DRAW_CMD_G3_STOREMTX;
        case G3OP_MTX_RESTORE:
            return DRAW_CMD_G3_RESTOREMTX;
        case G3OP_MTX_IDENTITY:
            return DRAW_CMD_G3_IDENTITY;
        case G3OP_MTX_LOAD_4x4:
            return DRAW_CMD_G3_LOADMTX44;
        case G3OP_MTX_LOAD_4x3:
            return DRAW_CMD_G3_LOADMTX43;
        case G3OP_MTX_MULT_4x4:
            return DRAW_CMD_G3_MTXMULT44;
        case G3OP_MTX_MULT_4x3:
            return DRAW_CMD_G3_MTXMULT43;
        case G3OP_MTX_MULT_3x3:
            return DRAW_CMD_G3_MTXMULT33;
        case G3OP_MTX_SCALE:
            return DRAW_CMD_G3_SCALE;
        case G3OP_MTX_TRANS:
            return DRAW_CMD_G3_TRANSLATE;
        case G3OP_COLOR:
            return DRAW_CMD_G3_COLOR;
        case G3OP_NORMAL:
            return DRAW_CMD_G3_NORMAL;
        case G3OP_TEXCOORD:
            return DRAW_CMD_G3_TEXCOORD;
        case G3OP_VTX_16:
            return DRAW_CMD_G3_VTX16;
        case G3OP_VTX_10:
            return DRAW_CMD_G3_VTX10;
        case G3OP_VTX_XY:
            return DRAW_CMD_G3_VTXXY;
        case G3OP_VTX_XZ:
            return DRAW_CMD_G3_VTXXZ;
        case G3OP_VTX_YZ:
            return DRAW_CMD_G3_VTXYZ;
        case G3OP_VTX_DIFF:
            return DRAW_CMD_G3_VTXDIFF;
        case G3OP_POLYGON_ATTR:
            return DRAW_CMD_G3_POLYGONATTR;
        case G3OP_TEXIMAGE_PARAM:
            return DRAW_CMD_G3_TEXIMAGEPARAM;
        case G3OP_TEXPLTT_BASE:
            return DRAW_CMD_G3_TEXPLTTBASE;
        case G3OP_DIF_AMB:
            return DRAW_CMD_G3_DIFF_AMB;
        case G3OP_SPE_EMI:
            return DRAW_CMD_G3_SPEC_EMI;
        case G3OP_LIGHT_VECTOR:
            return DRAW_CMD_G3_LIGHTVECTOR;
        case G3OP_LIGHT_COLOR:
            return DRAW_CMD_G3_LIGHTCOLOR;
        case G3OP_SHININESS:
            return DRAW_CMD_G3_SHININESS;
        case G3OP_BEGIN:
            return DRAW_CMD_G3_BEGIN;
        case G3OP_END:
            return DRAW_CMD_G3_END;
        case G3OP_SWAP_BUFFERS:
            return DRAW_CMD_G3_SWAPBUFFERS;
        case G3OP_VIEWPORT:
            return DRAW_CMD_G3_VIEWPORT;
        case G3OP_BOX_TEST:
            return DRAW_CMD_G3_BOXTEST;
        case G3OP_POS_TEST:
            return DRAW_CMD_G3_POSITIONTEST;
        case G3OP_VEC_TEST:
            return DRAW_CMD_G3_VECTORTEST;
        case G3OP_DUMMY_COMMAND:
            return DRAW_CMD_G3_DUMMY;
        default:
            return DRAW_CMD_CNT;
        }
}

static void ConvertMsgParams(draw_msg_t * msg)
{
    draw_msg_t tmpMsg;
    memcpy(&tmpMsg, msg, sizeof( draw_msg_t ));
    switch(msg->type)
    {
        // TODO Commands
        case DRAW_CMD_G3_VIEWPORT:
            break;

        case DRAW_CMD_G3_BOXTEST:
            msg->data.boxtest.x = (s16)tmpMsg.data.numsS64[1];
            msg->data.boxtest.y = (s16)tmpMsg.data.numsS64[2];
            msg->data.boxtest.z = (s16)tmpMsg.data.numsS64[3];
            msg->data.boxtest.width = (s16)tmpMsg.data.numsS64[4];
            msg->data.boxtest.height = (s16)tmpMsg.data.numsS64[5];
            msg->data.boxtest.depth = (s16)tmpMsg.data.numsS64[6];
            break;
        case DRAW_CMD_G3_PUSHMTX:
        case DRAW_CMD_G3_END:
        case DRAW_CMD_G3_NOP:
            break;
        case DRAW_CMD_G3_MTXMODE:
        case DRAW_CMD_G3_POPMTX:
        case DRAW_CMD_G3_STOREMTX:
        case DRAW_CMD_G3_RESTOREMTX:
        case DRAW_CMD_G3_BEGIN:
            msg->data.numU8 = msg->data.numsU64[1];
            break;
        case DRAW_CMD_G3_COLOR:
            msg->data.numU16 = msg->data.numsU64[1];
            break;
        case DRAW_CMD_G3_VTX10:
        case DRAW_CMD_G3_VTXDIFF:
        case DRAW_CMD_G3_TEXIMAGEPARAM:
        case DRAW_CMD_G3_TEXPLTTBASE:
        case DRAW_CMD_G3_TEXCOORD:
        case DRAW_CMD_G3_POLYGONATTR:
        case DRAW_CMD_G3_LIGHTVECTOR:
        case DRAW_CMD_G3_DIFF_AMB:
        case DRAW_CMD_G3_NORMAL:
        case DRAW_CMD_G3_LIGHTCOLOR:
        case DRAW_CMD_G3_SPEC_EMI:
            msg->data.numU32 = msg->data.numsU64[1];
            break;
        case DRAW_CMD_G3_VTX:
        case DRAW_CMD_G3_VTX16:
            msg->data.xyz.x = (s16)tmpMsg.data.numsS64[1];
            msg->data.xyz.y = (s16)tmpMsg.data.numsS64[2];
            msg->data.xyz.z = (s16)tmpMsg.data.numsS64[3];
            break;
        case DRAW_CMD_G3_VTXXY:
            msg->data.xyz.x = (s16)tmpMsg.data.numsS64[1];
            msg->data.xyz.y = (s16)tmpMsg.data.numsS64[2];
            break;
        case DRAW_CMD_G3_VTXXZ:
            msg->data.xyz.x = (s16)tmpMsg.data.numsS64[1];
            msg->data.xyz.z = (s16)tmpMsg.data.numsS64[2];
            break;
        case DRAW_CMD_G3_VTXYZ:
            msg->data.xyz.y = (s16)tmpMsg.data.numsS64[1];
            msg->data.xyz.z = (s16)tmpMsg.data.numsS64[2];
            break;
        case DRAW_CMD_G3_SCALE:
        case DRAW_CMD_G3_TRANSLATE:
            msg->data.scale[0] = (s32)tmpMsg.data.numsS64[1];
            msg->data.scale[1] = (s32)tmpMsg.data.numsS64[2];
            msg->data.scale[2] = (s32)tmpMsg.data.numsS64[3];
            break;
        case DRAW_CMD_G3_MTXMULT33:
            msg->data.mtx33._00 = (s32)tmpMsg.data.numsS64[1];
            msg->data.mtx33._01 = (s32)tmpMsg.data.numsS64[2];
            msg->data.mtx33._02 = (s32)tmpMsg.data.numsS64[3];
            msg->data.mtx33._10 = (s32)tmpMsg.data.numsS64[4];
            msg->data.mtx33._11 = (s32)tmpMsg.data.numsS64[5];
            msg->data.mtx33._12 = (s32)tmpMsg.data.numsS64[6];
            msg->data.mtx33._20 = (s32)tmpMsg.data.numsS64[7];
            msg->data.mtx33._21 = (s32)tmpMsg.data.numsS64[8];
            msg->data.mtx33._22 = (s32)tmpMsg.data.numsS64[9];
            break;
        case DRAW_CMD_G3_LOADMTX43:
        case DRAW_CMD_G3_MTXMULT43:
            msg->data.mtx43._00 = (s32)tmpMsg.data.numsS64[1];
            msg->data.mtx43._01 = (s32)tmpMsg.data.numsS64[2];
            msg->data.mtx43._02 = (s32)tmpMsg.data.numsS64[3];
            msg->data.mtx43._10 = (s32)tmpMsg.data.numsS64[4];
            msg->data.mtx43._11 = (s32)tmpMsg.data.numsS64[5];
            msg->data.mtx43._12 = (s32)tmpMsg.data.numsS64[6];
            msg->data.mtx43._20 = (s32)tmpMsg.data.numsS64[7];
            msg->data.mtx43._21 = (s32)tmpMsg.data.numsS64[8];
            msg->data.mtx43._22 = (s32)tmpMsg.data.numsS64[9];
            msg->data.mtx43._30 = (s32)tmpMsg.data.numsS64[10];
            msg->data.mtx43._31 = (s32)tmpMsg.data.numsS64[11];
            msg->data.mtx43._32 = (s32)tmpMsg.data.numsS64[12];
            break;
        case DRAW_CMD_G3_LOADMTX44:
        case DRAW_CMD_G3_MTXMULT44:
            msg->data.mtx44._00 = (s32)tmpMsg.data.numsS64[1];
            msg->data.mtx44._01 = (s32)tmpMsg.data.numsS64[2];
            msg->data.mtx44._02 = (s32)tmpMsg.data.numsS64[3];
            msg->data.mtx44._03 = (s32)tmpMsg.data.numsS64[4];
            msg->data.mtx44._10 = (s32)tmpMsg.data.numsS64[5];
            msg->data.mtx44._11 = (s32)tmpMsg.data.numsS64[6];
            msg->data.mtx44._12 = (s32)tmpMsg.data.numsS64[7];
            msg->data.mtx44._13 = (s32)tmpMsg.data.numsS64[8];
            msg->data.mtx44._20 = (s32)tmpMsg.data.numsS64[9];
            msg->data.mtx44._21 = (s32)tmpMsg.data.numsS64[10];
            msg->data.mtx44._22 = (s32)tmpMsg.data.numsS64[11];
            msg->data.mtx44._23 = (s32)tmpMsg.data.numsS64[12];
            msg->data.mtx44._30 = (s32)tmpMsg.data.numsS64[13];
            msg->data.mtx44._31 = (s32)tmpMsg.data.numsS64[14];
            msg->data.mtx44._32 = (s32)tmpMsg.data.numsS64[15];
            msg->data.mtx44._33 = (s32)tmpMsg.data.numsS64[16];
            break;	
        default:
            break;
    }
}

static u8 GetNumParamsFromCommandType(draw_command_type_t type)
{
    switch(type)
    {
        case DRAW_CMD_G3_NOP:
        case DRAW_CMD_G3_END:
        case DRAW_CMD_G3_IDENTITY:
        case DRAW_CMD_G3_PUSHMTX:
        case DRAW_CMD_G3_DUMMY:
            return 0;
        case DRAW_CMD_G3_STOREMTX:
        case DRAW_CMD_G3_RESTOREMTX:
        case DRAW_CMD_G3_MTXMODE:
        case DRAW_CMD_G3_LIGHTVECTOR:
        case DRAW_CMD_G3_DIFF_AMB:
        case DRAW_CMD_G3_SPEC_EMI:
        case DRAW_CMD_G3_POLYGONATTR:
        case DRAW_CMD_G3_LIGHTCOLOR:
        case DRAW_CMD_G3_NORMAL:
        case DRAW_CMD_G3_VIEWPORT:
        case DRAW_CMD_G3_TEXIMAGEPARAM:
        case DRAW_CMD_G3_TEXPLTTBASE:
        case DRAW_CMD_G3_VTX10:
        case DRAW_CMD_G3_VTXDIFF:
        case DRAW_CMD_G3_BEGIN:
        case DRAW_CMD_G3_COLOR:
        case DRAW_CMD_G3_TEXCOORD:
        case DRAW_CMD_G3_SWAPBUFFERS:
            return 1;
        case DRAW_CMD_G3_VTXXY:
        case DRAW_CMD_G3_VTXXZ:
        case DRAW_CMD_G3_VTXYZ:
            return 2;
        case DRAW_CMD_G3_SCALE:
        case DRAW_CMD_G3_TRANSLATE:
            return 3;
        case DRAW_CMD_G3_VTX:
        case DRAW_CMD_G3_VTX16:
        case DRAW_CMD_G3_POPMTX:
            return 4;
        case DRAW_CMD_G3_BOXTEST:
            return 6;
        case DRAW_CMD_G3_MTXMULT33:
            return 9;
        case DRAW_CMD_G3_LOADMTX43:
        case DRAW_CMD_G3_MTXMULT43:
            return 12;
        case DRAW_CMD_G3_LOADMTX44:
        case DRAW_CMD_G3_MTXMULT44:
            return 16;
        default:
            return 0;
    }
}

static u8 GetParamSizeFromCommandType(draw_command_type_t type)
{
    switch(type)
    {
        case DRAW_CMD_G3_NOP:
        case DRAW_CMD_G3_IDENTITY:
        case DRAW_CMD_G3_PUSHMTX:
        case DRAW_CMD_G3_DUMMY:
            return 0;
        case DRAW_CMD_G3_POPMTX:
            return 1;
        case DRAW_CMD_G3_VTX:
        case DRAW_CMD_G3_VTX16:
        case DRAW_CMD_G3_VTXXY:
        case DRAW_CMD_G3_VTXXZ:
        case DRAW_CMD_G3_VTXYZ:
        case DRAW_CMD_G3_BOXTEST:
            return 2;
        case DRAW_CMD_G3_LOADMTX43:
        case DRAW_CMD_G3_LOADMTX44:
        case DRAW_CMD_G3_MTXMULT33:
        case DRAW_CMD_G3_MTXMODE:
        case DRAW_CMD_G3_LIGHTVECTOR:
        case DRAW_CMD_G3_DIFF_AMB:
        case DRAW_CMD_G3_SPEC_EMI:
        case DRAW_CMD_G3_POLYGONATTR:
        case DRAW_CMD_G3_MTXMULT44:
        case DRAW_CMD_G3_LIGHTCOLOR:
        case DRAW_CMD_G3_MTXMULT43:
        case DRAW_CMD_G3_NORMAL:
        case DRAW_CMD_G3_VIEWPORT:
        case DRAW_CMD_G3_SCALE:
        case DRAW_CMD_G3_TRANSLATE:
        case DRAW_CMD_G3_STOREMTX:
        case DRAW_CMD_G3_RESTOREMTX:
        case DRAW_CMD_G3_TEXIMAGEPARAM:
        case DRAW_CMD_G3_TEXPLTTBASE:
        case DRAW_CMD_G3_VTX10:
        case DRAW_CMD_G3_VTXDIFF:
        case DRAW_CMD_G3_BEGIN:
        case DRAW_CMD_G3_COLOR:
        case DRAW_CMD_G3_TEXCOORD:
        case DRAW_CMD_G3_SWAPBUFFERS:
            return 4;
        default:
            return 0;
    }
}

static u8 isParamSigned(draw_command_type_t type)
{
    switch(type)
    {
        case DRAW_CMD_G3_NOP:
        case DRAW_CMD_G3_BEGIN:
        case DRAW_CMD_G3_IDENTITY:
        case DRAW_CMD_G3_PUSHMTX:
        case DRAW_CMD_G3_MTXMODE:
        case DRAW_CMD_G3_LIGHTVECTOR:
        case DRAW_CMD_G3_DIFF_AMB:
        case DRAW_CMD_G3_SPEC_EMI:
        case DRAW_CMD_G3_POLYGONATTR:
        case DRAW_CMD_G3_LIGHTCOLOR:
        case DRAW_CMD_G3_DUMMY:
        case DRAW_CMD_G3_VIEWPORT:
        case DRAW_CMD_G3_STOREMTX:
        case DRAW_CMD_G3_RESTOREMTX:
        case DRAW_CMD_G3_TEXIMAGEPARAM:
        case DRAW_CMD_G3_TEXPLTTBASE:
        case DRAW_CMD_G3_VTX10:
        case DRAW_CMD_G3_VTXDIFF:
        case DRAW_CMD_G3_COLOR:
        case DRAW_CMD_G3_TEXCOORD:
        case DRAW_CMD_G3_SWAPBUFFERS:
            return 0;
        case DRAW_CMD_G3_VTX:
        case DRAW_CMD_G3_VTX16:
        case DRAW_CMD_G3_NORMAL:
        case DRAW_CMD_G3_LOADMTX43:
        case DRAW_CMD_G3_LOADMTX44:
        case DRAW_CMD_G3_MTXMULT44:
        case DRAW_CMD_G3_MTXMULT43:
        case DRAW_CMD_G3_SCALE:
        case DRAW_CMD_G3_TRANSLATE:
        case DRAW_CMD_G3_POPMTX:
        case DRAW_CMD_G3_VTXXY:
        case DRAW_CMD_G3_VTXXZ:
        case DRAW_CMD_G3_VTXYZ:
        case DRAW_CMD_G3_MTXMULT33:
        case DRAW_CMD_G3_BOXTEST:
            return 1;
        default:
            return 0;
    }
}

static xyz_s_t prevXYZ;

void SIM_HandleG3Command(draw_msg_t * msg)
{
    u32 temp;
    switch( msg->type )
        {
        case DRAW_CMD_G3_NOP:
        case DRAW_CMD_G3_DUMMY:
            break;
        case DRAW_CMD_G3_BEGIN:
            G3SIM_Begin(msg->data.numU8);
            break;
        case DRAW_CMD_G3_MTXMODE:
            G3SIM_MtxMode(msg->data.numU8);
            break;
        case DRAW_CMD_G3_IDENTITY:
            G3SIM_Identity();
            break;
        case DRAW_CMD_G3_PUSHMTX:
            G3SIM_MtxPush();
            break;
        case DRAW_CMD_G3_POPMTX:
            G3SIM_MtxPop(msg->data.numU8);
            break;
        case DRAW_CMD_G3_STOREMTX:
            G3SIM_MtxStore(msg->data.numU8);
            break;
        case DRAW_CMD_G3_RESTOREMTX:
            G3SIM_MtxRestore(msg->data.numU8);
            break;
        case DRAW_CMD_G3_LOADMTX44:
            G3SIM_MtxLoad44(&msg->data.mtx44);
            break;
        case DRAW_CMD_G3_LOADMTX43:
            G3SIM_MtxLoad43(&msg->data.mtx43);
            break;
        case DRAW_CMD_G3_MTXMULT33:
            G3SIM_MtxMult33(&msg->data.mtx33);
            break;
        case DRAW_CMD_G3_MTXMULT43:
            G3SIM_MtxMult43(&msg->data.mtx43);
            break;
        case DRAW_CMD_G3_MTXMULT44:
            G3SIM_MtxMult44(&msg->data.mtx44);
            break;
        case DRAW_CMD_G3_SCALE:
            G3SIM_MtxScale(msg->data.scale);
            break;
        case DRAW_CMD_G3_TRANSLATE:
            G3SIM_MtxTranslate(msg->data.scale);
            break;
        case DRAW_CMD_G3_COLOR:
            G3SIM_Color(msg->data.numU16);
            break;
        case DRAW_CMD_G3_TEXIMAGEPARAM:
            
            G3SIM_FlushArray();
            G3SIM_TexImageParam(msg->data.numU32);
            break;
        case DRAW_CMD_G3_VTXXY:
            msg->type = DRAW_CMD_G3_VTX16;
            msg->data.xyz.z = prevXYZ.z;
            SIM_HandleG3Command( msg );
            break;
        case DRAW_CMD_G3_VTXXZ:
            msg->type = DRAW_CMD_G3_VTX16;
            msg->data.xyz.y = prevXYZ.y;
            SIM_HandleG3Command( msg );
            break;
        case DRAW_CMD_G3_VTXYZ:
            msg->type = DRAW_CMD_G3_VTX16;
            msg->data.xyz.x = prevXYZ.x;
            SIM_HandleG3Command( msg );
            break;
        case DRAW_CMD_G3_VTXDIFF:
            //The conversion works a bit different from vtx10
            msg->type = DRAW_CMD_G3_VTX16;
            temp = msg->data.numU32;

            s16 x = (s16)((temp & 0x000003FF) << 6) >> 6;
            s16 y = (s16)((temp & 0x000FFC00) >> 4) >> 6;
            s16 z = (s16)((temp & 0x3FF00000) >> 14) >> 6;
            
            msg->data.xyz.x = prevXYZ.x + x;
            msg->data.xyz.y = prevXYZ.y + y;
            msg->data.xyz.z = prevXYZ.z + z;
            SIM_HandleG3Command( msg );
            break;
        case DRAW_CMD_G3_VTX10:
            msg->type = DRAW_CMD_G3_VTX16;
            //Convert to fx16
            temp = msg->data.numU32;
            msg->data.xyz.x = (temp & 0b1111111111) << 6;
            msg->data.xyz.y = ((temp >> 10) & 0b1111111111) << 6;
            msg->data.xyz.z = ((temp >> 20) & 0b1111111111) << 6;
            SIM_HandleG3Command( msg );
            break;
        case DRAW_CMD_G3_VTX16:
            prevXYZ.x = msg->data.xyz.x;
            prevXYZ.y = msg->data.xyz.y;
            prevXYZ.z = msg->data.xyz.z;
            G3SIM_Vtx(msg->data.xyz.x, msg->data.xyz.y, msg->data.xyz.z);
            if( s_G3numInPoly == 0 || s_G3drawPoly == 1)
            {
                    s_G3drawPoly = 0;
                    GLfloat myVerts[(3*6) + (2*6)];
                    if( s_primType == GX_BEGIN_TRIANGLES || s_primType == GX_BEGIN_TRIANGLE_STRIP )
                    {
                        if( s_primType == GX_BEGIN_TRIANGLE_STRIP && !(s_G3triStripTriNum & 1) )
                        {
                            //Swap the last 2 verts
                            G3SIM_FxVtx_t temp;
                            memcpy(&temp, &s_g3PolygonVerts[1], sizeof(G3SIM_FxVtx_t));
                            memcpy(&s_g3PolygonVerts[1], &s_g3PolygonVerts[2], sizeof(G3SIM_FxVtx_t));
                            memcpy(&s_g3PolygonVerts[2], &temp, sizeof(G3SIM_FxVtx_t));
                        }
                        
                        G3SIM_SubmitPolygon(s_g3PolygonVerts, 3);
                        
                        if( s_primType == GX_BEGIN_TRIANGLE_STRIP && !(s_G3triStripTriNum & 1) )
                        {
                            //Swap the last 2 verts
                            G3SIM_FxVtx_t temp;
                        
                            memcpy(&temp, &s_g3PolygonVerts[1], sizeof(G3SIM_FxVtx_t));
                            memcpy(&s_g3PolygonVerts[1], &s_g3PolygonVerts[2], sizeof(G3SIM_FxVtx_t));
                            memcpy(&s_g3PolygonVerts[2], &temp, sizeof(G3SIM_FxVtx_t));
                        }
                        
                        if( s_primType == GX_BEGIN_TRIANGLE_STRIP )
                        {
                            //Copy the last two verts from the last polygon
                            memcpy(&s_g3PolygonVerts[0], &s_g3PolygonVerts[1], sizeof(G3SIM_FxVtx_t));
                            memcpy(&s_g3PolygonVerts[1], &s_g3PolygonVerts[2], sizeof(G3SIM_FxVtx_t));
                        }

                        s_numG3DrawsThisFrame++;
                    }
                    if( s_primType == GX_BEGIN_QUADS || s_primType == GX_BEGIN_QUAD_STRIP )
                    {
                        if( s_primType == GX_BEGIN_QUAD_STRIP )
                        {
                            G3SIM_FxVtx_t temp;
                            memcpy(&temp, &s_g3PolygonVerts[2], sizeof(G3SIM_FxVtx_t));
                            memcpy(&s_g3PolygonVerts[2], &s_g3PolygonVerts[3], sizeof(G3SIM_FxVtx_t));
                            memcpy(&s_g3PolygonVerts[3], &temp, sizeof(G3SIM_FxVtx_t));
                        }

                        G3SIM_SubmitPolygon(s_g3PolygonVerts, 4);

                        s_numG3DrawsThisFrame++;

                        if( s_primType == GX_BEGIN_QUAD_STRIP )
                        {
                            //Copy the last two verts from the last polygon
                            s_g3PolygonVerts[0].x = s_g3PolygonVerts[3].x;
                            s_g3PolygonVerts[0].y = s_g3PolygonVerts[3].y;
                            s_g3PolygonVerts[0].z = s_g3PolygonVerts[3].z;
                            s_g3PolygonVerts[0].w = s_g3PolygonVerts[3].w;

                            s_g3PolygonVerts[1].x = s_g3PolygonVerts[2].x;
                            s_g3PolygonVerts[1].y = s_g3PolygonVerts[2].y;
                            s_g3PolygonVerts[1].z = s_g3PolygonVerts[2].z;
                            s_g3PolygonVerts[1].w = s_g3PolygonVerts[2].w;

                            memcpy(&s_g3PolygonVerts[0], &s_g3PolygonVerts[3], sizeof(G3SIM_FxVtx_t));
                            memcpy(&s_g3PolygonVerts[1], &s_g3PolygonVerts[2], sizeof(G3SIM_FxVtx_t));
                        }
                    }
                    
            }
            break;
        case DRAW_CMD_G3_LIGHTCOLOR:
            G3SIM_LightColor(msg->data.numU32);
            break;
        case DRAW_CMD_G3_LIGHTVECTOR:
            G3SIM_LightVector(msg->data.numU32);
            break;
        case DRAW_CMD_G3_NORMAL:
            G3SIM_Normal(msg->data.numU32);
            break;
        case DRAW_CMD_G3_TEXCOORD:
            G3SIM_TexCoord(msg->data.numU32);
            break;
        case DRAW_CMD_G3_POLYGONATTR:
            G3SIM_PolygonAttr(msg->data.numU32);
            break;
        case DRAW_CMD_G3_DIFF_AMB:
            G3SIM_DiffAmb(msg->data.numU32);
            break;
        case DRAW_CMD_G3_SPEC_EMI:
            G3SIM_SpecEmi(msg->data.numU32);
            break;
        case DRAW_CMD_G3_DIRECT0:
            msg->type = DecodeG3Op(msg->data.numU32);
            SIM_HandleG3Command( msg );
            break;
        case DRAW_CMD_G3_DIRECT1:
            msg->type = DecodeG3Op(msg->data.numsU64[0]);
            //Get the parameter in the right place
            ConvertMsgParams( msg );
            SIM_HandleG3Command( msg );
            break;
        case DRAW_CMD_G3_SWAPBUFFERS:
            {
                u8 depthBufferMode = (msg->data.numU32 & 2) >> 1;
                G3SIM_SwapBuffers(depthBufferMode); //TODO: add flush attributes here for Z/W-buffering!
            }
            break;
        case DRAW_CMD_G3_VIEWPORT:
            //TODO
            break;
        case DRAW_CMD_G3_TEXPLTTBASE:
            G3SIM_FlushArray();
            G3SIM_TexPlttBase(msg->data.numU32);
            break;
        case DRAW_CMD_G3_BOXTEST:
            G3SIM_BoxTest(msg->data.boxtest.x,
                          msg->data.boxtest.y,
                          msg->data.boxtest.z,
                          msg->data.boxtest.width,
                          msg->data.boxtest.height,
                          msg->data.boxtest.depth);
            break;
        case DRAW_CMD_G3_CMD_LIST:
            //Process a command List
            {
                G3SIM_CommandBlock_t * cmdBlockPtr;
                draw_msg_t tempMsg;
                cmdBlockPtr = msg->data.ptr;
                u64 curOffset;
                curOffset = 0;
                u8 numParams;
                void * paramPtr;
                u8 paramSize;
                u8 paramSigned;
                while(curOffset<msg->size)
                {
                    paramPtr = (void*)cmdBlockPtr + 4;
                    curOffset += 4;
                    for(int i=0; i < 4; i++)
                    {
                        tempMsg.type = DecodeG3Op(cmdBlockPtr->ops[i]);
                        numParams = GetNumParamsFromCommandType(tempMsg.type);
                        paramSize = GetParamSizeFromCommandType(tempMsg.type);
                        paramSigned = isParamSigned(tempMsg.type);
                        for(int j=1; j < numParams+1; j++)
                        {
                            switch( paramSize )
                            {
                                case 0:
                                    break;
                                case 1:
                                    if( paramSigned )
                                    {
                                        tempMsg.data.numsS64[j] = (s64)*(s8*)paramPtr;
                                    }
                                    else
                                    {
                                        tempMsg.data.numsU64[j] = *(u8*)paramPtr;
                                    }
                                    break;
                                case 2:
                                    if( paramSigned )
                                    {
                                        tempMsg.data.numsS64[j] = (s64)*(s16*)paramPtr;
                                    }
                                    else
                                    {
                                        tempMsg.data.numsU64[j] = *(u16*)paramPtr;
                                    }
                                    break;
                                case 4:
                                    if( paramSigned )
                                    {
                                        tempMsg.data.numsS64[j] = (s64)*(s32*)paramPtr;
                                    }
                                    else
                                    {
                                        tempMsg.data.numsU64[j] = *(u32*)paramPtr;
                                    }
                                    break;
                                case 8:
                                    if( paramSigned )
                                    {
                                        tempMsg.data.numsS64[j] = *(s64*)paramPtr;
                                    }
                                    else
                                    {
                                        tempMsg.data.numsU64[j] = *(u64*)paramPtr;
                                    }
                                    break;
                                default:
                            }
                            paramPtr += paramSize;
                            curOffset += paramSize;
                        }
                        
                        ConvertMsgParams( &tempMsg );
                        SIM_HandleG3Command(&tempMsg);
                    }
                    cmdBlockPtr = (void *)paramPtr;
                }
            }
            free(msg->data.ptr);
            //TODO
            break;
        default:
            //printf( "Draw command %d not implemented\n", msg->type );
        }
}


void * runVblankInterrupt(void * arg){
    OS_IRQTable[OS_IE_V_BLANK-1]();
    return NULL;
}

void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
  //fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
  //         ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
  //          type, severity, message );
}

void * SIM_RenderInit(void * arg){

    SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK );
    SIM_Audio_Init(44100);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetSwapInterval(s_SIM_config.vsyncInterval);

    u32 windowHeight;
    u32 windowWidth;

    if(s_SIM_config.windowWidth == 0 || s_SIM_config.windowHeight == 0) {
        switch(s_SIM_config.screenLayout) {
            default:
            case SIM_CONFIG_SCREEN_LAYOUT_VERTICAL:
                windowHeight = SIM_NDS_SCREEN_HEIGHT*2*2;
                windowWidth = SIM_NDS_SCREEN_WIDTH*2;
                break;
            case SIM_CONFIG_SCREEN_LAYOUT_HORIZONTAL:
                windowHeight = SIM_NDS_SCREEN_HEIGHT*2;
                windowWidth = SIM_NDS_SCREEN_WIDTH*2*2;
                break;
            case SIM_CONFIG_SCREEN_LAYOUT_LARGE:
                windowHeight = SIM_NDS_SCREEN_HEIGHT*2*2;
                windowWidth = (SIM_NDS_SCREEN_WIDTH*2*2) + (SIM_NDS_SCREEN_WIDTH * 2);
                break;
        }
    } else {
        windowHeight = s_SIM_config.windowHeight;
        windowWidth = s_SIM_config.windowWidth;
    }

    char * windowTitle = "NitroSDK Application";

    if(strlen(s_SIM_game_name) > 0) {
        windowTitle = s_SIM_game_name;
    }

    window = SDL_CreateWindow( windowTitle, 100, 100, windowWidth, windowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE );
    context = SDL_GL_CreateContext(window);

    gladLoadGLLoader(SDL_GL_GetProcAddress);

    glViewport(0, 0, windowWidth, windowHeight);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f );
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

    glEnable( GL_DEBUG_OUTPUT );
    glDebugMessageCallback( MessageCallback, 0 );

    GLenum glErrCode;
    GLuint status;
    GLint logSize = 0;

    GLchar * errorBuf;

    glGenTextures(4, s_bgTextureId);
    glActiveTexture(GL_TEXTURE0);

    for(int i=0; i < 4; i++) {
        glActiveTexture(GL_TEXTURE0 + s_bgTexUnits[i]);
        glBindTexture( GL_TEXTURE_2D, s_bgTextureId[i] );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, SIM_NDS_SCREEN_WIDTH, SIM_NDS_SCREEN_HEIGHT*2, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }

    glGenTextures(5, s_objTextureId);
    glActiveTexture(GL_TEXTURE0);

    for(int i=0; i < 5; i++) {
        glActiveTexture(GL_TEXTURE0 + s_objTexUnits[i]);
        glBindTexture( GL_TEXTURE_2D, s_objTextureId[i] );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, SIM_NDS_SCREEN_WIDTH, SIM_NDS_SCREEN_HEIGHT*2, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }

#define GenerateTexture(_id,_width,_height) \
  glGenTextures(1, &_id); \
  glActiveTexture(GL_TEXTURE0); \
  glBindTexture(GL_TEXTURE_2D, _id); \
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); \
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); \
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); \
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); \
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

#define CompileShaderCommon(_id,_source) \
    glShaderSource( _id, 1, &_source, NULL ); \
    glCompileShader( _id ); \
    glGetShaderiv(_id, GL_COMPILE_STATUS, &status); \
    if( status != GL_TRUE ) \
    { \
        glGetShaderiv(g2FragmentShader, GL_INFO_LOG_LENGTH, &logSize); \
        errorBuf = malloc( logSize * sizeof( GLchar ) ); \
        glGetShaderInfoLog(_id, logSize, &logSize, (void *)errorBuf); \
        printf( "ERROR COMPILING _source SHADER:\n%s\n", errorBuf ); \
        free( errorBuf ); \
    }

#define CompileVertexShader(_id,_source) \
    _id = glCreateShader(GL_VERTEX_SHADER); \
    CompileShaderCommon(_id,_source);

#define CompileFragmentShader(_id,_source) \
    _id = glCreateShader(GL_FRAGMENT_SHADER); \
    CompileShaderCommon(_id,_source);
    
#define LinkShader(_id,_vertex,_fragment) \
    glAttachShader( _id, _vertex ); \
    glAttachShader( _id, _fragment ); \
    glLinkProgram( _id ); \
    glErrCode = glGetError(); \
    glGetProgramiv( _id, GL_LINK_STATUS, &status ); \
    if( status != GL_TRUE ) \
    { \
        glGetProgramiv(_id, GL_INFO_LOG_LENGTH, &logSize); \
        errorBuf = malloc( logSize * sizeof( GLchar ) ); \
        glGetProgramInfoLog(_id, logSize, &logSize, (void *)errorBuf); \
        glErrCode = glGetError(); \
        printf( "ERROR LINKING _id SHADER:\n%s\n", errorBuf ); \
        free( errorBuf ); \
    } \
    glValidateProgram(_id); \
    glUseProgram(_id);
    
    
    GenerateTexture(dummyScreenTextureId, SIM_NDS_SCREEN_WIDTH, SIM_NDS_SCREEN_HEIGHT*2);
    GenerateTexture(objWindowTextureId, SIM_NDS_SCREEN_WIDTH, SIM_NDS_SCREEN_HEIGHT*2);
    GenerateTexture(g3TextureId1024, 1024, 1024);
    GenerateTexture(g3TextureId1024x512, 1024, 512);
    GenerateTexture(g3TextureId512, 512, 512);
    GenerateTexture(g3TextureId256, 256, 256);
    GenerateTexture(g3TextureId128, 128, 128);
    GenerateTexture(g3TextureId128x64, 128, 64);
    GenerateTexture(g3TextureId128x256, 128, 256);
    GenerateTexture(g3TextureId64, 64, 64);
    GenerateTexture(g3TextureId64x16, 64, 16);
    GenerateTexture(g3TextureId64x32, 64, 32);
    GenerateTexture(g3TextureId64x128, 64, 128);
    GenerateTexture(g3TextureId32, 32, 32);
    GenerateTexture(g3TextureId32x16, 32, 16);
    GenerateTexture(g3TextureId32x64, 32, 64);
    GenerateTexture(g3TextureId32x128, 32, 128);
    GenerateTexture(g3TextureId16, 16, 16);
    GenerateTexture(g3TextureId16x8, 16, 8);
    GenerateTexture(g3TextureId16x32, 16, 32);
    GenerateTexture(g3TextureId16x64, 16, 64);
    GenerateTexture(g3TextureId8, 8, 8);
    GenerateTexture(g3TextureId8x16, 8, 16);

    //3D Framebuffer render texture
    glGenFramebuffers(1, &g3FrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, g3FrameBuffer);
    glGenTextures(1, &g3RenderTextureId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture( GL_TEXTURE_2D, g3RenderTextureId );
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, SIM_NDS_SCREEN_WIDTH * s_SIM_config.internalResolutionScale, SIM_NDS_SCREEN_HEIGHT*2*s_SIM_config.internalResolutionScale, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g3RenderTextureId, 0);
    glGenRenderbuffers(1, &g3RenderBufferId);
    glBindRenderbuffer(GL_RENDERBUFFER, g3RenderBufferId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SIM_NDS_SCREEN_WIDTH*s_SIM_config.internalResolutionScale, SIM_NDS_SCREEN_HEIGHT*2*s_SIM_config.internalResolutionScale);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, g3RenderBufferId);

    //Check FBO status
    int fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if( fboStatus != GL_FRAMEBUFFER_COMPLETE )
    {
        printf("Framebuffer error: %d\n", fboStatus);
    }

    //Unbind all that stuff
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glBindTexture( GL_TEXTURE_2D, g2TextureId );

    glGenFramebuffers(1, &g2FrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //Setup G3 Shader Program
    g3shaderProgramID = glCreateProgram();
    CompileVertexShader(myVertexShader, G3SIM_VertexShader);
    CompileFragmentShader(myFragmentShader, G3SIM_FragmentShader);
    LinkShader(g3shaderProgramID, myVertexShader, myFragmentShader);
    
    //Setup G2 Shader Program
    g2shaderProgramID = glCreateProgram();
    CompileVertexShader(g2VertexShader, G2SIM_VertexShader);
    CompileFragmentShader(g2FragmentShader, G2SIM_FragmentShader);
    LinkShader(g2shaderProgramID, g2VertexShader, g2FragmentShader);

    GLint texUnitLoc = glGetUniformLocation(g2shaderProgramID, "myTexture");
    glProgramUniform1i(g2shaderProgramID, texUnitLoc , 0);

    // Set up the texture units in the shader
    glUniform1iv(glGetUniformLocation(g2shaderProgramID, "bgTexture"), 4, s_bgTexUnits);
    glUniform1iv(glGetUniformLocation(g2shaderProgramID, "objTexture"), 5, s_objTexUnits);

    glGenVertexArrays(1, &g2vertexArray);
    glBindVertexArray(g2vertexArray);
    glGenBuffers(1, &g2vertexBuffer);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    SDL_GL_SwapWindow(window);

    //Setup ImGui TODO bring in imgui
    //SIM_GUI_Init(window, context);

    clock_gettime(CLOCK_MONOTONIC, &s_SIM_lastFrameEnd);
}


static void DrawEngine(BOOL isSub) {
    u8 bg0as3d;
    u8 maxPriority;
    u8 maxPriorityBG;
    u8 bgEnabled[4];
    u8 bgMode;
    GLuint bgOrder[4];
    GLuint bgPriorities[4];
    
    u32 dispSel = s_reg_GX_POWCNT >> 15;

    //Get the enabled BGs
    u32 dispcnt = isSub ? s_reg_GXS_DB_DISPCNT : s_reg_GX_DISPCNT;

    bgEnabled[0] = ( dispcnt >> 8 ) & 0x01;
    bg0as3d = ( dispcnt >> 3 ) & 0x01;
    bgEnabled[1] = ( dispcnt >> 8 ) & 0x02;
    bgEnabled[2] = ( dispcnt >> 8 ) & 0x04;
    bgEnabled[3] = ( dispcnt >> 8 ) & 0x08;

    //Get the graphics mode
    bgMode = (u8)(dispcnt & 0x7);

    //Get the backdrop color
    u8 backdropR;
    u8 backdropG;
    u8 backdropB;
    u16 backdropColor;
    backdropColor = isSub ? *(u16*)&s_HW_DB_BG_PLTT[0] : *(u16*)&s_HW_BG_PLTT[0];
    SIM_u16ToRGB(backdropColor, &backdropR, &backdropG, &backdropB);
    float backdropVec[4];

    backdropVec[0] = (float)backdropR / 255.0f;
    backdropVec[1] = (float)backdropG / 255.0f;
    backdropVec[2] = (float)backdropB / 255.0f;
    backdropVec[3] = 1.0f;

    glUniform1fv(glGetUniformLocation(g2shaderProgramID, "backdropColor"), 4, backdropVec);

    glProgramUniform4f(g2shaderProgramID, glGetUniformLocation(g2shaderProgramID, "backdropColor"), 
                       (float)backdropR / 255.0f, (float)backdropG / 255.0f, (float)backdropB / 255.0f, 1.0f);

    //Get the BG priorities
    u16 bg0cnt = isSub ? s_reg_G2S_DB_BG0CNT : s_reg_G2_BG0CNT;
    u16 bg1cnt = isSub ? s_reg_G2S_DB_BG1CNT : s_reg_G2_BG1CNT;
    u16 bg2cnt = isSub ? s_reg_G2S_DB_BG2CNT : s_reg_G2_BG2CNT;
    u16 bg3cnt = isSub ? s_reg_G2S_DB_BG3CNT : s_reg_G2_BG3CNT;

    bgPriorities[0] = bg0cnt & (u16)3;
    bgPriorities[1] = bg1cnt & (u16)3;
    bgPriorities[2] = bg2cnt & (u16)3;
    bgPriorities[3] = bg3cnt & (u16)3;

    // Set up the BGPriorities in the shader
    glUniform1iv(glGetUniformLocation(g2shaderProgramID, "bgPriorities"), 4, bgPriorities);

    //Determine the order to draw the BGs
    bgOrder[0] = 0;
    bgOrder[1] = 1;
    bgOrder[2] = 2;
    bgOrder[3] = 3;

    for( int i=0; i<4; i++ )
    {
        maxPriorityBG = 0;
        maxPriority = 0;
        for( int j =0; j<4; j++)
        {
            if(bgPriorities[j] == 0xFF)
            {
            continue;
            }
            if(bgPriorities[j] >= maxPriority)
            {
            maxPriorityBG = j;
            maxPriority = bgPriorities[j];
            }
        }
        bgOrder[i] = maxPriorityBG;
        bgPriorities[maxPriorityBG] = 0xFF;
    }

    // Set up the BGOrder in the shader
    glUniform1iv(glGetUniformLocation(g2shaderProgramID, "bgOrder"), 4, bgOrder);

    //Clear out the textures
    for(int i=0; i < 4; i++) {
        glActiveTexture(GL_TEXTURE0 + s_bgTexUnits[0]);
        glBindTexture(GL_TEXTURE_2D, dummyScreenTextureId);
    }

    for(int i=0; i < 5; i++) {
        glActiveTexture(GL_TEXTURE0 + s_objTexUnits[0]);
        glBindTexture(GL_TEXTURE_2D, dummyScreenTextureId);
    }

    //Draw the Sprites
    if(isSub) {
        memset( oam0sTex, 0, sizeof( oam0sTex ));
        memset( oam1sTex, 0, sizeof( oam1sTex ));
        memset( oam2sTex, 0, sizeof( oam2sTex ));
        memset( oam3sTex, 0, sizeof( oam3sTex ));
        memset( objWindowsTex, 0, sizeof( objWindowsTex ));
    } else {
        memset( oam0Tex, 0, sizeof( oam0Tex ));
        memset( oam1Tex, 0, sizeof( oam1Tex ));
        memset( oam2Tex, 0, sizeof( oam2Tex ));
        memset( oam3Tex, 0, sizeof( oam3Tex ));
        memset( objWindowTex, 0, sizeof( objWindowTex ));
    }

    for(int i=0; i < SIM_NDS_SCREEN_HEIGHT; i++)
    {
        for( int bgnum = 0x0C00; bgnum >= 0x0000; bgnum -= 0x0400 ){
            memset(OBJLine, 0, sizeof(OBJLine));
            memset(OBJWindow, 0, sizeof(OBJWindow));
            G2SIM_DrawOBJ(i, isSub, bgnum);
            u8 * pixelBuf;
            u8 * objWindowBuf = isSub ? objWindowsTex : objWindowTex;
            switch(bgnum){
                case 0xC00:
                    pixelBuf = isSub ? oam3sTex : oam3Tex;
                    break;
                case 0x800:
                    pixelBuf = isSub ? oam2sTex : oam2Tex;
                    break;
                case 0x400:
                    pixelBuf = isSub ? oam1sTex : oam1Tex;
                    break;
                case 0x000:
                    pixelBuf = isSub ? oam0sTex : oam0Tex;
                    break;
                default:
                    //Invalid case
                    pixelBuf = NULL;
                    break;
            }
            u8 r;
            u8 g;
            u8 b;
            u8 a;
            for( int j=0; j < 256; j++ )
            {
                if( OBJLine[isSub][j])
                {
                    u8 colorIdx = OBJLine[isSub][j] & 0xFF;
                    if( colorIdx )
                    {
                        u16 * objPltt = isSub ? (u16*)s_HW_DB_OBJ_PLTT : (u16*)s_HW_OBJ_PLTT;
                        u16 color = objPltt[colorIdx];
                        SIM_u16ToRGB( color, &r, &g, &b );
                        G2SIM_PlotPixel(pixelBuf, r, g, b, j, i, isSub);
                    }
                }
                if(OBJWindow[isSub][j])
                {
                    G2SIM_PlotPixel(objWindowBuf, 255, 255, 255, j, i, isSub);
                }
            }
        }
    }

    int drawnOAMs = 0;

    //Draw the BGs
    for( int i=0; i<4; i++)
    {
        u8 curBgNum = bgOrder[i];

        if( bgEnabled[ bgOrder[ i ] ] )
        {
            G2SIM_DrawBG(bgOrder[i], bgMode, bg0as3d, isSub);
        } else {
            glActiveTexture(GL_TEXTURE0 + s_bgTexUnits[bgOrder[i]]);
            glBindTexture( GL_TEXTURE_2D, dummyScreenTextureId);
        }
    }


    //write all OAMs to the textures based on their priorities
    for(int i=0; i < 4; i++) {
        glActiveTexture(GL_TEXTURE0 + s_objTexUnits[i]);
        glBindTexture( GL_TEXTURE_2D, s_objTextureId[i]);
        void * oamPixelBuf;
        switch(i){
            case 0:
                oamPixelBuf = isSub ? oam0sTex : oam0Tex;
                break;
            case 1:
                oamPixelBuf = isSub ? oam1sTex : oam1Tex;
                break;
            case 2:
                oamPixelBuf = isSub ? oam2sTex : oam2Tex;
                break;
            case 3:
                oamPixelBuf = isSub ? oam3sTex : oam3Tex;
                break;
            default:
                //invalid
                oamPixelBuf = NULL;
                break;
        }
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SIM_NDS_SCREEN_WIDTH, SIM_NDS_SCREEN_HEIGHT*2, GL_RGBA, GL_UNSIGNED_BYTE, (void *)oamPixelBuf);
    }

    glActiveTexture(GL_TEXTURE0 + s_bgTexUnits[0]);

    if(bgEnabled[0]) {
        if(bg0as3d) {
            glBindTexture( GL_TEXTURE_2D, g3RenderTextureId );
        } else {
            glBindTexture( GL_TEXTURE_2D, s_bgTextureId[0] );
        }
    } else {
        glBindTexture( GL_TEXTURE_2D, dummyScreenTextureId );
    }

    //Setup the OBJWindow texture
    glActiveTexture(GL_TEXTURE0 + s_objWindowTexUnit);
    glBindTexture( GL_TEXTURE_2D, objWindowTextureId );
    u8 * objWindowBuf = isSub ? objWindowsTex : objWindowTex;
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SIM_NDS_SCREEN_WIDTH, SIM_NDS_SCREEN_HEIGHT*2, GL_RGBA, GL_UNSIGNED_BYTE, (void *)objWindowBuf);
    glProgramUniform1i(g2shaderProgramID, glGetUniformLocation(g2shaderProgramID, "objWindowTexture") , s_objWindowTexUnit);

    //Pass BLDCNT, BLDALPHA, and BLDY value to the shader
    u16 bldcnt = isSub ? s_reg_G2S_DB_BLDCNT : s_reg_G2_BLDCNT;
    u16 bldalpha = isSub ? s_reg_G2S_DB_BLDALPHA : s_reg_G2_BLDALPHA;
    u16 bldy = isSub ? s_reg_G2S_DB_BLDY : s_reg_G2_BLDY;
    glProgramUniform1i(g2shaderProgramID, glGetUniformLocation(g2shaderProgramID, "bldcnt") , bldcnt);
    glProgramUniform1i(g2shaderProgramID, glGetUniformLocation(g2shaderProgramID, "bldalpha") , bldalpha);
    glProgramUniform1i(g2shaderProgramID, glGetUniformLocation(g2shaderProgramID, "bldy") , bldy);

    int topScreen = dispSel ? !isSub : isSub;
    glProgramUniform1i(g2shaderProgramID, glGetUniformLocation(g2shaderProgramID, "topScreen") , topScreen);

    //Pass DISPCNT and WINOUT to the shader
    glProgramUniform1i(g2shaderProgramID, glGetUniformLocation(g2shaderProgramID, "dispcnt") , dispcnt);
    u16 winout = isSub ? s_reg_G2S_DB_WINOUT : s_reg_G2_WINOUT;
    glProgramUniform1i(g2shaderProgramID, glGetUniformLocation(g2shaderProgramID, "winout") , winout);


    //Pass bg0as3d to the shader
    glProgramUniform1i(g2shaderProgramID, glGetUniformLocation(g2shaderProgramID, "bg0as3d") , bg0as3d);

    //Pass master bright info to the shader
    float masterBrightFactor = isSub ? (float)(s_reg_GXS_DB_MASTER_BRIGHT & 0b11111) : (float)(s_reg_GX_MASTER_BRIGHT & 0b11111);
    if(masterBrightFactor > 16.0f) {
        masterBrightFactor = 16.0f;
    }
    glProgramUniform1f(g2shaderProgramID, glGetUniformLocation(g2shaderProgramID, "inMasterBrightFactor") , masterBrightFactor);
    int masterBrightMode = isSub ? ((s_reg_GXS_DB_MASTER_BRIGHT >> 14) & 3) : ((s_reg_GX_MASTER_BRIGHT >> 14) & 3);
    glProgramUniform1i(g2shaderProgramID, glGetUniformLocation(g2shaderProgramID, "inMasterBrightMode") , masterBrightMode);

    glBindVertexArray(g2vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, g2vertexBuffer);
    DrawScreenQuad();

    //Clear out the textures
    for(int i=0; i < 4; i++) {
        glActiveTexture(GL_TEXTURE0 + s_bgTexUnits[0]);
        glBindTexture(GL_TEXTURE_2D, dummyScreenTextureId);
    }

    for(int i=0; i < 5; i++) {
        glActiveTexture(GL_TEXTURE0 + s_objTexUnits[0]);
        glBindTexture(GL_TEXTURE_2D, dummyScreenTextureId);
    }
}


static void HandleJoystickKeyDown(int aKey)
{
    if(aKey == s_SIM_config.padSettings.aJoyKey) {
        //A Button
        s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT & 0b1111111111111110;
    } else if(aKey == s_SIM_config.padSettings.bJoyKey) {
        //B Button
        s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT & 0b1111111111111101;
    } else if(aKey == s_SIM_config.padSettings.xJoyKey) {
        //X Button
        *(vu16*)HW_BUTTON_XY_BUF = *(vu16*)HW_BUTTON_XY_BUF & 0b1111100000000000;
    } else if(aKey == s_SIM_config.padSettings.yJoyKey) {
        //y Button
        *(vu16*)HW_BUTTON_XY_BUF = *(vu16*)HW_BUTTON_XY_BUF & 0b1111010000000000;
    } else if(aKey == s_SIM_config.padSettings.upJoyKey) {
        //Dpad Up
        s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT & 0b1111111110111111;
    } else if(aKey == s_SIM_config.padSettings.downJoyKey) {
        //Dpad Down
        s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT & 0b1111111101111111;
    } else if(aKey == s_SIM_config.padSettings.leftJoyKey) {
        //Dpad Left
        s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT & 0b1111111111011111;
    } else if(aKey == s_SIM_config.padSettings.rightJoyKey) {
        //Dpad Right
        s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT & 0b1111111111101111;
    } else if(aKey == s_SIM_config.padSettings.lJoyKey) {
        //L
        s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT & 0b1111110111111111;
    } else if(aKey == s_SIM_config.padSettings.rJoyKey) {
        //R
        s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT & 0b1111111011111111;
    } else if(aKey == s_SIM_config.padSettings.startJoyKey) {
        //Start button
        s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT & 0b1111111111110111;
    } else if(aKey == s_SIM_config.padSettings.selectJoyKey) {
        //Select button
        s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT & 0b1111111111111011;
    }
}

static void HandleJoystickKeyUp(int aKey)
{
    if(aKey == s_SIM_config.padSettings.aJoyKey) {
        //A Button
        s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT | 0b0000000000000001;
    } else if(aKey == s_SIM_config.padSettings.bJoyKey) {
        //B Button
        s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT | 0b0000000000000010;
    } else if(aKey == s_SIM_config.padSettings.xJoyKey) {
        //X Button
        *(vu16*)HW_BUTTON_XY_BUF = *(vu16*)HW_BUTTON_XY_BUF | 0b0000010000000000;
    } else if(aKey == s_SIM_config.padSettings.yJoyKey) {
        //y Button
        *(vu16*)HW_BUTTON_XY_BUF = *(vu16*)HW_BUTTON_XY_BUF | 0b0000100000000000;
    } else if(aKey == s_SIM_config.padSettings.upJoyKey) {
        //Dpad Up
        s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT | 0b0000000001000000;
    } else if(aKey == s_SIM_config.padSettings.downJoyKey) {
        //Dpad Down
        s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT | 0b0000000010000000;
    } else if(aKey == s_SIM_config.padSettings.leftJoyKey) {
        //Dpad Left
        s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT | 0b0000000000100000;
    } else if(aKey == s_SIM_config.padSettings.rightJoyKey) {
        //Dpad Right
        s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT | 0b0000000000010000;
    } else if(aKey == s_SIM_config.padSettings.lJoyKey) {
        //L
        s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT | 0b0000001000000000;
    } else if(aKey == s_SIM_config.padSettings.rJoyKey) {
        //R
        s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT | 0b0000000100000000;
    } else if(aKey == s_SIM_config.padSettings.startJoyKey) {
        //Start button
        s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT | 0b0000000000001000;
    } else if(aKey == s_SIM_config.padSettings.selectJoyKey) {
        //Select button
        s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT | 0b0000000000000100;
    }
}



void * SIM_Render(void *arg){

    SDL_Event Event;
    char c;
    int x;
    int y;
    u8 r;
    u8 g;
    u8 b;
    u16 keys;
    u32 i;

    //Draw the translucent 3D stuff
    G3SIM_DrawItems();

    do {
    //SIM_Net_Process();
    if((*((u32*)HW_VBLANK_COUNT_BUF) % 10) == 0) {
        SIM_Net_SendBeaconIndication();
    }


    //Unbind 3D Framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glUseProgram(g2shaderProgramID);

    glDisable( GL_DEPTH_TEST );
    glDisable( GL_CULL_FACE );

    int windowHeight;
    int windowWidth;
    int viewHeight;
    int viewWidth;
    int viewX = 0;
    int viewY = 0;
    float aspectRatio;

    SDL_GetWindowSize(window, &windowWidth, &windowHeight);

    // Scale the viewport to the correct aspect ratio and center it
    switch(s_SIM_config.screenLayout){
        default:
        case SIM_CONFIG_SCREEN_LAYOUT_VERTICAL:
            aspectRatio = 256.0f / 384.0f;
            break;
        case SIM_CONFIG_SCREEN_LAYOUT_HORIZONTAL:
            aspectRatio = 512.0f / 192.0f;
            break;
        case SIM_CONFIG_SCREEN_LAYOUT_LARGE:
            aspectRatio = 768.0f / 384.0f;
            break;
    }

        viewWidth = windowWidth;
        viewHeight = (int)((float)windowWidth / aspectRatio);
        viewY = (windowHeight - viewHeight) / 2;
        if(viewHeight > windowHeight) {
            // Too tall to fit in the window. Scale down the width instead
            viewHeight = windowHeight;
            viewWidth = (int)((float)viewHeight * aspectRatio);
            viewX = (windowWidth - viewWidth) / 2;
            viewY = 0;
        }

    glViewport(viewX, viewY, viewWidth, viewHeight);

    //Set up the bounding box of the touch panel
    if(s_SIM_config.swapScreens) {
        switch(s_SIM_config.screenLayout){
            default:
            case SIM_CONFIG_SCREEN_LAYOUT_VERTICAL:
                s_TouchPanelCoords[0] = viewX;
                s_TouchPanelCoords[1] = viewY;
                s_TouchPanelCoords[2] = viewX + viewWidth;
                s_TouchPanelCoords[3] = viewY + (viewHeight / 2);
                break;
            case SIM_CONFIG_SCREEN_LAYOUT_HORIZONTAL:
                s_TouchPanelCoords[0] = viewX;
                s_TouchPanelCoords[1] = viewY;
                s_TouchPanelCoords[2] = viewX + (viewWidth / 2);
                s_TouchPanelCoords[3] = viewY + viewHeight;
                break;
            case SIM_CONFIG_SCREEN_LAYOUT_LARGE:
                s_TouchPanelCoords[0] = viewX;
                s_TouchPanelCoords[1] = viewY;
                s_TouchPanelCoords[2] = viewX + ((viewWidth / 3) * 2);
                s_TouchPanelCoords[3] = viewY + viewHeight;
                break;
        }
    } else {
        switch(s_SIM_config.screenLayout){
            default:
            case SIM_CONFIG_SCREEN_LAYOUT_VERTICAL:
                s_TouchPanelCoords[0] = viewX;
                s_TouchPanelCoords[1] = viewY + (viewHeight / 2);
                s_TouchPanelCoords[2] = viewX + viewWidth;
                s_TouchPanelCoords[3] = viewY + viewHeight;
                break;
            case SIM_CONFIG_SCREEN_LAYOUT_HORIZONTAL:
                s_TouchPanelCoords[0] = viewX + (viewWidth / 2);
                s_TouchPanelCoords[1] = viewY;
                s_TouchPanelCoords[2] = viewX + viewWidth;
                s_TouchPanelCoords[3] = viewY + viewHeight;
                break;
            case SIM_CONFIG_SCREEN_LAYOUT_LARGE:
                s_TouchPanelCoords[0] = viewX + ((viewWidth / 3) * 2);
                s_TouchPanelCoords[1] = viewY + (viewHeight / 2);
                s_TouchPanelCoords[2] = viewX + viewWidth;
                s_TouchPanelCoords[3] = viewY + viewHeight;
                break;
        }
    }

    // Check Joysticks
    if(s_SIM_SDLJoystick == NULL && SDL_NumJoysticks() >= 1) {
        s_SIM_SDLJoystick = SDL_JoystickOpen(0);
    }
        
        while( SDL_PollEvent(&Event))
        {
            //TODO: Bring in imgui
            //SIM_GUI_ProcessEvent(&Event);
            if(Event.type == SDL_WINDOWEVENT)
            {
                switch(Event.window.event) {
                    case SDL_WINDOWEVENT_CLOSE:
                        exit(0);
                        break;
                }
            }
            if(Event.type == SDL_KEYDOWN)
            {
                int keyRead = Event.key.keysym.sym;
                if(keyRead == s_SIM_config.padSettings.aKey) {
                    //A Button
                    s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT & 0b1111111111111110;
                } else if(keyRead == s_SIM_config.padSettings.bKey) {
                    //B Button
                    s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT & 0b1111111111111101;
                } else if(keyRead == s_SIM_config.padSettings.xKey) {
                    //X Button
                    *(vu16*)HW_BUTTON_XY_BUF = *(vu16*)HW_BUTTON_XY_BUF & 0b1111100000000000;
                } else if(keyRead == s_SIM_config.padSettings.yKey) {
                    //y Button
                    *(vu16*)HW_BUTTON_XY_BUF = *(vu16*)HW_BUTTON_XY_BUF & 0b1111010000000000;
                } else if(keyRead == s_SIM_config.padSettings.upKey) {
                    //Dpad Up
                    s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT & 0b1111111110111111;
                } else if(keyRead == s_SIM_config.padSettings.downKey) {
                    //Dpad Down
                    s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT & 0b1111111101111111;
                } else if(keyRead == s_SIM_config.padSettings.leftKey) {
                    //Dpad Left
                    s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT & 0b1111111111011111;
                } else if(keyRead == s_SIM_config.padSettings.rightKey) {
                    //Dpad Right
                    s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT & 0b1111111111101111;
                } else if(keyRead == s_SIM_config.padSettings.lKey) {
                    //L
                    s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT & 0b1111110111111111;
                } else if(keyRead == s_SIM_config.padSettings.rKey) {
                    //R
                    s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT & 0b1111111011111111;
                } else if(keyRead == s_SIM_config.padSettings.startKey) {
                    //Start button
                    s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT & 0b1111111111110111;
                } else if(keyRead == s_SIM_config.padSettings.selectKey) {
                    //Select button
                    s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT & 0b1111111111111011;
                } else if(keyRead == s_SIM_config.padSettings.guiKey) {
                    //Toggle Debug GUI //TODO: bring in imgui
                    //SIM_GUI_Toggle();
                }
            }
            if(Event.type == SDL_KEYUP)
            {
                int keyRead = Event.key.keysym.sym;
                if(keyRead == s_SIM_config.padSettings.aKey) {
                    //A Button
                    s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT | 0b0000000000000001;
                } else if(keyRead == s_SIM_config.padSettings.bKey) {
                    //B Button
                    s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT | 0b0000000000000010;
                } else if(keyRead == s_SIM_config.padSettings.xKey) {
                    //X Button
                    *(vu16*)HW_BUTTON_XY_BUF = *(vu16*)HW_BUTTON_XY_BUF | 0b0000010000000000;
                } else if(keyRead == s_SIM_config.padSettings.yKey) {
                    //y Button
                    *(vu16*)HW_BUTTON_XY_BUF = *(vu16*)HW_BUTTON_XY_BUF | 0b0000100000000000;
                } else if(keyRead == s_SIM_config.padSettings.upKey) {
                    //Dpad Up
                    s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT | 0b0000000001000000;
                } else if(keyRead == s_SIM_config.padSettings.downKey) {
                    //Dpad Down
                    s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT | 0b0000000010000000;
                } else if(keyRead == s_SIM_config.padSettings.leftKey) {
                    //Dpad Left
                    s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT | 0b0000000000100000;
                } else if(keyRead == s_SIM_config.padSettings.rightKey) {
                    //Dpad Right
                    s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT | 0b0000000000010000;
                } else if(keyRead == s_SIM_config.padSettings.lKey) {
                    //L
                    s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT | 0b0000001000000000;
                } else if(keyRead == s_SIM_config.padSettings.rKey) {
                    //R
                    s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT | 0b0000000100000000;
                } else if(keyRead == s_SIM_config.padSettings.startKey) {
                    //Start button
                    s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT | 0b0000000000001000;
                } else if(keyRead == s_SIM_config.padSettings.selectKey) {
                    //Select button
                    s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT | 0b0000000000000100;
                }
            }
            if( Event.type == SDL_JOYBUTTONDOWN ) {
                HandleJoystickKeyDown(Event.jbutton.button);
            }
            if( Event.type == SDL_JOYBUTTONUP) {
                HandleJoystickKeyUp(Event.jbutton.button);
            }
            if( Event.type == SDL_JOYHATMOTION) {
                if(Event.jhat.value == SDL_HAT_CENTERED) {
                    // send keyUp event for all hats
                    HandleJoystickKeyUp(SIM_CONFIG_JOY_HAT_MASK | SDL_HAT_UP);
                    HandleJoystickKeyUp(SIM_CONFIG_JOY_HAT_MASK | SDL_HAT_DOWN);
                    HandleJoystickKeyUp(SIM_CONFIG_JOY_HAT_MASK | SDL_HAT_LEFT);
                    HandleJoystickKeyUp(SIM_CONFIG_JOY_HAT_MASK | SDL_HAT_RIGHT);
                } else{
                    if(Event.jhat.value & SDL_HAT_UP) {
                        HandleJoystickKeyDown(SIM_CONFIG_JOY_HAT_MASK | SDL_HAT_UP);
                    } else {
                        HandleJoystickKeyUp(SIM_CONFIG_JOY_HAT_MASK | SDL_HAT_UP);
                    }
                    if(Event.jhat.value & SDL_HAT_DOWN) {
                        HandleJoystickKeyDown(SIM_CONFIG_JOY_HAT_MASK | SDL_HAT_DOWN);
                    } else {
                        HandleJoystickKeyUp(SIM_CONFIG_JOY_HAT_MASK | SDL_HAT_DOWN);
                    }
                    if(Event.jhat.value & SDL_HAT_LEFT) {
                        HandleJoystickKeyDown(SIM_CONFIG_JOY_HAT_MASK | SDL_HAT_LEFT);
                    } else {
                        HandleJoystickKeyUp(SIM_CONFIG_JOY_HAT_MASK | SDL_HAT_LEFT);
                    }
                    if(Event.jhat.value & SDL_HAT_RIGHT) {
                        HandleJoystickKeyDown(SIM_CONFIG_JOY_HAT_MASK | SDL_HAT_RIGHT);
                    } else {
                        HandleJoystickKeyUp(SIM_CONFIG_JOY_HAT_MASK | SDL_HAT_RIGHT);
                    }
                }
            }
            if( Event.type == SDL_JOYAXISMOTION ) {
                if(Event.jaxis.value > s_SIM_config.padSettings.joyAxisDeadzone
                || Event.jaxis.value < (s_SIM_config.padSettings.joyAxisDeadzone*-1)) {
                    if(Event.jaxis.value > 0) {
                        HandleJoystickKeyDown(Event.jaxis.axis | SIM_CONFIG_JOY_AXIS_PLUS_MASK);
                        HandleJoystickKeyUp(Event.jaxis.axis | SIM_CONFIG_JOY_AXIS_MINUS_MASK);
                    } else {
                        HandleJoystickKeyDown(Event.jaxis.axis | SIM_CONFIG_JOY_AXIS_MINUS_MASK);
                        HandleJoystickKeyUp(Event.jaxis.axis | SIM_CONFIG_JOY_AXIS_PLUS_MASK);
                    }
                } else {
                    HandleJoystickKeyUp(Event.jaxis.axis | SIM_CONFIG_JOY_AXIS_PLUS_MASK);
                    HandleJoystickKeyUp(Event.jaxis.axis | SIM_CONFIG_JOY_AXIS_MINUS_MASK);
                }
            }
            if( Event.type == SDL_MOUSEBUTTONDOWN )
            {
                isMouseDown = 1;
                s_tpData.validity = 0;
                s_tpData.touch = 1;
            }
            if( Event.type == SDL_MOUSEBUTTONUP)
            {
                isMouseDown = 0;
                s_tpData.validity = 0;
                s_tpData.touch = 0;
            }
        }
        s_reg_PAD_KEYINPUT = s_reg_PAD_KEYINPUT;

        SDL_GetMouseState(&x, &y);

        //if(x < s_TouchPanelCoords[0]) {
        //	x = s_TouchPanelCoords[0];
        //}
        //if(x > s_TouchPanelCoords[2]){
        //	x = s_TouchPanelCoords[2];
        //}
        //if(y < s_TouchPanelCoords[1]) {
        //	y = s_TouchPanelCoords[1];
        //}
        //if(y > s_TouchPanelCoords[3]) {
        //	y = s_TouchPanelCoords[3];
        //}

        //Convert touch coordinate to DS screen coordinates
        if(x >= s_TouchPanelCoords[0] && x <= s_TouchPanelCoords[2] && y >= s_TouchPanelCoords[1] && y <= s_TouchPanelCoords[3])
        {
            s_tpData.x = (u16)map(x, s_TouchPanelCoords[0], s_TouchPanelCoords[2], 0, SIM_NDS_SCREEN_WIDTH);
            s_tpData.y = (u16)map(y, s_TouchPanelCoords[1], s_TouchPanelCoords[3], 0, SIM_NDS_SCREEN_HEIGHT);
        } else {
            s_tpData.validity = 0;
            s_tpData.touch = 0;
        }

        //TODO: bring in imgui
        //SIM_GUI_NewFrame();
        //SIM_GUI_Main();

        memset( bgtex, 0, sizeof(u8) * 4 * SIM_NDS_SCREEN_WIDTH * SIM_NDS_SCREEN_HEIGHT * 2 );
        memset( bg0tex, 0, sizeof(u8) * 4 * SIM_NDS_SCREEN_WIDTH * SIM_NDS_SCREEN_HEIGHT * 2 );
        memset( bg1tex, 0, sizeof(u8) * 4 * SIM_NDS_SCREEN_WIDTH * SIM_NDS_SCREEN_HEIGHT * 2 );
        memset( bg2tex, 0, sizeof(u8) * 4 * SIM_NDS_SCREEN_WIDTH * SIM_NDS_SCREEN_HEIGHT * 2 );
        memset( bg3tex, 0, sizeof(u8) * 4 * SIM_NDS_SCREEN_WIDTH * SIM_NDS_SCREEN_HEIGHT * 2 );
        memset( bg0stex, 0, sizeof(u8) * 4 * SIM_NDS_SCREEN_WIDTH * SIM_NDS_SCREEN_HEIGHT * 2 );
        memset( bg1stex, 0, sizeof(u8) * 4 * SIM_NDS_SCREEN_WIDTH * SIM_NDS_SCREEN_HEIGHT * 2 );
        memset( bg2stex, 0, sizeof(u8) * 4 * SIM_NDS_SCREEN_WIDTH * SIM_NDS_SCREEN_HEIGHT * 2 );
        memset( bg3stex, 0, sizeof(u8) * 4 * SIM_NDS_SCREEN_WIDTH * SIM_NDS_SCREEN_HEIGHT * 2 );

        //Draw MAIN engine
        DrawEngine(FALSE);


        //SUB engine
        DrawEngine(TRUE);

        //Run HBlank Interrupt
        OS_IRQTable[OS_IE_H_BLANK-1]();

        //Run VBlank Interrupt
        OS_IRQTable[OS_IE_V_BLANK-1]();
        s_HW_INTR_CHECK_BUF |= 1;
        *((u32*)HW_VBLANK_COUNT_BUF) = *((u32*)HW_VBLANK_COUNT_BUF) + 1;

        //TODO: bring in imgui
        //SIM_GUI_Render();

        // Calculate frametime
        struct timespec curTime;
        clock_gettime(CLOCK_MONOTONIC, &curTime);
        u64 frameNs;
        frameNs = ((curTime.tv_sec - s_SIM_lastFrameEnd.tv_sec) * 1000000000) + (curTime.tv_nsec - s_SIM_lastFrameEnd.tv_nsec);
        s_SIM_frameTime = frameNs;


        //Limit framerate to 60 fps
        if(s_SIM_config.capFrameRate)
        {


            // 60 Fps
            if(frameNs < 16600000) {
                u64 frameTimeRemaining = 16600000 - frameNs;
                u64 frameTimeRemainingMs = frameTimeRemaining / 1000000;
                if(frameTimeRemainingMs > 1){
                    SDL_Delay(frameTimeRemainingMs-1);
                }
            }
        }

        SDL_GL_SwapWindow(window);
        clock_gettime(CLOCK_MONOTONIC, &s_SIM_lastFrameEnd);

        s_numG3CommandsThisFrame = 0;
        s_numG3DrawsThisFrame = 0;
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
        s_curPolygonAttr.alpha = 1.0f;

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

    } while(/*SIM_GUI_IsGameLogicPaused()*/ FALSE /* TODO: bring in IMGUI*/);
        //Bind 3D Framebuffer & Renderbuffer
        glBindFramebuffer(GL_FRAMEBUFFER, g3FrameBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, g3RenderBufferId);

        //Setup 3D Viewport
        if( s_reg_GX_POWCNT >> 15 )
        {
            glViewport(0, SIM_NDS_SCREEN_HEIGHT*s_SIM_config.internalResolutionScale, SIM_NDS_SCREEN_WIDTH*s_SIM_config.internalResolutionScale, SIM_NDS_SCREEN_HEIGHT*s_SIM_config.internalResolutionScale);
        }
        else
        {
            glViewport(0, 0, SIM_NDS_SCREEN_WIDTH*s_SIM_config.internalResolutionScale, SIM_NDS_SCREEN_HEIGHT*s_SIM_config.internalResolutionScale);
        }
    

        //Activate 3D shader
        glUseProgram(g3shaderProgramID);

        glEnable( GL_DEPTH_TEST );
        glEnable( GL_CULL_FACE );

        glCullFace(GL_FRONT);
        glFrontFace(GL_CW);
}

void SIM_PreRenderVBlank()
{
    SDL_SemPost(vcountVblankSemaphore);
}

void SIM_PostRenderVBlank()
{
    if(G3SIM_SwapBuffersCalledThisFrame()) {
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f );
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        G3SIM_ResetSwapBuffersCall();
    }
    // Reset Vcount
    s_reg_GX_VCOUNT = 0;
    SDL_SemPost(vcountResetSemaphore);
}

int main(int argc, char* argv[]){
    //Load default config
    SIM_Config_LoadDefaults(&s_SIM_config);

    //Load configuration file
    if(!SIM_Config_LoadConfigFile(&s_SIM_config)){
        //Config file does not exist. Save config file with defaults
        SIM_Config_SaveConfigFile(&s_SIM_config);
    }

    //Initialize Nitro registers
    s_reg_PAD_KEYINPUT = 0x3ff;
    *(vu16*)HW_BUTTON_XY_BUF = 0b0111110000000000;

    memset( s_HW_OAM, 0, sizeof( s_HW_OAM ));
    memset( s_HW_DB_OAM, 0, sizeof( s_HW_DB_OAM ));

    s_reg_GX_VRAMCNT_A = 0;
    s_reg_GX_VRAMCNT_B = 0;
    s_reg_GX_VRAMCNT_C = 0;
    s_reg_GX_VRAMCNT_D = 0;
    s_reg_GX_VRAMCNT_E = 0;
    s_reg_GX_VRAMCNT_F = 0;
    s_reg_GX_VRAMCNT_G = 0;
    s_reg_GX_VRAMCNT_H = 0;
    s_reg_GX_VRAMCNT_I = 0;

    WIN_Init_sTexStartAddrTable();
    WIN_Init_sTexPlttStartAddrTable();

    //Read the first 0x160 bytes of the rom and put it in the HW_ROM_HEADER_BUF
    FILE * romFile;
    romFile = fopen( "main.srl", "r" );

    if(romFile != NULL){
        //Read the fields of the struct
        fread( (void *)HW_ROM_HEADER_BUF, 1, 0x160, romFile);
        memcpy(s_SIM_game_name, (void *)HW_ROM_HEADER_BUF, 12);
        s_SIM_game_name[12] = 0;
        fclose(romFile);
    }

    memset( (void *)HW_MAIN_MEM_SHARED, 0, 0x160 );

    SIM_pxiInit();

    pthread_t thread;
    SDL_Thread * pxiThread;
    SDL_Thread * pxi7to9Thread;
    int num;
    num =123;

    //Initialize mosaic table
    u32 i;
    u32 j;
    for( i=0; i < 16; i++ )
    {
        for( j=0; j < 256; j++ )
        {
            int offset = j % (i+1);
            s_mosaicTable[i][j] = offset;
        }
    }

    SND_ExChannelInit();
    SND_SeqInit();

    //Zero out some shared memory
    memset(s_HW_BG_PLTT, 0, sizeof( s_HW_BG_PLTT ) );
    memset(s_HW_OBJ_PLTT, 0, sizeof( s_HW_OBJ_PLTT ) );
    memset(s_HW_DB_BG_PLTT, 0, sizeof( s_HW_DB_BG_PLTT ) );
    memset(s_HW_DB_OBJ_PLTT, 0, sizeof( s_HW_DB_OBJ_PLTT ) );
    memset(s_HW_BG_VRAM, 0, sizeof( s_HW_BG_VRAM ) );
    memset(s_HW_DB_BG_VRAM, 0, sizeof( s_HW_DB_BG_VRAM ) );
    memset(s_HW_OBJ_VRAM, 0, sizeof( s_HW_OBJ_VRAM ) );
    memset(s_HW_DB_OBJ_VRAM, 0, sizeof( s_HW_DB_OBJ_VRAM ) );

    //Zero out some registers
    s_reg_G2_BG0CNT = 0;
    s_reg_G2_BG1CNT = 0;
    s_reg_G2_BG2CNT = 0;
    s_reg_G2_BG3CNT = 0;

    s_reg_G2S_DB_BG0CNT = 0;
    s_reg_G2S_DB_BG1CNT = 0;
    s_reg_G2S_DB_BG2CNT = 0;
    s_reg_G2S_DB_BG3CNT = 0;

    //Set up OS NVRAM Config
    SIM_Config_SetOSNVRAMConfig(&s_SIM_config);

    pxiThread = SDL_CreateThread(SIM_procPxiThread, "PXI", NULL);
    pxi7to9Thread = SDL_CreateThread(SIM_procPxi7to9Thread, "PXI7to9", NULL);

    WM_sp_SIM_init();

    #ifdef SDK_BUILD_WIN64
    SetConsoleOutputCP(65001);
    #endif

    SIM_RenderInit(NULL);
    G3SIM_DrawInit();

    //Init Sockets (only needed on windows)
    #ifdef SDK_BUILD_WIN64
    WSADATA wsa;
    
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("Failed to initialize winsock. Error Code : %d",WSAGetLastError());
        return 1;
    }
    #endif

    SIM_Net_Init();
    SIM_Net_StartThread();

    if(argc >= 2) {
        if(strcmp(argv[1], "--net-host") == 0) {
            SIM_Net_StartHost();
            SDL_SetWindowTitle(window, "Host");
        }
        if(strcmp(argv[1], "--net-client") == 0) {
            SIM_Net_StartClient(s_SIM_config.netSettings.serverIp);
            SDL_SetWindowTitle(window, "Client");
        }
    }

    NitroMain();
    return 0;
}

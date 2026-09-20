#include "simulator/sim_g3_CommandProcessor.hpp"

#include "simulator/drawmsg.h"
#include "simulator/g3_handler.h"
#include "simulator/g3_draw.h"

#include <cstring>
#include <cstdlib>


static xyz_s_t prevXYZ;

static draw_command_type_t DecodeG3Op(u32 op) {
  switch (op) {
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

static void ConvertMsgParams(draw_msg_t *msg) {
  draw_msg_t tmpMsg;
  memcpy(&tmpMsg, msg, sizeof(draw_msg_t));
  switch (msg->type) {
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


static u8 GetNumParamsFromCommandType(draw_command_type_t type) {
  switch (type) {
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

static u8 GetParamSizeFromCommandType(draw_command_type_t type) {
  switch (type) {
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

static u8 isParamSigned(draw_command_type_t type) {
  switch (type) {
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

namespace SIM::G3 {

void HandleCommandMessage(draw_msg_t *msg) {
  u32 temp;
  switch (msg->type) {
  case DRAW_CMD_G3_NOP:
  case DRAW_CMD_G3_DUMMY:
    break;
  case DRAW_CMD_G3_BEGIN:
    G3SIM_Begin(static_cast<GXBegin>(msg->data.numU8));
    break;
  case DRAW_CMD_G3_MTXMODE:
    G3SIM_MtxMode(static_cast<GXMtxMode>(msg->data.numU8));
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
    SIM_HandleG3Command(msg);
    break;
  case DRAW_CMD_G3_VTXXZ:
    msg->type = DRAW_CMD_G3_VTX16;
    msg->data.xyz.y = prevXYZ.y;
    SIM_HandleG3Command(msg);
    break;
  case DRAW_CMD_G3_VTXYZ:
    msg->type = DRAW_CMD_G3_VTX16;
    msg->data.xyz.x = prevXYZ.x;
    SIM_HandleG3Command(msg);
    break;
  case DRAW_CMD_G3_VTXDIFF: {
    // The conversion works a bit different from vtx10
    msg->type = DRAW_CMD_G3_VTX16;
    temp = msg->data.numU32;

    s16 x = (s16)((temp & 0x000003FF) << 6) >> 6;
    s16 y = (s16)((temp & 0x000FFC00) >> 4) >> 6;
    s16 z = (s16)((temp & 0x3FF00000) >> 14) >> 6;

    msg->data.xyz.x = prevXYZ.x + x;
    msg->data.xyz.y = prevXYZ.y + y;
    msg->data.xyz.z = prevXYZ.z + z;
    SIM_HandleG3Command(msg);
  } break;
  case DRAW_CMD_G3_VTX10:
    msg->type = DRAW_CMD_G3_VTX16;
    // Convert to fx16
    temp = msg->data.numU32;
    msg->data.xyz.x = (temp & 0b1111111111) << 6;
    msg->data.xyz.y = ((temp >> 10) & 0b1111111111) << 6;
    msg->data.xyz.z = ((temp >> 20) & 0b1111111111) << 6;
    SIM_HandleG3Command(msg);
    break;
  case DRAW_CMD_G3_VTX16:
    prevXYZ.x = msg->data.xyz.x;
    prevXYZ.y = msg->data.xyz.y;
    prevXYZ.z = msg->data.xyz.z;
    G3SIM_Vtx(msg->data.xyz.x, msg->data.xyz.y, msg->data.xyz.z);
    if (s_G3numInPoly == 0 || s_G3drawPoly == 1) {
      s_G3drawPoly = 0;
      GLfloat myVerts[(3 * 6) + (2 * 6)];
      if (s_primType == GX_BEGIN_TRIANGLES ||
          s_primType == GX_BEGIN_TRIANGLE_STRIP) {
        if (s_primType == GX_BEGIN_TRIANGLE_STRIP &&
            !(s_G3triStripTriNum & 1)) {
          // Swap the last 2 verts
          G3SIM_FxVtx_t temp;
          memcpy(&temp, &s_g3PolygonVerts[1], sizeof(G3SIM_FxVtx_t));
          memcpy(&s_g3PolygonVerts[1], &s_g3PolygonVerts[2],
                 sizeof(G3SIM_FxVtx_t));
          memcpy(&s_g3PolygonVerts[2], &temp, sizeof(G3SIM_FxVtx_t));
        }

        G3SIM_SubmitPolygon(s_g3PolygonVerts, 3);

        if (s_primType == GX_BEGIN_TRIANGLE_STRIP &&
            !(s_G3triStripTriNum & 1)) {
          // Swap the last 2 verts
          G3SIM_FxVtx_t temp;

          memcpy(&temp, &s_g3PolygonVerts[1], sizeof(G3SIM_FxVtx_t));
          memcpy(&s_g3PolygonVerts[1], &s_g3PolygonVerts[2],
                 sizeof(G3SIM_FxVtx_t));
          memcpy(&s_g3PolygonVerts[2], &temp, sizeof(G3SIM_FxVtx_t));
        }

        if (s_primType == GX_BEGIN_TRIANGLE_STRIP) {
          // Copy the last two verts from the last polygon
          memcpy(&s_g3PolygonVerts[0], &s_g3PolygonVerts[1],
                 sizeof(G3SIM_FxVtx_t));
          memcpy(&s_g3PolygonVerts[1], &s_g3PolygonVerts[2],
                 sizeof(G3SIM_FxVtx_t));
        }
      }
      if (s_primType == GX_BEGIN_QUADS || s_primType == GX_BEGIN_QUAD_STRIP) {
        if (s_primType == GX_BEGIN_QUAD_STRIP) {
          G3SIM_FxVtx_t temp;
          memcpy(&temp, &s_g3PolygonVerts[2], sizeof(G3SIM_FxVtx_t));
          memcpy(&s_g3PolygonVerts[2], &s_g3PolygonVerts[3],
                 sizeof(G3SIM_FxVtx_t));
          memcpy(&s_g3PolygonVerts[3], &temp, sizeof(G3SIM_FxVtx_t));
        }

        G3SIM_SubmitPolygon(s_g3PolygonVerts, 4);

        if (s_primType == GX_BEGIN_QUAD_STRIP) {
          // Copy the last two verts from the last polygon
          s_g3PolygonVerts[0].x = s_g3PolygonVerts[3].x;
          s_g3PolygonVerts[0].y = s_g3PolygonVerts[3].y;
          s_g3PolygonVerts[0].z = s_g3PolygonVerts[3].z;
          s_g3PolygonVerts[0].w = s_g3PolygonVerts[3].w;

          s_g3PolygonVerts[1].x = s_g3PolygonVerts[2].x;
          s_g3PolygonVerts[1].y = s_g3PolygonVerts[2].y;
          s_g3PolygonVerts[1].z = s_g3PolygonVerts[2].z;
          s_g3PolygonVerts[1].w = s_g3PolygonVerts[2].w;

          memcpy(&s_g3PolygonVerts[0], &s_g3PolygonVerts[3],
                 sizeof(G3SIM_FxVtx_t));
          memcpy(&s_g3PolygonVerts[1], &s_g3PolygonVerts[2],
                 sizeof(G3SIM_FxVtx_t));
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
    SIM_HandleG3Command(msg);
    break;
  case DRAW_CMD_G3_DIRECT1:
    msg->type = DecodeG3Op(msg->data.numsU64[0]);
    // Get the parameter in the right place
    ConvertMsgParams(msg);
    SIM_HandleG3Command(msg);
    break;
  case DRAW_CMD_G3_SWAPBUFFERS: {
    u8 depthBufferMode = (msg->data.numU32 & 2) >> 1;
    G3SIM_SwapBuffers(
        depthBufferMode); // TODO: add flush attributes here for Z/W-buffering!
  } break;
  case DRAW_CMD_G3_VIEWPORT:
    // TODO
    break;
  case DRAW_CMD_G3_TEXPLTTBASE:
    G3SIM_FlushArray();
    G3SIM_TexPlttBase(msg->data.numU32);
    break;
  case DRAW_CMD_G3_BOXTEST:
    G3SIM_BoxTest(msg->data.boxtest.x, msg->data.boxtest.y, msg->data.boxtest.z,
                  msg->data.boxtest.width, msg->data.boxtest.height,
                  msg->data.boxtest.depth);
    break;
  case DRAW_CMD_G3_CMD_LIST:
    // Process a command List
    {
      G3SIM_CommandBlock_t *cmdBlockPtr;
      draw_msg_t tempMsg;
      cmdBlockPtr = static_cast<G3SIM_CommandBlock_t *>(msg->data.ptr);
      u64 curOffset;
      curOffset = 0;
      u8 numParams;
      void *paramPtr;
      u8 paramSize;
      u8 paramSigned;
      while (curOffset < msg->size) {
        paramPtr = (void *)cmdBlockPtr + 4;
        curOffset += 4;
        for (int i = 0; i < 4; i++) {
          tempMsg.type = DecodeG3Op(cmdBlockPtr->ops[i]);
          numParams = GetNumParamsFromCommandType(tempMsg.type);
          paramSize = GetParamSizeFromCommandType(tempMsg.type);
          paramSigned = isParamSigned(tempMsg.type);
          for (int j = 1; j < numParams + 1; j++) {
            switch (paramSize) {
            case 0:
              break;
            case 1:
              if (paramSigned) {
                tempMsg.data.numsS64[j] = (s64) * (s8 *)paramPtr;
              } else {
                tempMsg.data.numsU64[j] = *(u8 *)paramPtr;
              }
              break;
            case 2:
              if (paramSigned) {
                tempMsg.data.numsS64[j] = (s64) * (s16 *)paramPtr;
              } else {
                tempMsg.data.numsU64[j] = *(u16 *)paramPtr;
              }
              break;
            case 4:
              if (paramSigned) {
                tempMsg.data.numsS64[j] = (s64) * (s32 *)paramPtr;
              } else {
                tempMsg.data.numsU64[j] = *(u32 *)paramPtr;
              }
              break;
            case 8:
              if (paramSigned) {
                tempMsg.data.numsS64[j] = *(s64 *)paramPtr;
              } else {
                tempMsg.data.numsU64[j] = *(u64 *)paramPtr;
              }
              break;
            default:
              break;
            }
            paramPtr += paramSize;
            curOffset += paramSize;
          }

          ConvertMsgParams(&tempMsg);
          SIM_HandleG3Command(&tempMsg);
        }
        cmdBlockPtr = static_cast<G3SIM_CommandBlock_t *>(paramPtr);
      }
    }
    free(msg->data.ptr);
    // TODO
    break;
  default:
    break;
    // printf( "Draw command %d not implemented\n", msg->type );
  }
}

}

// C APIs
void SIM_HandleG3Command(draw_msg_t * msg) {
    SIM::G3::HandleCommandMessage(msg);
}
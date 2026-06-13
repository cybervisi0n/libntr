#include <nitro/fx/fx.h>
#include <nitro/gx/g3.h>

#ifndef DRAWMSG_H
#define DRAWMSG_H

typedef enum {
	DRAW_CMD_G3_NOP,
	DRAW_CMD_G3_DIRECT0,
	DRAW_CMD_G3_DIRECT1,
	DRAW_CMD_G3_DIRECT2,
	DRAW_CMD_G3_DIRECT3,
	DRAW_CMD_G3_MTXMODE,
	DRAW_CMD_G3_PUSHMTX,
	DRAW_CMD_G3_POPMTX,
	DRAW_CMD_G3_STOREMTX,
	DRAW_CMD_G3_RESTOREMTX,
	DRAW_CMD_G3_IDENTITY,
	DRAW_CMD_G3_SCALE,
	DRAW_CMD_G3_TRANSLATE,
	DRAW_CMD_G3_LOADMTX44,
	DRAW_CMD_G3_LOADMTX43,
	DRAW_CMD_G3_MTXMULT33,
	DRAW_CMD_G3_MTXMULT43,
	DRAW_CMD_G3_MTXMULT44,
	DRAW_CMD_G3_COLOR,
	DRAW_CMD_G3_NORMAL,
	DRAW_CMD_G3_TEXCOORD,
	DRAW_CMD_G3_VTX,
	DRAW_CMD_G3_VTX10,
	DRAW_CMD_G3_VTX16,
	DRAW_CMD_G3_VTXXY,
	DRAW_CMD_G3_VTXXZ,
	DRAW_CMD_G3_VTXYZ,
	DRAW_CMD_G3_VTXDIFF,
	DRAW_CMD_G3_POLYGONATTR,
	DRAW_CMD_G3_TEXIMAGEPARAM,
	DRAW_CMD_G3_TEXPLTTBASE,
	DRAW_CMD_G3_MATERIALCOLORDIFFAMB,
	DRAW_CMD_G3_MATERIALCOLORSPECEMI,
	DRAW_CMD_G3_LIGHTVECTOR,
	DRAW_CMD_G3_LIGHTCOLOR,
	DRAW_CMD_G3_BEGIN,
	DRAW_CMD_G3_END,
	DRAW_CMD_G3_SWAPBUFFERS,
	DRAW_CMD_G3_VIEWPORT,
	DRAW_CMD_G3_BOXTEST,
	DRAW_CMD_G3_POSITIONTEST,
	DRAW_CMD_G3_VECTORTEST,
	DRAW_CMD_G3_DIFF_AMB,
	DRAW_CMD_G3_SPEC_EMI,
	DRAW_CMD_G3_SHININESS,
	DRAW_CMD_G3_DUMMY,
	DRAW_CMD_G3_CMD_LIST,
	DRAW_CMD_CNT
} draw_command_type_t;

struct xyz_s {
	s16 x;
	s16 y;
	s16 z;
};

struct diffAmb_s {
	u16 diffuse;
	u16 ambient;
	u8 IsSetVtxColor;
};

struct specEmi_s {
	u16 specular;
	u16 emission;
	u8 IsShininess;
};

struct polygonAttr_s {
	int light;
	u8 polyMode;
	u8 cullMode;
	int polygonID;
	int alpha;
	int misc;
};

struct boxtest_s {
	s16 x;
	s16 y;
	s16 z;
	s16 width;
	s16 height;
	s16 depth;
};

typedef struct xyz_s xyz_s_t;
typedef struct diffAmb_s diffAmb_s_t;
typedef struct specEmi_s specEmi_s_t;
typedef struct polygonAttr_s polygonAttr_s_t;
typedef struct boxtest_s boxtest_s_t;

union draw_msg_data {
	u8 numU8;
	s16 numS16;
	u16 numU16;
	s32 numS32;
	u32 numU32;
	u32 numsU32[33];
	s32 numsS32[33];
	u64 numsU64[33];
	s64 numsS64[33];
	xyz_s_t xyz;
	diffAmb_s_t diffAmb;
	specEmi_s_t specEmi;
	polygonAttr_s_t polygonAttr;
	boxtest_s_t boxtest;
	MtxFx33 mtx33;
	MtxFx43 mtx43;
	MtxFx44 mtx44;
	fx32 scale[3];
	void * ptr;
};
typedef union draw_msg_data draw_msg_data_t;

struct draw_msg {
	draw_command_type_t type;
	draw_msg_data_t data;
	u32 size;
};
typedef struct draw_msg draw_msg_t;

void SIM_HandleG3Command(draw_msg_t * msg);
#endif
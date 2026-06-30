#include <simulator/g3_draw.h>

#include <simulator/g3_handler.h>
#include <simulator/assert.h>
#include <string.h>
#include <stddef.h>

#include <nitro.h>
#include <nitro/gx/g3.h>
#include <nitro/gx/gx_load.h>
#include <nitro/gx/gx_vramcnt.h>

#ifdef SDK_BUILD_LINUX
#include <signal.h>
#endif

#include "tracy/TracyC.h"

#define G3_DRAW_MAX_ITEMS 1000

#define getbit(x,n) ( ( (x) >> (n) ) & 1 )

typedef struct {
	float zIdx;
	GLuint textureId;
	G3SIM_PolygonAttr_t polygonAttr;
	G3SIM_TexImageParam_t texImageParam;
	G3SIM_Vertex_t * verts;
	u32 vertsCount;
} g3_draw_item_t;

G3SIM_Vertex_t s_G3DrawVerts[G3_DRAW_MAX_VERTS];

//The item list is used for drawing translucent things sorted by Z position
static g3_draw_item_t s_G3DrawItemList[G3_DRAW_MAX_ITEMS];
static u32 s_G3DrawItemListCount = 0;

static u32 s_G3DrawCurVertIdx = 0;
static GLuint s_G3DrawVertexArray;
static GLuint s_G3DrawVertexBuffer;

extern G3SIM_TexImageParam_t s_texImageParam;
extern G3SIM_PolygonAttr_t s_curPolygonAttr;
extern GLuint g3shaderProgramID;
extern u8 s_SIM_g3tex[4*1024*1024];
extern GXVRamTex s_SIM_GXVRamTex;
extern GXVRamTexPltt s_SIM_GXVRamTexPltt;

extern GLuint SIM_GetTextureID();

static void* getTextureVramBank();
static void* getTexPlttVramBank();

static void* getTextureVramBank()
{
	void * vramBank = NULL;
	switch( s_SIM_GXVRamTex ){
		case GX_VRAM_TEX_0_A:
		case GX_VRAM_TEX_01_AB:
		case GX_VRAM_TEX_012_ABC:
		case GX_VRAM_TEX_0123_ABCD:
		case GX_VRAM_TEX_01_AC:
		case GX_VRAM_TEX_01_AD:
		case GX_VRAM_TEX_012_ABD:
		case GX_VRAM_TEX_012_ACD:
			vramBank = (void *)HW_LCDC_VRAM_A;
			break;
		case GX_VRAM_TEX_0_B:
		case GX_VRAM_TEX_01_BC:
		case GX_VRAM_TEX_012_BCD:
		case GX_VRAM_TEX_01_BD:
			vramBank = (void *)HW_LCDC_VRAM_B;
			break;
		case GX_VRAM_TEX_0_C:
		case GX_VRAM_TEX_01_CD:
			vramBank = (void *)HW_LCDC_VRAM_C;
			break;
		case GX_VRAM_TEX_0_D:
			vramBank = (void *)HW_LCDC_VRAM_A;
			break;
		default:
			//?
			break;
	}
	
	return vramBank;
}

static void* getTexPlttVramBank()
{
	void * vramBank = NULL;
	switch( s_SIM_GXVRamTexPltt ) {
		case GX_VRAM_TEXPLTT_NONE:
		case GX_VRAM_TEXPLTT_0123_E:
		case GX_VRAM_TEXPLTT_01234_EF:
		case GX_VRAM_TEXPLTT_012345_EFG:
			vramBank = (void *)HW_LCDC_VRAM_E;
			break;
		case GX_VRAM_TEXPLTT_0_F:
		case GX_VRAM_TEXPLTT_01_FG:
			vramBank = (void *)HW_LCDC_VRAM_F;
			break;
		case GX_VRAM_TEXPLTT_0_G:
			vramBank = (void *)HW_LCDC_VRAM_G;
			break;
	}
	
	return vramBank;
}

void G3SIM_AddVtx(G3SIM_Vertex_t * vtx)
{
	memcpy(&s_G3DrawVerts[s_G3DrawCurVertIdx], vtx, sizeof(G3SIM_Vertex_t));

	s_G3DrawCurVertIdx = s_G3DrawCurVertIdx + 1;

	SIM_assert(s_G3DrawCurVertIdx < G3_DRAW_MAX_VERTS);
	return;
}

void G3SIM_DrawArray()
{
	if( s_G3DrawCurVertIdx != 0 )
	{
		glBindVertexArray(s_G3DrawVertexArray);
		glBindBuffer(GL_ARRAY_BUFFER, s_G3DrawVertexBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(G3SIM_Vertex_t) * s_G3DrawCurVertIdx, s_G3DrawVerts);
		glDrawArrays(GL_TRIANGLES,0,s_G3DrawCurVertIdx);
	}
	return;
}

void G3SIM_DrawCleanUp()
{
	s_G3DrawCurVertIdx = 0;
}

void G3SIM_DrawInit()
{
	glGenVertexArrays(1, &s_G3DrawVertexArray);
	glBindVertexArray(s_G3DrawVertexArray);

	glGenBuffers(1, &s_G3DrawVertexBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, s_G3DrawVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(s_G3DrawVerts), NULL, GL_DYNAMIC_DRAW);


	glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(G3SIM_Vertex_t), (void*)offsetof(G3SIM_Vertex_t, x));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(G3SIM_Vertex_t), (void*)offsetof(G3SIM_Vertex_t, s));
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(G3SIM_Vertex_t), (void*)offsetof(G3SIM_Vertex_t, r));

	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

void G3SIM_FlushArray()
{
	if(s_G3DrawCurVertIdx == 0) {
		// No verts to draw
		return;
	}

	TracyCZone(FlushArrayZone, 1);

	if( s_texImageParam.textureFormat != GX_TEXFMT_NONE )
	{
		GLuint g3TextureId;
		g3TextureId = SIM_GetTextureID();
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture( GL_TEXTURE_2D, g3TextureId );

		if(s_texImageParam.flipS)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		} else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		}

		if(s_texImageParam.flipT)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		} else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}
		
		
		//Convert the DS texture data into a format opengl can understand
		memset(s_SIM_g3tex, 0, 4*s_texImageParam.textureSSize * s_texImageParam.textureTSize);
		u8 * vramTex = getTextureVramBank() + s_texImageParam.textureOffset;
		u8 * vramPltt = getTexPlttVramBank() + s_texPlttBase;
		u16 * colorAddr = (u16*)vramPltt;


		switch( s_texImageParam.textureFormat ){
			case GX_TEXFMT_A3I5:
				G3SIM_DecodeTexA3I5(vramTex, colorAddr, s_SIM_g3tex, s_texImageParam.textureSSize, s_texImageParam.textureTSize);
				//TODO
				break;
			case GX_TEXFMT_PLTT4:
				G3SIM_DecodeTex4(vramTex, colorAddr, s_SIM_g3tex, s_texImageParam.textureSSize, s_texImageParam.textureTSize);
				//TODO
				break;
			case GX_TEXFMT_PLTT16:
				G3SIM_DecodeTex16(vramTex, colorAddr, s_SIM_g3tex, s_texImageParam.textureSSize, s_texImageParam.textureTSize);
				break;
			case GX_TEXFMT_PLTT256:
				G3SIM_DecodeTex256(vramTex, colorAddr, s_SIM_g3tex, s_texImageParam.textureSSize, s_texImageParam.textureTSize);
				break;
			case GX_TEXFMT_COMP4x4:
				SIM_assert_always();
				//TODO
				break;
			case GX_TEXFMT_A5I3:
				G3SIM_DecodeTexA5I3(vramTex, colorAddr, s_SIM_g3tex, s_texImageParam.textureSSize, s_texImageParam.textureTSize);
				//TODO
				break;
			case GX_TEXFMT_DIRECT:
				G3SIM_DecodeTexDirect(vramTex, s_SIM_g3tex, s_texImageParam.textureSSize, s_texImageParam.textureTSize);
				break;
		}
		

		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, s_texImageParam.textureSSize, s_texImageParam.textureTSize, GL_RGBA, GL_UNSIGNED_BYTE, (void *)s_SIM_g3tex);

		GLint texUnitLoc = glGetUniformLocation(g3shaderProgramID, "myTexture");
		//set texture 0 in the shader
		glProgramUniform1i(g3shaderProgramID, texUnitLoc , 0);

		//Enable texture in the shader
		glProgramUniform1i(g3shaderProgramID, glGetUniformLocation(g3shaderProgramID, "useTexture") , 1);
	}
	else
	{
		//Disable texture in the shader
		glProgramUniform1i(g3shaderProgramID, glGetUniformLocation(g3shaderProgramID, "useTexture") , 0);							
	}

	// Set the polygon mode in the shader
	GLint polygonModeLoc = glGetUniformLocation(g3shaderProgramID, "polygonMode");
	glProgramUniform1i(g3shaderProgramID, polygonModeLoc , s_curPolygonAttr.polygonMode);

	// Set the fog attributes in the shader
	GLint useFogLoc = glGetUniformLocation(g3shaderProgramID, "useFog");
	int useFog = getbit(s_reg_G3X_DISP3DCNT, 7);
	if(!s_curPolygonAttr.fogEnable) {
		useFog = 0;
	}
	glProgramUniform1i(g3shaderProgramID, useFogLoc , useFog);
	if(useFog) {
		// Fog color
		GLint fogColorLoc = glGetUniformLocation(g3shaderProgramID, "fogColor");
		u32 fogRed = s_reg_G3X_FOG_COLOR & 0b11111;
		u32 fogGreen = (s_reg_G3X_FOG_COLOR >> 5) & 0b11111;
		u32 fogBlue = (s_reg_G3X_FOG_COLOR >> 10) & 0b11111;
		u32 fogAlpha = (s_reg_G3X_FOG_COLOR >> 16) & 0b11111;
		float fogRedF = (float)fogRed / 32.0f;
		float fogGreenF = (float)fogGreen / 32.0f;
		float fogBlueF = (float)fogBlue / 32.0f;
		float fogAlphaF = (float)fogAlpha / 32.0f;
		glProgramUniform4f(g3shaderProgramID, fogColorLoc, fogRedF, fogGreenF, fogBlueF, fogAlphaF);


		u32 fogShift = (s_reg_G3X_DISP3DCNT >> 8) & 0b1111;
		u32 fogStep = 0x400 >> fogShift;

		//Fog depth boundaries
		u32 fogOffset = s_reg_G3X_FOG_OFFSET;

		float fogDepthBoundary[32];
		float fogDensity[32];
		u8 * fogTablePtr = (u8*)&s_reg_G3X_FOG_TABLE_0;
		for(int i=0; i < 32; i++) {
			fogDepthBoundary[i] = ((float)(fogOffset + fogStep*(i+1)) / 32767.0f) * 2.0f - 1.0f;
			fogDensity[i] = (float)fogTablePtr[i] / 127.0f;
		}

		GLint fogDepthBoundaryLoc = glGetUniformLocation(g3shaderProgramID, "fogDepthBoundary");
		glProgramUniform1fv(g3shaderProgramID, fogDepthBoundaryLoc, 32, fogDepthBoundary);

		GLint fogDensityLoc = glGetUniformLocation(g3shaderProgramID, "fogDensity");
		glProgramUniform1fv(g3shaderProgramID, fogDensityLoc, 32, fogDensity);
	}



	// If this is a translucent draw, save it for the end of the frame to render it last
	if(
		( s_texImageParam.textureFormat == GX_TEXFMT_A3I5 
	   || s_texImageParam.textureFormat == GX_TEXFMT_A5I3
	   || s_curPolygonAttr.alpha < 0.99f )
	 && s_G3DrawCurVertIdx != 0) {
		g3_draw_item_t * item = &s_G3DrawItemList[s_G3DrawItemListCount];

		memset(item, 0, sizeof(g3_draw_item_t));


		if(s_texImageParam.textureFormat != GX_TEXFMT_NONE) {
			//Allocate and store off the texture
			glGenTextures(1, &item->textureId);
			glBindTexture(GL_TEXTURE_2D, item->textureId);
        	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
        	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, s_texImageParam.textureSSize, s_texImageParam.textureTSize, 0,
        	         GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, s_texImageParam.textureSSize, s_texImageParam.textureTSize, GL_RGBA, GL_UNSIGNED_BYTE, (void *)s_SIM_g3tex);
		}

		//Copy over the polygonattr
		memcpy(&item->polygonAttr, &s_curPolygonAttr, sizeof(G3SIM_PolygonAttr_t));

		//Copy over the texImageParam
		memcpy(&item->texImageParam, &s_texImageParam, sizeof(G3SIM_TexImageParam_t));

		//Allocate and copy the verts
		item->verts = malloc(sizeof(G3SIM_Vertex_t) * s_G3DrawCurVertIdx);
		item->vertsCount = s_G3DrawCurVertIdx;
		memcpy(item->verts, s_G3DrawVerts, sizeof(G3SIM_Vertex_t) * s_G3DrawCurVertIdx);

		s_G3DrawItemListCount++;
	} else {
		G3SIM_DrawArray();
	}

	G3SIM_DrawCleanUp();
	TracyCZoneEnd(FlushArrayZone);
}

void G3SIM_DrawItems()
{
	//TODO sort the items by Z coordinate


	//Draws all the translucent objects that were deferred to the end of the frame.
	for(int i=0; i<s_G3DrawItemListCount; i++) {
		g3_draw_item_t * item = &s_G3DrawItemList[i];
		if(item->textureId != 0){
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, item->textureId);
			//Enable texture in the shader
			glProgramUniform1i(g3shaderProgramID, glGetUniformLocation(g3shaderProgramID, "useTexture") , 1);
		} else {
			glProgramUniform1i(g3shaderProgramID, glGetUniformLocation(g3shaderProgramID, "useTexture") , 0);
		}

		if(item->texImageParam.flipS)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		} else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		}

		if(item->texImageParam.flipT)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		} else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}

		// Set the polygon mode in the shader
		GLint polygonModeLoc = glGetUniformLocation(g3shaderProgramID, "polygonMode");
		glProgramUniform1i(g3shaderProgramID, polygonModeLoc, item->polygonAttr.polygonMode);

		glBindVertexArray(s_G3DrawVertexArray);
		glBindBuffer(GL_ARRAY_BUFFER, s_G3DrawVertexBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(G3SIM_Vertex_t) * item->vertsCount, item->verts);
		glDrawArrays(GL_TRIANGLES,0, item->vertsCount);

		free(item->verts);

		if(item->textureId != 0) {
			glDeleteTextures(1, &item->textureId);
		}
	}
	s_G3DrawItemListCount = 0;
}
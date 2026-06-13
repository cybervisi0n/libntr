#include <simulator/glad/glad.h>
#include <nitro/types.h>

#ifndef SIM_G3_DRAW_H
#define SIM_G3_DRAW_H

#define G3_DRAW_MAX_VERTS 12000

typedef struct {
	//Position
	GLfloat x;
	GLfloat y;
	GLfloat z;
	GLfloat w;
	//TexCoord
	GLfloat s;
	GLfloat t;
	//Color
	GLfloat r;
	GLfloat g;
	GLfloat b;
	GLfloat a;
} G3SIM_Vertex_t;

void G3SIM_AddVtx(G3SIM_Vertex_t * vtx);
void G3SIM_DrawArray();
void G3SIM_DrawCleanUp();
void G3SIM_DrawInit();
void G3SIM_FlushArray();
void G3SIM_DrawItems();

#endif
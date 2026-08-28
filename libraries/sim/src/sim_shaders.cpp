#include <nitro.h>
#include <simulator/g3_handler.h>

//Shader
namespace SIM {

const char * G3VertexShader =
#include "shaders/g3_vertex.glsl"
;

const char * G3FragmentShader = 
#include "shaders/g3_fragment.glsl"
;

const char * G2VertexShader = 
#include "shaders/g2_vertex.glsl"
;

const char * G2FragmentShader =
#include "shaders/g2_fragment.glsl"
;

}


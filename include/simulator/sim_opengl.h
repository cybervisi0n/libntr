#ifndef SIM_OPENGL_H
#define SIM_OPENGL_H

#ifdef SDK_BUILD_PROSPERO
#define GL_GLEXT_PROTOTYPES 1
#include <GL/gl.h>
#elif SDK_BUILD_NX
#include <glad/glad.h>
#else
#include <simulator/glad/glad.h>
#endif

#endif

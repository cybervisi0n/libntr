#ifndef SIM_INTERNAL_HPP
#define SIM_INTERNAL_HPP

#include <cstddef>

#include "simulator/sim_opengl.h"

// TODO: turn these into real functions

#define CompileShaderCommon(_id, _source)                                      \
glShaderSource(_id, 1, &_source, NULL);                                      \
glCompileShader(_id);

#define CompileVertexShader(_id, _source)                                      \
_id = glCreateShader(GL_VERTEX_SHADER);                                      \
CompileShaderCommon(_id, _source);

#define CompileFragmentShader(_id, _source)                                    \
_id = glCreateShader(GL_FRAGMENT_SHADER);                                    \
CompileShaderCommon(_id, _source);

#define LinkShader(_id, _vertex, _fragment)                                    \
glAttachShader(_id, _vertex);                                                \
glAttachShader(_id, _fragment);                                              \
glLinkProgram(_id);                                                          \
glValidateProgram(_id);                                                      \
glUseProgram(_id);

#endif
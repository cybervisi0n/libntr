#include <simulator/sim_g3_GlRenderer.hpp>

#include <simulator/sim_g3_CommandHandler.hpp>
#include <simulator/assert.h>
#include <string.h>
#include <stddef.h>
#include <memory>

#include <nitro.h>
#include <nitro/gx/g3.h>
#include <nitro/gx/gx_load.h>
#include <nitro/gx/gx_vramcnt.h>
#include <unordered_map>
#include <simulator/sim_crc32.h>
#include "simulator/sim_g3_TextureCache.hpp"
#include "simulator/sim_g3_Texture.hpp"

#ifdef SDK_BUILD_LINUX
#include <signal.h>
#endif

#ifdef SDK_TRACY_ENABLE
#include "tracy/TracyC.h"
#endif

#define getbit(x,n) ( ( (x) >> (n) ) & 1 )

namespace SIM::G3 {

GlRenderer::GlRenderer() {}

GlRenderer::~GlRenderer() {}

void GlRenderer::Init() {
    glGenVertexArrays(1, &mVertexArray);
    glBindVertexArray(mVertexArray);

    glGenBuffers(1, &mVertexBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(RenderVertex), (void*)offsetof(RenderVertex, x));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(RenderVertex), (void*)offsetof(RenderVertex, s));
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(RenderVertex), (void*)offsetof(RenderVertex, r));

    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void GlRenderer::Draw(std::shared_ptr<DrawCall> drawCall) {
    size_t vertsCount = drawCall->GetRenderVertsCount();
    if( vertsCount != 0 )
    {
        glBindVertexArray(mVertexArray);
        glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);

        if(mVertexBufferCount >= vertsCount) {
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(RenderVertex) * vertsCount, drawCall->GetRenderVerts());
        } else {
            glBufferData(GL_ARRAY_BUFFER, sizeof(RenderVertex) * vertsCount, drawCall->GetRenderVerts(), GL_DYNAMIC_DRAW);
            mVertexBufferCount = vertsCount;
        }

        // Handle texture
        if(drawCall->GetTexture() == nullptr) {
            glProgramUniform1i(mShaderProgramId, glGetUniformLocation(mShaderProgramId, "useTexture") , 0);	
        } else {
            drawCall->GetTexture()->Activate();

            auto texImageParam = drawCall->GetTexImageParam();

            if(texImageParam.repeatS) {
                if(texImageParam.flipS)
                {
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
                } else {
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                }
            } else {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            }

            if(texImageParam.repeatT) {
                if(texImageParam.flipT)
                {
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
                } else {
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                }
            } else {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            }
            
            GLint texUnitLoc = glGetUniformLocation(mShaderProgramId, "myTexture");
            //set texture 0 in the shader
            glProgramUniform1i(mShaderProgramId, texUnitLoc , 0);

            //Enable texture in the shader
            glProgramUniform1i(mShaderProgramId, glGetUniformLocation(mShaderProgramId, "useTexture") , 1);
        }

        GLint polygonModeLoc = glGetUniformLocation(mShaderProgramId, "polygonMode");
        glProgramUniform1i(mShaderProgramId, polygonModeLoc , drawCall->GetPolygonAttr().polygonMode);

        // Fog uniforms
        GLint useFogLoc = glGetUniformLocation(mShaderProgramId, "useFog");
        glProgramUniform1i(mShaderProgramId, useFogLoc , drawCall->GetFogEnabled());
        if(drawCall->GetFogEnabled()) {
            GLint fogColorLoc = glGetUniformLocation(mShaderProgramId, "fogColor");
            glProgramUniform1fv(mShaderProgramId, fogColorLoc, 4, drawCall->GetFogColor());
            GLint fogDepthBoundaryLoc = glGetUniformLocation(mShaderProgramId, "fogDepthBoundary");
            glProgramUniform1fv(mShaderProgramId, fogDepthBoundaryLoc, 32, drawCall->GetFogDepthTable());

            GLint fogDensityLoc = glGetUniformLocation(mShaderProgramId, "fogDensity");
            glProgramUniform1fv(mShaderProgramId, fogDensityLoc, 32, drawCall->GetFogDensityTable());
        }

        glDrawArrays(GL_TRIANGLES, 0, vertsCount);
    }
}

}
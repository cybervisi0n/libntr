#include "sim_TouchCursor.hpp"

#include "simulator/sim.h"
#include "simulator/sim_opengl.h"
#include "sim_internal.hpp"

#include <SDL2/SDL.h>

const char * CursorVertexShader =
#include "shaders/simple_vertex.glsl"
;

const char * CursorFragmentShader =
#include "shaders/simple_fragment.glsl"
;

static SIM::TouchCursor sTouchCursor;

static constexpr auto CursorSizePixels = 5;

namespace SIM {

TouchCursor::TouchCursor() {}
TouchCursor::~TouchCursor() {}

void TouchCursor::DeltaX(int x) {
    mX = mX + x;
}

void TouchCursor::DeltaY(int y) {
    mY = mY + y;
}

void TouchCursor::Init() {
    glGenVertexArrays(1, &mVertexArray);
    glBindVertexArray(mVertexArray);

    glGenBuffers(1, &mVertexBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    mShaderProgramId = glCreateProgram();
    CompileVertexShader(mVertexShader, CursorVertexShader);
    CompileFragmentShader(mFragmentShader, CursorFragmentShader);
    LinkShader(mShaderProgramId, mVertexShader, mFragmentShader);
}

void TouchCursor::Draw() {
    Vertex cursorVerts[6];


    // Get the touch panel coordinates 
    const u32 * touchPanelCoords = SIM_GetTouchPanelCoords();
    /* u32[4] = top left (x,y) bottom right (x,y)*/

    // Limit coords to inside the touch panel
    if(mX < touchPanelCoords[0]) {
        mX = touchPanelCoords[0];
    }

    if(mY < touchPanelCoords[1]) {
        mY = touchPanelCoords[1];
    }

    if(mX > touchPanelCoords[2]) {
        mX = touchPanelCoords[2];
    }

    if(mY > touchPanelCoords[3]) {
        mY = touchPanelCoords[3];
    }


    auto sdlWindow = SIM_GetSDLWindow();
    int windowWidth = 0;
    int windowHeight = 0;

    SDL_GetWindowSizeInPixels(sdlWindow, &windowWidth, &windowHeight);

    float cursorDeltaX = ((float)CursorSizePixels / windowWidth);
    float cursorDeltaY = ((float)CursorSizePixels / windowHeight);

    float cursorCenterX = (((float)mX / windowWidth) * 2.0f) - 1.0f;
    float cursorCenterY = (((float)mY / windowHeight) * 2.0f) - 1.0f;







    cursorVerts[0] = {
        cursorCenterX - cursorDeltaX,
        cursorCenterY - cursorDeltaY,
        1.0f,
        0.0f,
        0.0f,
    };
    cursorVerts[1] = {
        cursorCenterX - cursorDeltaX,
        cursorCenterY + cursorDeltaY,
        1.0f,
        0.0f,
        0.5f,
    };
    cursorVerts[2] = {
        cursorCenterX + cursorDeltaX,
        cursorCenterY - cursorDeltaY,
        1.0f,
        1.0f,
        0.0f,
    };
    cursorVerts[3] = {
        cursorCenterX + cursorDeltaX,
        cursorCenterY - cursorDeltaY,
        1.0f,
        1.0f,
        0.0f,
    };
    cursorVerts[4] = {
        cursorCenterX + cursorDeltaX,
        cursorCenterY + cursorDeltaY,
        1.0f,
        1.0f,
        0.5f,
    };
    cursorVerts[5] = {
        cursorCenterX - cursorDeltaX,
        cursorCenterY + cursorDeltaY,
        1.0f,
        0.0f,
        0.5f,
    };

    glUseProgram(mShaderProgramId);
    glBindVertexArray(mVertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);

    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 6, cursorVerts, GL_STATIC_DRAW);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}



TouchCursor& GetTouchCursor() {
    return sTouchCursor;
}

}
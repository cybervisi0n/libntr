#include <nitro.h>
#include <simulator/g3_handler.h>

//Shader
const char * G3SIM_VertexShader =
"#version 420 core\n\
layout (location = 0) in vec3 position;\n\
layout (location = 1) in vec2 texCoord;\n\
layout (location = 2) in vec4 inColor;\n\
smooth out vec2 fragTexCoord;\n\
smooth out vec4 fragInColor;\n\
smooth out vec4 coords;\n\
void main()\n\
{\n\
    gl_Position = vec4(position.x, position.y, position.z, 1.0);\n\
    coords = vec4(position.x, position.y, position.z, 1.0);\n\
    fragTexCoord = vec2(texCoord.x, texCoord.y);\n\
    fragInColor = inColor;\n\
}\n\
";

const char * G3SIM_FragmentShader = 
"#version 420 core\n\
uniform sampler2D myTexture;\n\
uniform int useTexture;\n\
uniform int polygonMode;\n\
uniform int useFog;\n\
uniform vec4 fogColor;\n\
uniform float fogDepthBoundary[32];\n\
uniform float fogDensity[32];\n\
smooth in vec2 fragTexCoord;\n\
smooth in vec4 fragInColor;\n\
smooth in vec4 coords;\n\
out vec4 color;\n\
void main()\n\
{\n\
    vec4 shadedColor;\n\
    if( useTexture == 1 ) {\n\
        if(polygonMode == 0) {\n\
            shadedColor.r = texture(myTexture, fragTexCoord).r * fragInColor.r;\n\
            shadedColor.g = texture(myTexture, fragTexCoord).g * fragInColor.g;\n\
            shadedColor.b = texture(myTexture, fragTexCoord).b * fragInColor.b;\n\
            shadedColor.a = texture(myTexture, fragTexCoord).a * fragInColor.a;\n\
        } else {\n\
            shadedColor = texture(myTexture, fragTexCoord);\n\
        }\n\
        if( texture(myTexture, fragTexCoord).a < 0.05) {\n\
            discard;\n\
        }\n\
    } else {\n\
        shadedColor = fragInColor;\n\
    }\n\
    \n\
    if(useFog == 1) {\n\
        float fog_maxdist = 1.0;\n\
        float fog_mindist = 0.5;\n\
        float dist = length(coords.xyz);\n\
        float fogFactor = 0.0;\n\
        if(coords.z < fogDepthBoundary[0]) {\n\
            \n\
            fogFactor = fogDensity[0];\n\
        } else {\n\
            int foundIdx = 31;\n\
            for(int i=1; i<32; i++) {\n\
                if(coords.z < fogDepthBoundary[i]) {\n\
                    foundIdx = i;\n\
                    break;\n\
                }\n\
            }\n\
            if(foundIdx == 31) {\n\
                fogFactor = fogDensity[foundIdx];\n\
            } else {\n\
                float interpolationFactor = (fogDepthBoundary[foundIdx-1] - coords.z) / (fogDepthBoundary[foundIdx] - fogDepthBoundary[foundIdx-1]);\n\
                fogFactor = (fogDensity[foundIdx - 1] * (interpolationFactor)) + (fogDensity[foundIdx] * (1.0-interpolationFactor));\n\
            }\n\
        }\n\
        color = mix(shadedColor, fogColor, fogFactor);\n\
    } else {\n\
     color = shadedColor;\n\
    }\n\
}\n\
";

const char * G2SIM_VertexShader = 
"#version 420 core\n\
layout (location = 0) in vec3 position;\n\
layout (location = 1) in vec2 texCoord;\n\
smooth out vec2 fragTexCoord;\n\
void main()\n\
{\n\
    gl_Position = vec4(position.x, position.y, position.z, 1.);\n\
    fragTexCoord = vec2(texCoord.x, texCoord.y);\n\
}\n\
";
const char * G2SIM_FragmentShader = 
"#version 420 core\n\
uniform sampler2D myTexture;\n\
uniform sampler2D bgTexture[4];\n\
uniform sampler2D objTexture[5]; //The extra element is for OBJs drawn on top of all BGs\n\
uniform sampler2D objWindowTexture;\n\
uniform int bgOrder[4];\n\
uniform int bgPriorities[4];\n\
uniform int bldcnt;\n\
uniform int bldalpha;\n\
uniform int bldy;\n\
uniform int topScreen;\n\
uniform vec4 backdropColor;\n\
uniform int dispcnt;\n\
uniform int winout;\n\
uniform int bg0as3d;\n\
\n\
smooth in vec2 fragTexCoord;\n\
//uniform float inAlpha;\n\
uniform float inMasterBrightFactor;\n\
uniform int inMasterBrightMode;\n\
out vec4 color;\n\
void main()\n\
{\n\
    int oamsDrawn = 0;\n\
    int currentBgDrawn = 5; // Backdrop\n\
    bool objWindowEnabled = ((dispcnt >> 15) & 1) == 1;\n\
    if(topScreen == 1) {\n\
        if(fragTexCoord.t < 0.5){\n\
            discard;\n\
        }\n\
    } else {\n\
        if(fragTexCoord.t >= 0.5){\n\
            discard;\n\
        }\n\
    }\n\
    for(int i=0; i<4; i++) {\n\
        int bgNum = bgOrder[i];\n\
        int curPrio = bgPriorities[bgNum];\n\
        // Draw the OBJs that are at or below the current BG priority\n\
        for(int j=3-oamsDrawn; j>curPrio; j--) {\n\
            if(texture(objTexture[j], fragTexCoord).a > 0.1) {\n\
                color = texture(objTexture[j], fragTexCoord);\n\
                currentBgDrawn = 4;\n\
            }\n\
            oamsDrawn++;\n\
        }\n\
        \n\
        // Check obj window\n\
        if(objWindowEnabled && texture(objWindowTexture, fragTexCoord).a > 0.1)\n\
        {\n\
            // Inside the window\n\
            if(((winout >> (bgNum+8)) & 1) == 0) {\n\
                continue;\n\
            }\n\
        } else if (objWindowEnabled) {\n\
            // Outside the window\n\
            if(((winout >> bgNum) & 1) == 0) {\n\
                continue;\n\
            }\n\
        }\n\
        // Draw the current bg\n\
        if(texture(bgTexture[bgNum], fragTexCoord).a > 0.1) {\n\
            //Handle blending\n\
            if( (((bldcnt >> 6) & 0x3) == 1) && ((bldcnt >> bgNum & 1) == 1) ){\n\
                // Alpha Blending Mode. bgNum is Target 1.\n\
                float alphaA = min(16.0, float(bldalpha & 31)) / 16.0;\n\
                float alphaB = min(16.0, float((bldalpha >> 8) & 31)) / 16.0;\n\
                if(bgNum == 0 && bg0as3d != 0) {\n\
                    alphaA = texture(bgTexture[0], fragTexCoord).a;\n\
                    alphaB = 1.0 - alphaA;\n\
                }\n\
                if((currentBgDrawn < 4) && (((bldcnt >> (8+currentBgDrawn)) & 1) == 1) ) {\n\
                    color.r = min(1.0, ((texture(bgTexture[bgNum], fragTexCoord).r *alphaA) + (texture(bgTexture[currentBgDrawn], fragTexCoord).r * alphaB)));\n\
                    color.g = min(1.0, ((texture(bgTexture[bgNum], fragTexCoord).g *alphaA) + (texture(bgTexture[currentBgDrawn], fragTexCoord).g * alphaB)));\n\
                    color.b = min(1.0, ((texture(bgTexture[bgNum], fragTexCoord).b *alphaA) + (texture(bgTexture[currentBgDrawn], fragTexCoord).b * alphaB)));\n\
                    color.a = 1.0;\n\
                    currentBgDrawn = bgNum;\n\
                } else if( (currentBgDrawn == 4) && ( ( (bldcnt >> (8+currentBgDrawn)) & 1) == 1) ){\n\
                    // Alpha blending where the highest OBJ pixel is the target\n\
                    color.r = min(1.0, ( (texture(bgTexture[bgNum], fragTexCoord).r *alphaA) + color.r * alphaB) );\n\
                    color.g = min(1.0, ( (texture(bgTexture[bgNum], fragTexCoord).g *alphaA) + color.g * alphaB) );\n\
                    color.b = min(1.0, ( (texture(bgTexture[bgNum], fragTexCoord).b *alphaA) + color.b * alphaB) );\n\
                    color.a = 1.0;\n\
                } else if((currentBgDrawn == 5) && (((bldcnt >> (8+currentBgDrawn)) & 1) == 1)) {\n\
                    // Alpha blending where the backdrop is the target\n\
                    color.r = min(1.0, ((texture(bgTexture[bgNum], fragTexCoord).r *alphaA) + backdropColor.r * alphaB));\n\
                    color.g = min(1.0, ((texture(bgTexture[bgNum], fragTexCoord).g *alphaA) + backdropColor.g * alphaB));\n\
                    color.b = min(1.0, ((texture(bgTexture[bgNum], fragTexCoord).b *alphaA) + backdropColor.b * alphaB));\n\
                    color.a = 1.0;\n\
                } else {\n\
                    color = texture(bgTexture[bgNum], fragTexCoord);\n\
                    currentBgDrawn = bgNum;\n\
                }\n\
            } else if((((bldcnt >> 6) & 0x3) == 2) && ((bldcnt >> bgNum & 1) == 1)) {\n\
                // Brightness Increase Mode. bgNum is Target 1.\n\
                float brightFactor = min(16.0, float(bldy & 31)) / 16.0;\n\
                //color.r = min(1.0, ( texture(bgTexture[bgNum], fragTexCoord).r + ((1.0 - texture(bgTexture[bgNum], fragTexCoord).r)*brightFactor)) );\n\
                //color.g = min(1.0, ( texture(bgTexture[bgNum], fragTexCoord).g + ((1.0 - texture(bgTexture[bgNum], fragTexCoord).g)*brightFactor)) );\n\
                //color.b = min(1.0, ( texture(bgTexture[bgNum], fragTexCoord).b + ((1.0 - texture(bgTexture[bgNum], fragTexCoord).b)*brightFactor)) );\n\
                color.r = texture(bgTexture[bgNum], fragTexCoord).r + (1.0 - texture(bgTexture[bgNum], fragTexCoord).r)*brightFactor;\n\
                color.g = texture(bgTexture[bgNum], fragTexCoord).g + (1.0 - texture(bgTexture[bgNum], fragTexCoord).g)*brightFactor;\n\
                color.b = texture(bgTexture[bgNum], fragTexCoord).b + (1.0 - texture(bgTexture[bgNum], fragTexCoord).b)*brightFactor;\n\
                color.a = 1.0;\n\
                currentBgDrawn = bgNum;\n\
            } else if((((bldcnt >> 6) & 0x3) == 3) && ((bldcnt >> bgNum & 1) == 1)) {\n\
                // Brightness Decrease Mode. bgNum is Target 1.\n\
                float brightFactor = min(16.0, float(bldy & 31)) / 16.0;\n\
                color.r = max(0.0, ( texture(bgTexture[bgNum], fragTexCoord).r - ((texture(bgTexture[bgNum], fragTexCoord).r)*brightFactor)) );\n\
                color.g = max(0.0, ( texture(bgTexture[bgNum], fragTexCoord).g - ((texture(bgTexture[bgNum], fragTexCoord).g)*brightFactor)) );\n\
                color.b = max(0.0, ( texture(bgTexture[bgNum], fragTexCoord).b - ((texture(bgTexture[bgNum], fragTexCoord).b)*brightFactor)) );\n\
                color.a = 1.0;\n\
                currentBgDrawn = bgNum;\n\
            } else {\n\
                color.rgb = texture(bgTexture[bgNum], fragTexCoord).rgb;\n\
                color.a = 1.0;\n\
                currentBgDrawn = bgNum;\n\
            }\n\
        }\n\
    }\n\
    //Draw the remaining objs\n\
    for(int i=3-oamsDrawn; i>=0; i--) {\n\
        if(texture(objTexture[i], fragTexCoord).a > 0.1) {\n\
            color = texture(objTexture[i], fragTexCoord);\n\
            currentBgDrawn = 4;\n\
        }\n\
    }\n\
    if(currentBgDrawn == 5) {\n\
        color = backdropColor;\n\
    }\n\
    \n\
    if(inMasterBrightMode == 1) {\n\
        // brightness up\n\
        color.r = color.r + ((1.0 - color.r) * (inMasterBrightFactor/16.0));\n\
        color.g = color.g + ((1.0 - color.g) * (inMasterBrightFactor/16.0));\n\
        color.b = color.b + ((1.0 - color.b) * (inMasterBrightFactor/16.0));\n\
    } else if(inMasterBrightMode == 2) {\n\
        // brightness down\n\
        color.r = color.r - (color.r * (inMasterBrightFactor / 16.0));\n\
        color.g = color.g - (color.g * (inMasterBrightFactor / 16.0));\n\
        color.b = color.b - (color.b * (inMasterBrightFactor / 16.0));\n\
    }\n\
}\n\
";

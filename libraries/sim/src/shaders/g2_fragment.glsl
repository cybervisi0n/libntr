R""(
#version 420 core

uniform sampler2D myTexture;
uniform sampler2D bgTexture[4];
uniform sampler2D objTexture[5]; //The extra element is for OBJs drawn on top of all BGs
uniform sampler2D objWindowTexture;
uniform int bgOrder[4];
uniform int bgPriorities[4];
uniform int bldcnt;
uniform int bldalpha;
uniform int bldy;
uniform int topScreen;
uniform vec4 backdropColor;
uniform int dispcnt;
uniform int winout;
uniform int bg0as3d;

smooth in vec2 fragTexCoord;
//uniform float inAlpha;
uniform float inMasterBrightFactor;
uniform int inMasterBrightMode;
out vec4 color;
void main()
{
    int oamsDrawn = 0;
    int currentBgDrawn = 5; // Backdrop
    bool objWindowEnabled = ((dispcnt >> 15) & 1) == 1;
    if(topScreen == 1) {
        if(fragTexCoord.t < 0.5){
            discard;
        }
    } else {
        if(fragTexCoord.t >= 0.5){
            discard;
        }
    }
    for(int i=0; i<4; i++) {
        int bgNum = bgOrder[i];
        int curPrio = bgPriorities[bgNum];
        // Draw the OBJs that are at or below the current BG priority
        for(int j=3-oamsDrawn; j>curPrio; j--) {
            if(texture(objTexture[j], fragTexCoord).a > 0.1) {
                color = texture(objTexture[j], fragTexCoord);
                currentBgDrawn = 4;
            }
            oamsDrawn++;
        }
        
        // Check obj window
        if(objWindowEnabled && texture(objWindowTexture, fragTexCoord).a > 0.1)
        {
            // Inside the window
            if(((winout >> (bgNum+8)) & 1) == 0) {
                continue;
            }
        } else if (objWindowEnabled) {
            // Outside the window
            if(((winout >> bgNum) & 1) == 0) {
                continue;
            }
        }
        // Draw the current bg
        if(texture(bgTexture[bgNum], fragTexCoord).a > 0.1) {
            //Handle blending
            if( (((bldcnt >> 6) & 0x3) == 1) && ((bldcnt >> bgNum & 1) == 1) ){
                // Alpha Blending Mode. bgNum is Target 1.
                float alphaA = min(16.0, float(bldalpha & 31)) / 16.0;
                float alphaB = min(16.0, float((bldalpha >> 8) & 31)) / 16.0;
                if(bgNum == 0 && bg0as3d != 0) {
                    alphaA = texture(bgTexture[0], fragTexCoord).a;
                    alphaB = 1.0 - alphaA;
                }
                if((currentBgDrawn < 4) && (((bldcnt >> (8+currentBgDrawn)) & 1) == 1) ) {
                    color.r = min(1.0, ((texture(bgTexture[bgNum], fragTexCoord).r *alphaA) + (texture(bgTexture[currentBgDrawn], fragTexCoord).r * alphaB)));
                    color.g = min(1.0, ((texture(bgTexture[bgNum], fragTexCoord).g *alphaA) + (texture(bgTexture[currentBgDrawn], fragTexCoord).g * alphaB)));
                    color.b = min(1.0, ((texture(bgTexture[bgNum], fragTexCoord).b *alphaA) + (texture(bgTexture[currentBgDrawn], fragTexCoord).b * alphaB)));
                    color.a = 1.0;
                    currentBgDrawn = bgNum;
                } else if( (currentBgDrawn == 4) && ( ( (bldcnt >> (8+currentBgDrawn)) & 1) == 1) ){
                    // Alpha blending where the highest OBJ pixel is the target
                    color.r = min(1.0, ( (texture(bgTexture[bgNum], fragTexCoord).r *alphaA) + color.r * alphaB) );
                    color.g = min(1.0, ( (texture(bgTexture[bgNum], fragTexCoord).g *alphaA) + color.g * alphaB) );
                    color.b = min(1.0, ( (texture(bgTexture[bgNum], fragTexCoord).b *alphaA) + color.b * alphaB) );
                    color.a = 1.0;
                } else if((currentBgDrawn == 5) && (((bldcnt >> (8+currentBgDrawn)) & 1) == 1)) {
                    // Alpha blending where the backdrop is the target
                    color.r = min(1.0, ((texture(bgTexture[bgNum], fragTexCoord).r *alphaA) + backdropColor.r * alphaB));
                    color.g = min(1.0, ((texture(bgTexture[bgNum], fragTexCoord).g *alphaA) + backdropColor.g * alphaB));
                    color.b = min(1.0, ((texture(bgTexture[bgNum], fragTexCoord).b *alphaA) + backdropColor.b * alphaB));
                    color.a = 1.0;
                } else {
                    color = texture(bgTexture[bgNum], fragTexCoord);
                    currentBgDrawn = bgNum;
                }
            } else if((((bldcnt >> 6) & 0x3) == 2) && ((bldcnt >> bgNum & 1) == 1)) {
                // Brightness Increase Mode. bgNum is Target 1.
                float brightFactor = min(16.0, float(bldy & 31)) / 16.0;
                //color.r = min(1.0, ( texture(bgTexture[bgNum], fragTexCoord).r + ((1.0 - texture(bgTexture[bgNum], fragTexCoord).r)*brightFactor)) );
                //color.g = min(1.0, ( texture(bgTexture[bgNum], fragTexCoord).g + ((1.0 - texture(bgTexture[bgNum], fragTexCoord).g)*brightFactor)) );
                //color.b = min(1.0, ( texture(bgTexture[bgNum], fragTexCoord).b + ((1.0 - texture(bgTexture[bgNum], fragTexCoord).b)*brightFactor)) );
                color.r = texture(bgTexture[bgNum], fragTexCoord).r + (1.0 - texture(bgTexture[bgNum], fragTexCoord).r)*brightFactor;
                color.g = texture(bgTexture[bgNum], fragTexCoord).g + (1.0 - texture(bgTexture[bgNum], fragTexCoord).g)*brightFactor;
                color.b = texture(bgTexture[bgNum], fragTexCoord).b + (1.0 - texture(bgTexture[bgNum], fragTexCoord).b)*brightFactor;
                color.a = 1.0;
                currentBgDrawn = bgNum;
            } else if((((bldcnt >> 6) & 0x3) == 3) && ((bldcnt >> bgNum & 1) == 1)) {
                // Brightness Decrease Mode. bgNum is Target 1.
                float brightFactor = min(16.0, float(bldy & 31)) / 16.0;
                color.r = max(0.0, ( texture(bgTexture[bgNum], fragTexCoord).r - ((texture(bgTexture[bgNum], fragTexCoord).r)*brightFactor)) );
                color.g = max(0.0, ( texture(bgTexture[bgNum], fragTexCoord).g - ((texture(bgTexture[bgNum], fragTexCoord).g)*brightFactor)) );
                color.b = max(0.0, ( texture(bgTexture[bgNum], fragTexCoord).b - ((texture(bgTexture[bgNum], fragTexCoord).b)*brightFactor)) );
                color.a = 1.0;
                currentBgDrawn = bgNum;
            } else {
                color.rgb = texture(bgTexture[bgNum], fragTexCoord).rgb;
                color.a = 1.0;
                currentBgDrawn = bgNum;
            }
        }
    }
    //Draw the remaining objs
    for(int i=3-oamsDrawn; i>=0; i--) {
        if(texture(objTexture[i], fragTexCoord).a > 0.1) {
            color = texture(objTexture[i], fragTexCoord);
            currentBgDrawn = 4;
        }
    }
    if(currentBgDrawn == 5) {
        color = backdropColor;
    }
    
    if(inMasterBrightMode == 1) {
        // brightness up
        color.r = color.r + ((1.0 - color.r) * (inMasterBrightFactor/16.0));
        color.g = color.g + ((1.0 - color.g) * (inMasterBrightFactor/16.0));
        color.b = color.b + ((1.0 - color.b) * (inMasterBrightFactor/16.0));
    } else if(inMasterBrightMode == 2) {
        // brightness down
        color.r = color.r - (color.r * (inMasterBrightFactor / 16.0));
        color.g = color.g - (color.g * (inMasterBrightFactor / 16.0));
        color.b = color.b - (color.b * (inMasterBrightFactor / 16.0));
    }
}
)""
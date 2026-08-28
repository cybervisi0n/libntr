R""(
#version 420 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec4 inColor;

smooth out vec2 fragTexCoord;
smooth out vec4 fragInColor;
smooth out vec4 coords;

void main()
{
    gl_Position = vec4(position.x, position.y, position.z, 1.0);
    coords = vec4(position.x, position.y, position.z, 1.0);
    fragTexCoord = vec2(texCoord.x, texCoord.y);
    fragInColor = inColor;
}
)""

R""(
#version 420 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
smooth out vec2 fragTexCoord;

void main()
{
    gl_Position = vec4(position.x, position.y, position.z, 1.);
    fragTexCoord = vec2(texCoord.x, texCoord.y);
}
)""
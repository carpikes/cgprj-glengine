#version 330 core

layout(location = 0) in vec3 vpos;
out vec3 UVW;

uniform mat4 uViewProj;

void main()
{
    gl_Position = uViewProj * vec4(vpos, 1.0);
    UVW = -vpos;
}  

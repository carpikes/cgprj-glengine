#version 330 core

layout(location = 0) in vec3 vpos;
out vec3 UVW;

uniform mat4 uViewProj;

void main()
{
    vec4 pos = uViewProj * vec4(vpos, 1.0);
    gl_Position = pos.xyww;

    UVW = -vpos;
}  

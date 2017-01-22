#version 330 core

in vec3 UVW;

uniform samplerCube uSkybox;

out vec3 color;

void main()
{             
    color = texture(uSkybox, UVW).rgb;
}  

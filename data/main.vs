#version 330 core
layout(location = 0) in vec3 vpMs;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexUV;

uniform mat4 MVP;
uniform mat3 normalMatrix;
out vec2 UV;
out vec3 NORMAL;
out vec3 WORLDPOS;

void main() {
    gl_Position = MVP * vec4(vpMs,1);

    UV = vertexUV;
    NORMAL = normalize(normalMatrix * vertexNormal);
    WORLDPOS = vpMs;
}

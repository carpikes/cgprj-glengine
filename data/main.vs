#version 330 core
layout(location = 0) in vec3 vpMs;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexUV;

uniform mat4 uModelViewProj;
uniform mat4 uModelView;
uniform mat3 uNormalMatrix;

out vec2 UV;
out vec3 NORMAL;
out vec3 WORLDPOS;

void main() {
    UV = vertexUV;
    NORMAL = normalize(uNormalMatrix * vertexNormal);
    WORLDPOS = vec3(uModelView * vec4(vpMs,1));

    gl_Position = uModelViewProj * vec4(vpMs,1);
}

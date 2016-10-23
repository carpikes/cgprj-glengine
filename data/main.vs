#version 330 core

/*
   MS_ == ModelSpace
   WS_ == WorldSpace
   CS_ == CameraSpace
   PS_ == Projection Space
*/

layout(location = 0) in vec3 MS_vertexPosition;
layout(location = 1) in vec3 MS_vertexNormals;
layout(location = 2) in vec2 vertexUV;

uniform mat4 uModelViewProj;
uniform mat4 uModelToWorld;
uniform mat3 uWS_NormalMatrix;

out vec2 UV;
out vec3 WS_Normal;
out vec3 WS_Position;
out vec3 CS_EyeDirection;

uniform vec3 uWS_EyePos;

void main() {
    UV = vertexUV;
    WS_Normal = normalize(uWS_NormalMatrix * MS_vertexNormals);
    WS_Position = vec3(uModelToWorld * vec4(MS_vertexPosition,1));

    gl_Position = uModelViewProj * vec4(MS_vertexPosition,1); // MS -> PS
}

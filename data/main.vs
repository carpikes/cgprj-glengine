#version 330 core

/*
   VS_ == VertexSpace
   MS_ == ModelSpace
   WS_ == WorldSpace
   CS_ == CameraSpace
   PS_ == Projection Space
*/

layout(location = 0) in vec3 MS_vertexPosition;
layout(location = 1) in vec3 MS_vertexNormals;
layout(location = 2) in vec3 MS_vertexTangents;
layout(location = 3) in vec2 vertexUV;

uniform mat4 uModelViewProj;
uniform mat4 uModelToWorld;
uniform mat3 uWS_NormalMatrix;

out vec2 UV;
out vec3 WS_Normal;
out vec3 WS_Tangent;
out vec3 WS_Bitangent;
out vec3 WS_Position;
out vec3 CS_EyeDirection;

uniform vec3 uWS_EyePos;

void main() {
    vec4 rvp = vec4(MS_vertexPosition,1);

    UV = vertexUV;

    WS_Normal = normalize(uWS_NormalMatrix * MS_vertexNormals);
    WS_Tangent = normalize(uWS_NormalMatrix * MS_vertexTangents);
    WS_Bitangent = cross(WS_Tangent, WS_Normal);

    WS_Position = vec3(uModelToWorld * rvp);
    gl_Position = uModelViewProj * rvp; // MS -> PS
}

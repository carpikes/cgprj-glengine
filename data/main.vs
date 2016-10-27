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
layout(location = 2) in vec2 vertexUV;

uniform mat4 uModelViewProj;
uniform mat4 uModelToWorld;
uniform mat3 uWS_NormalMatrix;

out vec2 UV;
out vec3 WS_Normal;
out vec3 WS_Position;
out vec3 CS_EyeDirection;

uniform vec3 uWS_EyePos;
uniform float uTimer, uMaxHeight;
uniform int uTag;

mat4 rotationMatrix(vec3 a, float angle)
{
    a = normalize(a);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat4(oc * a.x * a.x + c, oc * a.x * a.y - a.z * s, 
                oc * a.z * a.x + a.y * s,  0.0,
                oc * a.x * a.y + a.z * s,  oc * a.y * a.y + c, 
                oc * a.y * a.z - a.x * s,  0.0,
                oc * a.z * a.x - a.y * s,  oc * a.y * a.z + a.x * s, 
                oc * a.z * a.z + c, 0.0,
                0.0, 0.0, 0.0, 1.0);
}

void main() {
    UV = vertexUV;
    WS_Normal = normalize(uWS_NormalMatrix * MS_vertexNormals);

    vec4 rvp = vec4(MS_vertexPosition,1);
    if(uTag != 0) {
        float c = cos(uTimer * 3.14f + sin(uTag) * 0.5f);
        float uu = uTimer / 2.0f + uTag;
        float h = MS_vertexPosition.y / (1.0f + uMaxHeight) / 5.0f;
        float rot = 0;
        mat4 rm = mat4(1);
        if(uTag % 2 == 0) {
            rot += (c * h) * 3.14f + uTag;
        } else {
          rvp.x += c * rvp.y / 10.0f;
          rvp.y /= 1.1f - abs(c) * 0.1f;
        }

        if(uTag % 5 == 1)
            rot += 3.14f / 2.0f * (floor(uu) + pow(fract(uu),6)) // ruota
                + sin((c * h) * 3.14f) * 3.0f;

        rm = rotationMatrix(vec3(0,1,0), rot / 2.0f);
        WS_Normal = normalize(vec3(rm * vec4(WS_Normal,0)));
        rvp = rm * rvp;
        //rvp.y += abs(c) - 0.1f;
    }

    WS_Position = vec3(uModelToWorld * rvp);

    gl_Position = uModelViewProj * rvp; // MS -> PS
}

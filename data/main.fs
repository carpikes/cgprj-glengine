#version 330 core

in vec2 UV;
in vec3 NORMAL;
in vec3 WORLDPOS;
out vec4 oColor;

uniform sampler2D uTexture;
uniform vec3 uLightPos, uLightRot;
uniform vec3 uEyePos;

void main() {
    vec4 t = texture(uTexture, UV).rgba;
    vec3 nNormal = normalize(NORMAL);
    vec4 diffuseFactor = clamp(t * dot(nNormal, -uLightRot), 0, 1);

    vec3 vertexToEye = normalize(uEyePos - WORLDPOS);
    vec3 lightReflect = normalize(reflect(uLightPos,  NORMAL));
    float specularFactor = pow(
        max(0.0f, dot( vertexToEye, lightReflect)), 
        20);

    vec4 specularColor = vec4(vec3(1,1,1) * specularFactor,1);
    vec4 ambientFactor = vec4(1,1,1,1) * 0.2;

    oColor = t * (ambientFactor + diffuseFactor) + specularColor;
}

#version 330 core

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

struct Light {
    int type;
    vec3 color;
    vec4 position;
    vec4 direction;

    vec3 attenuation; // (constant,linear,quadratic)
};

in vec2 UV;
in vec3 NORMAL;
in vec3 WORLDPOS;
out vec4 oColor;

uniform vec3 halfVector; // (fra camera e luce)

uniform sampler2D uTexture;
uniform vec3 uEyePos;
uniform vec3 uLightPos;
uniform vec3 uLightRot;
uniform Material uMaterial;
//uniform int uNumLights;
//uniform Light uLights[gl_MaxLights];
// TODO: ambient Color here

// TODO: questo e` per le directional. 
//       aggiungere un mega switch con i vari light model
void main() {
    vec4 t = texture(uTexture, UV).rgba;

    vec3 nLightRot = normalize(uLightRot);
    vec3 halfVector = normalize(uEyePos + uLightPos);

    float shininess = 100.0;
    float diffuse = max(0.0, dot(NORMAL, nLightRot));
    float specular = max(0.0, dot(NORMAL, halfVector));
    if(diffuse == 0.0)
        specular = 0.0;

    specular = pow(specular, shininess);
    vec3 uAmbientColor = vec3(1,1,1) * 0.3;
    vec3 uLightColor = vec3(1,1,1);
    vec3 uStrength = vec3(1,1,1);

    vec3 scatteredLight = uAmbientColor + uLightColor * diffuse;
    vec3 reflectedLight = uLightColor * specular * uStrength;

    vec3 rgb = t.rgb * scatteredLight + reflectedLight;
    oColor = vec4(rgb, t.a);


/*    vec4 diffuseFactor = clamp(t * dot(nNormal, -uLightRot), 0, 1);
    vec3 vertexToEye = normalize(uEyePos - WORLDPOS);
    vec3 lightReflect = normalize(reflect(uLightPos,  NORMAL));
    float specularFactor = pow(
        max(0.0f, dot( vertexToEye, lightReflect)), 
        20);

    vec4 specularColor = vec4(vec3(1,1,1) * specularFactor,1);
    vec4 ambientFactor = vec4(1,1,1,1) * 0.3;

    oColor = t * (ambientFactor + diffuseFactor) + specularColor;
*/
    // Reinhard <3
    // oColor *= 2.0;
    // oColor = oColor / (1.0 + oColor);
}

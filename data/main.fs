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

    vec3 lightPos = normalize(uLightPos - WORLDPOS);
    vec3 eyePos = normalize(-WORLDPOS);
    vec3 rVector = normalize(-reflect(lightPos,NORMAL));

    //vec3 halfVector = normalize(uEyePos + lightPos);

    float shininess = 10.0;
    float diffuse = max(0.0, dot(NORMAL, lightPos));
    float specular = max(0.0, dot(rVector, eyePos));
    if(diffuse == 0.0)
        specular = 0.0;

    specular = pow(specular, shininess);
    vec3 uAmbientColor = vec3(1,1,1) * 0.3;
    vec3 uLightColor = vec3(1,1,1);
    vec3 uStrength = vec3(1,1,1);

    vec3 scatteredLight = uAmbientColor + uLightColor * diffuse;
    vec3 reflectedLight = uLightColor * specular * uStrength;

    vec3 rgb = t.rgb * scatteredLight * 0.2 + reflectedLight;
    oColor = vec4(rgb, t.a);

    // Reinhard <3
    // oColor *= 2.0;
    // oColor = oColor / (1.0 + oColor);
}

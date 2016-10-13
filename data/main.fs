#version 330 core

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

struct PointLight {
    vec3 attenuation; // (constant,linear,quadratic)
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec2 UV;
in vec3 NORMAL;
in vec3 WORLDPOS;
in vec3 WWPOS;
out vec4 oColor;

uniform vec3 halfVector; // (fra camera e luce)

uniform sampler2D uTexture;
uniform vec3 uEyePos;
uniform vec3 uLightPos;
uniform vec3 uLightRot;
uniform Material uMaterial;

#define NR_POINT_LIGHTS 8
uniform PointLight pointLights[NR_POINT_LIGHTS];

vec3 addDirectionalLight(vec4 t, vec3 normal, vec3 eyePos) {
    vec3 lightPos = normalize(- uLightRot);
    vec3 rVector = reflect(lightPos, normal);

    float shininess = 30.0f;
    float diffuse = max(0.0f, dot(normal, lightPos));
    float specular = max(0.0f, dot(rVector, eyePos));
    if(diffuse == 0.0f)
        specular = 0.0f;

    specular = pow(specular, shininess);
    vec3 uAmbientColor = vec3(1,1,1) * 0.1f;
    vec3 uLightColor = vec3(1,1,1) * 0.8;
    vec3 uStrength = vec3(1,1,1);

    vec3 scatteredLight = uAmbientColor + uLightColor * diffuse;
    vec3 reflectedLight = uLightColor * specular * uStrength;

    return t.rgb * scatteredLight * 0.3 + reflectedLight;
}

// http://learnopengl.com/#!Lighting/Multiple-lights
vec3 addPointLight(vec4 t, PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1.0f);
    // Attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0f / (light.attenuation[0] + light.attenuation[1] * distance + 
  			     light.attenuation[2] * (distance * distance));    
    // Combine results
    vec3 ambient  = light.ambient  * vec3(t);
    vec3 diffuse  = light.diffuse  * diff * vec3(t);
    vec3 specular = light.specular * spec * vec3(1,1,1);
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

void main() {
    vec3 color;

    vec4 t = texture(uTexture, UV).rgba;
    vec3 eyePos = normalize(uEyePos - WORLDPOS);

    //color += addDirectionalLight(t, normalize(NORMAL), eyePos);

    for(int i=0;i<NR_POINT_LIGHTS;i++)
        color += addPointLight(t, pointLights[i], NORMAL, WORLDPOS, eyePos);

    oColor = vec4(color, t.a);
}

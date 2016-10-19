#version 330 core

struct Material {
    // TODO: emissiveColor + (emissiveTexture?)
    vec3 ambientColor, diffuseColor;
    float specularExponent;
    sampler2D ambientTexture, diffuseTexture, specularTexture;
    sampler2D bumpTexture, displacementTexture;
    uint flags;
};

struct PointLight {
    vec3 attenuation; // (constant,linear,quadratic)
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct AmbientLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 strength;
};

in vec2 UV;
in vec3 NORMAL;
in vec3 WORLDPOS;
out vec4 oColor;

uniform vec3 halfVector; // (fra camera e luce)

#define NR_POINT_LIGHTS 8
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform Material material;
uniform AmbientLight ambientLight;
uniform vec3 uEyePos;

vec3 addDirectionalLight(vec4 t, vec3 normal, vec3 eyePos) {
    vec3 lightPos = normalize(- ambientLight.direction);
    vec3 rVector = reflect(lightPos, normal);

    float diffuse = max(0.0f, dot(normal, lightPos));
    float specular = max(0.0f, dot(rVector, eyePos));
    if(diffuse == 0.0f)
        specular = 0.0f;

    specular = pow(specular, material.specularExponent);
    vec3 ambientColor = ambientLight.ambient;
    vec3 lightColor = ambientLight.diffuse;
    vec3 strength = ambientLight.strength;

    vec3 scatteredLight = ambientColor + lightColor * diffuse;
    vec3 reflectedLight = lightColor * specular * strength;

    return vec3(t) * scatteredLight + reflectedLight;
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
    vec3 color = vec3(0,0,0);

    vec4 t = texture(material.diffuseTexture, UV).rgba;
    vec3 eyePos = normalize(uEyePos - WORLDPOS);

    color += addDirectionalLight(t, normalize(NORMAL), eyePos);

    //for(int i=0;i<NR_POINT_LIGHTS;i++)
    //    color += addPointLight(t, pointLights[i], NORMAL, WORLDPOS, eyePos);

    oColor = vec4(color, t.a);
}

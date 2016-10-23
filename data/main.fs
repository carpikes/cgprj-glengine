#version 330 core

#define HAS_AMBIENT_TEXTURE     (1 << 0)
#define HAS_DIFFUSE_TEXTURE     (1 << 1)
#define HAS_SPECULAR_TEXTURE    (1 << 2)
#define HAS_BUMP_TEXTURE        (1 << 3)
#define HAS_DISPLACE_TEXTURE    (1 << 4)

#define NR_LIGHTS 8

struct Material {
    // TODO: emissiveColor + (emissiveTexture?)
    vec3      ambientColor;
    sampler2D ambientTexture;
    vec3      diffuseColor;
    sampler2D diffuseTexture;
    float     specularExponent;
    sampler2D specularTexture;
    sampler2D bumpTexture;
    sampler2D displacementTexture;
    int flags;
};

struct LightProperties {
    bool isEnabled;
    bool isLocal;
    bool isSpot;
    vec3 ambient;
    vec3 color;
    vec3 WS_position;
    vec3 WS_halfVector;

    vec3 coneDirection;
    float spotCosCutoff;
    float spotExponent;
    vec3 attenuation;
};

in vec2 UV;
in vec3 WS_Normal;
in vec3 WS_Position;
in vec3 CS_EyeDirection;
out vec4 oColor;

uniform LightProperties lights[NR_LIGHTS];
uniform Material material;
uniform vec3 uWS_EyePos;

vec3 lout_ambient, lout_diffuse, lout_specular;

// Reference: OpenGL Programming Guide 8th Edition (Pag 378)
void computeLight() {
    lout_ambient = vec3(0.0);
    lout_diffuse = vec3(0.0);
    lout_specular = vec3(0.0);

    for(int i=0; i < NR_LIGHTS; i++) {
        if(!lights[i].isEnabled)
            continue;

        vec3 WS_halfVector;
        vec3 WS_lightDirection = lights[i].WS_position;
        float attenuation = 1.0;

        if(lights[i].isLocal) {
            WS_lightDirection -= WS_Position;
            float lightDistance = length(WS_lightDirection);
            WS_lightDirection /= lightDistance;

            attenuation = 1.0 / (lights[i].attenuation[0]
                              +  lights[i].attenuation[1] * lightDistance
                              +  lights[i].attenuation[2] * lightDistance
                                                          * lightDistance);
            
            if(lights[i].isSpot) {
                // TODO normalize coneDirection
                float spotCos = dot(WS_lightDirection, -lights[i].coneDirection);
                if(spotCos < lights[i].spotCosCutoff)
                    attenuation = 0.0;
                else
                    attenuation *= pow(spotCos, lights[i].spotExponent);
            }

            vec3 cpos = normalize(-uWS_EyePos - WS_Position);
            WS_halfVector = normalize(WS_lightDirection + cpos);
        } else {
            WS_halfVector = lights[i].WS_halfVector;
        }

        float diffuse = max(0.0, dot(WS_Normal, WS_lightDirection));
        float specular = max(0.0, dot(WS_Normal, WS_halfVector));

        if(diffuse < 0.01)
            specular = 0.0;
        else
            specular = pow(specular, 30); // material.specularExponent);

        lout_ambient += lights[i].ambient * attenuation;
        lout_diffuse += lights[i].color * diffuse * attenuation;
        lout_specular += lights[i].color * specular * attenuation;
    }
}

vec3 computeMaterial() {
    vec3 ret = vec3(0,0,0);
    vec3 ambientColor = material.ambientColor;
    vec3 diffuseColor = material.diffuseColor;
    
    if((material.flags & HAS_AMBIENT_TEXTURE) > 0)
        ambientColor = texture(material.ambientTexture, UV).rgb; 
    else if((material.flags & HAS_DIFFUSE_TEXTURE) > 0)
        ambientColor = texture(material.diffuseTexture, UV).rgb; 

    if((material.flags & HAS_DIFFUSE_TEXTURE) > 0)
        diffuseColor = texture(material.diffuseTexture, UV).rgb; 

    ret += lout_ambient * ambientColor; 
    ret += lout_diffuse * diffuseColor;
    ret += lout_specular;
    return ret;
}

void main() {
    vec3 color = vec3(0,0,0);

    vec4 t = texture(material.diffuseTexture, UV).rgba;

    computeLight();
    color += computeMaterial();

    oColor = vec4(color, t.a);
}

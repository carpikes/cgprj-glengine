#version 330 core

/*
   VS_ == VertexSpace
   MS_ == ModelSpace
   WS_ == WorldSpace
   CS_ == CameraSpace
   PS_ == Projection Space
*/

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
uniform float uTimer;
uniform int uTag;

// Reference: OpenGL Programming Guide 8th Edition (Pag 378)
void computeLight() {
    lout_ambient = vec3(0.0);
    lout_diffuse = vec3(0.0);
    lout_specular = vec3(0.0);

    for(int i=0; i < NR_LIGHTS && i < 3; i++) {
        if(!lights[i].isEnabled)
            continue;

        vec3 WS_halfVector;
        vec3 VS_lightDirection = lights[i].WS_position;
        float attenuation = 1.0;

        if(lights[i].isLocal) {
            VS_lightDirection -= WS_Position;
            float lightDistance = length(VS_lightDirection);
            VS_lightDirection /= lightDistance;

            attenuation = 1.0 / (lights[i].attenuation[0]
                              +  lights[i].attenuation[1] * lightDistance
                              +  lights[i].attenuation[2] * lightDistance
                                                          * lightDistance);
            
//            if(lights[i].isSpot) {
//                // TODO normalize coneDirection
//                float spotCos = dot(VS_lightDirection, -lights[i].coneDirection);
//                if(spotCos < lights[i].spotCosCutoff)
//                    attenuation = 0.0;
//                else
//                    attenuation *= pow(spotCos, lights[i].spotExponent);
//            }

            vec3 VS_cameraPos = normalize(-uWS_EyePos - WS_Position);
            WS_halfVector = normalize(VS_lightDirection + VS_cameraPos);
//            if(abs(dot(VS_cameraPos, WS_Normal)) < 0.1) {
//                lout_ambient = vec3(0,0,0);
//                lout_diffuse = vec3(0,0,0);
//                lout_specular = vec3(0,0,0);
//                return;
//            }
        } else {
            WS_halfVector = lights[i].WS_halfVector;
        }

        float diffuse = max(0.0, dot(WS_Normal, VS_lightDirection));
        float specular = max(0.0, dot(WS_Normal, WS_halfVector));

        if(diffuse < 0.01)
            specular = 0.0;
        else
            specular = pow(specular, material.specularExponent);

        lout_ambient += lights[i].ambient * attenuation;
        lout_diffuse += lights[i].color * diffuse * attenuation;
        lout_specular += lights[i].color * specular * attenuation;
    }
}

float toon(float v) {
    v = (v + 0.1f) / 2.0f;
    v = fract(v) - (fract(v*10.0f)/10.0f);
    v = v * 2.0f;
    return v;
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

    float vv = (diffuseColor.r + diffuseColor.g + diffuseColor.b); //0.6f + (1.0f - abs(cos(uTimer * 3.1415))) * 0.8f;
    vv /= 3.0f;
    vv = pow(vv, 2.0f);
//    vv = clamp(vv, 0, 1);
    ret += (lout_ambient) * ambientColor; 
    ret += lout_diffuse * diffuseColor;
    ret += lout_specular * vv;

    float f = 1.4f;
    ret = vec3(pow(ret.x, f), pow(ret.y, f), pow(ret.z, f));
    ret = ret / (1.0f + ret);

    // TOON 
    //ret = vec3(toon(ret.r), toon(ret.g), toon(ret.b));

    return ret;
}

void main() {
    vec3 color = vec3(0,0,0);

    vec4 t = texture(material.diffuseTexture, UV).rgba;

    computeLight();

    color += computeMaterial();

    oColor = vec4(color, t.a);
}

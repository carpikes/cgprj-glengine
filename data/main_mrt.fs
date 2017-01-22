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
    vec3      specularColor;
    float     specularExponent;
    sampler2D specularTexture;
    sampler2D bumpTexture;
    sampler2D displacementTexture;
    int flags;
};

in vec2 UV;
in vec3 WS_Normal;
in vec3 WS_Tangent;
in vec3 WS_Bitangent;
in vec3 WS_Position;

layout (location = 0) out vec4 oPosition;
layout (location = 1) out vec3 oNormal;
layout (location = 2) out vec4 oAlbedoSpec;

uniform Material material;
uniform mat4 uModelToWorld;

vec3 WS_NormalPostProc = WS_Normal;
float specularComponent = 0.0f;

vec3 computeMaterial() {
    vec3 ret = vec3(0,0,0);
    vec3 ambientColor = material.ambientColor;
    vec3 diffuseColor = material.diffuseColor;
    vec3 specularColor = material.specularColor;
    
    if((material.flags & HAS_AMBIENT_TEXTURE) > 0)
        ambientColor = texture(material.ambientTexture, UV).rgb; 
    else if((material.flags & HAS_DIFFUSE_TEXTURE) > 0)
        ambientColor = texture(material.diffuseTexture, UV).rgb; 

    if((material.flags & HAS_DIFFUSE_TEXTURE) > 0)
        diffuseColor = texture(material.diffuseTexture, UV).rgb; 

    if((material.flags & HAS_SPECULAR_TEXTURE) > 0)
        specularColor = texture(material.specularTexture, UV).rgb; 

    ret += ambientColor; 
    ret += diffuseColor;
    specularComponent = 0.3 * specularColor.r 
                      + 0.59 * specularColor.g
                      + 0.11 * specularColor.b;

    return ret;
}

void computeNormal() {
    if((material.flags & HAS_BUMP_TEXTURE) > 0)
    {
        vec3 ncol = texture(material.bumpTexture, UV).rgb; 
        ncol = (2 * ncol) - 1;
        vec3 n = WS_Tangent * ncol.x + WS_Bitangent * ncol.y + WS_Normal * ncol.z;
        WS_NormalPostProc = normalize(n);
    }
}

void main() {
    vec3 color = vec3(0,0,0);

    vec3 t = texture(material.diffuseTexture, UV).rgb;

    computeNormal();
    color += computeMaterial();

    oPosition.rgb = WS_Position;
    oPosition.a = material.specularExponent;
    oNormal = WS_NormalPostProc;

    oAlbedoSpec.rgb = t * color;
    oAlbedoSpec.a = specularComponent;
}

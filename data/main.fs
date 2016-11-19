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

struct LightProperties {
    bool enabled;
    bool isSpot;
    vec3 ambient;
    vec3 color;
    vec3 WS_position;

    vec3 coneDirection;
    float spotCosCutoff;
    float spotExponent;
    vec3 attenuation;
};

struct AmbientLight {
    bool enabled;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
};

struct HemiLight {
    bool enabled;
    vec3 position;
    vec3 upColor;
    vec3 downColor;
};

in vec2 UV;
in vec3 WS_Normal;
in vec3 WS_Position;
in vec3 CS_EyeDirection;
out vec4 oColor;

uniform LightProperties lights[NR_LIGHTS];
uniform AmbientLight ambientLight;
uniform HemiLight hemiLight;

uniform Material material;
uniform vec3 uWS_EyePos;
uniform mat4 uModelToWorld;

vec3 lout_ambient, lout_diffuse, lout_specular;
uniform float uTimer;

void computeHemiLight() {
    if(!hemiLight.enabled)
        return;

    vec3 lightVec = normalize(hemiLight.position - WS_Position);
    float cosTheta = dot(WS_Normal, lightVec);
    float a = cosTheta * 0.5 + 0.5;
    lout_ambient += mix(hemiLight.downColor, hemiLight.upColor, a);
}

void computeAmbientLight() {
    if(!ambientLight.enabled)
        return;
    vec3 VS_lightDirection = normalize(ambientLight.direction);
    vec3 VS_cameraPos = normalize(-uWS_EyePos - WS_Position);
    vec3 VS_halfVector = normalize(VS_lightDirection + VS_cameraPos);

    float diffuse = max(0.0, dot(WS_Normal, VS_lightDirection));
    float specular = max(0.0, dot(WS_Normal, VS_halfVector));

    if(diffuse < 0.01)
        specular = 0.0;
    else
        specular = pow(specular, material.specularExponent);

    lout_ambient  += ambientLight.ambient;
    lout_diffuse  += ambientLight.diffuse * diffuse;
    lout_specular += ambientLight.diffuse * specular;
}

void computePointAndSpotlights() {
    for(int i=0; i < NR_LIGHTS; i++) {
        if(!lights[i].enabled)
            continue;

        vec3 VS_halfVector;
        vec3 VS_lightDirection = lights[i].WS_position;
        float attenuation = 1.0;

        VS_lightDirection -= WS_Position;
        float lightDistance = length(VS_lightDirection);
        VS_lightDirection /= lightDistance;

        attenuation = 1.0 / (lights[i].attenuation[0]
                            +  lights[i].attenuation[1] * lightDistance
                            +  lights[i].attenuation[2] * lightDistance
                                                        * lightDistance);
        
        if(lights[i].isSpot) {
            // TODO normalize coneDirection
            float spotCos = dot(VS_lightDirection, -lights[i].coneDirection);
            if(spotCos < lights[i].spotCosCutoff)
                attenuation = 0.0f;
            else
                attenuation *= pow(spotCos, lights[i].spotExponent);
        }

        vec3 VS_cameraPos = normalize(-uWS_EyePos - WS_Position);
        VS_halfVector = normalize(VS_lightDirection + VS_cameraPos);

        float diffuse = max(0.0, dot(WS_Normal, VS_lightDirection));
        float specular = max(0.0, dot(WS_Normal, VS_halfVector));

        if(diffuse < 0.01)
            specular = 0.0;
        else
            specular = pow(specular, material.specularExponent);

        lout_ambient += lights[i].ambient * attenuation;
        lout_diffuse += lights[i].color * diffuse * attenuation;
        lout_specular += lights[i].color * specular * attenuation;
    }
}

void computeLight() {
    lout_ambient = vec3(0.0);
    lout_diffuse = vec3(0.0);
    lout_specular = vec3(0.0);

    computeAmbientLight();
    computeHemiLight();
    computePointAndSpotlights();
}

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

    ret += lout_ambient * ambientColor; 
    ret += lout_diffuse * diffuseColor;
    ret += lout_specular * specularColor;

//    ret = ret / (1.0f + ret);
    return ret;
}

void main() {
    vec3 color = vec3(0,0,0);

    vec4 t = texture(material.diffuseTexture, UV).rgba;

    computeLight();
    color += computeMaterial();

    oColor = vec4(color, t.a);
}

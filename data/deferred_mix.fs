#version 330 core

/*
   VS_ == VertexSpace
   MS_ == ModelSpace
   WS_ == WorldSpace
   CS_ == CameraSpace
   PS_ == Projection Space
*/

#define NR_LIGHTS 10

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

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;

out vec3 oColor;

uniform LightProperties lights[NR_LIGHTS];
uniform AmbientLight ambientLight;
uniform HemiLight hemiLight;

uniform vec3 uWS_EyePos;

vec3 lout_ambient, lout_diffuse, lout_specular;
uniform float uTimer;

vec3 WS_Position = vec3(0);
vec3 WS_Normal = vec3(0);
float specularExponent = 1.0f;

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
        specular = pow(specular, specularExponent);

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
            specular = pow(specular, specularExponent);

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

void main() {
    vec3 color;
    float spec;

    color = texture(gAlbedo, UV).rgb;
    spec = texture(gNormal, UV).a;

    WS_Normal = texture(gNormal, UV).rgb;
    WS_Position = texture(gPosition, UV).rgb;
    specularExponent = texture(gPosition, UV).a;

    computeLight();

    oColor = lout_ambient * color + lout_diffuse * color + lout_specular * spec; 
    float val = texture(gAlbedo, UV).a;
    if(val == 0)
            discard;
}

#include <GLEngine/Light.h>

namespace GLEngine
{

bool AmbientLight::update(Shader& s, const Camera& c, const glm::mat4& wMat) {
    s.set("ambientLight.enabled", mEnabled);

    (void)wMat;
    (void)c;
    if(mEnabled) {
        //glm::vec3 mDirectionWS = glm::normalize(
        //    glm::vec3(wMat * glm::vec4(mDirection,0)));

        //glm::vec3 halfVector = glm::normalize(c.getCameraPos()) + 
        //    glm::normalize(mDirectionWS);
        //s.set("ambientLight.halfVector", glm::normalize(halfVector));
        s.set("ambientLight.direction", mDirection);
        s.set("ambientLight.ambient", mAmbientColor);
        s.set("ambientLight.diffuse", mDiffuseColor);
    }

    return true;
}

#ifdef _U
#error "_U is already defined"
#endif

#define _U(x,y) Utils::getUniformName("lights", x, y)
bool PointLight::update(int n, Shader& s) {
    if(n >= s.getMaxNumberOfLights()) {
        ERRP("Cannot enable light %d", n); 
        return false;
    }

    s.set(Utils::getUniformName("lights", n, "enabled"), mEnabled);

    if(mEnabled) {
        s.set(_U(n, "isSpot"), false);
        s.set(_U(n, "WS_position"), mPosition);
        s.set(_U(n, "attenuation"), mAtten);
        s.set(_U(n, "ambient"), mAmbientColor);
        s.set(_U(n, "color"), mDiffuseColor);
    }

    return true;
}

#undef _U
    
} /* GLEngine */ 

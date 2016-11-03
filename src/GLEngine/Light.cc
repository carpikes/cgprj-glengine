#include <GLEngine/Light.h>

namespace GLEngine
{

bool AmbientLight::enable(Shader& s) const {
    s.set("ambientLight.enabled", true);
    s.set("ambientLight.direction", -mDirection);
    s.set("ambientLight.ambient", mAmbientColor);
    s.set("ambientLight.diffuse", mDiffuseColor);
    return true;
}

bool AmbientLight::update(Shader& s, const Camera& c) const {
    glm::vec3 halfVector = glm::normalize(c.getCameraPos()) - mDirection;
    s.set("ambientLight.halfVector", halfVector);
    return true;
}

bool AmbientLight::disable(Shader& s) const {
    s.set("ambientLight_enabled", false);
    return true;
}

bool PointLight::enable(int n, Shader& s) const {
    if(n >= s.getMaxNumberOfLights()) {
        ERRP("Cannot enable light %d", n); 
        return false;
    }
    s.set(Utils::getUniformName("lights", n, "enabled"), true);
    s.set(Utils::getUniformName("lights", n, "isSpot"), false);
    s.set(Utils::getUniformName("lights", n, "WS_position"), mPosition);
    s.set(Utils::getUniformName("lights", n, "attenuation"), mAtten);
    s.set(Utils::getUniformName("lights", n, "ambient"), mAmbientColor);
    s.set(Utils::getUniformName("lights", n, "color"), mDiffuseColor);
    return true;
}

bool PointLight::disable(int n, Shader& s) const {
    s.set(Utils::getUniformName("lights", n, "enabled"), false);
    return true;
}
    
} /* GLEngine */ 

#include <GLEngine/Light.h>

namespace GLEngine
{

bool AmbientLight::enable(Shader& s) {
    s.set("ambientLight_enabled", true);
    s.set("ambientLight_direction", -mDirection);
    s.set("ambientLight_halfVector", mHalfVector);
    s.set("ambientLight_ambient", mAmbientColor);
    s.set("ambientLight_diffuse", mDiffuseColor);
    return true;
}

bool AmbientLight::disable(Shader& s) {
    s.set("ambientLight_enabled", false);
    return true;
}

bool PointLight::enable(int n, Shader& s) {
    if(n >= s.getMaxNumberOfLights()) {
        ERRP("Cannot enable light %d", n); 
        return false;
    }
    s.set(Utils::getUniformName("lights", n, "enabled"), true);
    s.set(Utils::getUniformName("lights", n, "isSpot"), false);
    s.set(Utils::getUniformName("lights", n, "WS_position"), mPosition);
    s.set(Utils::getUniformName("lights", n, "attenuation"), mAtten);
    s.set(Utils::getUniformName("lights", n, "ambient"), mAmbientColor);
    s.set(Utils::getUniformName("lights", n, "diffuse"), mDiffuseColor);
    return true;
}

bool PointLight::disable(int n, Shader& s) {
    s.set(Utils::getUniformName("lights", n, "enabled"), false);
    return true;
}
    
} /* GLEngine */ 

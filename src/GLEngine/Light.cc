#include <GLEngine/Light.h>

namespace GLEngine
{

bool AmbientLight::update(Shader& s) {
    s.set("ambientLight.enabled", mEnabled);

    if(mEnabled) {
        s.set("ambientLight.direction", mDirection);
        s.set("ambientLight.ambient", mAmbientColor);
        s.set("ambientLight.diffuse", mDiffuseColor);
    }

    return true;
}

bool HemiLight::update(Shader& s) {
    s.set("hemiLight.enabled", mEnabled);

    if(mEnabled) {
        s.set("hemiLight.position", mPosition);
        s.set("hemiLight.upColor", mUpColor);
        s.set("hemiLight.downColor", mDownColor);
    }

    return true;
}


#ifdef _U
#error "_U is already defined"
#endif

#define _U(y) Utils::getUniformName("lights", n, y)
bool PointLight::update(int n, Shader& s) {
    if(n >= s.getMaxNumberOfLights()) {
        ERRP("Cannot enable light %d", n); 
        return false;
    }

    s.set(_U("enabled"), mEnabled);

    if(mEnabled) {
        s.set(_U("isSpot"), mIsSpot);
        s.set(_U("WS_position"), mPosition);
        s.set(_U("attenuation"), mAtten);
        s.set(_U("ambient"), mAmbientColor);
        s.set(_U("color"), mDiffuseColor);
    }

    return true;
}

bool SpotLight::update(int n, Shader& s) {
    if(!PointLight::update(n, s))
        return false;

    if(mEnabled) {
        s.set(_U("coneDirection"), mDirection);
        s.set(_U("spotCosCutoff"), mCutoff);
        s.set(_U("spotExponent"), mExponent);
    }

    return true;
}

#undef _U
    
} /* GLEngine */ 

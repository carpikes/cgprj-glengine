#ifndef GLENGINE_LIGHT_H
#define GLENGINE_LIGHT_H

#include "Common.h"
#include "Shader.h"
#include "Utils.h"
#include "Camera.h"

namespace GLEngine
{

class Light {
    TAG_DEF("Light")
public:
    Light() : mEnabled(false) {}
    void setAmbientColor(const glm::vec3& ambient) { mAmbientColor = ambient; }
    glm::vec3 getAmbientColor() const { return mAmbientColor; }
        
    void setDiffuseColor(const glm::vec3& diffuse) { mDiffuseColor = diffuse; }
    glm::vec3 getDiffuseColor() const { return mDiffuseColor; }
    
    void enable() { mEnabled = true; }
    void disable() { mEnabled = false; }

    bool enabled() const { return mEnabled; }
protected:
    glm::vec3 mAmbientColor, mDiffuseColor;
    bool mEnabled;
};

class AmbientLight : public Light {
    TAG_DEF("AmbientLight")
public:
    void setDirection(const glm::vec3& direction) { 
        mDirection = glm::normalize(direction); 
    }
    glm::vec3 getDirection() const { return mDirection; }

    bool update(Shader& s, const Camera& c, const glm::mat4& wMat);
private:
    glm::vec3 mDirection, mHalfVector;
};

class PointLight : public Light {
    TAG_DEF("PointLight")
public:
    PointLight() {
        mIsSpot = false;
    }
    void setPosition(const glm::vec3& position) { mPosition = position; }
    glm::vec3 getPosition() const { return mPosition; }

    void setAttenuation(const glm::vec3& attenuation) { mAtten = attenuation; }
    glm::vec3 getAttenuation() const { return mAtten; }

    virtual bool update(int n, Shader &s);
protected:
    bool mIsSpot;
    glm::vec3 mPosition, mAtten;
};

class SpotLight : public PointLight {
    TAG_DEF("SpotLight")
public:
    SpotLight() {
        PointLight();
        mIsSpot = true;
        mCutoff = 0.0f;
        mExponent = 1.0f;
    }
    void setConeDirection(const glm::vec3& direction) { mDirection = direction; }
    glm::vec3 getConeDirection() const { return mDirection; }

    void setCutoff(float cutoff) { mCutoff = cutoff; }
    float getCutoff() const { return mCutoff; }
    void setExponent(float exponent) { mExponent = exponent; }
    float getExponent() const { return mExponent; }

    virtual bool update(int n, Shader &s);
protected:
    glm::vec3 mDirection;
    float mCutoff, mExponent;
};

typedef std::shared_ptr<Light> LightPtr;
typedef std::shared_ptr<AmbientLight> AmbientLightPtr;
typedef std::shared_ptr<PointLight> PointLightPtr;
typedef std::shared_ptr<SpotLight> SpotLightPtr;
} /* GLEngine */ 

#endif /* ifndef GLENGINE_LIGHT_H */

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
    
    void enable() { mEnabled = true; }
    void disable() { mEnabled = false; }

    bool enabled() const { return mEnabled; }
protected:
    bool mEnabled;
};

class ClassicLight : public Light {
public:
    void setAmbientColor(const glm::vec3& ambient) { mAmbientColor = ambient; }
    glm::vec3 getAmbientColor() const { return mAmbientColor; }
        
    void setDiffuseColor(const glm::vec3& diffuse) { mDiffuseColor = diffuse; }
    glm::vec3 getDiffuseColor() const { return mDiffuseColor; }
protected:
    glm::vec3 mAmbientColor, mDiffuseColor;
};

class AmbientLight : public ClassicLight {
    TAG_DEF("AmbientLight")
public:
    void setDirection(const glm::vec3& direction) { 
        mDirection = glm::normalize(direction); 
    }
    glm::vec3 getDirection() const { return mDirection; }

    bool update(Shader& s);
private:
    glm::vec3 mDirection, mHalfVector;
};

class HemiLight : public Light {
    TAG_DEF("HemiLight")
public:
    void setPosition(const glm::vec3& position) { mPosition = position; }
    glm::vec3 getPosition() const { return mPosition; }

    inline void setUpColor(const glm::vec3& color) { mUpColor = color; }
    inline void setDownColor(const glm::vec3& color) { mDownColor = color; }

    bool update(Shader& s);
private:
    glm::vec3 mPosition, mUpColor, mDownColor;
};

class PointLight : public ClassicLight {
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
        mCutoff = 0.0f; // 90 deg
        mExponent = 1.0f;
    }

    inline void setConeDirection(const glm::vec3& direction) { 
        mDirection = glm::normalize(direction);
    }

    inline glm::vec3 getConeDirection() const { 
        return mDirection; 
    }

    inline void setCutoffAngleDeg(float cutoff) { 
        mCutoff = cos(cutoff / 180.0f * M_PI); 
    }

    inline float getCutoffCosine() const {
        return mCutoff;
    }

    inline float getCutoffAngleDeg() const {
        return acos(mCutoff) / M_PI * 180.0f;
    }

    void setExponent(float exponent) { 
        mExponent = exponent; 
    }

    float getExponent() const { 
        return mExponent;
    }

    virtual bool update(int n, Shader &s);
protected:
    glm::vec3 mDirection;
    float mCutoff, mExponent;
};

typedef std::shared_ptr<Light> LightPtr;
typedef std::shared_ptr<AmbientLight> AmbientLightPtr;
typedef std::shared_ptr<PointLight> PointLightPtr;
typedef std::shared_ptr<SpotLight> SpotLightPtr;
typedef std::shared_ptr<HemiLight> HemiLightPtr;
} /* GLEngine */ 

#endif /* ifndef GLENGINE_LIGHT_H */

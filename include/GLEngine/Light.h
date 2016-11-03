#ifndef GLENGINE_LIGHT_H
#define GLENGINE_LIGHT_H

#include "Common.h"
#include "Shader.h"
#include "Utils.h"

namespace GLEngine
{

class Light {
    TAG_DEF("Light")
public:
    void setAmbientColor(const glm::vec3& ambient) { mAmbientColor = ambient; }
    glm::vec3 getAmbientColor() const { return mAmbientColor; }
        
    void setDiffuseColor(const glm::vec3& diffuse) { mDiffuseColor = diffuse; }
    glm::vec3 getDiffuseColor() const { return mDiffuseColor; }
protected:
    glm::vec3 mAmbientColor, mDiffuseColor;
};

class AmbientLight : public Light {
    TAG_DEF("AmbientLight")
public:
    void setDirection(const glm::vec3& direction) { mDirection = direction; }
    glm::vec3 getDirection() const { return mDirection; }

    void setHalfVector(const glm::vec3& direction) { mHalfVector = direction; }
    glm::vec3 getHalfVector() const { return mHalfVector; }

    bool enable(Shader& s);
    bool disable(Shader& s);
private:
    glm::vec3 mDirection, mHalfVector;
};

class PointLight : public Light {
    TAG_DEF("PointLight")
public:
    void setPosition(const glm::vec3& position) { mPosition = position; }
    glm::vec3 getPosition() const { return mPosition; }

    void setAttenuation(const glm::vec3& attenuation) { mAtten = attenuation; }
    glm::vec3 getAttenuation() const { return mAtten; }

    bool enable(int n, Shader& s);
    bool disable(int n, Shader& s);
private:
    glm::vec3 mPosition, mAtten;
};

typedef std::shared_ptr<Light> LightPtr;
typedef std::shared_ptr<AmbientLight> AmbientLightPtr;
typedef std::shared_ptr<PointLight> PointLightPtr;
} /* GLEngine */ 

#endif /* ifndef GLENGINE_LIGHT_H */

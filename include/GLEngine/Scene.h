#ifndef GLENGINE_SCENE_H
#define GLENGINE_SCENE_H

#include "Common.h"
#include "Camera.h"
#include "Light.h"
#include "Mesh.h"

namespace GLEngine
{

class Engine;
class Device;

class Scene {
    TAG_DEF("Scene")
public:
    Scene() : mAmbientLight(nullptr) {}

    std::vector<ObjectPtr>& getObjects() { return mObjects; }
    std::vector<PointLightPtr>& getLights() { return mLights; }
    AmbientLightPtr getAmbientLight() { return mAmbientLight; }
    HemiLightPtr getHemiLight() { return mHemiLight; }

    void addObject(ObjectPtr mesh) {
        mObjects.push_back(mesh);
    }

    void addLight(PointLightPtr light) {
        mLights.push_back(light); 
    }

    void setAmbientLight(AmbientLightPtr light) {
        mAmbientLight = light;
    }

    void setHemiLight(HemiLightPtr light) {
        mHemiLight = light;
    }

    virtual void update() {}

    bool removeFromDevice(Device& device);
    bool loadInDevice(Device& device);
private:
    std::vector<ObjectPtr> mObjects;
    std::vector<PointLightPtr> mLights;
    AmbientLightPtr mAmbientLight;
    HemiLightPtr mHemiLight;
};

typedef std::shared_ptr<Scene> ScenePtr;
} /* GLEngine */ 

#endif /* ifndef GLENGINE_SCENE_H */

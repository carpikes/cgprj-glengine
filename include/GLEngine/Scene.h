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
    Scene() {}

    const std::vector<ObjectPtr>& getObjects() const { return mObjects; }
    const std::vector<PointLightPtr>& getLights() const { return mLights; }
    const AmbientLight& getAmbientLight() const { return *mAmbientLight; }

    void addObject(ObjectPtr mesh) {
        mObjects.push_back(mesh);
    }

    void addLight(PointLightPtr light) {
        mLights.push_back(light); 
    }

    void setAmbientLight(AmbientLightPtr light) {
        mAmbientLight = light;
    }

    virtual void update() {}

    bool removeFromDevice(Device& renderer);
    bool loadInDevice(Device& renderer);
private:
    std::vector<ObjectPtr> mObjects;
    std::vector<PointLightPtr> mLights;
    AmbientLightPtr mAmbientLight;
};

typedef std::shared_ptr<Scene> ScenePtr;
} /* GLEngine */ 

#endif /* ifndef GLENGINE_SCENE_H */

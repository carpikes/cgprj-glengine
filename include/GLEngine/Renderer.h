#ifndef GLENGINE_RENDERER_H
#define GLENGINE_RENDERER_H

#include "Common.h"
#include "Material.h"
#include "Utils.h"
#include "Scene.h"
#include "ResourceManager.h"
#include "ShaderManager.h"
#include "MaterialManager.h"
#include "Device.h"
#include "Engine.h"

namespace GLEngine
{

using std::string;

class Renderer {
    TAG_DEF("Renderer")
public:
    Renderer(Device& device) : mScene(nullptr), mDevice(device) {}

    virtual bool setScene(ScenePtr scene) {
        if(mScene != nullptr)
            mScene->removeFromDevice(mDevice);
        mScene = scene;

        if(!mScene->loadInDevice(mDevice)) {
            mScene->removeFromDevice(mDevice);
            mScene = nullptr;
            return false;
        }

        return true;
    }

    // set clipping?

    virtual void renderFrame(const Camera& camera) = 0;
protected:
    ScenePtr mScene;
    Device& mDevice;
};

class DirectRenderer : public Renderer {
    TAG_DEF("DirectRenderer")
public:
    DirectRenderer(Device& device) : Renderer(device), mShader(nullptr) {}
    virtual bool setScene(ScenePtr scene);
    virtual void renderFrame(const Camera& camera);
private:
    Shader *mShader;
};

class DeferredRenderer : public Renderer {
    TAG_DEF("DeferredRenderer")
public:

    virtual void renderFrame(const Camera& camera) {
        (void) camera;
    /*
        1) attiva il MRT con i vari shader
        2) renderizza
     */
    }
};

} /* GLEngine */ 

#endif /* ifndef GLENGINE_RENDERER_H */

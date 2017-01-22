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
#include "GLUtils.h"

namespace GLEngine
{

using std::string;

class Renderer {
    TAG_DEF("Renderer")
public:
    Renderer(Device& device) : mDevice(device) {}

    // set clipping?
    virtual void renderFrame(ScenePtr scene, const Camera& camera) = 0;
protected:
    Device& mDevice;
};

class DirectRenderer : public Renderer {
    TAG_DEF("DirectRenderer")
public:
    DirectRenderer(Device& device) : Renderer(device), mShader(nullptr) {
        mShader = new Shader("../data/main.vs", "../data/main.fs");
    }

    virtual ~DirectRenderer() {
        delete mShader;
    }

    virtual void renderFrame(ScenePtr scene, const Camera& camera);
private:
    Shader *mShader;
};

class DeferredFirstPass : public Renderer {
    TAG_DEF("DeferredFirstPass")
public:
    DeferredFirstPass(Device& device) : Renderer(device), mShader(nullptr) {
		mShader = new Shader("../data/main_mrt.vs", "../data/main_mrt.fs");
    }

    virtual ~DeferredFirstPass() {
        delete mShader; 
    }

    virtual void renderFrame(ScenePtr scene, const Camera& camera);
private:
    Shader *mShader;
};

class DeferredLightPass : public Renderer {
    TAG_DEF("DeferredLightPass")
public:
    DeferredLightPass(Device& device) : Renderer(device), mShader(nullptr) {
		mShader = new Shader("../data/void.vs", "../data/deferred_mix.fs");
    }

    virtual ~DeferredLightPass() {
        delete mShader; 
    }

    virtual void renderFrame(ScenePtr scene, const Camera& camera);
private:
    Shader *mShader;
};

} /* GLEngine */ 

#endif /* ifndef GLENGINE_RENDERER_H */

#ifndef GLENGINE_ENGINE_H
#define GLENGINE_ENGINE_H

#include "Common.h"
#include "Scene.h"
#include "Device.h"
#include "MaterialManager.h"
#include "ResourceManager.h"
#include "Camera.h"

namespace GLEngine
{

class Engine : public Singleton<Engine> {
    friend class Singleton<Engine>;
public:
    virtual ~Engine() {
        delete mDevice; 
    }

    Device* getDevice() {
        return mDevice;
    }

    ResourceManager& getResourceManager() {
        return mResManager;
    } 

    MaterialManager& getMaterialManager() {
        return mMatManager;
    }

    Shader& getDefaultShader() {
        return mDefaultShader;
    }

    void setDataPath(const string& path) {
        if(path.length() == 0) {
            setDataPath(".");
            return;
        } 
        
        mResManager.setPath(path);
    }
private:
    Shader mDefaultShader;
    Device *mDevice;
    Scene *mCurScene;
    ResourceManager mResManager;
    MaterialManager mMatManager;

    Engine() : 
        mDefaultShader("main.vs", "main.fs"),
        mDevice(new Device()), 
        mCurScene(nullptr), 
        mResManager(this) 
    {
        // TODO: hardcoded, right?
        setDataPath("../data");
        if(!mDevice->init(1920, 1080, "GLEngine test", 4))
            throw "Cannot init device.";
    }

};

#define sEngine Engine::instance()
    
} /* GLEngine */ 

#endif /* ifndef GLENGINE_ENGINE_H */

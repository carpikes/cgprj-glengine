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
        return *mDefaultShader;
    }

    void setDataPath(const string& path) {
        if(path.length() == 0) {
            setDataPath(".");
            return;
        } 
        
        mResManager.setPath(path);
    }
private:
    Device *mDevice;
    ShaderPtr mDefaultShader;
    Scene *mCurScene;
    ResourceManager mResManager;
    MaterialManager mMatManager;

    Engine() : 
        mDevice(new Device()), 
        mDefaultShader(nullptr),
        mCurScene(nullptr), 
        mResManager(this) 
    {
        // TODO: hardcoded, right?
        setDataPath("../data");
        if(!mDevice->init(1280, 720, "GLEngine test", 4))
            throw "Cannot init device.";

        mDefaultShader = std::make_shared<Shader>
            ("../data/main.vs", "../data/main.fs");
    }

};

#define sEngine Engine::instance()
    
} /* GLEngine */ 

#endif /* ifndef GLENGINE_ENGINE_H */

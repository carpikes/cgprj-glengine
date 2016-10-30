#ifndef GLENGINE_ENGINE_H
#define GLENGINE_ENGINE_H

#include "Common.h"
#include "Scene.h"
#include "Renderer.h"
#include "MaterialManager.h"
#include "Camera.h"

namespace GLEngine
{

class Engine {
public:
    Engine(const string& dataPath ) : mRenderer(nullptr), mCurScene(nullptr), 
                                      mResManager(this) {
        mRenderer = new Renderer();

        setDataPath(dataPath);
        // TODO: hardcoded, right?
        if(!mRenderer->init(1920, 1080, "GLEngine test", 4))
            throw "Cannot start renderer";
    }

    virtual ~Engine() {
        delete mRenderer; 
    }

    void setScene(Scene *scene) {
        if(scene == mCurScene)
            return;

        if(mCurScene != nullptr) {
            mCurScene->shutDown();
        }

        mCurScene = scene; 
        mCurScene->setUp(mRenderer);
    }

    Renderer* getRenderer() {
        return mRenderer;
    }

    ResourceManager& getResourceManager() {
        return mResManager;
    } 

    MaterialManager& getMaterialManager() {
        return mMatManager;
    }

    void setDataPath(const string& path) {
        if(path.length() == 0) {
            setDataPath(".");
            return;
        } 
        
        mRenderer->setPath(path);
        mResManager.setPath(path);
    }
private:
    Renderer *mRenderer;
    Scene *mCurScene;
    ResourceManager mResManager;
    MaterialManager mMatManager;
};
    
} /* GLEngine */ 

#endif /* ifndef GLENGINE_ENGINE_H */

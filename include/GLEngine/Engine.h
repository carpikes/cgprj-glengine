#ifndef GLENGINE_ENGINE_H
#define GLENGINE_ENGINE_H

#include <GLEngine/Common.h>
#include <GLEngine/Scene.h>
#include <GLEngine/Renderer.h>
#include <GLEngine/MaterialManager.h>

namespace GLEngine
{

class Engine {
public:
    Engine() : mRenderer(nullptr), mCurScene(nullptr) {
        mRenderer = new Renderer();
    }

    virtual ~Engine(){
        delete mRenderer; 
    }

    void setScene(Scene *scene) {
        if(scene == mCurScene)
            return;

        if(mCurScene != nullptr) {
            // mScene->setEngine(nullptr);
        }

        mCurScene = scene; 
        // mScene->setEngine(this);
    }

    void run() {
        //while(mRenderer->isRunning()) {
            
        //}
    }

    ResourceManager& getResourceManager() {
        return mResManager;
    } 

    MaterialManager& getMaterialManager() {
        return mMatManager;
    }
private:
    Renderer *mRenderer;
    Scene *mCurScene;
    ResourceManager mResManager;
    MaterialManager mMatManager;
};
    
} /* GLEngine */ 

#endif /* ifndef GLENGINE_ENGINE_H */

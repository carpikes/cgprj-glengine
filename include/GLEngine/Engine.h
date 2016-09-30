#ifndef GLENGINE_ENGINE_H
#define GLENGINE_ENGINE_H

#include <GLEngine/Common.h>
#include <GLEngine/Scene.h>
#include <GLEngine/Renderer.h>
#include <GLEngine/MaterialManager.h>
#include <GLEngine/Camera.h>

namespace GLEngine
{

class Engine {
public:
    Engine(const string& dataPath ) : mRenderer(nullptr), mCurScene(nullptr), 
                                      mResManager(this) {
        mRenderer = new Renderer();

        setDataPath(dataPath);
        // TODO: hardcoded, right?
        if(!mRenderer->init(1024, 768, "GLEngine test", 4))
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

    void run() {
        if(mCurScene == nullptr)
            return; 

        LookAtCamera *c = new LookAtCamera(75.0f, 4.0f/3.0f);
        float cnt = 0.0f;
        while(mRenderer->isRunning()) {
            //c->setCameraPos(glm::vec3(20,10,20));
            c->setCameraPos(glm::vec3(20 * sin(-cnt/2.0f),10,20 * cos(-cnt/2.0f)));
            c->setTargetPos(glm::vec3(0,2,0));
            c->setUpVector(glm::vec3(0,1,0));
            //mCurScene->setLightRot(glm::vec3(sin(cnt),cos(cnt),0));
            mCurScene->setLightRot(glm::vec3(0,1,0));
            //mCurScene->setLightPos(glm::vec3(20 * sin(cnt), 20 * cos(cnt), 0));
            mCurScene->setLightPos(glm::vec3(0,0,1));
            c->update();

            mRenderer->prepareFrame();
            mCurScene->render(c);
            mRenderer->endFrame();

            cnt += 0.01f;
        }
        delete c;
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

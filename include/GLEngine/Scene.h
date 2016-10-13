#ifndef GLENGINE_SCENE_H
#define GLENGINE_SCENE_H

#include <GLEngine/Common.h>
#include <GLEngine/Renderer.h>
#include <GLEngine/Camera.h>

namespace GLEngine
{

using std::vector;

class Engine;

class Scene {
    TAG_DEF("Scene")
public:
    Scene(Engine *engine) : mRenderer(nullptr), mCanRender(false), 
        mEngine(engine) {}

    bool addObject(ObjectPtr mesh) {
        mObjects.push_back(mesh);
        return true;
    }

    void setLightRot(const glm::vec3& rot) {
        mRenderer->setLightRot(glm::normalize(rot));
    }

    /** Load all resources */
    bool setUp(Renderer *renderer);

    /* E se perdo il renderer prima di chiamare shutDown? */
    bool shutDown();

    // TODO: camera? 

    /** Render a frame */
    bool render(Camera *camera);
private:
    Renderer *mRenderer;
    bool mCanRender;
    Engine *mEngine;
    std::vector<ObjectPtr> mObjects;
    std::vector<VideoPtr> mVideoPtrs, mTextureIds;
    glm::vec3 mLightRot;
};
    
} /* GLEngine */ 

#endif /* ifndef GLENGINE_SCENE_H */

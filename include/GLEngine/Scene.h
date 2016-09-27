#ifndef GLENGINE_SCENE_H
#define GLENGINE_SCENE_H

#include <GLEngine/Common.h>
#include <GLEngine/Renderer.h>

namespace GLEngine
{

using std::vector;

class Camera{};
class Engine;

class Scene {
    TAG_DEF("Scene")
public:
    Scene(Engine *engine) : mRenderer(nullptr), mCanRender(false), 
        mEngine(engine) {}

    bool addMesh(MeshPtr mesh) {
        mMeshes.push_back(mesh);
        return true;
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
    std::vector<MeshPtr> mMeshes;
    std::vector<VideoPtr> mVideoPtrs, mTextureIds;
};
    
} /* GLEngine */ 

#endif /* ifndef GLENGINE_SCENE_H */

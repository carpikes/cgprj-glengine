#include <GLEngine/Scene.h>
#include <GLEngine/Material.h>
#include <GLEngine/MaterialManager.h>
#include <GLEngine/Engine.h>

namespace GLEngine
{
    

bool Scene::setUp(Renderer *renderer) {
    if(mCanRender) {
        // liberare memoria se rendererer != quello allocato
        if(mRenderer == renderer) {
            LOG("You are calling setUp() multiple times. Stop it!");
            return false;
        } // cambiato il renderer

        if(mRenderer != nullptr)
            shutDown(); 
        mCanRender = false;
    }

    // move meshes & textures to videocard 
    size_t numberOfBuffers = 0;
    for(const ObjectPtr o : mObjects) {
        if(o->getMesh()->getEngineTag() == 0) {
            numberOfBuffers += o->getMesh()->getParts().size();
            o->getMesh()->setEngineTag(1);
        }
    }

    mVideoPtrs.clear();
    mVideoPtrs.resize(numberOfBuffers, InvalidVideoPtr);

    if(!renderer->allocateVertexBuffers(numberOfBuffers, mVideoPtrs)) {
        ERRP("Cannot allocate %d vertex buffers", numberOfBuffers);
        mVideoPtrs.clear();
        return false;
    }
    // TODO allocate faces buffer

    size_t i = 0;
    for(ObjectPtr o : mObjects) {
        MeshPtr m = o->getMesh();
        if(m->getEngineTag() == 1) {
            for(MeshPart& mp : m->getParts()) {
                if(mp.videoPtr() == InvalidVideoPtr) {
                    renderer->writeVertices(mVideoPtrs[i], mp.vertices());
                    mp.setVideoPtr(mVideoPtrs[i]);
                    i++;
                }
                //renderer->writeFaces(mVideoPtrs[i], mp->faces());
            }
            m->setEngineTag(2);
        }
    }

    // e qua le texture
        
    std::unordered_set<Image *> usedImages;
    MaterialManager& matManager = mEngine->getMaterialManager();
    for(const ObjectPtr o : mObjects)
        for(MeshPart& mp : o->getMesh()->getParts()) {
            MaterialPtr mat = matManager.get(mp.material());
            if(mat != nullptr)
                mat->appendTextures(usedImages);
        }

    mTextureIds.clear();
    mRenderer->allocateTextureBuffers(usedImages.size(), mTextureIds);
    mRenderer->writeTexture(mTextureIds, usedImages);
    
    mRenderer = renderer;
    mCanRender = true;
    return false;
}

bool Scene::shutDown() {
    // FIXME: remove data from videocard
    
    mCanRender = false;
    return true;
}

bool Scene::render(Camera *camera) {
    if(!mCanRender)
        return false;

    for(int i=0;i<3;i++)
        glEnableVertexAttribArray(i);

    glm::vec3 cameraPos = camera->getCameraPos();
    glm::mat4 viewMat = camera->getViewMatrix();
    glm::mat4 projMat = camera->getProjMatrix();

    mRenderer->setEyePos(cameraPos);

    MaterialManager& mtlMgr = mEngine->getMaterialManager();
    for(ObjectPtr o : mObjects) {
        MeshPtr m = o->getMesh();
        if(m == nullptr)
            continue;
        glm::mat4 mvMat = viewMat * o->getModelMatrix();
        glm::mat4 mvpMat = projMat * mvMat;

        mRenderer->setMatrices(o->getModelMatrix(), mvpMat, o->getNormalMatrix());
        // Da spostare nella mesh
        for(MeshPart& p : m->getParts()) {
            MaterialPtr mtl = mtlMgr.get(p.material());
            if(mtl == nullptr)
                continue;

            VideoPtr ptr = p.videoPtr();

            glBindBuffer(GL_ARRAY_BUFFER, ptr);
            for(int i=0;i<3;i++)
                glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                      (void *) (i * sizeof(glm::vec3)));

            mRenderer->setMaterialParams(&(*mtl));

            glDrawArrays(GL_TRIANGLES, 0, p.vertices().size());
        } 
    }

    for(int i=0;i<3;i++)
        glDisableVertexAttribArray(i);
    return true;
}
    
} /* GLEngine */ 

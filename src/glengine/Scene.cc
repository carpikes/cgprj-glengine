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
    for(const MeshPtr& m : mMeshes)
        numberOfBuffers += m->getParts().size();

    mVideoPtrs.clear();
    mVideoPtrs.resize(numberOfBuffers, InvalidVideoPtr);

    if(!renderer->allocateVertexBuffers(numberOfBuffers, mVideoPtrs)) {
        ERRP("Cannot allocate %d vertex buffers", numberOfBuffers);
        mVideoPtrs.clear();
        return false;
    }
    // TODO allocate faces buffer

    assert(mVideoPtrs.size() >= mMeshes.size());

    size_t i = 0;
    for(MeshPtr& m : mMeshes) {
        for(MeshPart& mp : m->getParts()) {
            renderer->writeVertices(mVideoPtrs[i], mp.vertices());
            mp.setVideoPtr(mVideoPtrs[i]);
            //renderer->writeFaces(mVideoPtrs[i], mp->faces());
            i++;
        }

    }

    // e qua le texture
        
    std::unordered_set<Image *> usedImages;
    MaterialManager& matManager = mEngine->getMaterialManager();
    for(const MeshPtr m : mMeshes)
        for(MeshPart& mp : m->getParts()) {
            MaterialPtr mat = matManager.get(mp.material());
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

    return true;
}
    
} /* GLEngine */ 

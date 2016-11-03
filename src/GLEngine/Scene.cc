#include <GLEngine/Scene.h>
#include <GLEngine/Material.h>
#include <GLEngine/MaterialManager.h>
#include <GLEngine/Engine.h>

namespace GLEngine
{
    
bool Scene::removeFromDevice(Device& device) {
    (void)device;
    return true;
}

bool Scene::loadInDevice(Device& device) {
    // move meshes & textures to videocard 
    size_t numberOfBuffers = 0;
    for(const ObjectPtr o : mObjects) {
        if(o->getMesh()->getEngineTag() == 0) {
            numberOfBuffers += o->getMesh()->getParts().size();
            o->getMesh()->setEngineTag(1);
        }
    }

    std::vector<VideoPtr> mVideoPtrs, mTextureIds;
    mVideoPtrs.clear();
    mVideoPtrs.resize(numberOfBuffers, InvalidVideoPtr);

    if(!device.allocVertexBuffers(numberOfBuffers, mVideoPtrs)) {
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
                    device.writeVertices(mVideoPtrs[i], mp.vertices());
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
    MaterialManager& matManager = sEngine.getMaterialManager();
    for(const ObjectPtr o : mObjects)
        for(MeshPart& mp : o->getMesh()->getParts()) {
            MaterialPtr mat = matManager.get(mp.material());
            if(mat != nullptr)
                mat->appendTextures(usedImages);
        }

    mTextureIds.clear();
    device.allocTextureBuffers(usedImages.size(), mTextureIds);
    device.writeTexture(mTextureIds, usedImages);
    
    return true;
}

} /* GLEngine */ 

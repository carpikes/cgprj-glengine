#include <GLEngine/ResourceManager.h>
#include <GLEngine/Utils.h>
#include <GLEngine/FileReader/OBJFileReader.h>

namespace GLEngine
{

ImagePtr ResourceManager::getImage(const string& name) {
    auto r = mImages.find(name);

    if( r == mImages.end()) {
        ImagePtr ptr = ImageFactory::load(name);
        if(ptr == nullptr) {
            ERRP("Asking invalid resource %s", name.c_str());
            return nullptr;
        }

        DEBP("Loaded resource %s", rname.c_str());
        mImages[name] = ptr;
        return ptr;
    }

    return r->second;
}

MeshPtr ResourceManager::getMesh(const string& name) {
    auto r = mMeshes.find(name);

    if(r == mMeshes.end()) {
        MeshPtr ptr = loadObj(name, mPath);
        if(ptr == nullptr) {
            ERRP("Asking invalid resource %s", name.c_str());
            return nullptr;
        }

        DEBP("Loaded resource %s", rname.c_str());
        mMeshes[name] = ptr;
        return ptr;
    }

    return r->second;
}

MeshPtr ResourceManager::loadObj(const string& name, const string& path) {
    MeshPtr o = std::make_shared<Mesh>();
    OBJFileReader obj(this->mEngine, path);
    if(obj.load(name, *o))
        return o;

    return nullptr; 
}

} /* GLEngine */ 

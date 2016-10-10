#include <GLEngine/ResourceManager.h>
#include <GLEngine/Utils.h>
#include <GLEngine/FileReader/OBJFileReader.h>

namespace GLEngine
{

void* ResourceManager::_load(const string& name) {
    auto r = mResources.find(name);
    if(r != mResources.end())
        return nullptr;

    void *res = nullptr;
    string ext = Utils::getExtension(name);

    string rname = mPath + "/" + name;
    LOGP("Loading resource %s", rname.c_str());
    if(!ext.compare("obj")) 
        res = loadObj(name, mPath);
    else if(!ext.compare("png"))
        res = loadPng(rname);
    else 
        ERRP("Invalid extension for resource %s", name.c_str());

    if( res != nullptr) {
        LOGP("Loaded resource %s", rname.c_str());
        mResources[name] = res;
    }
    return res;
}

void* ResourceManager::_get(const string& name) {
    auto r = mResources.find(name);

    DEBP("Requesting texture %s", name.c_str());
    if( r == mResources.end()) {
        DEBP("Loading texture %s", name.c_str());
        void *ptr = _load(name);
        if(ptr == nullptr) {
            ERRP("Asking invalid resource %s", name.c_str());
            return nullptr;
        }
        return ptr;
    }

    return r->second;
}

void *ResourceManager::loadObj(const string& name, const string& path) {
    Mesh *o = new Mesh;
    OBJFileReader obj(this->mEngine, path);
    if(obj.load(name, *o))
        return o;

    delete o;
    return nullptr; 
}

} /* GLEngine */ 

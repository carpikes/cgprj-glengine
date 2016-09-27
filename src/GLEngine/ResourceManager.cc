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
    if(!ext.compare("obj")) 
        res = loadObj(rname);
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

    if( r == mResources.end()) {
        void *ptr = _load(name);
        if(ptr == nullptr) {
            ERRP("Asking invalid resource %s", name.c_str());
            return nullptr;
        }
        return ptr;
    }

    return r->second;
}

void *ResourceManager::loadObj(const string& name) {
    Mesh *o = new Mesh;
    OBJFileReader obj(this);
    if(obj.load(name, *o))
        return o;

    delete o;
    return nullptr; 
}

} /* GLEngine */ 

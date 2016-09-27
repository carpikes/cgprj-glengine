#ifndef GLENGINE_RESOURCEMANAGER_H
#define GLENGINE_RESOURCEMANAGER_H

#include <GLEngine/Common.h>
#include <GLEngine/Mesh.h>
#include <GLEngine/Material.h>
#include <GLEngine/ImageFactory.h>

namespace GLEngine
{

using std::string;

class Engine;

class ResourceManager {
    TAG_DEF("ResourceManager")
public:
    ResourceManager(Engine *engine) : mPath("."), mEngine(engine) {}

    template<typename T>
    inline T* get(const string& name) {
        return (T*) this->_get(name);
    }

    void setPath(const string& path) {
        mPath = path;
    }
private:
    std::unordered_map<string, void*> mResources;
    string mPath;
    Engine *mEngine;

    enum eTypes {
        TYPE_UNK,
        TYPE_PNG,
        TYPE_OBJ,
        TYPE_MTL,
    };

    void *_load(const string& name);
    void *_get(const string& name);

    
    void *loadObj(const string& path, const string& name);

    void *loadPng(const string& name) { 
        return ImageFactory::load(name); 
    }
};

} /* GLEngine */ 

#endif /* ifndef GLENGINE_RESOURCEMANAGER_H */

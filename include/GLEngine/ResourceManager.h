#ifndef GLENGINE_RESOURCEMANAGER_H
#define GLENGINE_RESOURCEMANAGER_H

#include <GLEngine/Common.h>
#include <GLEngine/Mesh.h>
#include <GLEngine/Material.h>
#include <GLEngine/ImageFactory.h>
#include <GLEngine/FileReader/OBJFileReader.h>

namespace GLEngine
{

using std::string;

class ResourceManager {
    TAG_DEF("ResourceManager")
public:
    ResourceManager() {}

    template<typename T>
    inline T* get(const string& name) {
        return (T*) this->_get(name);
    }
private:
    std::unordered_map<string, void*> mResources;

    enum eTypes {
        TYPE_UNK,
        TYPE_PNG,
        TYPE_OBJ,
        TYPE_MTL,
    };

    void *_load(const string& name);
    void *_get(const string& name);

    
    void *loadObj(const string& name) { 
        Mesh *o = new Mesh;
        OBJFileReader obj;
        if(obj.load(name, *o))
            return o;

        delete o;
        return nullptr; 
    }

    void *loadPng(const string& name) { 
        return ImageFactory::load(name); 
    }
};

} /* GLEngine */ 

#endif /* ifndef GLENGINE_RESOURCEMANAGER_H */

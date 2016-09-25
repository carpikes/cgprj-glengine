#ifndef GLENGINE_RESOURCEMANAGER_H
#define GLENGINE_RESOURCEMANAGER_H

#include <Common.h>
#include <Singleton.h>
#include <PNG.h>
#include <Object.h>
#include <FileReader/OBJFileReader.h>

namespace GLEngine
{

using std::string;
using std::shared_ptr;

template<typename T> struct ResourceFactory {
    static T *load(const string& name);
};

template<> struct ResourceFactory<Material> {
    static Material *load(const string& name) {
        return new Material; 
    }
};

template<> struct ResourceFactory<Image>{
    static Image *load(const string& name) {
        return ImageFactory::load(name);
    }
};

template<> struct ResourceFactory<ObjectGroup>{
    static ObjectGroup *load(const string& name) {
        ObjectGroup *o = new ObjectGroup;
        OBJFileReader obj;
        if(obj.load(name, *o))
            return o;

        delete o;
        return nullptr;
    }
};

template<typename T>
class ResourceManager : public Singleton<ResourceManager<T>> {
    TAG_DEF("ResourceManager")
    friend class Singleton<ResourceManager<T>>;
public:

    static inline T* get(const string& name) {
        return ResourceManager<T>::instance()._get(name);
    }

    static inline bool load(const string& name) {
        return ResourceManager<T>::instance()._load(name);
    }

    static std::unordered_map<string, T*>& expose() {
        return ResourceManager<T>::instance().mResources;
    }

private:
    ResourceManager() {}
    std::unordered_map<string, T*> mResources;

    enum eTypes {
        TYPE_UNK,
        TYPE_PNG,
        TYPE_OBJ,
        TYPE_MTL,
    };

    bool _load(const string& name) {
        T* res = ResourceFactory<T>::load(name);
        if( res != nullptr) {
            LOGP("Loaded resource %s", name.c_str());
            mResources[name] = res;
            return true;
        }
        return false;
    }

    T* _get(const string& name) {
        auto r = mResources.find(name);

        if( r == mResources.end()) {
            ERRP("Asking invalid material %s", name.c_str());
            return nullptr;
        }

        return r->second;
    }

};

} /* GLEngine */ 

#endif /* ifndef GLENGINE_RESOURCEMANAGER_H */

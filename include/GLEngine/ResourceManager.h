#ifndef GLENGINE_RESOURCEMANAGER_H
#define GLENGINE_RESOURCEMANAGER_H

#include "Common.h"
#include "Mesh.h"
#include "Material.h"
#include "ImageFactory.h"

namespace GLEngine
{

using std::string;

class Engine;

class ResourceManager {
    TAG_DEF("ResourceManager")
public:
    ResourceManager(Engine *engine) : mPath("."), mEngine(engine) {}
    ~ResourceManager() { }

    ImagePtr getImage(const string& name);
    MeshPtr getMesh(const string& name);


    void setPath(const string& path) {
        mPath = path;
    }
private:
    std::unordered_map<string, ImagePtr> mImages;
    std::unordered_map<string, MeshPtr> mMeshes;
    std::unordered_map<string, MaterialPtr> mMaterials;

    string mPath;
    Engine *mEngine;

    enum eTypes {
        TYPE_UNK,
        TYPE_PNG,
        TYPE_OBJ,
    };

    MeshPtr loadObj(const string& name, const string& path);
};

} /* GLEngine */ 

#endif /* ifndef GLENGINE_RESOURCEMANAGER_H */

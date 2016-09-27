#ifndef GLENGINE_MATERIALMANAGER_H
#define GLENGINE_MATERIALMANAGER_H

#include <GLEngine/Common.h>
#include <GLEngine/Material.h>

namespace GLEngine
{

class MaterialManager {
public:
    MaterialPtr get(const std::string& name) {
        auto ptr = mMaterials.find(name);
        if(ptr == mMaterials.end())
            return nullptr;
        return ptr->second;
    }
private:
    std::unordered_map<std::string, MaterialPtr> mMaterials;

};

    
} /* GLEngine */ 


#endif /* ifndef GLENGINE_MATERIALMANAGER_H */

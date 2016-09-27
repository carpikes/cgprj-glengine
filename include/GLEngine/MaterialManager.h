#ifndef GLENGINE_MATERIALMANAGER_H
#define GLENGINE_MATERIALMANAGER_H

#include <GLEngine/Common.h>
#include <GLEngine/Material.h>

namespace GLEngine
{

class MaterialManager {
    TAG_DEF("MaterialManager")
public:
    bool add(const std::string& name, MaterialPtr material) {
        if(get(name) != nullptr) {
            ERRP("Material %s already exists", name.c_str());
            return false;
        }

        mMaterials.insert(std::pair<std::string, MaterialPtr>(name, material));
        return true;
    }

    MaterialPtr get(const std::string& name) {
        auto ptr = mMaterials.find(name);
        if(ptr == mMaterials.end())
            return nullptr;
        return ptr->second;
    }

    static const std::string buildMaterialName(const std::string& libName, 
                                        const std::string& matName) {
        std::stringstream ss;
        ss << libName;
        ss << "::";
        ss << matName;
        return ss.str();
    }
private:
    std::unordered_map<std::string, MaterialPtr> mMaterials;

};

    
} /* GLEngine */ 


#endif /* ifndef GLENGINE_MATERIALMANAGER_H */

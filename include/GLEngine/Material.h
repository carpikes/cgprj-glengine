#ifndef GLENGINE_MATERIAL_H
#define GLENGINE_MATERIAL_H

#include "Common.h"
#include "Image.h"
#include "Texture.h"
#include "Shader.h"

namespace GLEngine
{

struct Material {
    TAG_DEF("Material")
    Material() {
        mAmbientTexture = mDiffuseTexture = mSpecularTexture =
        mHighlightTexture = mAlphaTexture = mBumpTexture =
        mDisplacementTexture = mStencilTexture = nullptr;
        mSpecularExponent = 0.0f;
        mAlpha = mOpticalDensity = 1.0f;
    }

    glm::vec3 mAmbientColor, mDiffuseColor;
    glm::vec3 mSpecularColor, mTransmissionFilter;
    float mSpecularExponent, mAlpha, mOpticalDensity;

    Texture *mAmbientTexture, *mDiffuseTexture, *mSpecularTexture;
    Texture *mHighlightTexture, *mAlphaTexture; // not used
    Texture *mBumpTexture, *mDisplacementTexture, *mStencilTexture; // not used

    void appendTextures(std::unordered_set<Image *>& out) {
        Texture* arr[] = {  
            mAmbientTexture, mDiffuseTexture, mSpecularTexture,
            mHighlightTexture, mAlphaTexture, mBumpTexture,
            mDisplacementTexture, mStencilTexture
        };

        for(Texture *t : arr)
            if(t != nullptr && t->img != nullptr)
                out.insert(t->img);
    }

    void enable(Shader& shader) {
        int flags = 0;

        shader.set("material.ambientColor", mAmbientColor);
        shader.set("material.diffuseColor", mDiffuseColor);
        shader.set("material.specularExponent", mSpecularExponent);

        flags |= shader.setTexture("material.ambientTexture", 
                                 mAmbientTexture, AMBIENT_TEXTURE);
        flags |= shader.setTexture("material.diffuseTexture", 
                                 mDiffuseTexture, DIFFUSE_TEXTURE);
        flags |= shader.setTexture("material.specularTexture", 
                                 mSpecularTexture, SPECULAR_TEXTURE);
        flags |= shader.setTexture("material.bumpTexture", 
                                 mBumpTexture, BUMP_TEXTURE);
        flags |= shader.setTexture("material.displacementTexture",
                                 mDisplacementTexture, DISPLACE_TEXTURE);

        shader.set("material.flags", flags);
    }
};

typedef std::shared_ptr<Material> MaterialPtr;
    
} /* GLEngine */ 

#endif /* ifndef GLENGINE_MATERIAL_H */

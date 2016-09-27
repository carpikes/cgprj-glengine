#ifndef GLENGINE_MATERIAL_H
#define GLENGINE_MATERIAL_H

#include <GLEngine/Common.h>
#include <GLEngine/Image.h>

namespace GLEngine
{

struct Texture {
    Texture(Image *img) : img(img) { }
    bool mBlendH, mBlendV;
    float mBoostSharpness;
    float mBrightnessMod, mContrastMod; /* Default: 0 1 */
    Image *img;
    // Offset, scale, turbulence
};

struct Material {
    Material() {
        mAmbientTexture = mDiffuseTexture = mSpecularTexture =
        mHighlightTexture = mAlphaTexture = mBumpTexture =
        mDisplacementTexture = mStencilTexture = nullptr;
    }

    glm::vec3 mAmbientColor, mDiffuseColor;
    glm::vec3 mSpecularColor, mTransmissionFilter;
    float mSpecularExponent, mAlpha, mOpticalDensity;

    Texture *mAmbientTexture, *mDiffuseTexture, *mSpecularTexture;
    Texture *mHighlightTexture, *mAlphaTexture; // not used
    Texture *mBumpTexture, *mDisplacementTexture, *mStencilTexture; // not used

    void appendTextures(std::unordered_set<Image *>& out) {
        Texture* arr[] = {  mAmbientTexture, mDiffuseTexture, mSpecularTexture,
                            mHighlightTexture, mAlphaTexture, mBumpTexture,
                            mDisplacementTexture, mStencilTexture};

        for(Texture *t : arr)
            if(t != nullptr && t->img != nullptr)
                out.insert(t->img);
    }  
};

typedef std::shared_ptr<Material> MaterialPtr;
    
} /* GLEngine */ 

#endif /* ifndef GLENGINE_MATERIAL_H */

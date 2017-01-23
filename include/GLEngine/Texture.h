#ifndef GLENGINE_TEXTURE_H
#define GLENGINE_TEXTURE_H

#include "Common.h"
#include "Image.h"

namespace GLEngine
{

enum eTextures {
    AMBIENT_TEXTURE     = 0,
    DIFFUSE_TEXTURE     = 1,
    SPECULAR_TEXTURE    = 2,
    BUMP_TEXTURE        = 3,
    DISPLACE_TEXTURE    = 4,
};

struct Texture {
    Texture(ImagePtr img) : img(img) { }
    ~Texture() { }
    bool mBlendH, mBlendV;
    float mBoostSharpness;
    float mBrightnessMod, mContrastMod; /* Default: 0 1 */
    ImagePtr img;
    // Offset, scale, turbulence
};

typedef std::shared_ptr<Texture> TexturePtr;

    
} /* GLEngine */ 

#endif

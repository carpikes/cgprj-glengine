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
    Texture(Image *img) : img(img) { }
    bool mBlendH, mBlendV;
    float mBoostSharpness;
    float mBrightnessMod, mContrastMod; /* Default: 0 1 */
    Image *img;
    // Offset, scale, turbulence
};

    
} /* GLEngine */ 

#endif

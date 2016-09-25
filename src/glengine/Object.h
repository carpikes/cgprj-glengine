#ifndef GLENGINE_OBJECT_H
#define GLENGINE_OBJECT_H

#include "Common.h"
#include "PNG.h"

namespace GLEngine
{

using std::string;
using std::vector;

#pragma pack(1)
struct Vertex {
    glm::vec3 vertex, normal;
    glm::vec2 uv;
};
#pragma pack()

struct Texture {
    Texture(Image *img) : img(img) { }
    bool mBlendH, mBlendV;
    float mBoostSharpness;
    float mBrightnessMod, mContrastMod; /* Default: 0 1 */
    const Image *img;
    // Offset, scale, turbulence
};

struct Material {
    glm::vec3 mAmbientColor, mDiffuseColor, mSpecularColor, mTransmissionFilter;
    float mSpecularExponent, mAlpha, mOpticalDensity;

    Texture *mAmbientTexture, *mDiffuseTexture, *mSpecularTexture;
    Texture *mHighlightTexture, *mAlphaTexture;
    Texture *mBumpTexture, *mDisplacementTexture, *mStencilTexture;
};

class Object {
    TAG_DEF("Object")
public:
    inline void setTag(const string& tag) { mTag = tag; }
    inline const string& getTag() const { return mTag; }

    inline void setPosition(const glm::vec3& position) { mPosition = position; }
    inline const glm::vec3& getPosition() const { return mPosition; }

    inline void setOrientation(const glm::vec4& o) { mOrientation = o; }
    inline const glm::vec4& getOrientation() const { return mOrientation; }
    
    inline vector<Vertex>& vertices() { return mVertices; }
    inline vector<uint16_t> &faces() { return mFaces; }

    inline void setMaterialType(const string& type) { mMaterialType = type; }
    inline const string& material() const { return mMaterialType; }
private:
    string mTag;
    glm::vec3 mPosition;
    glm::vec4 mOrientation;

    vector<Vertex> mVertices;
    vector<uint16_t> mFaces;

    string mMaterialType;
    bool mUseShading; // unused
};

struct ObjectGroup {
    vector<Object> objects;
};


} /* GLEngine */ 

#endif /* ifndef GLENGINE_OBJECT_H

#include "Common.h" */

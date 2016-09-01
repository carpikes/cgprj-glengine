#ifndef GLENGINE_OBJECT_H
#define GLENGINE_OBJECT_H

#include "Common.h"

namespace GLEngine
{

using std::string;
using std::vector;

class Object {
    TAG_DEF("Object")
public:
    inline void setTag(const string& tag) { mTag = tag; }
    inline const string& getTag() const { return mTag; }

    inline void setPosition(const glm::vec3& position) { mPosition = position; }
    inline const glm::vec3& getPosition() const { return mPosition; }

    inline void setOrientation(const glm::vec4& o) { mOrientation = o; }
    inline const glm::vec4& getOrientation() const { return mOrientation; }
    
    inline vector<glm::vec3>& vertices() { return mVertices; }
    inline vector<glm::vec3>& normals() { return mNormals; }
    inline vector<glm::vec2>& uvs() { return mUVs; }
private:
    string mTag;
    glm::vec3 mPosition;
    glm::vec4 mOrientation;

    vector<glm::vec3> mVertices, mNormals;
    vector<glm::vec2> mUVs;
};

} /* GLEngine */ 

#endif /* ifndef GLENGINE_OBJECT_H

#include "Common.h" */

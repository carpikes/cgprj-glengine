#ifndef GLENGINE_MESH_H
#define GLENGINE_MESH_H

#include <GLEngine/Common.h>
#include <GLEngine/Image.h>
#include <GLEngine/Material.h>
#include <GLEngine/Renderer.h>

namespace GLEngine
{

using std::vector;

class MeshPart {
    TAG_DEF("MeshPart")
public:
    MeshPart() : mVideoPtr(InvalidVideoPtr) {}
    inline void setPosition(const glm::vec3& position) { mPosition = position; }
    inline const glm::vec3& getPosition() const { return mPosition; }

    inline void setOrientation(const glm::vec4& o) { mOrientation = o; }
    inline const glm::vec4& getOrientation() const { return mOrientation; }
    
    inline vector<Vertex>& vertices() { return mVertices; }
    inline vector<uint16_t> &faces() { return mFaces; }

    inline void setMaterial(const std::string& material) { mMaterial = material; }
    inline const std::string& material() const { return mMaterial; }

    void setVideoPtr(VideoPtr p) { mVideoPtr = p; }
    VideoPtr videoPtr() const { return mVideoPtr; }
private:
    glm::vec3 mPosition;
    glm::vec4 mOrientation;

    vector<Vertex> mVertices;
    vector<uint16_t> mFaces;

    std::string mMaterial;
    VideoPtr mVideoPtr;
};

class Mesh {
public:
    vector<MeshPart>& getParts() { return mObjects; } 
private:
    vector<MeshPart> mObjects;
};
    
typedef std::shared_ptr<Mesh> MeshPtr;
} /* GLEngine */ 

#endif /* ifndef GLENGINE_MESH_H */

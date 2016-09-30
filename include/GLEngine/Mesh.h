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
    
    inline vector<Vertex>& vertices() { return mVertices; }
    inline vector<uint16_t> &faces() { return mFaces; }

    inline void setMaterial(const std::string& material) { mMaterial = material; }
    inline const std::string& material() const { return mMaterial; }

    void setVideoPtr(VideoPtr p) { mVideoPtr = p; }
    VideoPtr videoPtr() const { return mVideoPtr; }
private:

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

class Object {
public:
    Object(MeshPtr mesh) : mMesh(mesh), mScaling(1.0,1.0,1.0) {}

    inline void setPosition(const glm::vec3& position) {mPosition = position;}
    inline const glm::vec3& getPosition() const { return mPosition; }

    inline void setOrientation(const glm::quat& o) { mOrientation = o; }
    inline const glm::quat& getOrientation() const { return mOrientation; }
    inline void setScaling(const glm::vec3& scaling) { mScaling = scaling; }
    inline const glm::vec3& getScaling() const { return mScaling; }
    
    // TODO ottimizzare sta cosa
    glm::mat4 getModelMatrix() const {
        glm::mat4 scaling = glm::scale(glm::mat4(1.0f), mScaling);
        glm::mat4 rotate = glm::mat4_cast(mOrientation);
        glm::mat4 translate = glm::translate(glm::mat4(1.0f), mPosition);
        return translate * rotate * scaling;
    }

    MeshPtr getMesh() { return mMesh; }
private:
    MeshPtr mMesh;
    glm::vec3 mPosition, mScaling;
    glm::quat mOrientation;
};

typedef std::shared_ptr<Object> ObjectPtr;
    
} /* GLEngine */ 

#endif /* ifndef GLENGINE_MESH_H */

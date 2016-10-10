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
    Mesh() : mEngineTag(0) {}
    vector<MeshPart>& getParts() { return mObjects; } 
    
    void setEngineTag(int tag) { mEngineTag = tag; }
    int getEngineTag() const { return mEngineTag; }
private:
    vector<MeshPart> mObjects;
    int mEngineTag;
};

typedef std::shared_ptr<Mesh> MeshPtr;

class Object {
public:
    Object(MeshPtr mesh) : mMesh(mesh), mScaling(1.0,1.0,1.0), 
                           mCacheInvalidate(true) {}

    inline void setPosition(const glm::vec3& position) { 
        mPosition = position;
        mCacheInvalidate = true;
    }
    inline const glm::vec3& getPosition() const { return mPosition; }

    inline void setOrientation(const glm::quat& o) { 
        mOrientation = o; 
        mCacheInvalidate = true;
    }
    inline const glm::quat& getOrientation() const { return mOrientation; }
    inline void setScaling(const glm::vec3& scaling) { 
        mScaling = scaling; 
        mCacheInvalidate = true;
    }
    inline const glm::vec3& getScaling() const { return mScaling; }
    
    // TODO ottimizzare sta cosa
    glm::mat4 getModelMatrix() {
        updateCache();
        return mCachedModelMatrix;
    }

    glm::mat3 getNormalMatrix() {
        updateCache();
        return mCachedRotationMatrix;
    }

    MeshPtr getMesh() { return mMesh; }
private:
    MeshPtr mMesh;
    glm::vec3 mPosition, mScaling;
    glm::quat mOrientation;
    bool mCacheInvalidate;
    glm::mat4 mCachedModelMatrix;
    glm::mat3 mCachedRotationMatrix;

    inline void updateCache() {
        if(mCacheInvalidate) {
            glm::mat4 scaling = glm::scale(glm::mat4(1.0f), mScaling);
            glm::mat4 rotate = glm::mat4_cast(mOrientation);
            glm::mat4 translate = glm::translate(glm::mat4(1.0f), mPosition);
            mCachedModelMatrix = translate * rotate * scaling;
            mCachedRotationMatrix = glm::mat3_cast(mOrientation); // TODO derivarla da rotate
            mCacheInvalidate = false;
        }
    }
};

typedef std::shared_ptr<Object> ObjectPtr;
    
} /* GLEngine */ 

#endif /* ifndef GLENGINE_MESH_H */

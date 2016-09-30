#ifndef GLENGINE_CAMERA_H
#define GLENGINE_CAMERA_H

#include <GLEngine/Common.h>

namespace GLEngine
{

class Camera {
public: 
    const glm::mat4 getVPMatrix() const {
        return mProjMatrix * mViewMatrix;
    }
protected:
    glm::mat4 mViewMatrix, mProjMatrix;
    
};

class LookAtCamera : public Camera {
public:
    LookAtCamera(float fov, float screenRatio) : 
                mCameraPos(0,0,10), mTargetPos(0,0,0), mUpVector(0,1,0) {
        mProjMatrix = glm::perspective(glm::radians(fov), screenRatio, 0.01f,
                                       1000.0f);
        update();
    }

    void setCameraPos(const glm::vec3& pos) { mCameraPos = pos; }
    void setTargetPos(const glm::vec3& pos) { mTargetPos = pos; }
    void setUpVector(const glm::vec3& pos) { mUpVector = pos; }

    glm::vec3 getCameraPos() { return mCameraPos; }

    void update() {
        mViewMatrix = glm::lookAt(mCameraPos, mTargetPos, mUpVector);
    }
private:
    glm::vec3 mCameraPos, mTargetPos, mUpVector;
};
    
} /* GLEngine */ 

#endif /* ifndef GLENGINE_CAMERA_H */

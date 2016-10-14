#ifndef GLENGINE_CAMERA_H
#define GLENGINE_CAMERA_H

#include <GLEngine/Common.h>
#include <GLEngine/InputHandler.h>

namespace GLEngine
{

class Camera {
public: 
    const glm::mat4 getViewMatrix() const { return mViewMatrix; }
    const glm::mat4 getProjMatrix() const { return mProjMatrix; }
    void setCameraPos(const glm::vec3& pos) { mCameraPos = pos; }
    glm::vec3 getCameraPos() { return mCameraPos; }
protected:
    glm::mat4 mViewMatrix, mProjMatrix;
    glm::vec3 mCameraPos;
};

class LookAtCamera : public Camera {
    TAG_DEF("LookAtCamera");
public:
    LookAtCamera(float fov, float screenRatio);

    void setTargetPos(const glm::vec3& pos) { mTargetPos = pos; }
    void setUpVector(const glm::vec3& pos) { mUpVector = pos; }

    void update();
protected:
    glm::vec3 mTargetPos, mUpVector;
};

class FirstPersonCamera : public Camera, public InputHandler {
    TAG_DEF("FirstPersonCamera");
public:
    FirstPersonCamera(float fov, float screenRatio);

    void update();

    void handleKeyPress(int key, int scancode, int action, int mods);
    void handleMouseMove(double xpos, double ypos);

private:
    float mBeta, mGamma, mAlpha;
    double mLastMouseX, mLastMouseY;
    glm::vec4 mDelta, mCameraSpeed;
    bool mFirstTime;
};
    
} /* GLEngine */ 

#endif /* ifndef GLENGINE_CAMERA_H */

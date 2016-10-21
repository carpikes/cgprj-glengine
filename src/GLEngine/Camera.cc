#include <GLEngine/Camera.h>

namespace GLEngine
{


LookAtCamera::LookAtCamera(float fov, float screenRatio) {
    mTargetPos = glm::vec3(0,0,0);
    mUpVector = glm::vec3(0,1,0);
    mCameraPos = glm::vec3(0,10,0);
    mProjMatrix = glm::perspective(glm::radians(fov), screenRatio, 0.01f,
                                    1000.0f);
    update();
}

void LookAtCamera::update() {
    mViewMatrix = glm::lookAt(mCameraPos, mTargetPos, mUpVector);
}


FirstPersonCamera::FirstPersonCamera(float fov, float screenRatio) {
    mFirstTime = true;
    mAlpha = 0;
    mBeta = mGamma = 0;
    mDelta = glm::vec4(0,0,0,1); 
    mCameraPos = glm::vec4(0,-5,0,1);
    mLastMouseX = mLastMouseY = -1;

    mProjMatrix = glm::perspective(glm::radians(fov), screenRatio, 0.01f,
                                    1000.0f);
}

void FirstPersonCamera::update() {
    mCameraPos.y = std::min(mCameraPos.y, -5.0f);
    mAlpha = std::max(mAlpha, -3.14f/2.0f+0.1f);
    mAlpha = std::min(mAlpha, 3.14f/2.0f-0.1f);
    glm::mat4 m1 = glm::translate(glm::vec3(mCameraPos)), m;

    // non cambiare l'ordine, questo e` perfetto :P
    m = glm::rotate(m, mGamma, glm::vec3(0,0,1));
    m = glm::rotate(m, mAlpha, glm::vec3(1,0,0));
    m = glm::rotate(m, mBeta, glm::vec3(0,1,0));

    constexpr float MaxSpeed = 5.0f;
    mCameraSpeed *= 0.85f;
    mCameraSpeed += (mDelta * m) / 3.0f;
    mCameraSpeed.x = std::min(MaxSpeed, std::max(mCameraSpeed.x, -MaxSpeed));
    mCameraSpeed.y = std::min(MaxSpeed, std::max(mCameraSpeed.y, -MaxSpeed));
    mCameraSpeed.z = std::min(MaxSpeed, std::max(mCameraSpeed.z, -MaxSpeed));
    mCameraPos += glm::vec3(mCameraSpeed);

    mViewMatrix = m * m1;
}

void FirstPersonCamera::handleKeyPress(int key, int scancode, int action, 
                                       int mods) {
    if(action < 0 || action > 1)
        return;

    switch(key) {
        case GLFW_KEY_W: mDelta.z = (float)action; break;
        case GLFW_KEY_A: mDelta.x = (float)action; break;
        case GLFW_KEY_S: mDelta.z = -1.0f * (float)action; break;
        case GLFW_KEY_D: mDelta.x = -1.0f * (float)action; break;
        break;
    }
    (void)scancode;
    (void)mods;
}

void FirstPersonCamera::handleMouseMove(double xpos, double ypos) {
    if(mFirstTime == false) {
        float dx = (xpos - mLastMouseX) / 1024.0f;
        float dy = (ypos - mLastMouseY) / 768.0f;

        mBeta += dx * 8.0f;
        mAlpha += dy * 8.0f;
    }

    mLastMouseX = xpos;
    mLastMouseY = ypos;
    mFirstTime = false;
}

} /* GLEngine */ 

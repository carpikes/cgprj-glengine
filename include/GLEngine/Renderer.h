#ifndef GLENGINE_RENDERER_H
#define GLENGINE_RENDERER_H

#include <GLEngine/Common.h>
#include <GLEngine/Material.h>
#include <GLEngine/Utils.h>
#include <GLEngine/ResourceManager.h>
#include <GLEngine/ShaderManager.h>
#include <GLEngine/InputHandler.h>

namespace GLEngine
{

using std::string;

class Renderer {
    TAG_DEF("Renderer")
public:

    bool init(size_t width, size_t height, const string& title, 
              uint32_t aaSamples);

    bool isRunning();
    void prepareFrame();
    void endFrame();
    void run();

    void setMatrices(const glm::mat4& modelView, 
                     const glm::mat4& modelViewProj,
                     const glm::mat3& normalMat);
    void setLightRot(const glm::vec3& light);
    void setEyePos(const glm::vec3& eye);
    void setPointLight(size_t i, glm::vec3 position, glm::vec3 atten,
                                 glm::vec3 ambient, glm::vec3 diffuse,
                                 glm::vec3 specular);

    void onKeyPress(int key, int scancode, int action, int mods);
    void onMouseMove(double xpos, double ypos);

    bool allocateVertexBuffers(size_t number, std::vector<VideoPtr>& out);
    bool allocateTextureBuffers(size_t number, std::vector<VideoPtr>& out);

    void writeVertices(VideoPtr buffer, const std::vector<Vertex>& vertices);
    void writeTexture(const std::vector<VideoPtr>& buffer, 
            const std::unordered_set<Image *>& images);

    void setPath(const string& path) {
        mShaderManager.setPath(path);
    }

    void registerInputHandler(InputHandler *in) {
        mInputHandlers.push_back(in); 
    }

private:
    VideoPtr mProgramId, mLightPosPtr, mLightRotPtr, mEyePosPtr;
    VideoPtr mModelViewPtr, mModelViewProjPtr, mNormalMatrixPtr;
    ShaderManager mShaderManager;
    GLFWwindow* mWindow;
    size_t mWidth, mHeight;
    uint32_t mAASamples;
    std::vector<InputHandler *> mInputHandlers;

    struct PointLightPtr {
        VideoPtr position, attenuation, ambient, diffuse, specular;
    } mLights[8];
};

} /* GLEngine */ 

#endif /* ifndef GLENGINE_RENDERER_H */

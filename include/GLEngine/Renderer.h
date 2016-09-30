#ifndef GLENGINE_RENDERER_H
#define GLENGINE_RENDERER_H

#include <GLEngine/Common.h>
#include <GLEngine/Material.h>
#include <GLEngine/Utils.h>
#include <GLEngine/ResourceManager.h>
#include <GLEngine/ShaderManager.h>

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

    void setMVP(const glm::mat4& mvp);
    void setLightPos(const glm::vec3& light);
    void setLightRot(const glm::vec3& light);
    void setEyePos(const glm::vec3& eye);

    bool allocateVertexBuffers(size_t number, std::vector<VideoPtr>& out);
    bool allocateTextureBuffers(size_t number, std::vector<VideoPtr>& out);

    void writeVertices(VideoPtr buffer, const std::vector<Vertex>& vertices);
    void writeTexture(const std::vector<VideoPtr>& buffer, 
            const std::unordered_set<Image *>& images);

    void setPath(const string& path) {
        mShaderManager.setPath(path);
    }

private:
    VideoPtr mProgramId, mMVPPtr, mLightPosPtr, mLightRotPtr, mEyePosPtr;
    ShaderManager mShaderManager;
    GLFWwindow* mWindow;
    size_t mWidth, mHeight;
    uint32_t mAASamples;
};

} /* GLEngine */ 

#endif /* ifndef GLENGINE_RENDERER_H */

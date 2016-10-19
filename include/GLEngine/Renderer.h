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

    void setAmbientLight(const glm::vec3& direction, 
                         const glm::vec3& ambientColor, 
                         const glm::vec3& lightColor, 
                         const glm::vec3& specularStrength);

    void setEyePos(const glm::vec3& eye);
    void setPointLight(size_t i, glm::vec3 position, glm::vec3 atten,
                                 glm::vec3 ambient, glm::vec3 diffuse,
                                 glm::vec3 specular);

    void setMaterialParams(const Material *mat);

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

    enum eTextures {
        AMBIENT_TEXTURE     = 1,
        DIFFUSE_TEXTURE     = 2,
        SPECULAR_TEXTURE    = 3,
        BUMP_TEXTURE        = 4,
        DISPLACE_TEXTURE    = 5,
    };

    struct MaterialPtr {
        VideoPtr ambientColor, ambientTexture;
        VideoPtr diffuseColor, diffuseTexture;
        VideoPtr specularTexture, specularExponent;
        VideoPtr bumpTexture, displacementTexture;
        VideoPtr flags;
    } mMaterialPtr;

    struct AmbientLightPtr {
        VideoPtr direction, ambient, diffuse, strength;
    } mAmbientLightPtr;

    void setVec3(const VideoPtr ptr, const glm::vec3& v) {
        glUniform3f(ptr, v.x, v.y, v.z);
    }

    void setFloat(const VideoPtr ptr, const float v) {
        glUniform1f(ptr, v);
    }

    uint32_t setTexture(const VideoPtr ptr, const Texture* t, uint8_t pos) {
        if(t == nullptr || t->img == nullptr)
            return 0;
        glActiveTexture(GL_TEXTURE0 + (pos-1));
        glUniform1i(ptr, (pos-1));
        glBindTexture(GL_TEXTURE_2D, t->img->videoPtr());
        return (1 << pos);
    }

};

} /* GLEngine */ 

#endif /* ifndef GLENGINE_RENDERER_H */

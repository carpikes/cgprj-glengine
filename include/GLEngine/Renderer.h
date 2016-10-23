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

    void setMatrices(const glm::mat4& modelToWorld, 
                     const glm::mat4& modelViewProj,
                     const glm::mat3& normalMat);

    void disableLight(size_t i) {
        assert(i < 8);
        setBool(mLights[i].isEnabled, false);
    }
    void setAmbientLight(size_t i, const glm::vec3& direction, 
                                   const glm::vec3& ambientColor, 
                                   const glm::vec3& lightColor);

    void setPointLight(size_t i, glm::vec3 position, glm::vec3 atten,
                                 glm::vec3 ambient, glm::vec3 diffuse);

    void setEyePos(const glm::vec3& eye);

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

    void setDebugHV(glm::vec4& hv) {
        glUniform3f(uDebugHV, hv.x, hv.y, hv.z);
    }

private:
    VideoPtr mProgramId, mLightPosPtr, mLightRotPtr, mEyePosPtr;
    VideoPtr mModelToWorldPtr, mModelViewProjPtr, mNormalMatrixPtr;
    ShaderManager mShaderManager;
    GLFWwindow* mWindow;
    size_t mWidth, mHeight;
    uint32_t mAASamples;
    VideoPtr uDebugHV;
    std::vector<InputHandler *> mInputHandlers;

    struct LightPtr {
        VideoPtr isEnabled, isLocal, isSpot, ambient, color, WS_position;
        VideoPtr WS_halfVector, coneDirection, spotCosCutoff, spotExponent;
        VideoPtr attenuation;
    } mLights[8];

    enum eTextures {
        AMBIENT_TEXTURE     = 0,
        DIFFUSE_TEXTURE     = 1,
        SPECULAR_TEXTURE    = 2,
        BUMP_TEXTURE        = 3,
        DISPLACE_TEXTURE    = 4,
    };

    struct MaterialPtr {
        VideoPtr ambientColor, ambientTexture;
        VideoPtr diffuseColor, diffuseTexture;
        VideoPtr specularTexture, specularExponent;
        VideoPtr bumpTexture, displacementTexture;
        VideoPtr flags;
    } mMaterialPtr;

    void setBool(const VideoPtr ptr, bool b) {
        glUniform1i(ptr, b);
    }

    void setVec3(const VideoPtr ptr, const glm::vec3& v) {
        glUniform3f(ptr, v.x, v.y, v.z);
    }

    void setFloat(const VideoPtr ptr, const float v) {
        glUniform1f(ptr, v);
    }

    uint32_t setTexture(const VideoPtr ptr, const Texture* t, uint8_t pos) {
        if(t == nullptr || t->img == nullptr)
            return 0;

        glActiveTexture(GL_TEXTURE0 + pos);
        glUniform1i(ptr, pos);
        glBindTexture(GL_TEXTURE_2D, t->img->videoPtr());
        return (1 << pos);
    }

};

} /* GLEngine */ 

#endif /* ifndef GLENGINE_RENDERER_H */

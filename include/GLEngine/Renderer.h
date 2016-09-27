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

    void run();

    // da implementare. meglio qua o da altre parti? (tipo engine)

    bool allocateVertexBuffers(size_t number, std::vector<VideoPtr>& out);
    bool allocateTextureBuffers(size_t number, std::vector<VideoPtr>& out);

    void writeVertices(VideoPtr buffer, const std::vector<Vertex>& vertices);
    void writeTexture(const std::vector<VideoPtr>& buffer, 
            const std::unordered_set<Image *>& images);
private:
    ShaderManager mShaderManager;
    GLFWwindow* mWindow;
    size_t mWidth, mHeight;
    uint32_t mAASamples;
};

} /* GLEngine */ 

#endif /* ifndef GLENGINE_RENDERER_H */
#ifndef GLENGINE_DEVICE_H
#define GLENGINE_DEVICE_H

#include "Common.h"
#include "InputHandler.h"
#include "Image.h"

namespace GLEngine
{

class Device {
    TAG_DEF("Device")
public:
    bool init(size_t width, size_t height, const std::string& title, 
              uint32_t aaSamples);
    bool isRunning();

    void beginFrame();
    void endFrame();

    void onKeyPress(int key, int scancode, int action, int mods);
    void onMouseMove(double xpos, double ypos);

    bool allocVertexBuffers(size_t number, std::vector<VideoPtr>& out);
    bool allocTextureBuffers(size_t number, std::vector<VideoPtr>& out);

    void writeVertices(VideoPtr buffer, 
            const std::vector<Vertex>& vertices);
    void writeTexture(const std::vector<VideoPtr>& buffer, 
            const std::unordered_set<Image *>& images);

    size_t width() const { return mWidth; }
    size_t height() const { return mHeight; }

    void registerInputHandler(InputHandler *in) {
        mInputHandlers.push_back(in); 
    }

    VideoPtr getDefaultVAO() const {
        return mDefaultVAO;
    }
private:
    GLFWwindow* mWindow;
    size_t mWidth, mHeight;
    uint32_t mAASamples;
    std::vector<InputHandler *> mInputHandlers;
    VideoPtr mDefaultVAO;
};
    
} /* GLEngine */ 

#endif /* ifndef GLENGINE_DEVICE_H */

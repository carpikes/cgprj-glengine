#include <GLEngine/Device.h>

namespace GLEngine
{

void sKeyCallback(GLFWwindow *w, int key, int scancode, int action, int mods) {
    Device *device = (Device *) glfwGetWindowUserPointer(w);
    if(device != nullptr)
        device->onKeyPress(key, scancode, action, mods);
}

void sMouseCallback(GLFWwindow *w, double xpos, double ypos) {
    Device *device = (Device *)glfwGetWindowUserPointer(w);
    if(device != nullptr)
        device->onMouseMove(xpos, ypos);
}

/* Reference:   http://www.opengl-tutorial.org/beginners-tutorials/
 *              tutorial-1-opening-a-window/
 */
bool Device::init(size_t width, size_t height, const std::string& title, 
            uint32_t aaSamples) {

    if(!glfwInit()) {
        ERR("Cannot init GLFW");
        return false;
    }

    mWidth = width;
    mHeight = height;
    mAASamples = aaSamples;

    glfwWindowHint(GLFW_SAMPLES, aaSamples); 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    mWindow = glfwCreateWindow( mWidth, mHeight, title.c_str(), NULL, NULL);
    if(mWindow == NULL) {
        ERR("Cannot open a GLFW window.");
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(mWindow);

    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        ERR("Cannot initialize GLEW");
        glfwTerminate();
        return false;
    }

    glGenVertexArrays(1, &mDefaultVAO);
    glBindVertexArray(mDefaultVAO);

    glfwSetWindowUserPointer(mWindow, this);
    glfwSetInputMode(mWindow, GLFW_STICKY_KEYS, GL_TRUE); 
    glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 
    glfwSetKeyCallback(mWindow, sKeyCallback);
    glfwSetCursorPosCallback(mWindow, sMouseCallback);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    return true;
}

bool Device::isRunning() {
    return  glfwGetKey(mWindow, GLFW_KEY_ESCAPE) != GLFW_PRESS && 
           !glfwWindowShouldClose(mWindow);
}

void Device::beginFrame() {
    glClearColor(0.0f,0.0f,0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Device::endFrame() {
    glfwSwapBuffers(mWindow);
    glfwPollEvents();
}

bool Device::allocVertexBuffers(size_t number, std::vector<VideoPtr>& out) {
    out.resize(number, InvalidVideoPtr);
    glGenBuffers(number, &out[0]);
    return true;
}

bool Device::allocTextureBuffers(size_t number, std::vector<VideoPtr>& out) {
    out.resize(number, InvalidVideoPtr);
    glGenTextures(number, &out[0]);
    return true;
}

void Device::writeVertices(VideoPtr buffer, 
        const std::vector<Vertex>& vertices) {
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 
            vertices.size() * sizeof(Vertex) + 4, 
            &vertices[0], GL_STATIC_DRAW); // mmh..static..
}

void Device::writeTexture(const std::vector<VideoPtr>& buffer, 
        const std::unordered_set<Image *>& images) {
    assert(buffer.size() == images.size());

    size_t bufpos = 0;
    for(Image* img : images) {
        img->setVideoPtr(buffer[bufpos]);
        glBindTexture(GL_TEXTURE_2D, buffer[bufpos]);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img->width(), img->height(),
                0, GL_RGB, GL_UNSIGNED_BYTE, &(img->Data()[0]));

        //float aniso = 0.0f;
        //glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
        //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);

        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        bufpos++;
    }
}

void Device::onKeyPress(int key, int scancode, int action, int mods) {
    for(InputHandler *i : mInputHandlers)
        i->handleKeyPress(key, scancode, action, mods);
}

void Device::onMouseMove(double xpos, double ypos) {
    for(InputHandler *i : mInputHandlers)
        i->handleMouseMove(xpos, ypos);
}

    
} /* GLEngine */ 

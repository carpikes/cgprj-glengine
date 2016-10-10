#include <GLEngine/Renderer.h>

namespace GLEngine
{

void sKeyCallback(GLFWwindow *w, int key, int scancode, int action, int mods) {
    Renderer *renderer = (Renderer *)glfwGetWindowUserPointer(w);
    renderer->onKeyPress(key, scancode, action, mods);
}

void sMouseCallback(GLFWwindow *w, double xpos, double ypos) {
    Renderer *renderer = (Renderer *)glfwGetWindowUserPointer(w);
    renderer->onMouseMove(xpos, ypos);
}

/* Reference:   http://www.opengl-tutorial.org/beginners-tutorials/
 *              tutorial-1-opening-a-window/
 */
bool Renderer::init(size_t width, size_t height, const string& title, 
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

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    glfwSetWindowUserPointer(mWindow, this);
    glfwSetInputMode(mWindow, GLFW_STICKY_KEYS, GL_TRUE); 
    glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 
    glfwSetKeyCallback(mWindow, sKeyCallback);
    glfwSetCursorPosCallback(mWindow, sMouseCallback);

    mProgramId = mShaderManager.load( {
        { ShaderType::VERTEX_SHADER, "main.vs" },
        { ShaderType::FRAGMENT_SHADER, "main.fs" }
    });

    if(mProgramId == 0)
        return false; 

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glUseProgram(mProgramId);
    mMVPPtr = glGetUniformLocation(mProgramId, "MVP");
    mNormalMatrixPtr = glGetUniformLocation(mProgramId, "normalMatrix");
    mLightPosPtr = glGetUniformLocation(mProgramId, "uLightPos");
    mLightRotPtr = glGetUniformLocation(mProgramId, "uLightRot");
    mEyePosPtr = glGetUniformLocation(mProgramId, "uEyePos");

    assert(mMVPPtr != GL_INVALID_VALUE);
    assert(mNormalMatrixPtr != GL_INVALID_VALUE);

    return true;
}

bool Renderer::isRunning() {
    return  glfwGetKey(mWindow, GLFW_KEY_ESCAPE) != GLFW_PRESS && 
           !glfwWindowShouldClose(mWindow);
}

void Renderer::prepareFrame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::endFrame() {
    glfwSwapBuffers(mWindow);
    glfwPollEvents();
    usleep(20000);
}

void Renderer::setMVP(const glm::mat4& mvp) {
    glUniformMatrix4fv(mMVPPtr, 1, GL_FALSE, &mvp[0][0]);
}

void Renderer::setNormalMatrix(const glm::mat3& normal) {
    glUniformMatrix3fv(mNormalMatrixPtr, 1, GL_FALSE, &normal[0][0]);
}

void Renderer::setLightPos(const glm::vec3& light) {
    glUniform3f(mLightPosPtr, light.x, light.y, light.z);
}

void Renderer::setLightRot(const glm::vec3& light) {
    glUniform3f(mLightRotPtr, light.x, light.y, light.z);
}

void Renderer::setEyePos(const glm::vec3& eye) {
    glUniform3f(mEyePosPtr, eye.x, eye.y, eye.z);
}

void Renderer::run() {
}

bool Renderer::allocateVertexBuffers(size_t number, std::vector<VideoPtr>& out) {
    LOGP("Allocating %u vertex buffers", number);
    out.resize(number, InvalidVideoPtr);
    glGenBuffers(number, &out[0]);
    return true;
}

bool Renderer::allocateTextureBuffers(size_t number, std::vector<VideoPtr>& out) {
    LOGP("Allocating %u texture buffers", number);
    out.resize(number, InvalidVideoPtr);
    glGenTextures(number, &out[0]);
    return true;
}

void Renderer::writeVertices(VideoPtr buffer, const std::vector<Vertex>& vertices) {
    //LOGP("Writing %u vertices to videocard", vertices.size());
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 
            (vertices.size()) * sizeof(Vertex), 
            &vertices[0], GL_STATIC_DRAW); // mmh..static..
}

void Renderer::writeTexture(const std::vector<VideoPtr>& buffer, 
        const std::unordered_set<Image *>& images) {
    LOGP("Moving %u textures to videocard", images.size());

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

void Renderer::onKeyPress(int key, int scancode, int action, int mods) {
    for(InputHandler *i : mInputHandlers)
        i->handleKeyPress(key, scancode, action, mods);
}

void Renderer::onMouseMove(double xpos, double ypos) {
    for(InputHandler *i : mInputHandlers)
        i->handleMouseMove(xpos, ypos);
}

} /* GLEngine */ 

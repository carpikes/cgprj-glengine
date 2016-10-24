#include <GLEngine/Renderer.h>

namespace GLEngine
{

void sKeyCallback(GLFWwindow *w, int key, int scancode, int action, int mods) {
    Renderer *renderer = (Renderer *)glfwGetWindowUserPointer(w);
    if(renderer != nullptr)
        renderer->onKeyPress(key, scancode, action, mods);
}

void sMouseCallback(GLFWwindow *w, double xpos, double ypos) {
    Renderer *renderer = (Renderer *)glfwGetWindowUserPointer(w);
    if(renderer != nullptr)
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

    MaterialPtr &m = mMaterialPtr;

    struct { VideoPtr *ptr; const char *name; } uniforms[] = {
        { &mModelViewProjPtr,       "uModelViewProj"                },
        { &mModelToWorldPtr,        "uModelToWorld"                 },
        { &mNormalMatrixPtr,        "uWS_NormalMatrix"              },
        { &mEyePosPtr,              "uWS_EyePos"                    },
        { &mTimerPtr,               "uTimer"                        },
        { &mVideoTagPtr,            "uTag"                          },

        { &m.flags,                 "material.flags"                },
        { &m.ambientColor,          "material.ambientColor"         },
        { &m.diffuseColor,          "material.diffuseColor"         },
        { &m.ambientTexture,        "material.ambientTexture"       },
        { &m.diffuseTexture,        "material.diffuseTexture"       },
        //{ &m.specularTexture,       "material.specularTexture"      },
        { &m.specularExponent,      "material.specularExponent"     },
        //{ &m.bumpTexture,           "material.bumpTexture"          },
        //{ &m.displacementTexture,   "material.displacementTexture"  },
    };

    for(size_t i = 0; i < sizeof(uniforms) / sizeof(uniforms[0]); i++) {
        *uniforms[i].ptr = glGetUniformLocation(mProgramId, uniforms[i].name);
        if(*uniforms[i].ptr == 0xffffffff) {
            ERRP("Cannot obtain ptr for %s", uniforms[i].name);
            exit(-1);
        }
    }

#ifdef _U
#error _U is already defined
#endif

#define _U(x) do { \
    mLights[i].x = glGetUniformLocation(mProgramId, \
    Utils::getUniformName("lights", i, #x).c_str()); \
    if(mLights[i].x == 0xffffffff) { \
        ERRP("Cannot obtain ptr for %s", Utils::getUniformName("lights", i, #x).c_str()); \
        exit(-1); \
    } \
} while(0)

    for(int i=0;i<8;i++) {
        _U(isEnabled); _U(isLocal); _U(isSpot);
        _U(ambient); _U(color); _U(WS_position);
        _U(WS_halfVector); _U(coneDirection); _U(spotCosCutoff);
        _U(spotExponent); _U(attenuation);
    }
#undef _U

    
    return true;
}

void Renderer::setPointLight(size_t i, glm::vec3 position, glm::vec3 atten,
                                glm::vec3 ambient, glm::vec3 diffuse) {
    assert(i < 8);
    LightPtr &l = mLights[i];
    setBool(l.isEnabled, true);
    setBool(l.isLocal, true);
    setBool(l.isSpot, false);
    setVec3(l.WS_position, position);
    setVec3(l.attenuation, atten);
    setVec3(l.ambient, ambient);
    setVec3(l.color, diffuse);
}

void Renderer::setAmbientLight(size_t i, const glm::vec3& direction, 
                                         const glm::vec3& ambientColor, 
                                         const glm::vec3& lightColor) {
    // FIXME: half vector in world space
    assert(false);
    assert(i < 8);
    LightPtr &l = mLights[i];
    setBool(l.isEnabled, true);
    setBool(l.isLocal, false);
    setBool(l.isSpot, false);
    setVec3(l.WS_position, -direction);
    setVec3(l.ambient, ambientColor);
    setVec3(l.color, lightColor);
}

bool Renderer::isRunning() {
    return  glfwGetKey(mWindow, GLFW_KEY_ESCAPE) != GLFW_PRESS && 
           !glfwWindowShouldClose(mWindow);
}

void Renderer::prepareFrame(float timer) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUniform1f(mTimerPtr, timer);
}

void Renderer::endFrame() {
    glfwSwapBuffers(mWindow);
    glfwPollEvents();
}

void Renderer::setMatrices(const glm::mat4& modelToWorld, 
                           const glm::mat4& modelViewProj,
                           const glm::mat3& normalMat) {
    glUniformMatrix4fv(mModelToWorldPtr, 1, GL_FALSE, &modelToWorld[0][0]);
    glUniformMatrix4fv(mModelViewProjPtr, 1, GL_FALSE, &modelViewProj[0][0]);
    glUniformMatrix3fv(mNormalMatrixPtr, 1, GL_FALSE, &normalMat[0][0]);
}

void Renderer::setMaterialParams(const Material *mat) {
    uint32_t flags = 0;

    setVec3(mMaterialPtr.ambientColor, mat->mAmbientColor);
    setVec3(mMaterialPtr.diffuseColor, mat->mDiffuseColor);
    setFloat(mMaterialPtr.specularExponent, mat->mSpecularExponent);

    flags |= setTexture(mMaterialPtr.ambientTexture, 
                        mat->mAmbientTexture, AMBIENT_TEXTURE);
    flags |= setTexture(mMaterialPtr.diffuseTexture, 
                        mat->mDiffuseTexture, DIFFUSE_TEXTURE);
    flags |= setTexture(mMaterialPtr.specularTexture, 
                        mat->mSpecularTexture, SPECULAR_TEXTURE);
    flags |= setTexture(mMaterialPtr.bumpTexture, 
                        mat->mBumpTexture, BUMP_TEXTURE);
    flags |= setTexture(mMaterialPtr.displacementTexture,
                        mat->mDisplacementTexture, DISPLACE_TEXTURE);

    glUniform1i(mMaterialPtr.flags, flags);
}

// TODO in world space!
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
            vertices.size() * sizeof(Vertex) + 4, 
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

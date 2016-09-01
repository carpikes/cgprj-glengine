#ifndef GLENGINE_BACKEND_H
#define GLENGINE_BACKEND_H

#include <Common.h>

namespace GLEngine
{

using std::string;

class Backend {
    TAG_DEF("Backend")
public:

    /* Reference:   http://www.opengl-tutorial.org/beginners-tutorials/
     *              tutorial-1-opening-a-window/
     */
    bool init(size_t width, size_t height, const string& title, 
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
        if( mWindow == NULL ){
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

        return true;
    }

    void run() {
        glfwSetInputMode(mWindow, GLFW_STICKY_KEYS, GL_TRUE); 

        do {
            glfwSwapBuffers(mWindow);
            glfwPollEvents();
        } while( glfwGetKey(mWindow, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
                !glfwWindowShouldClose(mWindow));
    }
private:
    GLFWwindow* mWindow;
    size_t mWidth, mHeight;
    uint32_t mAASamples;
};

} /* GLEngine */ 

#endif /* ifndef GLENGINE_BACKEND_H */

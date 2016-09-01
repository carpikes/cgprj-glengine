#ifndef GLENGINE_BACKEND_H
#define GLENGINE_BACKEND_H

#include <Common.h>
#include <Object.h>

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

        GLuint VertexArrayID;
        glGenVertexArrays(1, &VertexArrayID);
        glBindVertexArray(VertexArrayID);

        return true;
    }

    void run() {
        glfwSetInputMode(mWindow, GLFW_STICKY_KEYS, GL_TRUE); 

        Object *obj = mObjects[0];
        // TODO ...^ lol

        GLuint vertexbuffer;
        glGenBuffers(1, &vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(obj->vertices().data()), 
                obj->vertices().data(), GL_STATIC_DRAW);

        do {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

            glDrawArrays(GL_TRIANGLES, 0, obj->vertices().size());
            glDisableVertexAttribArray(0);
            
            glfwSwapBuffers(mWindow);
            glfwPollEvents();
        } while( glfwGetKey(mWindow, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
                !glfwWindowShouldClose(mWindow));
    }

    void addObject(Object *o) {
        mObjects.push_back(o);
    }

private:
    GLFWwindow* mWindow;
    size_t mWidth, mHeight;
    uint32_t mAASamples;

    std::vector<Object *> mObjects;
};

} /* GLEngine */ 

#endif /* ifndef GLENGINE_BACKEND_H */

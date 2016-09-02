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
        glBufferData(GL_ARRAY_BUFFER, obj->vertices().size() * sizeof(glm::vec3), 
                &obj->vertices()[0], GL_STATIC_DRAW);

        GLuint programId = loadShader("main.vs", "main.fs");
        if(programId == 0)
            return; 

        glm::mat4 cameraMat = glm::lookAt(
            glm::vec3(5,1,2), glm::vec3(0,2,0), glm::vec3(0,1,0));
        glm::mat4 projMat = glm::perspective(
                glm::radians(75.0f), 4.0f/3.0f, 0.1f, 100.0f);

        LOGP("Vertices: %u\n", obj->vertices().size());
        float cnt = 0;
        do {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glUseProgram(programId);
            GLuint matrixID = glGetUniformLocation(programId, "MVP");

            glm::mat4 model = glm::rotate(cnt, glm::vec3(0,1,0));
            glm::mat4 mvpMat = projMat * cameraMat * model;
            
            glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvpMat[0][0]);

            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

            glDrawArrays(GL_TRIANGLES, 0, obj->vertices().size());
            glDisableVertexAttribArray(0);
            
            glfwSwapBuffers(mWindow);
            glfwPollEvents();
            cnt += 0.1f;
        } while( glfwGetKey(mWindow, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
                !glfwWindowShouldClose(mWindow));
    }

    void addObject(Object *o) {
        mObjects.push_back(o);
    }

    bool readFile(const string& file, string& out) {
        std::ifstream fp(file.c_str());
        if(!fp.good())
            return false;

        std::stringstream buf;
        buf << fp.rdbuf();
        out = buf.str();
        return true;
    }

    GLuint loadShader(const string& vsName, const string& fsName) {
        string vsCode, fsCode;
        if(!readFile(vsName, vsCode)) {
            ERRP("Cannot read vertex shader %s", vsName.c_str()); 
            return 0;
        }

        if(!readFile(fsName, fsCode)) {
            ERRP("Cannot read fragment shader %s", fsName.c_str()); 
            return 0;
        }

        GLint result = GL_FALSE;
        int infoLogLen;

        GLuint vsId = glCreateShader(GL_VERTEX_SHADER);
        GLuint fsId = glCreateShader(GL_FRAGMENT_SHADER);

        LOGP("Compiling vertex shader %s", vsName.c_str());
        const char* vsSrc = vsCode.c_str();
        glShaderSource(vsId, 1, &vsSrc, NULL);
        glCompileShader(vsId);
        
        glGetShaderiv(vsId, GL_COMPILE_STATUS, &result);
        glGetShaderiv(vsId, GL_INFO_LOG_LENGTH, &infoLogLen);
        if(infoLogLen > 0) {
            vector<char> errMsg(infoLogLen+1);
            glGetShaderInfoLog(vsId, infoLogLen, NULL, &errMsg[0]);
            ERRP("VS Compiler error: %s", &errMsg[0]);
            glDeleteShader(vsId);
            glDeleteShader(fsId);
            return 0;
        }

        LOGP("Compiling fragment shader %s", fsName.c_str());
        const char* fsSrc = fsCode.c_str();
        glShaderSource(fsId, 1, &fsSrc, NULL);
        glCompileShader(fsId);

        glGetShaderiv(fsId, GL_COMPILE_STATUS, &result);
        glGetShaderiv(fsId, GL_INFO_LOG_LENGTH, &infoLogLen);
        if(infoLogLen > 0) {
            vector<char> errMsg(infoLogLen+1);
            glGetShaderInfoLog(fsId, infoLogLen, NULL, &errMsg[0]);
            ERRP("FS Compiler error: %s", &errMsg[0]);
            glDeleteShader(vsId);
            glDeleteShader(fsId);
            return 0;
        }

        LOG("Linking");

        GLuint programId = glCreateProgram();
        glAttachShader(programId, vsId);
        glAttachShader(programId, fsId);
        glLinkProgram(programId);

        glGetShaderiv(programId, GL_LINK_STATUS, &result);
        glGetShaderiv(programId, GL_INFO_LOG_LENGTH, &infoLogLen);
        if(infoLogLen > 0) {
            vector<char> errMsg(infoLogLen+1);
            glGetShaderInfoLog(programId, infoLogLen, NULL, &errMsg[0]);
            ERRP("Linking error: %s", &errMsg[0]);
            glDeleteShader(vsId);
            glDeleteShader(fsId);
            return 0;
        }

        glDetachShader(programId, vsId);
        glDetachShader(programId, fsId);

        glDeleteShader(vsId);
        glDeleteShader(fsId);

        return programId;
    }

private:
    GLFWwindow* mWindow;
    size_t mWidth, mHeight;
    uint32_t mAASamples;

    std::vector<Object *> mObjects;
};

} /* GLEngine */ 

#endif /* ifndef GLENGINE_BACKEND_H */

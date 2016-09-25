#ifndef GLENGINE_BACKEND_H
#define GLENGINE_BACKEND_H

#include <Common.h>
#include <Object.h>
#include "ResourceManager.h"

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

    void loadTextures() {
        typedef std::unordered_map<string, Image*> map_t;
        const map_t m = ResourceManager<Image>::expose();

        GLuint *texId = new GLuint[m.size()];
        glGenTextures(m.size(), texId); 

        int cnt=0;
        LOGP("Moving %d textures to videocard", m.size());
        for(map_t::const_iterator i = m.cbegin(); i != m.cend(); i++) {
            Image *img = i->second;
            if(img == NULL) {
                ERR("Error, img == NULL");
                continue;
            }
            img->setGlId(texId[cnt]);

            glBindTexture(GL_TEXTURE_2D, texId[cnt]);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img->width(), img->height(),
                    0, GL_RGB, GL_UNSIGNED_BYTE, &(img->Data()[0]));

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            cnt++;
        }

        delete[] texId;
    }

    void run() {
        glfwSetInputMode(mWindow, GLFW_STICKY_KEYS, GL_TRUE); 

        vector<GLuint> vbuffers;
        for(size_t i=0;i<mObjects.size();i++) {
            Object *obj = mObjects[i];
            GLuint vertexbuffer;
            glGenBuffers(1, &vertexbuffer);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
            glBufferData(GL_ARRAY_BUFFER, 
                    (obj->vertices().size()) * sizeof(Vertex), 
                    &obj->vertices()[0], GL_STREAM_DRAW);

            LOGP("Array %i: %u", i, obj->vertices().size());
            vbuffers.push_back(vertexbuffer);
        }

        loadTextures();

        LOGP("VBuffers size: %u", vbuffers.size());

        GLuint programId = loadShader("main.vs", "main.fs");
        if(programId == 0)
            return; 

        glm::mat4 cameraMat = glm::lookAt(
            glm::vec3(5,2,2), glm::vec3(0,2,0), glm::vec3(0,1,0));
        glm::mat4 projMat = glm::perspective(
                glm::radians(75.0f), 4.0f/3.0f, 0.01f, 1000.0f);

        float cnt = 0;

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        //
        glm::vec3 light(0,0,0);
        do {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glUseProgram(programId);
            GLuint matrixID = glGetUniformLocation(programId, "MVP");
            GLuint lightID = glGetUniformLocation(programId, "uLight");

            glm::mat4 model = glm::rotate(cnt, glm::vec3(0,1,0));
            glm::mat4 mvpMat = projMat * cameraMat * model;
            
            glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvpMat[0][0]);

            light.x = cos(cnt*2.0f);
            light.z = sin(cnt*2.0f);
            light = glm::normalize(light);
            glUniform3f(lightID, light.x, light.y, light.z);

            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glEnableVertexAttribArray(2);
            for(size_t i=0;i<mObjects.size();i++) {
                glBindBuffer(GL_ARRAY_BUFFER, vbuffers[i]);
                Object *obj = mObjects[i];
                if(obj == NULL) { ERR("OBJ == null"); return; }
                Material *mtl = ResourceManager<Material>::get(obj->material());
                if(mtl == NULL) { ERR("MTL == null"); return; }
                if(mtl->mDiffuseTexture != NULL)
                    glBindTexture(GL_TEXTURE_2D, mtl->mDiffuseTexture->img->glId());
                else
                    glBindTexture(GL_TEXTURE_2D, 0);

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(2*sizeof(glm::vec3)));
                glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(sizeof(glm::vec3)));
                glDrawArrays(GL_TRIANGLES, 0, mObjects[i]->vertices().size());
            }
            glDisableVertexAttribArray(1);
            glDisableVertexAttribArray(0);
            
            glfwSwapBuffers(mWindow);
            glfwPollEvents();
            cnt += 0.01f;
        } while( glfwGetKey(mWindow, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
                !glfwWindowShouldClose(mWindow));
    }

    void addObject(ObjectGroup *o) {
        for(size_t i = 0;i<o->objects.size(); i++)
            mObjects.push_back(&o->objects[i]);
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

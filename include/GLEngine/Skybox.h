#ifndef GLENGINE_SKYBOX_H
#define GLENGINE_SKYBOX_H

#include "Common.h"
#include "Utils.h"
#include "ImageFactory.h"
#include "Camera.h"
#include "Scene.h"

namespace GLEngine
{

using std::vector;
using std::string;

// https://learnopengl.com/code_viewer.php?code=advanced/cubemaps_skybox
const GLfloat skyboxVertices[] = {
    -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
     
    -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f
};

class Skybox
{
    TAG_DEF("Skybox")
public:
    Skybox(const vector<string>& files) {
        assert(files.size() == 6);

        for(int i=0;i<6;i++)
        {
            ImagePtr img = ImageFactory::load(files[i]);
            assert(img != NULL);
            mBox.push_back(img); 
        }

        mShader = new Shader("../data/skybox.vs", "../data/skybox.fs");
        assert(mShader != NULL);
    }

    void enable() {
        LOG("Enable()");
        glGenTextures(1, &mTexID);
        glGenVertexArrays(1, &mSkyboxVAO);
        glGenBuffers(1, &mSkyboxVBO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, mTexID);
        for(int i=0;i<6;i++)
        {
            ImagePtr img = mBox[i];
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 
                         img->format(), img->width(), img->height(), 0, 
                         img->format(), GL_UNSIGNED_BYTE, &(img->Data())[0]);

            #define _CUBE_MAP GL_TEXTURE_CUBE_MAP
            glTexParameteri(_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            #undef _CMAP
        }

        glBindVertexArray(mSkyboxVAO);

        glBindBuffer(GL_ARRAY_BUFFER, mSkyboxVBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), 
                     &skyboxVertices, GL_STATIC_DRAW);

        glBindVertexArray(0);
    }

    void render(const Camera& c) {
        GLint oldCullFace, oldDepthFunc;

        const glm::mat4 viewMat = glm::mat4(glm::mat3(c.getViewMatrix()));
        const glm::mat4 projMat = c.getProjMatrix();

        glBindVertexArray(mSkyboxVAO);
        //glBindBuffer(GL_ARRAY_BUFFER, mSkyboxVBO);
        glEnableVertexAttribArray(0);

        glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFace);
        glGetIntegerv(GL_DEPTH_FUNC,&oldDepthFunc);

        glDepthMask(GL_FALSE);
        glCullFace(GL_FRONT);
        glDepthFunc(GL_LEQUAL);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, mTexID);

        mShader->enable();
        mShader->set("uViewProj", projMat * viewMat);
        mShader->set("uSkybox", 0);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        glDepthMask(GL_TRUE);
        glDisableVertexAttribArray(0);
        glBindVertexArray(0);

        glCullFace(oldCullFace);
        glDepthFunc(oldDepthFunc);
    }

    void disable() {
        glDeleteTextures(1, &mTexID);
        glDeleteVertexArrays(1, &mSkyboxVAO);
        glDeleteBuffers(1, &mSkyboxVBO);
    }

    ~Skybox() {
        delete mShader;
    }
private:
    vector<ImagePtr> mBox;
    VideoPtr mTexID;
    VideoPtr mSkyboxVAO, mSkyboxVBO;
    Shader *mShader;
};

typedef std::shared_ptr<Skybox> SkyboxPtr;
    
} /* GLEngine */ 

#endif /* ifndef GLENGINE_SKYBOX_H */

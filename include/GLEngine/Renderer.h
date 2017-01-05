#ifndef GLENGINE_RENDERER_H
#define GLENGINE_RENDERER_H

#include "Common.h"
#include "Material.h"
#include "Utils.h"
#include "Scene.h"
#include "ResourceManager.h"
#include "ShaderManager.h"
#include "MaterialManager.h"
#include "Device.h"
#include "Engine.h"

namespace GLEngine
{

using std::string;

class Renderer {
    TAG_DEF("Renderer")
public:
    Renderer(Device& device) : mScene(nullptr), mDevice(device) {}

    virtual bool setScene(ScenePtr scene) {
        if(mScene != nullptr)
            mScene->removeFromDevice(mDevice);
        mScene = scene;

        if(!mScene->loadInDevice(mDevice)) {
            mScene->removeFromDevice(mDevice);
            mScene = nullptr;
            return false;
        }

        return true;
    }

    // set clipping?

    virtual void renderFrame(const Camera& camera) = 0;
protected:
    ScenePtr mScene;
    Device& mDevice;
};

class DirectRenderer : public Renderer {
    TAG_DEF("DirectRenderer")
public:
    DirectRenderer(Device& device) : Renderer(device), mShader(nullptr) {}
    virtual bool setScene(ScenePtr scene);
    virtual void renderFrame(const Camera& camera);
private:
    Shader *mShader;
};

class LolRenderer 
{
    TAG_DEF("LolRenderer");
public:
    LolRenderer(Device& device) : mDirect(device)
    {
        glGenBuffers(1, &quad_vertexbuffer);

        glGenFramebuffers(1, &FramebufferName);
        glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

		glGenTextures(1, &renderedTexture);

		glBindTexture(GL_TEXTURE_2D, renderedTexture);

		glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, 1280, 720, 0,GL_RGB, 
					 GL_UNSIGNED_BYTE, 0);

		// Poor filtering. Needed !
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glGenRenderbuffers(1, &depthrenderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1280, 720);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
								  GL_RENDERBUFFER, depthrenderbuffer);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
						     renderedTexture, 0);

		GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

		assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

		mShader = new Shader("../data/void.vs", "../data/void.fs");
    }

    virtual void renderFrame(const Camera& camera)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
        glViewport(0,0,1280,720);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        mDirect.renderFrame(camera); 

        // --------------------------------
        
        GLuint quad_VertexArrayID;
        glGenVertexArrays(1, &quad_VertexArrayID);
        glBindVertexArray(quad_VertexArrayID);

        static const GLfloat g_quad_vertex_buffer_data[] = {
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            1.0f,  1.0f, 0.0f,
        };

        glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), 
                     g_quad_vertex_buffer_data, GL_STATIC_DRAW);


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, renderedTexture);

        mShader->enable();
        mShader->set("renderedTexture", 0);
        mShader->set("time",Time);
        Time+=0.02f;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0,0,1280,720);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glDrawArrays(GL_TRIANGLES, 0, 6); // From index 0 to 3 -> 1 triangle

        glDisableVertexAttribArray(0);
    }

    bool setScene(ScenePtr scene) {
        return mDirect.setScene(scene);
    }

private:
    float Time = 0;
    GLuint depthrenderbuffer;
    GLuint renderedTexture;
    GLuint FramebufferName = 0;
    GLuint quad_vertexbuffer;

    Shader *mShader;
    DirectRenderer mDirect;
};

class DeferredRenderer : public Renderer {
    TAG_DEF("DeferredRenderer")
public:

    virtual void renderFrame(const Camera& camera) {
        (void) camera;
    /*
        1) attiva il MRT con i vari shader
        2) renderizza
     */
    }
};

} /* GLEngine */ 

#endif /* ifndef GLENGINE_RENDERER_H */

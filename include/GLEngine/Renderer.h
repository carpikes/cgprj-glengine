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

class Pipeline {
protected:
    Pipeline(Device& device) : mDevice(device) {
    
    }

    Device& mDevice;
};

class ToonShaderPipeline : public Pipeline {
public:
    ToonShaderPipeline(Device &device) : Pipeline(device), mDirect(device) {
    
    }

private:
    DirectRenderer mDirect;
};

class GLTexture {
public:
    GLTexture() {
        glGenTextures(1, &mTex); 
    }

    ~GLTexture() {
        glDeleteTextures(1, &mTex); 
    }

    void enable() const {
        glBindTexture(GL_TEXTURE_2D, mTex); 
    }

    void allocate(int w, int h, GLenum format, GLenum type) const {
        enable();
		glTexImage2D(GL_TEXTURE_2D, 0,format, w, h, 0, format, type, 0);
        // valori di default
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    }

    VideoPtr get() const {
        return mTex;
    }
private:
    VideoPtr mTex;
};


class Renderbuffer {
public:
    Renderbuffer() {
        glGenRenderbuffers(1, &mRb); 
    }

    ~Renderbuffer() {
        glDeleteRenderbuffers(1, &mRb);
    }

    void enable() const {
        glBindRenderbuffer(GL_RENDERBUFFER, mRb);
    }

    VideoPtr get() const {
        return mRb;
    }

    void allocate(GLenum format, int w, int h) {
        enable();
        glRenderbufferStorage(GL_RENDERBUFFER, format, w, h);
        // todo controllare che w*h non superino GL_MAX_RENDERBUFFER_SIZE
        // Pag 186 librone opengl
    }
private:
    VideoPtr mRb;
};

class Framebuffer {
public:
    Framebuffer() {
        glGenFramebuffers(1, &mFb);
    }

    ~Framebuffer() {
        glDeleteFramebuffers(1, &mFb);
    }

    void enable() const {
        glBindFramebuffer(GL_FRAMEBUFFER, mFb);
    }

    VideoPtr get() const { 
        return mFb; 
    }

    bool isOk() const {
		return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    }

    void linkTo(const Renderbuffer& rb, GLenum attachment) {
        enable();
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, 
                                  GL_RENDERBUFFER, rb.get());
    }

    void linkTo(const GLTexture& tex, GLenum attachment) {
        enable();
		glFramebufferTexture(GL_FRAMEBUFFER, attachment, tex.get(), 0);
    }
private:
    VideoPtr mFb;
};

class LolRenderer 
{
    TAG_DEF("LolRenderer");
public:
    LolRenderer(Device& device) : mDirect(device)
    {
        glGenBuffers(1, &mQuadVBO);

        mRenderTexture.allocate(1280, 720, GL_RGB, GL_UNSIGNED_BYTE);
        mRenderBuffer.allocate(GL_DEPTH_COMPONENT, 1280, 720);

        mFrameBuffer.linkTo(mRenderBuffer, GL_DEPTH_ATTACHMENT);
        mFrameBuffer.linkTo(mRenderTexture, GL_COLOR_ATTACHMENT0);

		GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

        assert(mFrameBuffer.isOk());

		mShader = new Shader("../data/void.vs", "../data/toon_shader.fs");

        glGenVertexArrays(1, &mQuadVAO);
        glBindVertexArray(mQuadVAO);

        static const GLfloat g_quad_vertex_buffer_data[] = {
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            1.0f,  1.0f, 0.0f,
        };

        glBindBuffer(GL_ARRAY_BUFFER, mQuadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), 
                     g_quad_vertex_buffer_data, GL_STATIC_DRAW);
    }

    virtual ~LolRenderer() {
        glDeleteBuffers(1, &mQuadVBO); 
        glDeleteVertexArrays(1, &mQuadVAO);
    }

    virtual void renderFrame(const Camera& camera)
    {
        mFrameBuffer.enable();
        glViewport(0,0,1280,720);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        mDirect.renderFrame(camera); 

        // --------------------------------
        
        glActiveTexture(GL_TEXTURE0);
        mRenderTexture.enable();

        mShader->enable();
        mShader->set("renderedTexture", 0);
        mShader->set("time",Time);
        Time+=0.02f;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0,0,1280,720);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, mQuadVBO);
		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(0);
    }

    bool setScene(ScenePtr scene) {
        return mDirect.setScene(scene);
    }

private:
    float Time = 0;
    GLuint mQuadVBO;
    GLuint mQuadVAO;
    Framebuffer mFrameBuffer;
    Renderbuffer mRenderBuffer;
    GLTexture mRenderTexture;

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

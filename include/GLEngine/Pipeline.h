#ifndef GLENGINE_PIPELINE_H
#define GLENGINE_PIPELINE_H

#include "Common.h"
#include "Renderer.h"

namespace GLEngine
{
    
class Pipeline {
protected:
    Pipeline(Device& device) : mDevice(device) { }

    virtual void renderFrame(ScenePtr scene, const Camera& camera) = 0;

    Device& mDevice;
};

class SimplePipeline : public Pipeline {
    TAG_DEF("SimplePipeline")
public:
    SimplePipeline(Device &device) : Pipeline(device), mDirect(device) {
        Framebuffer::enableNull();        
    }

    virtual void renderFrame(ScenePtr scene, const Camera& camera) {
        mDirect.renderFrame(scene, camera); 
    }
private:
    DirectRenderer mDirect;
};

class DeferredPipeline : public Pipeline {
    TAG_DEF("DeferredPipeline")
public:
    void setExposure(float v) { mExposure = v; }
    float getExposure() const { return mExposure; }

    DeferredPipeline(Device &device) : Pipeline(device), mFirstPass(device),
                                       mLightPass(device) {
        mExposure = 1.0f;
        glGenBuffers(1, &mQuadVBO);
		mShader = new Shader("../data/void.vs", "../data/void.fs");

        // CONFIGURE FIRST PASS BUFFER
        mRenderPosition.allocate(mDevice.width(), mDevice.height(), 
                                 GL_RGBA16F, GL_RGB, GL_FLOAT);
        mRenderNormal.allocate(mDevice.width(), mDevice.height(), 
                               GL_RGBA16F, GL_RGB, GL_FLOAT);
        mRenderAlbedo.allocate(mDevice.width(), mDevice.height(), 
                               GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);

        mRenderBuffer.allocate(GL_DEPTH_COMPONENT, mDevice.width(), 
                               mDevice.height());

        mLightTex.allocate(mDevice.width(), mDevice.height(), 
                          GL_RGBA16F, GL_RGB, GL_FLOAT);

        mFrameBuffer.enable();

        mFrameBuffer.linkTo(mRenderBuffer, GL_DEPTH_ATTACHMENT);
        mFrameBuffer.linkTo(mRenderPosition, GL_COLOR_ATTACHMENT0);
        mFrameBuffer.linkTo(mRenderNormal, GL_COLOR_ATTACHMENT1);
        mFrameBuffer.linkTo(mRenderAlbedo, GL_COLOR_ATTACHMENT2);

		GLenum DrawBuffers[3] = {
		    GL_COLOR_ATTACHMENT0, 
		    GL_COLOR_ATTACHMENT1,
            GL_COLOR_ATTACHMENT2
        };

		glDrawBuffers(3, DrawBuffers);

        assert(mFrameBuffer.isOk());
        
        // CONFIGURE LIGHT PASS BUFFER
        
        mLightFB.enable();
        mLightFB.linkTo(mRenderBuffer, GL_DEPTH_ATTACHMENT);
        mLightFB.linkTo(mLightTex, GL_COLOR_ATTACHMENT0);
		GLenum LightDrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, LightDrawBuffers);

		assert(mLightFB.isOk());

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

    ~DeferredPipeline() {
        glDeleteBuffers(1, &mQuadVBO); 
        glDeleteVertexArrays(1, &mQuadVAO);
        delete mShader;
    }

    virtual void renderFrame(ScenePtr scene, const Camera& camera)
    {
        // -- FIRST PASS: (GEOMETRY -> MRT)
        glClearColor(0,0,0,0);

        mFrameBuffer.enable();
        glViewport(0,0,mDevice.width(),mDevice.height());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        mFirstPass.renderFrame(scene, camera); 

        // -- LIGHT PASS (MRT -> TEXTURE)
        
        mLightFB.enable();

        mRenderPosition.enable(0);
        mRenderNormal.enable(1);
        mRenderAlbedo.enable(2);

        glViewport(0,0,mDevice.width(),mDevice.height());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mLightPass.renderFrame(scene, camera);

        glBindVertexArray(mQuadVAO);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, mQuadVBO);
		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(0);

        SkyboxPtr skybox = scene->getSkybox();
        if(skybox != nullptr)
            skybox->render(camera);

        // -- FINAL PASS (TEXTURE -> EFFECTS -> SCREEN)
        
        Framebuffer::enableNull();
        mLightTex.enable(0);

        glViewport(0,0,mDevice.width(),mDevice.height());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mShader->enable();
        mShader->set("renderedTexture", 0);
        mShader->set("exposure", mExposure);

        glBindVertexArray(mQuadVAO);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, mQuadVBO);
		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(0);

    }

private:
    GLuint mQuadVBO;
    GLuint mQuadVAO;
    float mExposure;
    Framebuffer mFrameBuffer, mLightFB;
    Renderbuffer mRenderBuffer;
    GLTexture mRenderPosition, mRenderNormal, mRenderAlbedo, mLightTex;

    DeferredFirstPass mFirstPass;
    DeferredLightPass mLightPass;
    Shader *mShader;
};


class ToonShaderPipeline : public Pipeline {
    TAG_DEF("ToonShaderPipeline")
public:
    ToonShaderPipeline(Device &device) : Pipeline(device), mDirect(device) {
        glGenBuffers(1, &mQuadVBO);

        mRenderTexture.allocate(mDevice.width(), mDevice.height(), GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);
        mRenderBuffer.allocate(GL_DEPTH_COMPONENT, mDevice.width(), mDevice.height());

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
        glBindVertexArray(0);
    }

    ~ToonShaderPipeline() {
        glDeleteBuffers(1, &mQuadVBO); 
        glDeleteVertexArrays(1, &mQuadVAO);
        delete mShader;
    }

    virtual void renderFrame(ScenePtr scene, const Camera& camera)
    {
        mFrameBuffer.enable();
        glViewport(0,0,mDevice.width(),mDevice.height());
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        mDirect.renderFrame(scene, camera); 

        // --------------------------------
        
        glBindVertexArray(mQuadVAO);
        mRenderTexture.enable(0);

        mShader->enable();
        mShader->set("renderedTexture", 0);

        Framebuffer::enableNull();
        glViewport(0,0,mDevice.width(),mDevice.height());

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, mQuadVBO);
		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(0);
        glBindVertexArray(0);
    }

private:
    GLuint mQuadVBO;
    GLuint mQuadVAO;
    Framebuffer mFrameBuffer;
    Renderbuffer mRenderBuffer;
    GLTexture mRenderTexture;

    Shader *mShader;
    DirectRenderer mDirect;
};

    
} /* GLEngine */ 

#endif /* ifndef GLENGINE_PIPELINE_H */

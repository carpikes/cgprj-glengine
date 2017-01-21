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

class ToonShaderPipeline : public Pipeline {
    TAG_DEF("ToonShaderPipeline")
public:
    ToonShaderPipeline(Device &device) : Pipeline(device), mDirect(device) {
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

    ~ToonShaderPipeline() {
        glDeleteBuffers(1, &mQuadVBO); 
        glDeleteVertexArrays(1, &mQuadVAO);
        delete mShader;
    }

    virtual void renderFrame(ScenePtr scene, const Camera& camera)
    {
        mFrameBuffer.enable();
        glViewport(0,0,1280,720);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        mDirect.renderFrame(scene, camera); 

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

    
} /* GLEngine */ 

#endif /* ifndef GLENGINE_PIPELINE_H */

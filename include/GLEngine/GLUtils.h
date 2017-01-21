#ifndef GLENGINE_GLUTILS_H
#define GLENGINE_GLUTILS_H

#include "Common.h"

namespace GLEngine 
{

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
    static void enableNull() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

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

}

#endif /* ifndef GLENGINE_GLUTILS_H */

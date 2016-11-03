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

    virtual void renderFrame(Camera *camera) = 0;
protected:
    ScenePtr mScene;
    Device& mDevice;
};

class DirectRender : public Renderer {
    TAG_DEF("DirectRenderer")
public:
    DirectRender(Device& device) : Renderer(device) {}
    virtual void renderFrame(Camera *camera) {
        MaterialManager& mtlMgr = sEngine.getMaterialManager();

        for(int i=0;i<3;i++)
            glEnableVertexAttribArray(i);

        glm::vec3 cameraPos = camera->getCameraPos();
        glm::mat4 viewMat = camera->getViewMatrix();
        glm::mat4 projMat = camera->getProjMatrix();

        Shader &shader = sEngine.getDefaultShader();
        shader.enable();
        shader.set("uWS_EyePos", cameraPos);

        for(const ObjectPtr o : mScene->getObjects()) {
            MeshPtr m = o->getMesh();
            if(m == nullptr)
                continue;

            glm::mat4 mvpMat = projMat * viewMat * o->getModelMatrix();

            shader.set("uModelToWorld", o->getModelMatrix());
            shader.set("uModelViewProj", mvpMat);
            shader.set("uWS_NormalMatrix", o->getNormalMatrix());

            // Da spostare nella mesh
            for(MeshPart& p : m->getParts()) {
                MaterialPtr mtl = mtlMgr.get(p.material());
                if(mtl == nullptr)
                    continue;

                VideoPtr ptr = p.videoPtr();

                glBindBuffer(GL_ARRAY_BUFFER, ptr);
                for(int i=0;i<3;i++)
                    glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, 
                                          sizeof(Vertex),
                                          (void *) (i * sizeof(glm::vec3)));

                mtl->enable(shader);

                glDrawArrays(GL_TRIANGLES, 0, p.vertices().size());
            } 
        }

        for(int i=0;i<3;i++)
            glDisableVertexAttribArray(i);
    }
};

class DeferredRenderer : public Renderer {
    TAG_DEF("DeferredRenderer")
public:

    virtual void renderFrame(Camera *camera) {
        (void) camera;
    /*
        1) attiva il MRT con i vari shader
        2) renderizza
     */
    }
};

} /* GLEngine */ 

#endif /* ifndef GLENGINE_RENDERER_H */

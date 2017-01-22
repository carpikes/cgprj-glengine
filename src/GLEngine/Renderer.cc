#include <GLEngine/Renderer.h>

namespace GLEngine
{

void DirectRenderer::renderFrame(ScenePtr scene, const Camera& camera) {
    if(mShader == nullptr)
        return;

    MaterialManager& mtlMgr = sEngine.getMaterialManager();

    glBindVertexArray(mDevice.getDefaultVAO());
    for(int i=0;i<4;i++)
        glEnableVertexAttribArray(i);

    glm::vec3 cameraPos = camera.getCameraPos();
    glm::mat4 viewMat = camera.getViewMatrix();
    glm::mat4 projMat = camera.getProjMatrix();

    mShader->enable();
    mShader->set("uWS_EyePos", cameraPos);
    
    auto& lights = scene->getLights();
    int n_light = 0;
    for(const PointLightPtr i : lights) {
        if(!i->enabled())
            continue;

        if(n_light >= mShader->getMaxNumberOfLights()) {
            ERR("Too many lights");
            break;
        }
        i->update(n_light++, *mShader);
    }

    for(const ObjectPtr o : scene->getObjects()) {
        MeshPtr mesh = o->getMesh();
        if(mesh == nullptr)
            continue;

        glm::mat4 mvpMat = projMat * viewMat * o->getModelMatrix();

        mShader->set("uModelToWorld", o->getModelMatrix());
        mShader->set("uModelViewProj", mvpMat);
        mShader->set("uWS_NormalMatrix", o->getNormalMatrix());

        if(scene->getAmbientLight() != nullptr)
            scene->getAmbientLight()->update(*mShader);
        if(scene->getHemiLight() != nullptr)
            scene->getHemiLight()->update(*mShader);

        // Da spostare nella mesh
        for(MeshPart& part : mesh->getParts()) {
            MaterialPtr mtl = mtlMgr.get(part.material());
            if(mtl == nullptr)
                continue;

            glBindBuffer(GL_ARRAY_BUFFER, part.videoPtr());
            for(int i=0;i<4;i++)
                glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                      (void *) (i * sizeof(glm::vec3)));

            mtl->enable(*mShader);

            glDrawArrays(GL_TRIANGLES, 0, part.vertices().size());
        } 
    }

    for(int i=0;i<4;i++)
        glDisableVertexAttribArray(i);
    glBindVertexArray(0);
}

void DeferredFirstPass::renderFrame(ScenePtr scene, const Camera& camera) {
    if(mShader == nullptr)
        return;

    glBindVertexArray(mDevice.getDefaultVAO());
    MaterialManager& mtlMgr = sEngine.getMaterialManager();

    for(int i=0;i<4;i++)
        glEnableVertexAttribArray(i);

    glm::mat4 viewMat = camera.getViewMatrix();
    glm::mat4 projMat = camera.getProjMatrix();

    mShader->enable();
    
    for(const ObjectPtr o : scene->getObjects()) {
        MeshPtr mesh = o->getMesh();
        if(mesh == nullptr)
            continue;

        glm::mat4 mvpMat = projMat * viewMat * o->getModelMatrix();

        mShader->set("uModelToWorld", o->getModelMatrix());
        mShader->set("uModelViewProj", mvpMat);
        mShader->set("uWS_NormalMatrix", o->getNormalMatrix());

        // Da spostare nella mesh
        for(MeshPart& part : mesh->getParts()) {
            MaterialPtr mtl = mtlMgr.get(part.material());
            if(mtl == nullptr)
                continue;

            glBindBuffer(GL_ARRAY_BUFFER, part.videoPtr());
            for(int i=0;i<4;i++)
                glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                      (void *) (i * sizeof(glm::vec3)));

            mtl->enable(*mShader);

            glDrawArrays(GL_TRIANGLES, 0, part.vertices().size());
        } 
    }

    for(int i=0;i<4;i++)
        glDisableVertexAttribArray(i);
}

void DeferredLightPass::renderFrame(ScenePtr scene, const Camera& camera) {
    if(mShader == nullptr)
        return;

    for(int i=0;i<4;i++)
        glEnableVertexAttribArray(i);

    glm::vec3 cameraPos = camera.getCameraPos();

    mShader->enable();
    mShader->set("gPosition", 0);
    mShader->set("gNormal", 1);
    mShader->set("gAlbedo", 2);
    mShader->set("uWS_EyePos", cameraPos);
    
    auto& lights = scene->getLights();
    int n_light = 0;
    for(const PointLightPtr i : lights) {
        if(!i->enabled())
            continue;

        if(n_light >= mShader->getMaxNumberOfLights()) {
            ERR("Too many lights");
            break;
        }
        i->update(n_light++, *mShader);
    }

    if(scene->getAmbientLight() != nullptr)
        scene->getAmbientLight()->update(*mShader);
    if(scene->getHemiLight() != nullptr)
        scene->getHemiLight()->update(*mShader);
}
} /* GLEngine */ 

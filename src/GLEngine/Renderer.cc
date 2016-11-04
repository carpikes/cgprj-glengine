#include <GLEngine/Renderer.h>

namespace GLEngine
{

bool DirectRenderer::setScene(ScenePtr scene) {
    if(!Renderer::setScene(scene))
        return false;

    mShader = &(sEngine.getDefaultShader());
    mShader->enable();

    return true;
}

void DirectRenderer::renderFrame(const Camera& camera) {
    if(mShader == nullptr)
        return;

    MaterialManager& mtlMgr = sEngine.getMaterialManager();

    for(int i=0;i<3;i++)
        glEnableVertexAttribArray(i);

    glm::vec3 cameraPos = camera.getCameraPos();
    glm::mat4 viewMat = camera.getViewMatrix();
    glm::mat4 projMat = camera.getProjMatrix();

    mShader->set("uWS_EyePos", cameraPos);
    
    auto& lights = mScene->getLights();
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

    for(const ObjectPtr o : mScene->getObjects()) {
        MeshPtr mesh = o->getMesh();
        if(mesh == nullptr)
            continue;

        glm::mat4 mvpMat = projMat * viewMat * o->getModelMatrix();

        mShader->set("uModelToWorld", o->getModelMatrix());
        mShader->set("uModelViewProj", mvpMat);
        mShader->set("uWS_NormalMatrix", o->getNormalMatrix());

        if(mScene->getAmbientLight() != nullptr)
            mScene->getAmbientLight()->update(*mShader, camera, 
                    o->getModelMatrix());


        // Da spostare nella mesh
        for(MeshPart& part : mesh->getParts()) {
            MaterialPtr mtl = mtlMgr.get(part.material());
            if(mtl == nullptr)
                continue;

            glBindBuffer(GL_ARRAY_BUFFER, part.videoPtr());
            for(int i=0;i<3;i++)
                glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                      (void *) (i * sizeof(glm::vec3)));

            mtl->enable(*mShader);

            glDrawArrays(GL_TRIANGLES, 0, part.vertices().size());
        } 
    }

    for(int i=0;i<3;i++)
        glDisableVertexAttribArray(i);
}

} /* GLEngine */ 

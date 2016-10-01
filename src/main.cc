#include <GLEngine/Common.h>
#include <GLEngine/Renderer.h>
#include <GLEngine/Scene.h>
#include <GLEngine/Engine.h>
#include <GLEngine/FileReader/OBJFileReader.h>
#include <GLEngine/ResourceManager.h>

using namespace GLEngine;

TAG_DEF("Main")

int main(int argc, char *argv[]) {
    LOG("Starting"); (void) argc; (void) argv;
    Engine glEngine("../data");

    MeshPtr pikachu(glEngine.getResourceManager().get<Mesh>("pikachu.obj"));
    if(!pikachu)
        return 0;

    MeshPtr pav(glEngine.getResourceManager().get<Mesh>("pav.obj"));
    if(!pav)
        return 0;

    Scene sc(&glEngine);

    for(int i=0;i<100;i++) {
        ObjectPtr pika = std::make_shared<Object>(pikachu);
        float px = (i / 10) * 5 - 25;
        float py = (i % 10) * 5 - 25;
        pika->setPosition(glm::vec3(px, 0, py));
        pika->setOrientation(glm::angleAxis(i / 6.28f, glm::vec3(0.f,1.f,0.f)));
        sc.addObject(pika);
    }
    ObjectPtr pPav = std::make_shared<Object>(pav);
    sc.addObject(pPav);

    glEngine.setScene(&sc);
    glEngine.run();
    //b.addObject(pikachu);

    //b.run();
    return 0;
}

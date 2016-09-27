#include <GLEngine/Common.h>
#include <GLEngine/Renderer.h>
#include <GLEngine/Engine.h>
#include <GLEngine/FileReader/OBJFileReader.h>
#include <GLEngine/ResourceManager.h>

using namespace GLEngine;

TAG_DEF("Main")

int main(int argc, char *argv[]) {
    LOG("Starting"); (void) argc; (void) argv;
    Engine glEngine;

    glEngine.setDataPath("../data");
    Mesh *pikachu = glEngine.getResourceManager().get<Mesh>("pikachu.obj");
    if(pikachu)
        LOG("Pikachu loaded ok");
    else 
        return 0;

    //b.addObject(pikachu);

    //b.run();
    return 0;
}

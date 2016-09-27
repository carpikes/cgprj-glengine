#include <GLEngine/Common.h>
#include <GLEngine/Renderer.h>
#include <GLEngine/Engine.h>
#include <GLEngine/FileReader/OBJFileReader.h>
#include <GLEngine/ResourceManager.h>

using namespace GLEngine;

TAG_DEF("Main")

int main(int argc, char *argv[])
{
    (void) argc; (void) argv;
    LOG("Starting");
    //Renderer b;
    Engine e;

    //b.init(1024,768, "Test", 4);

    //sResourceManager.setBackend(&b);

    e.getResourceManager().setPath("../data");
    Mesh *pikachu = e.getResourceManager().get<Mesh>("pikachu.obj");
    if(pikachu)
        LOG("Pikachu loaded ok");
    else 
        return 0;

    //b.addObject(pikachu);

    //b.run();
    return 0;
}

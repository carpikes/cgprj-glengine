#include <Common.h>
#include <PNG.h>
#include <Backend.h>
#include <FileReader/OBJFileReader.h>
#include <ResourceManager.h>

using namespace GLEngine;

TAG_DEF("Main")

int main(int argc, char *argv[])
{
    LOG("Starting");

    ResourceManager<ObjectGroup>::load("/tmp/blastoise/BR_Blastoise.obj");

    ObjectGroup *pikachu = ResourceManager<ObjectGroup>::get("/tmp/blastoise/BR_Blastoise.obj");
    if(pikachu)
        LOG("Pikachu loaded ok");
    else 
        return 0;

    Backend b;

    b.addObject(pikachu);
    b.init(1024, 768, "Title", 4);

    b.run();
    return 0;
}

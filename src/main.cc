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

//    const string filename = "/tmp/ala/XY_AlakazamMega.obj";
    const string filename = "pikachu.obj";
    ResourceManager<ObjectGroup>::load(filename);

    ObjectGroup *pikachu = ResourceManager<ObjectGroup>::get(filename);
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

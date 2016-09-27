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
    Backend b;

    b.init(1024,768, "Test", 4);

    sResourceManager.setBackend(&b);

    ObjectGroup *pikachu = sResourceManager.get<ObjectGroup>("pikachu.obj");
    if(pikachu)
        LOG("Pikachu loaded ok");
    else 
        return 0;

    b.addObject(pikachu);

    b.run();
    return 0;
}

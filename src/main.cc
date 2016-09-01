#include <Common.h>
#include <PNG.h>
#include <Backend.h>

using namespace GLEngine;

TAG_DEF("Main")

int main(int argc, char *argv[])
{
    LOG("Starting");
    shared_ptr<Image> img = ImageFactory::load("ciao.png");

    if(img == nullptr) {
        LOG("Cannot open ciao.png");
        return -1;
    }
    
    LOGP("ciao.png: W = %d, H = %d", img->width(), img->height());

    Backend b;

    b.init(1024, 768, "Title", 4);

    b.run();
    return 0;
}

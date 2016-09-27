#ifndef GLENGINE_IMAGEFACTORY_H
#define GLENGINE_IMAGEFACTORY_H

#include <GLEngine/Common.h>
#include <GLEngine/Image.h>
#include <GLEngine/FileReader/PNGFileReader.h>

namespace GLEngine
{

class ImageFactory {
    TAG_DEF("ImageFactory")
public:
    static Image *load(const std::string& name) {
        std::ifstream file(name.c_str(), std::ios::binary | std::ios::ate);
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        if(size < 4) {
            ERRP("Cannot open %s", name.c_str());
            return nullptr;
        }

        std::vector<char> buffer(size);
        if(!file.read(buffer.data(), size))  {
            ERRP("Cannot open %s", name.c_str());
            return nullptr;
        }

        if(PNGFileFormat::checkHeader(buffer)) {
            PNGFileFormat pngReader;
            Image *img = new Image;

            if(!pngReader.readImage(buffer, *img)) {
                ERRP("Cannot read %s", name.c_str());
                delete img;
                return nullptr;
            }

            DEBP("Loaded PNG image %s", name.c_str());
            return img;
        }

        ERRP("Unsupported file format for %s", name.c_str());
        return nullptr;
    }
private:
    ImageFactory();
    // TODO: 5 costruttori
};



} /* GLEngine */ 

#endif /* ifndef GLENGINE_IMAGEFACTORY_H */

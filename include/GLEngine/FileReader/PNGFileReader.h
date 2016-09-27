#ifndef GLENGINE_PNGFILEREADER_H
#define GLENGINE_PNGFILEREADER_H

#include <GLEngine/Common.h>
#include <GLEngine/Image.h>

namespace GLEngine
{

class ImageFileFormat {
public:
    virtual bool readImage(const std::vector<char>& data, Image& out) = 0;
};

class PNGFileFormat : ImageFileFormat {
    TAG_DEF("PNGFileFormat")
public:
    static bool checkHeader(const std::vector<char>& data) {
        if(data.size() < 8)
            return false;
        return !png_sig_cmp((uint8_t *)&data[0], 0, 8);
    }

    /* Reference: https://stackoverflow.com/questions/11296644/
     *            loading-png-textures-to-opengl-with-libpng-only
     */
    bool readImage(const std::vector<char>& data, Image& out);
private:
    std::vector<char> mReadData;
    size_t mReadOffset;

    /* Reference: https://github.com/EpicGames/UnrealEngine/blob/release/Engine/
     *            Source/Runtime/ImageWrapper/Private/PngImageWrapper.cpp
     */
    static void readData(png_structp ptr, png_bytep data, png_size_t len);
};

} /* GLEngine */ 

#endif /* ifndef GLENGINE_PNGFILEREADER_H */

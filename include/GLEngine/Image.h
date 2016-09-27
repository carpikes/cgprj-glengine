#ifndef GLENGINE_IMAGE_H
#define GLENGINE_IMAGE_H

#include <GLEngine/Common.h>

namespace GLEngine
{

/* Image contiene i bytes di un'immagine e sue info a riguardo */
class Image {
    TAG_DEF("Image")

    friend class ImageFactory;
    friend class PNGFileFormat;
    public:
        const std::vector<uint8_t>& Data() const { return mData; }
        size_t width() const { return mWidth; }
        size_t height() const { return mHeight; }
        GLint format() const { return mFormat; }

        VideoPtr videoPtr() const { return mId; }
        void setVideoPtr(VideoPtr id) { mId = id; }
    private:
        Image() { }

        std::vector<uint8_t> mData;
        size_t mWidth, mHeight;
        GLint mFormat;
        VideoPtr mId;
};

} /* GLEngine */ 

#endif /* ifndef GLENGINE_IMAGE_H */

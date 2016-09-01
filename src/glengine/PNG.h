#ifndef GLENGINE_PNG_H
#define GLENGINE_PNG_H

#include <Common.h>
#include <Log.h>

namespace GLEngine {

using std::ifstream;
using std::ios;
using std::shared_ptr;
using std::streamsize;
using std::string;
using std::vector;
using std::memcpy;

class Image {
    TAG_DEF("Image")

    friend class ImageFactory;
    friend class PNGFileFormat;
    public:
        const vector<uint8_t>& Data() const { return mData; }
        const size_t width() const { return mWidth; }
        const size_t height() const { return mHeight; }
        const GLint format() const { return mFormat; }
    private:
        Image() { }

        vector<uint8_t> mData;
        size_t mWidth, mHeight;
        GLint mFormat;
};

class ImageFileFormat {
public:
    virtual bool readImage(const vector<char>& data, Image& out) = 0;
};

class PNGFileFormat : ImageFileFormat {
    TAG_DEF("PNGFileFormat")
public:
    static bool checkHeader(const vector<char>& data) {
        if(data.size() < 8)
            return false;
        return !png_sig_cmp((uint8_t *)&data[0], 0, 8);
    }

    /* Reference: https://stackoverflow.com/questions/11296644/
     *            loading-png-textures-to-opengl-with-libpng-only
     */
    bool readImage(const vector<char>& data, Image& out) {
        if(!checkHeader(data))
            return false;
        
        png_structp png_ptr = png_create_read_struct(
                PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if(!png_ptr)
            return false;

        png_infop info_ptr = png_create_info_struct(png_ptr);
        if(!info_ptr) {
            png_destroy_read_struct(&png_ptr, NULL, NULL);
            return false;
        }

        png_infop end_info = png_create_info_struct(png_ptr);
        if(!end_info) {
            png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
            return false;
        }

        if(setjmp(png_jmpbuf(png_ptr))) {
            png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
            return false;
        }

        // FIXME: una copia intera? :/
        mReadData = data;
        mReadOffset = 0;
        png_set_read_fn(png_ptr, this, PNGFileFormat::readData);
        png_read_info(png_ptr, info_ptr);
        
        int bit_depth, color_type;
        png_uint_32 temp_width, temp_height;

        png_get_IHDR(png_ptr, info_ptr, &temp_width, &temp_height, &bit_depth,
                     &color_type, NULL, NULL, NULL);

        out.mWidth = temp_width;
        out.mHeight = temp_height;

        if(bit_depth != 8) {
            std::cerr << "Unsupported bit depth " << bit_depth << std::endl;
            png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
            return false;
        }

        switch(color_type) {
            case PNG_COLOR_TYPE_RGB:
                out.mFormat = GL_RGB;
                break;
            case PNG_COLOR_TYPE_RGB_ALPHA:
                out.mFormat = GL_RGBA;
                break;
            default:
                std::cerr << "Unsupported color type " << color_type << std::endl;
                png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
                return false;
        }

        png_read_update_info(png_ptr, info_ptr);

        int rowbytes = png_get_rowbytes(png_ptr, info_ptr);
        rowbytes += 3 - ((rowbytes - 1) % 4); // <- ????

        out.mData.resize(rowbytes * temp_height * sizeof(png_byte) + 15);

        png_bytep* row_pointers = new png_bytep[temp_height];

        for(size_t i = 0; i < temp_height; i++)
            row_pointers[temp_height - i - 1] = &out.mData[i * rowbytes];

        png_read_image(png_ptr, row_pointers);

        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        delete[] row_pointers;
        return true;
    }
private:
    vector<char> mReadData;
    size_t mReadOffset;

    /* Reference: https://github.com/EpicGames/UnrealEngine/blob/release/Engine/
     *            Source/Runtime/ImageWrapper/Private/PngImageWrapper.cpp
     */
    static void readData(png_structp png_ptr,  png_bytep data, 
                         png_size_t length) {
        PNGFileFormat *ctx = (PNGFileFormat *) png_get_io_ptr(png_ptr);

        assert(ctx->mReadOffset + length < ctx->mReadData.size());
        memcpy(data, &ctx->mReadData[ctx->mReadOffset], length);

        ctx->mReadOffset += length;
    }
};

class ImageFactory {
    TAG_DEF("ImageFactory")
public:
    static shared_ptr<Image> load(const string& name) {
        ifstream file(name.c_str(), ios::binary | ios::ate);
        std::streamsize size = file.tellg();
        file.seekg(0, ios::beg);

        if(size < 4) {
            ERRP("Cannot open %s", name.c_str());
            return nullptr;
        }

        vector<char> buffer(size);
        if(!file.read(buffer.data(), size))  {
            ERRP("Cannot open %s", name.c_str());
            return nullptr;
        }

        if(PNGFileFormat::checkHeader(buffer)) {
            PNGFileFormat pngReader;
            shared_ptr<Image> img(new Image);

            if(!pngReader.readImage(buffer, *img)) {
                ERRP("Cannot read %s", name.c_str());
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

#endif /* ifndef GLENGINE_PNG_H */

#include <GLEngine/FileReader/PNGFileReader.h>

namespace GLEngine
{
    
bool PNGFileFormat::readImage(const std::vector<char>& data, Image& out) {
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
        /*case PNG_COLOR_TYPE_PALETTE:
            out.mFormat = GL_RGB;
            break; */
        case PNG_COLOR_TYPE_RGB_ALPHA:
            out.mFormat = GL_RGBA;
            break;
        default:
            std::cerr << "Unsupported color " << color_type << std::endl;
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

void PNGFileFormat::readData(png_structp ptr, png_bytep data, png_size_t len) {
    PNGFileFormat *ctx = (PNGFileFormat *) png_get_io_ptr(ptr);

    assert(ctx->mReadOffset + len < ctx->mReadData.size());
    memcpy(data, &ctx->mReadData[ctx->mReadOffset], len);

    ctx->mReadOffset += len;
}

} /* GLEngine */ 

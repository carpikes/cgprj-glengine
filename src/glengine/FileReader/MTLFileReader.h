#ifndef GLENGINE_FILEREADER_MTL
#define GLENGINE_FILEREADER_MTL
#pragma once

#include <Common.h>
#include <Object.h>

namespace GLEngine
{

using std::function;
using std::pair;
using std::string;
using std::vector;

class MTLFileReader {
    TAG_DEF("MTLFileReader")
public:
    bool load(const string& name);

private:
    string mFileName;
    Material *mCurMaterial;

    typedef struct { 
        string name;
        uint8_t type;
        std::function<bool(FILE*, uint8_t, MTLFileReader*)> func;
    } funcmap_t;

    #define HANDLER(x) \
        static bool handle##x(FILE *fp, uint8_t type, MTLFileReader *mtl)

    HANDLER(ReadVector);
    HANDLER(ReadFloat);
    HANDLER(ReadTexture);
    HANDLER(NotImplemented);
    HANDLER(BeginMaterial);
    HANDLER(ReadInteger);

    #undef HANDLER
    static const funcmap_t functions[];
};
    
} /* GLEngine */ 

#endif /* ifndef GLENGINE_FILEREADER_MTL */

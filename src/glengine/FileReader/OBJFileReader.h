#ifndef GLENGINE_FILEREADER_OBJ
#define GLENGINE_FILEREADER_OBJ

#include <Common.h>
#include <Object.h>

namespace GLEngine
{

using std::function;
using std::pair;
using std::string;
using std::vector;

class OBJFileReader {
    TAG_DEF("OBJFileReader")
public:
    bool load(const string& name, Object &out);

private:
    string mFileName;

    vector<glm::vec3> mVertices, mNormals;
    vector<glm::vec2> mUv;
    vector<int32_t> mVertexIdx, mUvIdx, mNormalIdx;

    bool process(Object *out);

    typedef pair<string, std::function<bool(FILE*, OBJFileReader*)>> funcmap_t;

    static bool handleReadVertex(FILE *fp, OBJFileReader *obj);
    static bool handleReadUV(FILE *fp, OBJFileReader *obj);
    static bool handleReadNormal(FILE *fp, OBJFileReader *obj);
    static bool handleReadFace(FILE *fp, OBJFileReader *obj);
    static bool handleSetMaterialLib(FILE *fp, OBJFileReader *obj);
    static bool handleUseMaterial(FILE *fp, OBJFileReader *obj);
    static bool handleSetShading(FILE *fp, OBJFileReader *obj);
    static const funcmap_t functions[7];
};
    
} /* GLEngine */ 

#endif /* ifndef GLENGINE_FILEREADER_OBJ */

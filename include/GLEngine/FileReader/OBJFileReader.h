#ifndef GLENGINE_FILEREADER_OBJ
#define GLENGINE_FILEREADER_OBJ
#pragma once

#include <GLEngine/Common.h>
#include <GLEngine/Mesh.h>

namespace GLEngine
{

using std::function;
using std::pair;
using std::string;
using std::vector;

class OBJFileReader {
    TAG_DEF("OBJFileReader")
public:
    OBJFileReader(ResourceManager *mgr) : 
        mResManager(mgr), mFlushObject(false), mFacesType(0) {}

    bool load(const string& name, Mesh &out);

private:
    ResourceManager *mResManager;
    string mFileName, mMaterialName, mNewMaterialName, mCurSubGroup;
    bool mFlushObject;

    // 0: Uninited, &1 = hasNormals, &2 = hasUv, &4 = Initialized
    uint32_t mFacesType;

    vector<glm::vec3> mVertices, mNormals;
    vector<glm::vec2> mUv;
    vector<int32_t> mVertexIdx, mUvIdx, mNormalIdx;

    bool process(MeshPart &out);

    typedef pair<string, std::function<bool(FILE*, OBJFileReader*)>> funcmap_t;

    static bool handleReadVertex(FILE *fp, OBJFileReader *obj);
    static bool handleReadUV(FILE *fp, OBJFileReader *obj);
    static bool handleReadNormal(FILE *fp, OBJFileReader *obj);
    static bool handleReadFace(FILE *fp, OBJFileReader *obj);
    static bool handleSetMaterialLib(FILE *fp, OBJFileReader *obj);
    static bool handleUseMaterial(FILE *fp, OBJFileReader *obj);
    static bool handleSetShading(FILE *fp, OBJFileReader *obj);
    static bool handleChangeGroup(FILE *fp, OBJFileReader *obj);
    static bool handleSkipLine(FILE *fp, OBJFileReader *obj);
    static bool readTriplet(const char *s, int32_t out[3]);
    static const funcmap_t functions[];

    inline void cleanIndices() { 
        mVertexIdx.clear(); 
        mUvIdx.clear(); 
        mNormalIdx.clear();
    }
};
    
} /* GLEngine */ 

#endif /* ifndef GLENGINE_FILEREADER_OBJ */

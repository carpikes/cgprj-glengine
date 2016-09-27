#ifndef GLENGINE_FILEREADER_OBJ
#define GLENGINE_FILEREADER_OBJ
#pragma once

#include <GLEngine/Common.h>
#include <GLEngine/Mesh.h>
#include <GLEngine/Engine.h>

namespace GLEngine
{

using std::function;
using std::pair;
using std::string;
using std::vector;

class OBJFileReader {
    TAG_DEF("OBJFileReader")
public:
    OBJFileReader(Engine *engine, const string& path) : 
        mEngine(engine), mFlushObject(false), mFacesType(0), mPath(path) {}

    bool load(const string& name, Mesh &out);

private:
    Engine *mEngine;
    string mFileName, mMaterialName, mNewMaterialName, mCurSubGroup;
    string mCurMatlibName;
    bool mFlushObject;

    // 0: Uninited, &1 = hasNormals, &2 = hasUv, &4 = Initialized
    uint32_t mFacesType;

    const string& mPath;

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

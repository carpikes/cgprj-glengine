#include "OBJFileReader.h"
#include "MTLFileReader.h"
#include <ResourceManager.h>

namespace GLEngine
{
    
const OBJFileReader::funcmap_t OBJFileReader::functions[] = {
    std::make_pair("v",         OBJFileReader::handleReadVertex),
    std::make_pair("vt",        OBJFileReader::handleReadUV),
    std::make_pair("vn",        OBJFileReader::handleReadNormal),
    std::make_pair("f",         OBJFileReader::handleReadFace),
    std::make_pair("mtllib",    OBJFileReader::handleSetMaterialLib),
    std::make_pair("usemtl",    OBJFileReader::handleUseMaterial),
    std::make_pair("s",         OBJFileReader::handleSetShading),
    std::make_pair("g",         OBJFileReader::handleChangeGroup),
    std::make_pair("o",         OBJFileReader::handleSkipLine),
};

bool OBJFileReader::load(const string& name, ObjectGroup &out) {
    mFileName = name;

    FILE *fp = std::fopen(name.c_str(), "r");
    if(!fp) {
        ERRP("Cannot load %s", name.c_str());
        return false;
    }

    mVertices.clear(); mNormals.clear(); mUv.clear();
    cleanIndices();
    
    out.objects.clear();

    Object obj;

    mFlushObject = false;
    mCurSubGroup = "";
    while(1) {
        char word[128] = {0};
        if(fscanf(fp, "%120s", word) != 1)
            break;

        size_t len = strlen(word);
        if(len >= 120) {
            ERRP("Cannot load %s, lines are too long", name.c_str());
            fclose(fp);
            return false;
        }

        if(len == 0)
            continue;

        // #...? Skip the whole line
        if(word[0] == '#') {
            fscanf(fp, "%*[^\n]\n");
            continue;
        }

        bool invalid = true;
        for(const auto& i : functions) {
            if(!std::strcmp((i.first).c_str(), word)) {
                if(!i.second(fp, this)) {
                    fclose(fp);
                    return false;
                }
                invalid = false;
                break;
            }
        }

        if(invalid) {
            ERRP("Cannot understand %s in %s", word, name.c_str());
            fscanf(fp, "%*[^\n]\n"); // Skip whole line
        }

        // per il primo usemtl
        if(mFlushObject && mVertexIdx.size() == 0) {
            ERR("Skipping object");
            mMaterialName = mNewMaterialName;
            mFlushObject = false;
        }

        if(mFlushObject) {
            if(!process(obj)) {
                ERRP("Cannot process subgroup '%s' in %s", mCurSubGroup.c_str(),
                                                           name.c_str());
                fclose(fp);
                return false;
            }
            cleanIndices();
            mFlushObject = false;
            mMaterialName = mNewMaterialName;
            out.objects.push_back(obj);
            obj = Object();
        }
    }

    if(!process(obj)) {
        ERRP("Cannot process subgroup '%s' in %s", mCurSubGroup.c_str(),
                                                    name.c_str());
        fclose(fp);
        return false;
    }

    fclose(fp);
    out.objects.push_back(obj);
    return true;
} 

bool OBJFileReader::handleReadVertex(FILE *fp, OBJFileReader *obj) {
    glm::vec3 vertex;
    int r = fscanf(fp, "%f %f %f\n", 
                    &vertex.x, &vertex.y, &vertex.z);
    if(r != 3) {
        ERRP("Cannot read vertex in %s", obj->mFileName.c_str());
        return false;
    }

    obj->mVertices.push_back(vertex);
    return true;
}

bool OBJFileReader::handleReadUV(FILE *fp, OBJFileReader *obj) { 
    glm::vec2 vertex;
    int r = fscanf(fp, "%f %f\n", 
                    &vertex.x, &vertex.y);
    if(r != 2) {
        ERRP("Cannot read vertex texture in %s", obj->mFileName.c_str());
        return false;
    }

    obj->mUv.push_back(vertex);
    return true;
}

bool OBJFileReader::handleReadNormal(FILE *fp, OBJFileReader *obj) { 
    glm::vec3 vertex;
    int r = fscanf(fp, "%f %f %f\n", 
                    &vertex.x, &vertex.y, &vertex.z);
    if(r != 3) {
        ERRP("Cannot read vertex normal in %s", obj->mFileName.c_str());
        return false;
    }

    obj->mNormals.push_back(vertex);
    return true;
}

bool OBJFileReader::readTriplet(const char *s, int32_t out[3]) {
    size_t len = strlen(s);
    int32_t n = 0, pos=0;

    bool hasComponent = false;
    out[0] = out[1] = out[2] = 0;
    for(size_t i=0;i<len;i++) {
        char c = s[i];
        if(c >= '0' && c <= '9') {
            n = n * 10 + c - '0';
            hasComponent = true;
        } else if(c == '/') {
            out[pos++] = hasComponent ? n : 0;
            hasComponent = false;
            n = 0;
            if(pos == 3)
                return false;
        } else {
            ERR("Invalid triplet");
            return false;
        }
    }
    if(hasComponent)
        out[pos] = n;

    return true;
}

bool OBJFileReader::handleReadFace(FILE *fp, OBJFileReader *obj) {
    int32_t vIdx[3], uvIdx[3], nIdx[3];

    for(int j=0;j<3;j++) {
        char str[64] = {0};
        int r = fscanf(fp, "%s", str);
        if(r != 1)
            return false;

        int32_t arr[3];
        if(!readTriplet(str, arr))
            return false;
        vIdx[j] = arr[0];
        uvIdx[j] = arr[1];
        nIdx[j] = arr[2];
    }

    // first time
    if(obj->mFacesType == 0) {
        if(vIdx[0] < 0) {
            ERR("No vertex");
            return false;
        }
        obj->mFacesType = 4 | (((uvIdx[0] > 0) & 1) << 1) | ((nIdx[0] > 0) & 1);
        LOGP("HasUvMapping: %d; HasNormalMapping: %d, FacesType: %d", 
            ((uvIdx[0] > 0) & 1),
            (nIdx[0] > 0), obj->mFacesType);
    }

    for(int j=0;j<3;j++) {
        if( (bool)(obj->mFacesType & 2) == (uvIdx[j] < 1) ) {
            ERR("Inconsistent UV mapping");
            return false;
        }
        if( (bool)(obj->mFacesType & 1) == (nIdx[j] < 1) ) {
            ERR("Inconsistent normal mapping");
            return false;
        }
    }

    for(auto i : vIdx)
        obj->mVertexIdx.push_back(i);

    if((obj->mFacesType & 2))
        for(auto i : uvIdx)
            obj->mUvIdx.push_back(i);

    if((obj->mFacesType & 1))
        for(auto i : nIdx)
            obj->mNormalIdx.push_back(i);
    return true;
}

bool OBJFileReader::handleSetMaterialLib(FILE *fp, OBJFileReader *obj) {
    char name[32] = {0};
    if(fscanf(fp, "%32s", name) != 1)
        return false;

    MTLFileReader mtl;
    mtl.load(name);
    return true;
}

bool OBJFileReader::handleUseMaterial(FILE *fp, OBJFileReader *obj) {
    char name[32] = {0};
    if(fscanf(fp, "%32s", name) != 1)
        return false;

    LOGP("Setting material to %s", name);
    obj->mNewMaterialName = name;
    obj->mFlushObject = true;
    return true;
}

bool OBJFileReader::handleSetShading(FILE *fp, OBJFileReader *obj) {
    fscanf(fp, "%*s");
    return true;
}

bool OBJFileReader::handleSkipLine(FILE *fp, OBJFileReader *obj) {
    fscanf(fp, "%*[^\n]\n");
    return true;
}

bool OBJFileReader::handleChangeGroup(FILE *fp, OBJFileReader *obj) {
    char name[32] = {0};
    if(fscanf(fp, "%32s", name) != 1)
        return false;
    LOGP("Changing group to %s", name);

    obj->mCurSubGroup = name; //FIXME: Questo fa apparire errori con nome errato
    return true;
}

bool OBJFileReader::process(Object &out) {
    //assert(mVertexIdx.size() == mUvIdx.size());
    //assert(mVertexIdx.size() == mNormalIdx.size());

    LOGP("Vertices: %u, Vertex IDX: %u", mVertices.size(), mVertexIdx.size());
    for(size_t i = 0; i < mVertexIdx.size(); i++) {
        int32_t normal = 0, uv = 0;
        int32_t vertex = mVertexIdx[i];
        if(mFacesType & 1)
            normal = mNormalIdx[i];
        if(mFacesType & 2)
            uv = mUvIdx[i];
        if(vertex < 1 || (size_t)vertex > mVertices.size() || 
                         (size_t)normal > mNormals.size()  ||
                         (size_t)uv > mUv.size()) {
            ERRP("Invalid index (%d,%d,%d)", vertex, normal, uv);
            return false;
        }

        Vertex vx;
        vx.vertex = mVertices[vertex-1];
        if(normal > 0) vx.normal = mNormals[normal-1];
        if(uv > 0) vx.uv = mUv[uv-1];

        out.vertices().push_back(vx);

        out.faces().push_back(i);
    }
    LOGP("Writing material name %s", this->mMaterialName.c_str());
    out.setMaterialType(this->mMaterialName);
    return true;
}

} /* GLEngine */ 

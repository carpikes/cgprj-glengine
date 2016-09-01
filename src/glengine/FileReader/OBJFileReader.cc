#include "OBJFileReader.h"

namespace GLEngine
{
    
const OBJFileReader::funcmap_t OBJFileReader::functions[7] = {
    std::make_pair("v",         OBJFileReader::handleReadVertex),
    std::make_pair("vt",        OBJFileReader::handleReadUV),
    std::make_pair("vn",        OBJFileReader::handleReadNormal),
    std::make_pair("f",         OBJFileReader::handleReadFace),
    std::make_pair("mtllib",    OBJFileReader::handleSetMaterialLib),
    std::make_pair("usemtl",    OBJFileReader::handleUseMaterial),
    std::make_pair("s",         OBJFileReader::handleSetShading)
};

bool OBJFileReader::load(const string& name, Object &out) {
    mFileName = name;

    FILE *fp = std::fopen(name.c_str(), "r");
    if(!fp) {
        ERRP("Cannot load %s", name.c_str());
        return false;
    }

    mVertices.clear(); mNormals.clear(); mUv.clear();
    mVertexIdx.clear(); mUvIdx.clear(); mNormalIdx.clear();

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

        if(!process(&out)) {
            fclose(fp);
            return false;
        }
    }

    fclose(fp);
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

bool OBJFileReader::handleReadFace(FILE *fp, OBJFileReader *obj) {
    int32_t vIdx[3], uvIdx[3], nIdx[3];
    int r = fscanf(fp, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
                        &vIdx[0], &vIdx[1], &vIdx[2],
                        &uvIdx[0], &uvIdx[1], &uvIdx[2],
                        &nIdx[0], &nIdx[1], &nIdx[2]);

    if(r != 9) {
        ERRP("Cannot read face in %s", obj->mFileName.c_str());
        return false;
    }

    for(auto i : vIdx)
        obj->mVertexIdx.push_back(i);

    for(auto i : uvIdx)
        obj->mUvIdx.push_back(i);

    for(auto i : nIdx)
        obj->mNormalIdx.push_back(i);
    return true;
}

bool OBJFileReader::handleSetMaterialLib(FILE *fp, OBJFileReader *obj) {
    return true;
}

bool OBJFileReader::handleUseMaterial(FILE *fp, OBJFileReader *obj) {
    return true;
}

bool OBJFileReader::handleSetShading(FILE *fp, OBJFileReader *obj) {
    return true;
}

bool OBJFileReader::process(Object *out) {
    assert(mVertexIdx.size() == mUvIdx.size());
    assert(mVertexIdx.size() == mNormalIdx.size());

    for(size_t i = 0; i < mVertexIdx.size(); i++) {
        int32_t vertex = mVertexIdx[i], normal = mNormalIdx[i];
        int32_t uv = mUvIdx[i];
        if(vertex < 1 || (size_t)vertex > mVertices.size() || 
            normal < 1 || (size_t)normal > mNormals.size()  ||
            uv < 1     || (size_t)uv > mUv.size()) {
            ERRP("Invalid index (%d,%d,%d)", vertex, normal, uv);
            return false;
        }

        out->vertices().push_back(mVertices[vertex-1]);
        out->normals().push_back(mNormals[normal-1]);
        out->uvs().push_back(mUv[uv-1]);
    }
    return true;
}

} /* GLEngine */ 

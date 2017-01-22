#include <GLEngine/FileReader/OBJFileReader.h>
#include <GLEngine/FileReader/MTLFileReader.h>
#include <GLEngine/MaterialManager.h>

namespace GLEngine
{

#define HANDLER(x) \
bool OBJFileReader::handle##x(FILE *fp, OBJFileReader *obj)
    
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

bool OBJFileReader::load(const string& name, Mesh &out) {
    mFileName = mPath + "/" + name;
    FILE *fp = std::fopen(mFileName.c_str(), "r");
    if(!fp) {
        ERRP("Cannot load %s", name.c_str());
        return false;
    }
    DEBP("Loading %s", name.c_str());

    mVertices.clear(); mNormals.clear(); mUv.clear();
    cleanIndices();
    
    out.getParts().clear();

    MeshPart obj;

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
            DEB("Skipping object");
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
            DEBP("%lu vertices have mat name '%s'", obj.vertices().size(),
                    mMaterialName.c_str());
            out.getParts().push_back(obj);
            obj = MeshPart();
            mMaterialName = mNewMaterialName;
        }
    }

    if(!process(obj)) {
        ERRP("Cannot process subgroup '%s' in %s", mCurSubGroup.c_str(),
                                                    name.c_str());
        fclose(fp);
        return false;
    }

    fclose(fp);
    out.getParts().push_back(obj);
    out.finalize();

    return true;
} 

HANDLER(ReadVertex) {
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

HANDLER(ReadUV) { 
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

HANDLER(ReadNormal) { 
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

HANDLER(ReadFace) {
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
        DEBP("HasUvMapping: %d; HasNormalMapping: %d, FacesType: %d", 
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

HANDLER(SetMaterialLib) {
    char name[64] = {0};
    if(fscanf(fp, "%32s", name) != 1)
        return false;

    obj->mCurMatlibName = name;

    std::string objPath = Utils::getPath(obj->mFileName);
    MTLFileReader mtl(obj->mEngine, objPath);

    Utils::cleanFileName(obj->mCurMatlibName);
    if(!mtl.load(obj->mCurMatlibName))
        return false;
    return true;
}

HANDLER(UseMaterial) {
    char name[64] = {0};
    if(fscanf(fp, "%32s", name) != 1)
        return false;

    if(obj->mCurMatlibName.length() == 0) {
        ERR("Calling 'usemtl' without a 'mtllib'!");
        return false;
    }

    DEBP("Setting material to %s", name);
    obj->mNewMaterialName = MaterialManager::buildMaterialName(
        obj->mCurMatlibName, name);
    obj->mFlushObject = true;
    return true;
}

HANDLER(SetShading) {
    (void) obj;
    fscanf(fp, "%*s");
    return true;
}

HANDLER(SkipLine) {
    (void) obj;
    fscanf(fp, "%*[^\n]\n");
    return true;
}

HANDLER(ChangeGroup) {
    char name[64] = {0};
    if(fscanf(fp, "%32s", name) != 1)
        return false;
    DEBP("Changing group to %s", name);

    obj->mCurSubGroup = name; //FIXME: Questo fa apparire errori con nome errato
    return true;
}

bool OBJFileReader::process(MeshPart &out) {
    //assert(mVertexIdx.size() == mUvIdx.size());
    //assert(mVertexIdx.size() == mNormalIdx.size());
    if(mVertexIdx.size() == 0)
        return true;

    DEBP("Vertices: %u, Vertex IDX: %u", mVertices.size(), mVertexIdx.size());
    for(size_t i = 0; i < mVertexIdx.size(); i+=3) {
        glm::vec3 vertices[3], normals[3];
        glm::vec2 uv[3];

        for(int j=0;j<3;j++) {
            int32_t vpos = mVertexIdx[i+j]-1;
            if(vpos < 0 || vpos >= (int32_t)mVertices.size()) {
                LOGP("Vertex %d has invalid location", i);
                return false;
            }
            vertices[j] = mVertices[vpos];
        }

        bool recalcNormals = false;

        if(!(mFacesType & 1)) // no normals
            recalcNormals = true;

        for(int j=0;j<3 && !recalcNormals;j++) {
            int32_t npos = mNormalIdx[i+j]-1;
            if(npos < 0 || npos >= (int32_t)mNormals.size())
                recalcNormals = true;
            else 
                normals[j] = glm::normalize(mNormals[npos]);
        }
        
        // normals are invalid. recalculate them from scratch
        glm::vec3 d1 = vertices[1] - vertices[0];
        glm::vec3 d2 = vertices[2] - vertices[0];
        if(recalcNormals) {
            glm::vec3 normal = glm::normalize(glm::cross(d1, d2));
            for(int j=0;j<3;j++)
                normals[j] = normal;
        }

        // has UV
        glm::vec3 tangent;
        if(mFacesType & 2) {
            for(int j=0;j<3;j++)
                uv[j] = mUv[mUvIdx[i+j]-1];

            glm::vec2 du1 = uv[1]-uv[0];
            glm::vec2 du2 = uv[2]-uv[0];

            float r = 1.0f / (du1.x * du2.y - du1.y * du2.x);
            tangent = glm::normalize((d1 * du2.y - d2 * du1.y)*r);
        } else {
            LOG("No texture!?");
        }

        for(size_t j = 0; j < 3; j++) {
            int32_t uv = 0;
            if(mFacesType & 2)
                uv = mUvIdx[i+j];

            Vertex vx;
            vx.vertex = vertices[j];
            vx.normal = normals[j];
            vx.tangent = tangent;

            if(uv > 0) 
                vx.uv = mUv[uv-1];
            else
                ERR("UV WAT?");

            out.vertices().push_back(vx);

            /*
            LOGP("Vertex %d: (%f,%f,%f) N(%f,%f,%f) T(%f,%f,%f) UV(%f,%f)", i+j,
                    vx.vertex.x, vx.vertex.y, vx.vertex.z,
                    vx.normal.x, vx.normal.y, vx.normal.z,
                    vx.tangent.x, vx.tangent.y, vx.tangent.z,
                    vx.uv.x, vx.uv.y);
            */

        }

        out.faces().push_back(i);
    }

    DEBP("Writing material name %s", this->mMaterialName.c_str());
    out.setMaterial(this->mMaterialName);
    return true;
}

#undef HANDLER

} /* GLEngine */ 

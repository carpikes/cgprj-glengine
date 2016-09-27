#include <GLEngine/FileReader/MTLFileReader.h>
#include <GLEngine/MaterialManager.h>

namespace GLEngine
{

#define HANDLER(x) \
bool MTLFileReader::handle##x(FILE *fp, uint8_t type, MTLFileReader *mtl)
    
const MTLFileReader::funcmap_t MTLFileReader::functions[] = {
    { "Ka",       0, MTLFileReader::handleReadVector    },
    { "Kd",       1, MTLFileReader::handleReadVector    },
    { "Ks",       2, MTLFileReader::handleReadVector    },
    { "Tf",       3, MTLFileReader::handleReadVector    },

    { "Ni",       0, MTLFileReader::handleReadFloat     },
    { "Ns",       1, MTLFileReader::handleReadFloat     },
    { "d",        2, MTLFileReader::handleReadFloat     },
    { "Tr",       3, MTLFileReader::handleReadFloat     },

    { "map_Ka",   0, MTLFileReader::handleReadTexture   },
    { "map_Kd",   1, MTLFileReader::handleReadTexture   },
    { "map_Ks",   2, MTLFileReader::handleReadTexture   },
    { "map_Ns",   3, MTLFileReader::handleReadTexture   },
    { "map_bump", 4, MTLFileReader::handleReadTexture   },
    { "bump",     4, MTLFileReader::handleReadTexture   },
    { "decal",    5, MTLFileReader::handleReadTexture   },
    { "disp",     6, MTLFileReader::handleReadTexture   },
    { "map_d",    7, MTLFileReader::handleReadTexture   }, // dissolve map(?)
    { "illum",    0, MTLFileReader::handleReadInteger   },

    { "newmtl",   0, MTLFileReader::handleBeginMaterial }
};

bool MTLFileReader::load(const string& name) {
    mFileName = name;

    FILE *fp = std::fopen(name.c_str(), "r");
    if(!fp) {
        ERRP("Cannot load %s", name.c_str());
        return false;
    }

    mCurMaterial = NULL;
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
            if(!std::strcmp((i.name).c_str(), word)) {
                if(!i.func(fp, i.type, this)) {
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
    }

    mCurMaterial = NULL;

    fclose(fp);
    return true;
} 

HANDLER(NotImplemented) {
    LOG("Command not implemented");
    if(mtl->mCurMaterial == NULL) return false;
    fscanf(fp, "%*[^\n]\n");

    return true;
}

HANDLER(ReadVector) {
    if(mtl->mCurMaterial == NULL) return false;
    glm::vec3 v;

    int r = fscanf(fp, "%f %f %f\n", 
                    &v.x, &v.y, &v.z);
    if(r != 3) {
        ERRP("Cannot read value in %s", mtl->mFileName.c_str());
        return false;
    }

    switch(type) {
        case 0: mtl->mCurMaterial->mAmbientColor = v; break;
        case 1: mtl->mCurMaterial->mDiffuseColor = v; break;
        case 2: mtl->mCurMaterial->mSpecularColor = v; break;
        case 3: mtl->mCurMaterial->mTransmissionFilter = v; break;
        default:
            ERR("Undefined behavior. Skipping\n");
            break;
    }

    return true;
}

HANDLER(ReadFloat) {
    if(mtl->mCurMaterial == NULL) return false;
    float v;

    int r = fscanf(fp, "%f\n", &v);
    if(r != 1) {
        ERRP("Cannot read value in %s", mtl->mFileName.c_str());
        return false;
    }

    switch(type) {
        case 0: mtl->mCurMaterial->mOpticalDensity = v; break;
        case 1: mtl->mCurMaterial->mSpecularExponent = v; break;
        case 2: 
        case 3: 
            mtl->mCurMaterial->mAlpha = glm::clamp(
                    type == 2 ? v : 1.0f - v, 0.0f, 1.0f); 
            break;
        default:
            ERR("Undefined behavior. Skipping\n");
            break;
    }

    return true;
}

HANDLER(ReadInteger) {
    if(mtl->mCurMaterial == NULL) return false;
    fscanf(fp, "%*[^\n]\n");
    return true;
}

HANDLER(ReadTexture) {
    if(mtl->mCurMaterial == NULL) return false;
    char textureName[128] = {0};
    int r = fscanf(fp, "%120s\n", textureName);
    if(r != 1) {
        ERRP("Cannot read texture file in %s", mtl->mFileName.c_str());
        return false;
    }

    bool res = mtl->mResManager->get<Image>(textureName);
    if(!res) {
        ERRP("Cannot load texture %s", textureName);
        return false;
    }

    Image *img = mtl->mResManager->get<Image>(textureName);
    Texture *tex = new Texture(img);

    switch(type) {
        case 0: mtl->mCurMaterial->mAmbientTexture = tex; break;
        case 1: mtl->mCurMaterial->mDiffuseTexture = tex; break;
        case 2: mtl->mCurMaterial->mSpecularTexture = tex; break;
        case 3: mtl->mCurMaterial->mHighlightTexture = tex; break;
        case 4: mtl->mCurMaterial->mBumpTexture = tex; break;
        case 6: mtl->mCurMaterial->mDisplacementTexture = tex; break;
        default: 
            ERRP("Unk type %d", type);
            free(tex);
    }

    return true;
}

HANDLER(BeginMaterial) {
    char name[32] = {0};
    if(fscanf(fp, "%32s", name) != 1)
        return false;

    // FIXME: i materiali li gestisce il materialmanager!
    if(!mtl->mResManager->get<Material>(name)) {
        ERR("dafaq");
        exit(-1);
    }

    LOGP("Parsing material %s", name);
    mtl->mCurMaterial = mtl->mResManager->get<Material>(name);
    return true;
}

/*
bool MTLFileReader::handleUseMaterial(FILE *fp, MTLFileReader *obj) {
    char name[32] = {0};
    if(fscanf(fp, "%32s", name) != 1)
        return false;

    LOGP("Setting material to %s", name);
    obj->mNewMaterialName = name;
    obj->mFlushObject = true;
    return true;
}

bool MTLFileReader::handleSetShading(FILE *fp, MTLFileReader *obj) {
    fscanf(fp, "%*s");
    return true;
}

bool MTLFileReader::handleChangeGroup(FILE *fp, MTLFileReader *obj) {
    char name[32] = {0};
    if(fscanf(fp, "%32s", name) != 1)
        return false;
    LOGP("Changing group to %s", name);

    obj->mCurSubGroup = name; //FIXME: Questo fa apparire errori con nome errato
    return true;
}

bool MTLFileReader::process(Object &out) {
    assert(mVertexIdx.size() == mUvIdx.size());
    assert(mVertexIdx.size() == mNormalIdx.size());

    LOGP("Vertices: %u, Vertex IDX: %u", mVertices.size(), mVertexIdx.size());
    for(size_t i = 0; i < mVertexIdx.size(); i++) {
        int32_t vertex = mVertexIdx[i], normal = mNormalIdx[i];
        int32_t uv = mUvIdx[i];
        if(vertex < 1 || (size_t)vertex > mVertices.size() || 
            normal < 1 || (size_t)normal > mNormals.size()  ||
            uv < 1     || (size_t)uv > mUv.size()) {
            ERRP("Invalid index (%d,%d,%d)", vertex, normal, uv);
            return false;
        }

        out.vertices().push_back(mVertices[vertex-1]);
        out.normals().push_back(mNormals[normal-1]);
        out.uvs().push_back(mUv[uv-1]);

        out.setMaterialType(this->mMaterialName);
    }
    return true;
}
*/

#undef HANDLER

} /* GLEngine */ 

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
    { "Ke",       3, MTLFileReader::handleNotImplemented},

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
    string fullName = mPath + "/" + name;

    FILE *fp = std::fopen(fullName.c_str(), "r");
    if(!fp) {
        ERRP("Cannot load %s", fullName.c_str());
        return false;
    }

    mCurMaterial = NULL;
    while(1) {
        char word[128] = {0};
        if(fscanf(fp, "%120s", word) != 1)
            break;

        size_t len = strlen(word);
        if(len >= 120) {
            ERRP("Cannot load %s, lines are too long", fullName.c_str());
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
            ERRP("Cannot understand %s in %s", word, fullName.c_str());
            fscanf(fp, "%*[^\n]\n"); // Skip whole line
        }
    }

    mCurMaterial = NULL;

    fclose(fp);
    return true;
} 

HANDLER(NotImplemented) { 
    (void) type;
    DEB("Command not implemented");
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
    (void)type;
    if(mtl->mCurMaterial == NULL) return false;
    fscanf(fp, "%*[^\n]\n");
    return true;
}

HANDLER(ReadTexture) {
    ResourceManager& resMgr = mtl->mEngine->getResourceManager();

    if(mtl->mCurMaterial == NULL) return false;
    char textureName[256] = {0};
    int r = fscanf(fp, "%250s\n", textureName);
    if(r != 1) {
        ERRP("Cannot read texture file in %s", mtl->mFileName.c_str());
        throw "Cannot continue";
        return false;
    }

    std::stringstream texPath, texPath2;
    texPath << mtl->mPath << "/" << textureName;
    // Pokemon models hack
    texPath2 << mtl->mPath << "/Textures/" << textureName;

    string tPath = texPath.str(), tPath2 = texPath2.str();
    Utils::cleanFileName(tPath);
    Utils::cleanFileName(tPath2);
    Image *img = resMgr.get<Image>(tPath);
    if(!img) {
    //    img = resMgr.get<Image>(tPath2);
        if(!img) {
            ERRP("Cannot load texture %s", tPath.c_str());
            return false;
        }
        tPath = tPath2;
    }

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
    (void) type;
    MaterialManager& mtlMgr = mtl->mEngine->getMaterialManager();
    char name[64] = {0};
    if(fscanf(fp, "%32s", name) != 1)
        return false;

    DEBP("Parsing material %s", name);
    mtl->mCurMaterial = std::make_shared<Material>();

    const string fullName = mtlMgr.buildMaterialName(mtl->mFileName, name);
    mtlMgr.add(fullName, mtl->mCurMaterial);
    return true;
}

#undef HANDLER

} /* GLEngine */ 

#include <GLEngine/Common.h>
#include <GLEngine/Utils.h>
#include <GLEngine/ShaderManager.h>

namespace GLEngine
{

GLuint ShaderManager::load(const std::vector<ShaderElement>& elements) {
    std::vector<GLuint> compiled;
    GLuint programId = 0;

    if(elements.size() != 2)
        return false;

    bool canLink = compileShaders(elements, compiled);
    if(canLink) {
        programId = linkShaders(compiled);
        if(programId != 0)
            for(GLuint u : compiled)
                glDetachShader(programId, u);
    }
    freeShaders(compiled);
    return programId; // ritornare una risorsa con shared_ptr?
}

void ShaderManager::freeShaders(std::vector<GLuint>& shaders) {
    for(GLuint u : shaders)
        glDeleteShader(u);

    shaders.clear();
}

GLuint ShaderManager::linkShaders(std::vector<GLuint>& shaders) {
    GLint result = GL_FALSE;
    int infoLogLen = 0;
    GLuint programId = 0;

    LOG("Linking"); 
    programId = glCreateProgram();
    for(GLuint u : shaders)
        glAttachShader(programId, u);
    glLinkProgram(programId);
    
    glGetShaderiv(programId, GL_LINK_STATUS, &result);
    glGetShaderiv(programId, GL_INFO_LOG_LENGTH, &infoLogLen);
    if(infoLogLen > 0) {
        std::vector<char> errMsg(infoLogLen+1);
        glGetShaderInfoLog(programId, infoLogLen, NULL, &errMsg[0]);
        ERRP("Linking error: %s", &errMsg[0]);
        assert(false);
    }

    return programId;
}

bool ShaderManager::compileShaders(const std::vector<ShaderElement>& el, 
                                   std::vector<GLuint>& out) {
    if(out.size() > 0)
        freeShaders(out);
    out.clear();

    if(el.size() == 0)
        return false;

    for(const ShaderElement& e : el) {
        std::string src;
        if(!Utils::readFile(mPath, e.filename, src)) {
            ERRP("Cannot read shader %s in %s", e.filename.c_str(), 
                                                mPath.c_str()); 
            return 0;
        }

        GLenum type = 0;
        switch(e.type) {
            case VERTEX_SHADER: type = GL_VERTEX_SHADER; break;
            case FRAGMENT_SHADER: type = GL_FRAGMENT_SHADER; break;
            default:
                ERR("Unknown shader type");
                return 0;
        }

        GLint result = GL_FALSE;
        int infoLogLen = 0;

        GLuint id = glCreateShader(type);
        out.push_back(id);

        LOGP("Compiling shader %s", e.filename.c_str());

        const char* ssrc = src.c_str();
        glShaderSource(id, 1, &ssrc, NULL);
        glCompileShader(id);

        glGetShaderiv(id, GL_COMPILE_STATUS, &result);
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLen);
        if(result == GL_FALSE || infoLogLen > 0) {
            std::vector<char> errMsg(infoLogLen+1);
            glGetShaderInfoLog(id, infoLogLen, NULL, &errMsg[0]);
            ERRP("Compiler error: %s", &errMsg[0]);
            return false;
        }
    } 
    return true;
}
    
} /* GLEngine */ 

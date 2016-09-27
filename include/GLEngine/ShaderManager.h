#ifndef GLENGINE_SHADERMANAGER_H
#define GLENGINE_SHADERMANAGER_H

#include <GLEngine/Common.h>

namespace GLEngine
{

enum ShaderType {
    VERTEX_SHADER,
    FRAGMENT_SHADER,
};

struct ShaderElement {
    ShaderType type;
    const std::string& filename;
};

class ShaderManager {
    TAG_DEF("ShaderManager")
public:
    GLuint load(const std::vector<ShaderElement>& elements);
private:
    void freeShaders(std::vector<GLuint>& shaders);
    GLuint linkShaders(std::vector<GLuint>& shaders);
    bool compileShaders(const std::vector<ShaderElement>& el, 
            std::vector<GLuint>& out);
};
    
} /* GLEngine */ 

#endif /* ifndef GLENGINE_SHADERMANAGER_H */

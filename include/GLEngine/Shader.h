#ifndef GLENGINE_SHADER_H
#define GLENGINE_SHADER_H 

#include "Common.h"
#include "ShaderManager.h"
#include "Texture.h"

namespace GLEngine
{

class Shader {
    TAG_DEF("Shader")

public:
    Shader(const std::string& vs, const std::string& fs) {
        ShaderManager sm;
        mProgramId = sm.load( {
            { ShaderType::VERTEX_SHADER, vs },
            { ShaderType::FRAGMENT_SHADER, fs }
        });
        if(mProgramId == InvalidVideoPtr)
            ERR("Cannot load shader");
    }

    void enable() {
        glUseProgram(mProgramId);
    }

    void set(const std::string& name, bool v) {
        VideoPtr ptr = glGetUniformLocation(mProgramId, name.c_str());
        glUniform1i(ptr, v);
    }

    void set(const std::string& name, int v) {
        VideoPtr ptr = glGetUniformLocation(mProgramId, name.c_str());
        glUniform1i(ptr, v);
    }

    void set(const std::string& name, float v) {
        VideoPtr ptr = glGetUniformLocation(mProgramId, name.c_str());
        glUniform1f(ptr, v);
    }

    void set(const std::string& name, const glm::vec3& v) {
        VideoPtr ptr = glGetUniformLocation(mProgramId, name.c_str());
        glUniform3f(ptr, v.x, v.y, v.z);
    }

    void set(const std::string& name, const glm::mat3& v) {
        VideoPtr ptr = glGetUniformLocation(mProgramId, name.c_str());
        glUniformMatrix3fv(ptr, 1, GL_FALSE, &v[0][0]);
    }

    void set(const std::string& name, const glm::mat4& v) {
        VideoPtr ptr = glGetUniformLocation(mProgramId, name.c_str());
        glUniformMatrix4fv(ptr, 1, GL_FALSE, &v[0][0]);
    }

    int getMaxNumberOfLights() const {
        return 8; // LOL
    }

    uint32_t setTexture(const std::string& name, 
                        const Texture* t, uint8_t pos) {
        if(t == nullptr || t->img == nullptr)
            return 0;

        VideoPtr ptr = glGetUniformLocation(mProgramId, name.c_str());
        glActiveTexture(GL_TEXTURE0 + pos);
        glUniform1i(ptr, pos);
        glBindTexture(GL_TEXTURE_2D, t->img->videoPtr());
        return (1 << pos);
    }

private:
    VideoPtr mProgramId;
};

typedef std::shared_ptr<Shader> ShaderPtr;
    
} /* GLEngine */ 

#endif /* ifndef GLENGINE_SHADER_H */

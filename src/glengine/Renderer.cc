#include <GLEngine/Renderer.h>

namespace GLEngine
{

/* Reference:   http://www.opengl-tutorial.org/beginners-tutorials/
 *              tutorial-1-opening-a-window/
 */
bool Renderer::init(size_t width, size_t height, const string& title, 
            uint32_t aaSamples) {
    if(!glfwInit()) {
        ERR("Cannot init GLFW");
        return false;
    }

    mWidth = width;
    mHeight = height;
    mAASamples = aaSamples;

    glfwWindowHint(GLFW_SAMPLES, aaSamples); 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    mWindow = glfwCreateWindow( mWidth, mHeight, title.c_str(), NULL, NULL);
    if(mWindow == NULL) {
        ERR("Cannot open a GLFW window.");
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(mWindow);

    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        ERR("Cannot initialize GLEW");
        glfwTerminate();
        return false;
    }

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    return true;
}

void Renderer::run() {
    glfwSetInputMode(mWindow, GLFW_STICKY_KEYS, GL_TRUE); 

    vector<GLuint> vbuffers;


    LOGP("VBuffers size: %u", vbuffers.size());

    GLuint programId = mShaderManager.load( {
        { ShaderType::VERTEX_SHADER, "main.vs" },
        { ShaderType::FRAGMENT_SHADER, "main.fs" }
    });

    if(programId == 0)
        return; 

    glm::mat4 cameraMat = glm::lookAt(
        glm::vec3(5,2,2), glm::vec3(0,2,0), glm::vec3(0,1,0));
    glm::mat4 projMat = glm::perspective(
            glm::radians(75.0f), 4.0f/3.0f, 0.01f, 1000.0f);

    float cnt = 0;

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    //
    glm::vec3 light(0,0,0);
    do {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(programId);
        GLuint matrixID = glGetUniformLocation(programId, "MVP");
        GLuint lightID = glGetUniformLocation(programId, "uLight");

        glm::mat4 model = glm::rotate(cnt, glm::vec3(0,1,0));
        glm::mat4 mvpMat = projMat * cameraMat * model;
        
        glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvpMat[0][0]);

        light.x = cos(cnt*2.0f);
        light.z = sin(cnt*2.0f);
        light = glm::normalize(light);
        glUniform3f(lightID, light.x, light.y, light.z);

        /*
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        for(size_t i=0;i<mObjects.size();i++) {
            glBindBuffer(GL_ARRAY_BUFFER, vbuffers[i]);
            Object *obj = mObjects[i];
            if(obj == NULL) { ERR("OBJ == null"); return; }
            Material *mtl = ResourceManager<Material>::get(obj->material());
            if(mtl == NULL) { ERR("MTL == null"); return; }
            if(mtl->mDiffuseTexture != NULL)
                glBindTexture(GL_TEXTURE_2D, mtl->mDiffuseTexture->img->glId());
            else
                glBindTexture(GL_TEXTURE_2D, 0);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(2*sizeof(glm::vec3)));
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(sizeof(glm::vec3)));
            glDrawArrays(GL_TRIANGLES, 0, mObjects[i]->vertices().size());
        }
        glDisableVertexAttribArray(2);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);
        */
        
        glfwSwapBuffers(mWindow);
        glfwPollEvents();
        cnt += 0.01f;
    } while( glfwGetKey(mWindow, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
            !glfwWindowShouldClose(mWindow));
}

bool Renderer::allocateVertexBuffers(size_t number, std::vector<VideoPtr>& out) {
    LOGP("Allocating %u vertex buffers", number);
    out.resize(number, InvalidVideoPtr);
    glGenBuffers(number, &out[0]);
    return true;
}

bool Renderer::allocateTextureBuffers(size_t number, std::vector<VideoPtr>& out) {
    LOGP("Allocating %u texture buffers", number);
    out.resize(number, InvalidVideoPtr);
    glGenTextures(number, &out[0]);
    return true;
}

void Renderer::writeVertices(VideoPtr buffer, const std::vector<Vertex>& vertices) {
    LOGP("Writing %u vertices to videocard", vertices.size());
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 
            (vertices.size()) * sizeof(Vertex), 
            &vertices[0], GL_STATIC_DRAW); // mmh..static..
}

void Renderer::writeTexture(const std::vector<VideoPtr>& buffer, 
        const std::unordered_set<Image *>& images) {
    LOGP("Moving %u textures to videocard", images.size());

    assert(buffer.size() == images.size());

    size_t bufpos = 0;
    for(Image* img : images) {
        img->setVideoPtr(buffer[bufpos]);
        glBindTexture(GL_TEXTURE_2D, buffer[bufpos]);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img->width(), img->height(),
                0, GL_RGB, GL_UNSIGNED_BYTE, &(img->Data()[0]));

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        bufpos++;
    }
}
    
} /* GLEngine */ 

#ifndef GLEngine_Common_h
#define GLEngine_Common_h

#include <algorithm>
#include <cstring>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <png.h>

#include "Log.h"

#pragma pack(1)
struct Vertex {
    glm::vec3 vertex, normal;
    glm::vec2 uv;
};
#pragma pack()

typedef GLuint VideoPtr;
static constexpr VideoPtr InvalidVideoPtr = 0;

#endif /* ifndef GLEngine_Common_h */

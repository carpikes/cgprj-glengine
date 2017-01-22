add_subdirectory("GLEngine")
file(GLOB_RECURSE EXECUTABLE_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cc *.h)

include_directories("${CMAKE_SOURCE_DIR}/include")
add_executable(demo main.cc)

#SET(CMAKE_C_COMPILER "/usr/bin/clang")
#SET(CMAKE_CXX_COMPILER "/usr/bin/clang++")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -std=c++11 -g -O0 -fsanitize=address")
target_link_libraries(demo GLEngine glfw GLEW GL png)

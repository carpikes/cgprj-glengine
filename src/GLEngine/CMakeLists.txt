include_directories(${CMAKE_CURRENT_SOURCE_DIR}/FileReader)
file(GLOB_RECURSE glengine_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cc *.h)

include_directories("${CMAKE_SOURCE_DIR}/include")
add_library(GLEngine STATIC ${glengine_SRCS})

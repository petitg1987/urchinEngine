cmake_minimum_required(VERSION 3.7)
project(urchin3dEngine)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_CXX_STANDARD 17)

add_definitions(-DGLEW_STATIC -DGL_GLEXT_PROTOTYPES -ffast-math)
include_directories(src ../common/src /usr/include/freetype2)

if (WIN32)
    set(OPENGL_LIB glew32 opengl32)
else()
    set(OPENGL_LIB GLEW)
endif()
file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
add_library(urchin3dEngine SHARED ${SOURCE_FILES})
target_link_libraries(urchin3dEngine freetype ${OPENGL_LIB} urchinCommon -static-libstdc++)
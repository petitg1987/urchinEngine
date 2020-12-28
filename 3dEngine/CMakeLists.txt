cmake_minimum_required(VERSION 3.7)
project(urchin3dEngine)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
if (WIN32)
    set(GLEW_LIB glew32)
    set(FREETYPE_LIB freetype)
else()
    set(LINK_OPT -rdynamic) #necessary for SignalHandler to work correctly
    set(GLEW_LIB GLEW.a)
    set(FREETYPE_LIB freetype.a png16.a brotlidec.a brotlicommon.a z.a)
endif()

add_definitions(-ffast-math -Wall -Wextra -Wpedantic -Wconversion -Werror -DGLEW_STATIC)
include_directories(src ../common/src /usr/include/freetype2)

file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.inl" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
add_library(urchin3dEngine STATIC ${SOURCE_FILES})
target_link_libraries(urchin3dEngine ${LINK_OPT} ${FREETYPE_LIB} ${GLEW_LIB} OpenGL::GL urchinCommon -static-libstdc++)

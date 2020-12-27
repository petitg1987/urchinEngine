cmake_minimum_required(VERSION 3.7)
project(urchin3dEngine)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
if (WIN32)
    set(GLEW_LIB glew32)
else()
    set(GLEW_LIB GLEW)
    set(LINK_OPT -rdynamic) #necessary for SignalHandler to work correctly
endif()

add_definitions(-fPIC -fno-semantic-interposition -ffast-math -Wall -Wextra -Wpedantic -Wconversion -Werror -DGLEW_STATIC)
include_directories(src ../common/src /usr/include/freetype2)

file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.inl" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
add_library(urchin3dEngine SHARED ${SOURCE_FILES})
target_link_libraries(urchin3dEngine ${LINK_OPT} freetype ${GLEW_LIB} OpenGL::GL urchinCommon -static-libstdc++)

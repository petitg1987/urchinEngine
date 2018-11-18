cmake_minimum_required(VERSION 3.10)
set(OpenGL_GL_PREFERENCE "GLVND")
project(urchinEngine)

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    add_definitions(-D_DEBUG)
endif()

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(common)
add_subdirectory(3dEngine)
add_subdirectory(physicsEngine)
add_subdirectory(soundEngine)
add_subdirectory(networkEngine)
add_subdirectory(AIEngine)
add_subdirectory(mapHandler)
if (NOT WIN32) #not handled on Windows OS
    add_subdirectory(mapEditor)
    add_subdirectory(unitTest)
endif()
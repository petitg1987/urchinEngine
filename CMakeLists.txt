cmake_minimum_required(VERSION 3.16)
project(urchinEngine)

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    add_definitions(-DURCHIN_DEBUG)
endif()

set(URCHIN_BUILD_TOOLS "YES" CACHE STRING "Build the urchin engine tools (map editor, test runner).")
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(common)
add_subdirectory(3dEngine)
add_subdirectory(physicsEngine)
add_subdirectory(soundEngine)
add_subdirectory(networkEngine)
add_subdirectory(AIEngine)
add_subdirectory(aggregation)
if ((URCHIN_BUILD_TOOLS MATCHES "YES") AND (NOT WIN32)) #not handled on Windows
    add_subdirectory(mapEditor)
    add_subdirectory(test)
endif()

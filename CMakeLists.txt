cmake_minimum_required(VERSION 3.10)
project(urchinEngine)

if (CMAKE_BUILD_TYPE MATCHES "Release")
    add_definitions(-DNDEBUG) #disable assert
endif()

set(URCHIN_BUILD_TOOLS "YES" CACHE STRING "Build the urchin engine tools (map editor, test runner).")
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(common)
add_subdirectory(3dEngine)
add_subdirectory(physicsEngine)
add_subdirectory(soundEngine)
add_subdirectory(networkEngine)
add_subdirectory(AIEngine)
add_subdirectory(mapHandler)
if ((URCHIN_BUILD_TOOLS MATCHES "YES") AND (NOT WIN32)) #not handled on Windows
#TODO    add_subdirectory(mapEditor)
#TODO    add_subdirectory(test)
endif()

cmake_minimum_required(VERSION 3.16)
project(urchinEngine)

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    add_definitions(-DURCHIN_DEBUG)
endif()

set(URCHIN_BUILD_TOOLS "YES" CACHE STRING "Build the urchin engine tools (map editor, test runner).")
set(CMAKE_CXX_STANDARD 20)

set(PRIVATE_URCHIN_HEADERS common/src/UrchinCommon.h 3dEngine/src/Urchin3dEngine.h physicsEngine/src/UrchinPhysicsEngine.h soundEngine/src/UrchinSoundEngine.h aiEngine/src/UrchinAIEngine.h aggregation/src/UrchinAggregation.h)
get_directory_property(hasParent PARENT_DIRECTORY)
if(hasParent)
    set(URCHIN_HEADERS ${PRIVATE_URCHIN_HEADERS} PARENT_SCOPE)
else()
    set(URCHIN_HEADERS ${PRIVATE_URCHIN_HEADERS})
endif()

add_subdirectory(common)
add_subdirectory(3dEngine)
add_subdirectory(physicsEngine)
add_subdirectory(soundEngine)
add_subdirectory(networkEngine)
add_subdirectory(aiEngine)
add_subdirectory(aggregation)
if ((URCHIN_BUILD_TOOLS MATCHES "YES") AND (NOT WIN32)) #not handled on Windows
    add_subdirectory(mapEditor)
    add_subdirectory(test)
endif()

cmake_minimum_required(VERSION 3.16)
project(urchinAggregation)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_CXX_STANDARD 20)

if (NOT WIN32)
    set(LINK_OPT -rdynamic) #necessary for SignalHandler to work correctly
endif()

add_definitions(-ffast-math -Wall -Wextra -Wpedantic -Wconversion -Werror)
include_directories(src ../common/src ../3dEngine/src ../physicsEngine/src ../soundEngine/src ../AIEngine/src)

file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
add_library(urchinAggregation STATIC ${SOURCE_FILES})
target_link_libraries(urchinAggregation ${LINK_OPT} urchinCommon urchin3dEngine urchinPhysicsEngine urchinSoundEngine urchinAIEngine)
target_precompile_headers(urchinAggregation PRIVATE
        ../common/src/UrchinCommon.h
        ../3dEngine/src/Urchin3dEngine.h
        ../physicsEngine/src/UrchinPhysicsEngine.h
        ../soundEngine/src/UrchinSoundEngine.h
        ../AIEngine/src/UrchinAIEngine.h)

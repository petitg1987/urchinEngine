cmake_minimum_required(VERSION 3.16)
project(urchinAggregation)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_CXX_STANDARD 23)
if ((CMAKE_BUILD_TYPE MATCHES "Release") OR (CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo"))
    SET(CMAKE_UNITY_BUILD TRUE)
endif()

if (NOT WIN32)
    set(LINK_OPT -Wl,--export-dynamic) #necessary for SignalHandler to work correctly
endif()

add_definitions(-fno-stack-protector -ffast-math -Wall -Wextra -Wpedantic -Wconversion -Werror)
include_directories(src ../common/src ../3dEngine/src ../physicsEngine/src ../soundEngine/src ../aiEngine/src)

file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
add_library(urchinAggregation STATIC ${SOURCE_FILES})
target_link_libraries(urchinAggregation ${LINK_OPT} urchinCommon urchin3dEngine urchinPhysicsEngine urchinSoundEngine urchinAIEngine)
target_precompile_headers(urchinAggregation PRIVATE
        ../common/src/UrchinCommon.h
        ../3dEngine/src/Urchin3dEngine.h
        ../physicsEngine/src/UrchinPhysicsEngine.h
        ../soundEngine/src/UrchinSoundEngine.h
        ../aiEngine/src/UrchinAIEngine.h)

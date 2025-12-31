cmake_minimum_required(VERSION 3.16)
project(urchinPhysicsEngine)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_CXX_STANDARD 23)
if ((CMAKE_BUILD_TYPE MATCHES "Release") OR (CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo"))
    SET(CMAKE_UNITY_BUILD TRUE)
endif()

find_package(Threads REQUIRED)
if (NOT WIN32)
    set(LINK_OPT -Wl,--export-dynamic) #necessary for SignalHandler to work correctly
endif()

add_definitions(-ffast-math -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wnull-dereference -Wimplicit-fallthrough -Wnon-virtual-dtor -Woverloaded-virtual -Wextra-semi -Werror)
include_directories(src ../common/src)

file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.inl" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
add_library(urchinPhysicsEngine STATIC ${SOURCE_FILES})
target_link_libraries(urchinPhysicsEngine ${LINK_OPT} Threads::Threads urchinCommon)
target_precompile_headers(urchinPhysicsEngine PRIVATE "../common/src/UrchinCommon.h")

cmake_minimum_required(VERSION 3.16)
project(urchinCommon)

#set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_CXX_STANDARD 23)
if ((CMAKE_BUILD_TYPE MATCHES "Release") OR (CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo"))
    SET(CMAKE_UNITY_BUILD TRUE)
endif()

if (WIN32)
    set(IMAGE_HLP_LIB imagehlp)
else()
    set(LINK_OPT -Wl,--export-dynamic dl) #necessary for SignalHandler to work correctly
endif()

add_definitions(-ffast-math -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wnull-dereference -Wimplicit-fallthrough -Wnon-virtual-dtor -Woverloaded-virtual -Wextra-semi -Werror)
include_directories(src)

file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.inl" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
add_library(urchinCommon STATIC ${SOURCE_FILES})
target_link_libraries(urchinCommon ${LINK_OPT} ${IMAGE_HLP_LIB})

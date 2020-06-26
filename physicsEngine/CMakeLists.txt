cmake_minimum_required(VERSION 3.7)
project(urchinPhysicsEngine)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_CXX_STANDARD 17)

add_definitions(-ffast-math -Wall -Wextra -Wpedantic -Werror)
include_directories(src ../common/src)

file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
add_library(urchinPhysicsEngine SHARED ${SOURCE_FILES})
target_link_libraries(urchinPhysicsEngine pthread urchinCommon -static-libstdc++)

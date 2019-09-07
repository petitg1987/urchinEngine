cmake_minimum_required(VERSION 3.7)
project(urchinAIEngine)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_CXX_STANDARD 17)

add_definitions(-ffast-math)
include_directories(src ../common/src)

file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
add_library(urchinAIEngine SHARED ${SOURCE_FILES})
target_link_libraries(urchinAIEngine urchinCommon -static-libstdc++)

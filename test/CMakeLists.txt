cmake_minimum_required(VERSION 3.7)
project(test)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_CXX_STANDARD 17)

add_definitions(-ffast-math -Wall)
include_directories(src ../common/src ../physicsEngine/src ../AIEngine/src)

file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
add_executable(testRunner ${SOURCE_FILES})
target_link_libraries(testRunner pthread cppunit urchinCommon urchinPhysicsEngine urchinAIEngine)

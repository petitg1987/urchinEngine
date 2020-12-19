cmake_minimum_required(VERSION 3.7)
project(test)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)

add_definitions(-ffast-math -Wall -Wextra -Wpedantic -Wconversion -Werror)
include_directories(src ../common/src ../3dEngine/src ../physicsEngine/src ../AIEngine/src)

file(GLOB_RECURSE MAIN_TESTS_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
add_executable(testExecutor ${MAIN_TESTS_SOURCE_FILES})
target_link_libraries(testExecutor Threads::Threads cppunit urchinCommon urchin3dEngine urchinPhysicsEngine urchinAIEngine)

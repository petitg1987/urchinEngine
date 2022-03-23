cmake_minimum_required(VERSION 3.16)
project(test)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_CXX_STANDARD 20)

find_package(Threads REQUIRED)
if (NOT WIN32)
    set(LINK_OPT -rdynamic) #necessary for SignalHandler to work correctly
endif()

add_definitions(-fno-stack-protector -ffast-math -Wall -Wextra -Wpedantic -Wconversion -Werror)
include_directories(src ../common/src ../3dEngine/src ../physicsEngine/src ../aiEngine/src)

file(GLOB_RECURSE MAIN_TESTS_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
add_executable(testExecutor ${MAIN_TESTS_SOURCE_FILES})
target_link_libraries(testExecutor ${LINK_OPT} Threads::Threads cppunit urchinCommon urchin3dEngine urchinPhysicsEngine urchinAIEngine)
target_precompile_headers(testExecutor PRIVATE
        ../common/src/UrchinCommon.h
        ../3dEngine/src/Urchin3dEngine.h
        ../physicsEngine/src/UrchinPhysicsEngine.h
        ../aiEngine/src/UrchinAIEngine.h)

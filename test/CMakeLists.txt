cmake_minimum_required(VERSION 3.7)
project(test)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)

add_definitions(-ffast-math -Wall -Wextra -Wpedantic -Wconversion -Werror)
include_directories(src ../common/src ../3dEngine/src ../physicsEngine/src ../AIEngine/src)

file(GLOB_RECURSE MAIN_TESTS_SOURCE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/*Test.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*Test.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/*IT.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*IT.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/*Helper.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*Helper.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/MainTestExecutor.cpp")
add_executable(mainTestExecutor ${MAIN_TESTS_SOURCE_FILES})
target_link_libraries(mainTestExecutor Threads::Threads cppunit urchinCommon urchin3dEngine urchinPhysicsEngine urchinAIEngine)

file(GLOB_RECURSE MONKEY_TESTS_SOURCE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/*MT.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*MT.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/*Helper.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*Helper.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/MonkeyTestExecutor.cpp")
add_executable(monkeyTestExecutor ${MONKEY_TESTS_SOURCE_FILES})
target_link_libraries(monkeyTestExecutor Threads::Threads cppunit urchinCommon urchin3dEngine urchinPhysicsEngine urchinAIEngine)

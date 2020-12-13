cmake_minimum_required(VERSION 3.7)
project(test)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)

add_definitions(-ffast-math -Wall -Wextra -Wpedantic -Wconversion -Werror)
include_directories(src ../common/src ../3dEngine/src ../physicsEngine/src ../AIEngine/src)

file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
list(FILTER SOURCE_FILES EXCLUDE REGEX ".*TestExecutor.cpp")
add_executable(mainTestExecutor ${SOURCE_FILES} "${CMAKE_CURRENT_SOURCE_DIR}/src/MainTestExecutor.cpp")
target_link_libraries(mainTestExecutor Threads::Threads cppunit urchinCommon urchin3dEngine urchinPhysicsEngine urchinAIEngine)
add_executable(monkeyTestExecutor ${SOURCE_FILES} "${CMAKE_CURRENT_SOURCE_DIR}/src/MonkeyTestExecutor.cpp")
target_link_libraries(monkeyTestExecutor Threads::Threads cppunit urchinCommon urchin3dEngine urchinPhysicsEngine urchinAIEngine)

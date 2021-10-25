cmake_minimum_required(VERSION 3.16)
project(urchinMapEditor)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

if (NOT WIN32)
    set(LINK_OPT -rdynamic) #necessary for SignalHandler to work correctly
endif()

add_definitions(-ffast-math -Wall -Wextra -Wpedantic -Wconversion -Werror)
include_directories(src ../common/src ../3dEngine/src ../physicsEngine/src ../soundEngine/src ../AIEngine/src ../mapHandler/src)

#QT
find_package(Qt5Core)
find_package(Qt5Widgets)
find_package(Qt5Gui)
SET(QT_LIB Qt5::Core Qt5::Widgets Qt5::Gui)

file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
add_custom_target(copyShaders rsync -r ${CMAKE_CURRENT_SOURCE_DIR}/../3dEngine/resources/shaders/spirv/* ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/)
add_executable(urchinMapEditor ${SOURCE_FILES})
add_dependencies(urchinMapEditor copyShaders)
target_link_libraries(urchinMapEditor ${LINK_OPT} ${QT_LIB} urchinCommon urchin3dEngine urchinPhysicsEngine urchinSoundEngine urchinAIEngine urchinMapHandler)

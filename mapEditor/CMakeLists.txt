cmake_minimum_required(VERSION 3.7)
project(urchinMapEditor)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_definitions(-ffast-math)
include_directories(src ../common/src ../3dEngine/src ../physicsEngine/src ../soundEngine/src ../AIEngine/src ../mapHandler/src)

find_package(Qt5Widgets)
find_package(Qt5Core)
find_package(Qt5Gui)
find_package(Qt5OpenGL)
find_package(OpenGL)

file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
add_custom_target(copyShaders rsync -r ${CMAKE_CURRENT_SOURCE_DIR}/../3dEngine/resources/shaders/* ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/)
add_executable(urchinMapEditor ${SOURCE_FILES})
add_dependencies(urchinMapEditor copyShaders)
qt5_use_modules(urchinMapEditor Widgets OpenGL)
target_link_libraries(urchinMapEditor ${QT_LIBRARIES} ${OPENGL_LIBRARIES} urchinCommon urchin3dEngine urchinPhysicsEngine urchinSoundEngine urchinAIEngine urchinMapHandler)

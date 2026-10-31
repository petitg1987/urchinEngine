cmake_minimum_required(VERSION 3.16)
project(urchinMapEditor)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

if (NOT WIN32)
    set(LINK_OPT -Wl,--export-dynamic) #necessary for SignalHandler to work correctly
endif()

add_definitions(-ffast-math -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wnull-dereference -Wimplicit-fallthrough -Wnon-virtual-dtor -Woverloaded-virtual -Wextra-semi -Werror)
include_directories(src ../common/src ../3dEngine/src ../physicsEngine/src ../soundEngine/src ../aiEngine/src ../aggregation/src)
list(TRANSFORM URCHIN_HEADERS PREPEND ../)

#QT
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)
SET(QT_LIB Qt6::Core Qt6::Widgets Qt6::Gui)

#WAYLAND (pointer-warp-v1 & relative-pointer protocols to move the mouse cursor)
find_package(PkgConfig REQUIRED)
pkg_check_modules(WAYLAND_CLIENT REQUIRED wayland-client)
pkg_get_variable(WAYLAND_PROTOCOLS_DIR wayland-protocols pkgdatadir)
pkg_get_variable(WAYLAND_SCANNER wayland-scanner wayland_scanner)
set(WAYLAND_GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/wayland)
file(MAKE_DIRECTORY ${WAYLAND_GENERATED_DIR})
set(WAYLAND_PROTOCOL_SOURCES "")
foreach(PROTOCOL_PATH staging/pointer-warp/pointer-warp-v1 unstable/relative-pointer/relative-pointer-unstable-v1)
    get_filename_component(PROTOCOL_NAME ${PROTOCOL_PATH} NAME)
    set(PROTOCOL_XML ${WAYLAND_PROTOCOLS_DIR}/${PROTOCOL_PATH}.xml)
    add_custom_command(
            OUTPUT ${WAYLAND_GENERATED_DIR}/${PROTOCOL_NAME}-client-protocol.h ${WAYLAND_GENERATED_DIR}/${PROTOCOL_NAME}-protocol.c
            COMMAND ${WAYLAND_SCANNER} client-header ${PROTOCOL_XML} ${WAYLAND_GENERATED_DIR}/${PROTOCOL_NAME}-client-protocol.h
            COMMAND ${WAYLAND_SCANNER} private-code ${PROTOCOL_XML} ${WAYLAND_GENERATED_DIR}/${PROTOCOL_NAME}-protocol.c
            DEPENDS ${PROTOCOL_XML})
    list(APPEND WAYLAND_PROTOCOL_SOURCES ${WAYLAND_GENERATED_DIR}/${PROTOCOL_NAME}-protocol.c ${WAYLAND_GENERATED_DIR}/${PROTOCOL_NAME}-client-protocol.h)
endforeach()
add_library(urchinWaylandProtocols STATIC ${WAYLAND_PROTOCOL_SOURCES})
target_compile_options(urchinWaylandProtocols PRIVATE -w)
target_include_directories(urchinWaylandProtocols PUBLIC ${WAYLAND_GENERATED_DIR} ${WAYLAND_CLIENT_INCLUDE_DIRS})
target_link_libraries(urchinWaylandProtocols PUBLIC ${WAYLAND_CLIENT_LIBRARIES})

file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
add_custom_target(mapSyncShaders ALL rsync -r ${CMAKE_CURRENT_SOURCE_DIR}/../3dEngine/resources/shaders/spirv/ ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/ --delete DEPENDS urchin3dEngine)
add_executable(urchinMapEditor ${SOURCE_FILES})
add_dependencies(urchinMapEditor mapSyncShaders)
target_link_libraries(urchinMapEditor ${LINK_OPT} ${QT_LIB} urchinCommon urchin3dEngine urchinPhysicsEngine urchinSoundEngine urchinAIEngine urchinAggregation urchinWaylandProtocols)
target_precompile_headers(urchinMapEditor PRIVATE ${URCHIN_HEADERS})

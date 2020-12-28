cmake_minimum_required(VERSION 3.7)
project(urchinCommon)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_CXX_STANDARD 17)

if (WIN32)
    set(IMAGE_HLP_LIB imagehlp)
else()
    set(LINK_OPT -rdynamic dl) #necessary for SignalHandler to work correctly
endif()

add_definitions(-ffast-math -Wall -Wextra -Wpedantic -Wconversion -Werror)
include_directories(src)

file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.inl" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
add_library(urchinCommon STATIC ${SOURCE_FILES})
target_link_libraries(urchinCommon ${LINK_OPT} -static-libstdc++ ${IMAGE_HLP_LIB})

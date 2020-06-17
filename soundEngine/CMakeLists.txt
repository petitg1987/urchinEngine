cmake_minimum_required(VERSION 3.7)
project(urchinSoundEngine)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_CXX_STANDARD 17)

add_definitions(-ffast-math -Wall)
include_directories(src ../common/src)

file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
if (WIN32)
    set(OPENAL_LIB openal32)
else()
    set(OPENAL_LIB openal)
endif()
add_library(urchinSoundEngine SHARED ${SOURCE_FILES})
target_link_libraries(urchinSoundEngine pthread ${OPENAL_LIB} sndfile urchinCommon -static-libstdc++)

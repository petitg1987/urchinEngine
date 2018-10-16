cmake_minimum_required(VERSION 3.7)
project(urchinNetworkEngine)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
        src/request/Request.cpp
        src/request/Request.h)

include_directories(src ../common/src)

add_library(urchinNetworkEngine SHARED ${SOURCE_FILES})
target_link_libraries(urchinNetworkEngine curl urchinCommon)
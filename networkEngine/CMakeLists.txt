cmake_minimum_required(VERSION 3.16)
project(urchinNetworkEngine)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_CXX_STANDARD 23)
if ((CMAKE_BUILD_TYPE MATCHES "Release") OR (CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo"))
    SET(CMAKE_UNITY_BUILD TRUE)
endif()

if (WIN32)
    set(CURL_LIB curl.a advapi32.a bcrypt.a ssl.a crypto.a brotlidec.a brotlicommon.a z.a nghttp2.a gdi32.a zstd.a psl.a unistring.a ws2_32.a iconv.a crypt32.a)
else()
    include(deps/curl/curl-static.cmake) #only used on Linux because build is too slow on Windows

    set(LINK_OPT -Wl,--export-dynamic) #necessary for SignalHandler to work correctly
    set(CURL_LIB curl.a ssl.a crypto.a brotlidec.a brotlicommon.a z.a nghttp2.a)
endif()

add_definitions(-ffast-math -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wnull-dereference -Wimplicit-fallthrough -Wnon-virtual-dtor -Woverloaded-virtual -Wextra-semi -Werror -DCURL_STATICLIB)
include_directories(src ../common/src)

file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
add_library(urchinNetworkEngine STATIC ${SOURCE_FILES})
if (NOT WIN32)
    add_dependencies(urchinNetworkEngine curl-static)
endif()
target_link_libraries(urchinNetworkEngine ${LINK_OPT} ${CURL_LIB} urchinCommon)
target_precompile_headers(urchinNetworkEngine PRIVATE "../common/src/UrchinCommon.h")

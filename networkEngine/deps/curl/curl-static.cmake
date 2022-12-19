include(ExternalProject)

SET_DIRECTORY_PROPERTIES(PROPERTIES CLEAN_NO_CUSTOM 1)
if ((CMAKE_BUILD_TYPE MATCHES "Release") OR (CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo"))
    SET(CMAKE_UNITY_BUILD TRUE)
endif()

set(CURL_ROOT               ${CMAKE_BINARY_DIR}/deps/curl)
set(CURL_LIB_DIR            ${CURL_ROOT}/lib)
set(CURL_INCLUDE_DIR        ${CURL_ROOT}/include)
set(CURL_CONFIGURE          cd ${CURL_ROOT}/src/curl-static && ./configure CPPFLAGS=-DNGHTTP2_STATICLIB --prefix=${CURL_ROOT} --program-suffix=custom --disable-shared --enable-static --disable-ldap --disable-sspi --disable-ftp --disable-file --disable-dict --disable-telnet --disable-tftp --disable-hsts --disable-alt-svc --disable-rtsp --disable-pop3 --disable-imap --disable-smtp --disable-gopher --disable-smb --without-librtmp --without-libidn --with-ssl --with-nghttp2)
set(CURL_MAKE               cd ${CURL_ROOT}/src/curl-static && make)
set(CURL_INSTALL            cd ${CURL_ROOT}/src/curl-static && make install)

if(${CMAKE_VERSION} VERSION_LESS "3.24.0") #for build in Docker image
    ExternalProject_Add(curl-static
            URL                         ${CMAKE_CURRENT_SOURCE_DIR}/deps/curl/curl-7.82.0.zip
            URL_HASH                    MD5=103a746d6b53f3ab184daa6260add764
            PREFIX                      ${CURL_ROOT}
            CONFIGURE_COMMAND           ${CURL_CONFIGURE}
            BUILD_COMMAND               ${CURL_MAKE}
            INSTALL_COMMAND             ${CURL_INSTALL})
else()
    ExternalProject_Add(curl-static
            URL                         ${CMAKE_CURRENT_SOURCE_DIR}/deps/curl/curl-7.82.0.zip
            URL_HASH                    MD5=103a746d6b53f3ab184daa6260add764
            DOWNLOAD_EXTRACT_TIMESTAMP  true
            PREFIX                      ${CURL_ROOT}
            CONFIGURE_COMMAND           ${CURL_CONFIGURE}
            BUILD_COMMAND               ${CURL_MAKE}
            INSTALL_COMMAND             ${CURL_INSTALL})
endif()

#Dependencies: ${CMAKE_BINARY_DIR}/deps/curl/lib/pkgconfig/libcurl.pc (see Libs.private)

link_directories (BEFORE ${CURL_LIB_DIR})
include_directories(BEFORE ${CURL_INCLUDE_DIR})

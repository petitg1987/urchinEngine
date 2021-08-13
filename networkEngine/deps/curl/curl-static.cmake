include(ExternalProject)

set(CURL_ROOT               ${CMAKE_BINARY_DIR}/deps/curl)
set(CURL_LIB_DIR            ${CURL_ROOT}/lib)
set(CURL_INCLUDE_DIR        ${CURL_ROOT}/include)
set(CURL_CONFIGURE          cd ${CURL_ROOT}/src/curl-build && ./configure CPPFLAGS=-DNGHTTP2_STATICLIB --prefix=${CURL_ROOT} --program-suffix=custom --disable-shared --enable-static --disable-ldap --disable-sspi --disable-ftp --disable-file --disable-dict --disable-telnet --disable-tftp --disable-rtsp --disable-pop3 --disable-imap --disable-smtp --disable-gopher --disable-smb --without-librtmp --without-libidn --with-ssl --with-nghttp2)
set(CURL_MAKE               cd ${CURL_ROOT}/src/curl-build && make)
set(CURL_INSTALL            cd ${CURL_ROOT}/src/curl-build && make install)

ExternalProject_Add(curl-static
        URL                 ${CMAKE_CURRENT_SOURCE_DIR}/deps/curl/curl-7.78.0.zip
        URL_HASH            MD5=bbe9fc18e0aef947567ce7c4084a3430
        PREFIX              ${CURL_ROOT}
        CONFIGURE_COMMAND   ${CURL_CONFIGURE}
        BUILD_COMMAND       ${CURL_MAKE}
        INSTALL_COMMAND     ${CURL_INSTALL})

#Dependencies: ${CMAKE_BINARY_DIR}/deps/curl/lib/pkgconfig/libcurl.pc (see Libs.private)

link_directories (BEFORE ${CURL_LIB_DIR})
include_directories(BEFORE ${CURL_INCLUDE_DIR})

cmake_minimum_required(VERSION 3.16)
project(urchin3dEngine)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_CXX_STANDARD 17)

#Compile shaders
set(shadersDir "${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders")
set(compiledShadersDir "${shadersDir}/spirv")
file(GLOB shaderFiles "${shadersDir}/*.*")
foreach (shaderFile ${shaderFiles})
    get_filename_component(shaderFilename ${shaderFile} NAME)
    set(compiledShaderName "${shaderFilename}.spv")
    if ("${shaderFilename}" MATCHES "^_.*")
        continue()
    endif()

    add_custom_command( OUTPUT ${compiledShadersDir}/${compiledShaderName}
            DEPENDS ${shadersDir}/${shaderFilename}
            COMMAND glslc --target-env=vulkan1.1 -O -Werror ${shadersDir}/${shaderFilename} -o ${compiledShadersDir}/${compiledShaderName})
    set(compiledShaderNames "${compiledShadersDir}/${compiledShaderName};${compiledShaderNames}")
endforeach()
add_custom_target(urchinShader ALL DEPENDS "${compiledShaderNames}")

if (WIN32)
    set(FREETYPE_LIB freetype.a)
    SET(VULKAN_LIB vulkan-1)
else()
    set(LINK_OPT -rdynamic) #necessary for SignalHandler to work correctly
    set(FREETYPE_LIB freetype.a png16.a brotlidec.a brotlicommon.a z.a)
    SET(VULKAN_LIB vulkan)
endif()

add_definitions(-ffast-math -Wall -Wextra -Wpedantic -Wconversion -Werror)
include_directories(src ../common/src /usr/include/freetype2)

file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.inl" "${CMAKE_CURRENT_SOURCE_DIR}/src/libs/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
add_library(urchin3dEngine STATIC ${SOURCE_FILES})
add_dependencies(urchin3dEngine urchinShader)
target_link_libraries(urchin3dEngine ${LINK_OPT} ${FREETYPE_LIB} ${VULKAN_LIB} urchinCommon)

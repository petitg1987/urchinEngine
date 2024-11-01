cmake_minimum_required(VERSION 3.16)
project(urchin3dEngine)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_CXX_STANDARD 20)
if ((CMAKE_BUILD_TYPE MATCHES "Release") OR (CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo"))
    SET(CMAKE_UNITY_BUILD TRUE)
endif()

#Compile shaders
set(shadersDir "${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders")
set(compiledShadersDir "${shadersDir}/spirv")
file(GLOB shaderFiles "${shadersDir}/*.*")
foreach (shaderFile ${shaderFiles})
    get_filename_component(shaderFilename ${shaderFile} NAME)
    if ("${shaderFilename}" MATCHES "^_.*")
        set(commonShaderFiles "${shadersDir}/${shaderFilename};${commonShaderFiles}")
    endif()
endforeach()
foreach (shaderFile ${shaderFiles})
    get_filename_component(shaderFilename ${shaderFile} NAME)
    set(compiledShaderName "${shaderFilename}.spv")
    if ("${shaderFilename}" MATCHES "^_.*")
        continue()
    endif()

    set(shaderStage "unknown")
    if ("${shaderFilename}" MATCHES "vert$")
        set(shaderStage "vertex")
    elseif("${shaderFilename}" MATCHES "frag$")
        set(shaderStage "fragment")
    elseif("${shaderFilename}" MATCHES "comp$")
        set(shaderStage "compute")
    endif()

    add_custom_command( OUTPUT ${compiledShadersDir}/${compiledShaderName}
            DEPENDS "${shadersDir}/${shaderFilename};${commonShaderFiles}"
            COMMAND glslc --target-env=vulkan1.3 -fshader-stage=${shaderStage} -O -g -Werror ${shadersDir}/${shaderFilename} -o ${compiledShadersDir}/${compiledShaderName})
    set(compiledShaderNames "${compiledShadersDir}/${compiledShaderName};${compiledShaderNames}")
endforeach()
add_custom_target(urchinShader ALL DEPENDS "${compiledShaderNames}")

if (WIN32)
    set(FREETYPE_LIB freetype.a libbz2)
    SET(VULKAN_LIB vulkan-1)
else()
    set(LINK_OPT -rdynamic) #necessary for SignalHandler to work correctly
    set(FREETYPE_LIB freetype.a bz2.a png16.a brotlidec.a brotlicommon.a z.a)
    SET(VULKAN_LIB vulkan)
endif()

add_definitions(-fno-stack-protector -ffast-math -Wall -Wextra -Wpedantic -Wconversion -Werror)
include_directories(src ../common/src /usr/include/freetype2)

file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.inl" "${CMAKE_CURRENT_SOURCE_DIR}/src/libs/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
add_library(urchin3dEngine STATIC ${SOURCE_FILES})
add_dependencies(urchin3dEngine urchinShader)
target_link_libraries(urchin3dEngine ${LINK_OPT} ${FREETYPE_LIB} ${VULKAN_LIB} urchinCommon)
target_precompile_headers(urchin3dEngine PRIVATE "../common/src/UrchinCommon.h")

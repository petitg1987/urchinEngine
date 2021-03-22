#include <stdexcept>
#include <fstream>

#include "ShaderBuilder.h"
#include "graphic/render/shader/config/ShaderConfig.h"
#include "graphic/render/shader/builder/TokenReplacerShader.h"

namespace urchin {

    std::shared_ptr<Shader> ShaderBuilder::createShader(const std::string& vertexShaderFilename, const std::string& geometryShaderFilename,
                                                        const std::string& fragmentShaderFilename) {
        std::map<std::string, std::string> emptyTokens;
        return createShader(vertexShaderFilename, geometryShaderFilename, fragmentShaderFilename, emptyTokens);
    }

    std::shared_ptr<Shader> ShaderBuilder::createShader(const std::string& vertexShaderFilename, const std::string& geometryShaderFilename,
                                                        const std::string& fragmentShaderFilename, const std::map<std::string, std::string>& tokens) {
        std::vector<std::pair<Shader::ShaderType, std::vector<char>>> shaderSources;

        //vertex shader
        std::vector<char> vertexShader = compile(vertexShaderFilename, tokens, shaderc_glsl_vertex_shader);
        shaderSources.emplace_back(std::make_pair(Shader::VERTEX, vertexShader));

        //geometry shader
        if (!geometryShaderFilename.empty()) {
            std::vector<char> geometryShader = compile(geometryShaderFilename, tokens, shaderc_glsl_geometry_shader);
            shaderSources.emplace_back(std::make_pair(Shader::GEOMETRY, geometryShader));
        }

        //fragment shader
        std::vector<char> fragmentShader = compile(fragmentShaderFilename, tokens, shaderc_glsl_fragment_shader);
        shaderSources.emplace_back(std::make_pair(Shader::FRAGMENT, fragmentShader));

        return std::make_shared<Shader>(shaderSources);
    }

    std::vector<char> ShaderBuilder::compile(const std::string &filename, const std::map<std::string, std::string>& tokens, shaderc_shader_kind shaderKind) {
        std::vector<char> rawShaderCode = readFile(ShaderConfig::instance()->getShadersDirectory() + filename);
        std::string shaderCode = TokenReplacerShader::replaceTokens(std::string(rawShaderCode.begin(), rawShaderCode.end()), tokens);

        auto compiler = shaderc_compiler_initialize();
        auto compileResult = shaderc_compile_into_spv(compiler, shaderCode.c_str(), shaderCode.size(), shaderKind, filename.c_str(),"main", nullptr);
        auto compileStatus = shaderc_result_get_compilation_status(compileResult);

        if(compileStatus != shaderc_compilation_status_success) {
            const char* compilationErrorMessage = shaderc_result_get_error_message(compileResult);
            std::string errorStr = "Compilation error (" + std::to_string(compileStatus) + ") on " + filename + ":" + std::string(compilationErrorMessage);
            throw std::runtime_error(errorStr);
        }

        std::size_t spirVSize = shaderc_result_get_length(compileResult);
        const char* spirVPtr = shaderc_result_get_bytes(compileResult);
        std::vector<char> spirV(spirVPtr, spirVPtr + spirVSize);

        shaderc_result_release(compileResult);
        shaderc_compiler_release(compiler);

        return spirV;
    }

    std::vector<char> ShaderBuilder::readFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Unable to open file: " + filename);
        }

        std::size_t fileSize = (std::size_t)file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), static_cast<std::streamsize>(fileSize));
        file.close();

        return buffer;
    }

}

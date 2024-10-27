#include <stdexcept>
#include <fstream>
#include <cstring>

#include <graphics/render/shader/ShaderBuilder.h>
#include <graphics/render/shader/ShaderConfig.h>

namespace urchin {

    std::unique_ptr<Shader> ShaderBuilder::createShader(const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename) {
        return createShader(vertexShaderFilename, fragmentShaderFilename, std::unique_ptr<ShaderConstants>());
    }

    std::unique_ptr<Shader> ShaderBuilder::createShader(const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename,
                                                        std::unique_ptr<ShaderConstants> shaderConstants) {
        std::vector<std::pair<Shader::ShaderType, std::vector<char>>> shaderSources;
        std::string shadersDirectory = ShaderConfig::instance().getShadersDirectory();

        shaderSources.emplace_back(Shader::VERTEX, readFile(shadersDirectory + vertexShaderFilename));
        shaderSources.emplace_back(Shader::FRAGMENT, readFile(shadersDirectory + fragmentShaderFilename));

        std::size_t shaderId = computeShaderId(vertexShaderFilename, fragmentShaderFilename, shaderConstants.get());
        std::string shaderName = FileUtil::getFileNameNoExtension(FileUtil::getFileNameNoExtension(vertexShaderFilename)) + "-"
                + FileUtil::getFileNameNoExtension(FileUtil::getFileNameNoExtension(fragmentShaderFilename));
        return std::make_unique<Shader>(shaderId, shaderName, shaderSources, std::move(shaderConstants));
    }

    std::unique_ptr<Shader> ShaderBuilder::createComputeShader(const std::string& computeShaderFilename, std::unique_ptr<ShaderConstants> shaderConstants) {
        std::vector<std::pair<Shader::ShaderType, std::vector<char>>> shaderSources;
        std::string shadersDirectory = ShaderConfig::instance().getShadersDirectory();

        shaderSources.emplace_back(Shader::COMPUTE, readFile(shadersDirectory + computeShaderFilename));

        std::size_t shaderId = computeShaderId(computeShaderFilename, "", shaderConstants.get());
        std::string shaderName = FileUtil::getFileNameNoExtension(FileUtil::getFileNameNoExtension(computeShaderFilename));
        return std::make_unique<Shader>(shaderId, shaderName, shaderSources, std::move(shaderConstants));
    }

    /**
     * Shader which does nothing (useful for testing)
     */
    std::unique_ptr<Shader> ShaderBuilder::createNullShader() { //TODO remove
        std::vector<std::pair<Shader::ShaderType, std::vector<char>>> shaderSources;
        return std::make_unique<Shader>(0, "_null_", shaderSources, std::unique_ptr<ShaderConstants>());
    }

    std::vector<char> ShaderBuilder::readFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Unable to open file: " + filename);
        }

        auto fileSize = (std::size_t)file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), static_cast<std::streamsize>(fileSize));
        file.close();

        return buffer;
    }

    std::size_t ShaderBuilder::computeShaderId(std::string_view vertexShaderFilename, std::string_view fragmentShaderFilename, const ShaderConstants* shaderConstants) {
        std::size_t shaderId = 0;

        HashUtil::combine(shaderId, vertexShaderFilename, fragmentShaderFilename);

        if (shaderConstants) {
            const auto* variablesData = static_cast<std::size_t*>(shaderConstants->getData());
            std::size_t variablesSize = shaderConstants->sumVariablesSize();
            unsigned int numRead = MathFunction::ceilToUInt((float)variablesSize / (float)sizeof(std::size_t));
            for (std::size_t i = 0; i < numRead; ++i) {
                std::size_t readSize = std::min(variablesSize - i * sizeof(std::size_t), sizeof(std::size_t));
                std::size_t readValue = 0;

                std::memcpy(&readValue, variablesData + i, readSize);
                HashUtil::combine(shaderId, readValue);
            }
        }

        return shaderId;
    }

}

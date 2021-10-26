#include <stdexcept>
#include <fstream>
#include <cstring>

#include <api/render/shader/builder/ShaderBuilder.h>
#include <api/render/shader/config/ShaderConfig.h>

namespace urchin {

    std::unique_ptr<Shader> ShaderBuilder::createShader(const std::string& vertexShaderFilename, const std::string& geometryShaderFilename,
                                                        const std::string& fragmentShaderFilename) {
        return createShader(vertexShaderFilename, geometryShaderFilename, fragmentShaderFilename, std::unique_ptr<ShaderConstants>());
    }

    std::unique_ptr<Shader> ShaderBuilder::createShader(const std::string& vertexShaderFilename, const std::string& geometryShaderFilename,
                                                        const std::string& fragmentShaderFilename, std::unique_ptr<ShaderConstants> shaderConstants) {
        std::vector<std::pair<Shader::ShaderType, std::vector<char>>> shaderSources;
        std::string shadersDirectory = ShaderConfig::instance().getShadersDirectory();

        shaderSources.emplace_back(std::make_pair(Shader::VERTEX, readFile(shadersDirectory + vertexShaderFilename)));
        if (!geometryShaderFilename.empty()) {
            shaderSources.emplace_back(std::make_pair(Shader::GEOMETRY, readFile(shadersDirectory + geometryShaderFilename)));
        }
        shaderSources.emplace_back(std::make_pair(Shader::FRAGMENT, readFile(shadersDirectory + fragmentShaderFilename)));

        std::size_t shaderId = computeShaderId(vertexShaderFilename, geometryShaderFilename, fragmentShaderFilename, shaderConstants.get());
        std::string shaderName = FileUtil::getFileNameNoExtension(FileUtil::getFileNameNoExtension(vertexShaderFilename));
        return std::make_unique<Shader>(shaderId, shaderName, shaderSources, std::move(shaderConstants));
    }

    /**
     * Shader which does nothing (useful for testing)
     */
    std::unique_ptr<Shader> ShaderBuilder::createNullShader() {
        std::vector<std::pair<Shader::ShaderType, std::vector<char>>> shaderSources;
        return std::make_unique<Shader>(0, "_null_", shaderSources, std::unique_ptr<ShaderConstants>());
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

    std::size_t ShaderBuilder::computeShaderId(const std::string& vertexShaderFilename, const std::string& geometryShaderFilename,
                                               const std::string& fragmentShaderFilename, const ShaderConstants* shaderConstants) {
        std::size_t shaderId = 0;

        HashUtil::combine(shaderId, vertexShaderFilename, geometryShaderFilename, fragmentShaderFilename);

        if (shaderConstants) {
            auto* variablesData = static_cast<std::size_t*>(shaderConstants->getData());
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

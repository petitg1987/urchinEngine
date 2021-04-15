#include <stdexcept>
#include <fstream>

#include "ShaderBuilder.h"
#include "graphic/render/shader/config/ShaderConfig.h"

namespace urchin {

    std::shared_ptr<Shader> ShaderBuilder::createShader(const std::string& vertexShaderFilename, const std::string& geometryShaderFilename,
                                                        const std::string& fragmentShaderFilename) {
        return createShader(vertexShaderFilename, geometryShaderFilename, fragmentShaderFilename, std::unique_ptr<ShaderConstants>());
    }

    std::shared_ptr<Shader> ShaderBuilder::createShader(const std::string& vertexShaderFilename, const std::string& geometryShaderFilename,
                                                        const std::string& fragmentShaderFilename, std::unique_ptr<ShaderConstants> shaderConstants) {
        std::vector<std::pair<Shader::ShaderType, std::vector<char>>> shaderSources;

        //vertex shader
        shaderSources.emplace_back(std::make_pair(Shader::VERTEX, readFile(ShaderConfig::instance()->getShadersDirectory() + vertexShaderFilename)));

        //geometry shader
        if (!geometryShaderFilename.empty()) {
            shaderSources.emplace_back(std::make_pair(Shader::GEOMETRY, readFile(ShaderConfig::instance()->getShadersDirectory() + geometryShaderFilename)));
        }

        //fragment shader
        shaderSources.emplace_back(std::make_pair(Shader::FRAGMENT, readFile(ShaderConfig::instance()->getShadersDirectory() + fragmentShaderFilename)));

        std::string shaderName = urchin::FileUtil::getFileName(vertexShaderFilename);
        return std::make_shared<Shader>(shaderName, shaderSources, std::move(shaderConstants));
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

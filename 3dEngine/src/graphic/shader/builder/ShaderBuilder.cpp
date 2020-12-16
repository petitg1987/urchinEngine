#include <GL/glew.h>
#include <fstream>
#include <stdexcept>

#include "graphic/shader/builder/ShaderBuilder.h"
#include "graphic/shader/config/ShaderConfig.h"

namespace urchin {

    std::unique_ptr<Shader> ShaderBuilder::createShader(const std::string& vertexShaderFilename, const std::string& geometryShaderFilename,
                                              const std::string& fragmentShaderFilename) {
        std::map<std::string, std::string> emptyTokens;
        return createShader(vertexShaderFilename, geometryShaderFilename, fragmentShaderFilename, emptyTokens);
    }

    std::unique_ptr<Shader> ShaderBuilder::createShader(const std::string& vertexShaderFilename, const std::string& geometryShaderFilename,
                                              const std::string& fragmentShaderFilename, const std::map<std::string, std::string>& tokens) {
        unsigned int shaderId = glCreateProgram();

        //vertex shader
        const std::string& vertexShaderFileSource = readEntireFile(ShaderConfig::instance()->getShadersDirectory() + vertexShaderFilename);
        const std::string& vertexShaderSource = TokenReplacerShader::replaceTokens(vertexShaderFileSource, tokens);
        const char* vertexShaderSourceChar = vertexShaderSource.c_str();
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSourceChar, nullptr);
        glCompileShader(vertexShader);
        checkShaderFile(vertexShader, vertexShaderFilename);
        glAttachShader(shaderId, vertexShader);

        //geometry shader
        if (!geometryShaderFilename.empty()) {
            const std::string& geometryShaderFileSource = readEntireFile(ShaderConfig::instance()->getShadersDirectory() + geometryShaderFilename);
            const std::string& geometryShaderSource = TokenReplacerShader::replaceTokens(geometryShaderFileSource, tokens);
            const char* geometryShaderSourceChar = geometryShaderSource.c_str();
            unsigned int geometryShader = glCreateShader(GL_GEOMETRY_SHADER);

            glShaderSource(geometryShader, 1, &geometryShaderSourceChar, nullptr);
            glCompileShader(geometryShader);
            checkShaderFile(geometryShader, geometryShaderFilename);
            glAttachShader(shaderId, geometryShader);
        }

        //fragment shader
        const std::string& fragmentShaderFileSource = readEntireFile(ShaderConfig::instance()->getShadersDirectory() + fragmentShaderFilename);
        const std::string& fragmentShaderSource = TokenReplacerShader::replaceTokens(fragmentShaderFileSource, tokens);
        const char* fragmentShaderSourceChar = fragmentShaderSource.c_str();
        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSourceChar, nullptr);
        glCompileShader(fragmentShader);
        checkShaderFile(fragmentShader, fragmentShaderFilename);
        glAttachShader(shaderId, fragmentShader);

        //link
        glLinkProgram(shaderId);
        checkShader(shaderId, vertexShaderFilename + ", " + geometryShaderFilename + ", " + fragmentShaderFilename);

        return std::make_unique<Shader>(shaderId);
    }

    std::string ShaderBuilder::readEntireFile(const std::string& filename) {
        std::string fileContent;
        fileContent.reserve(10000);

        std::ifstream file;
        file.open(filename, std::ios::in|std::ios::binary);
        if (file.fail()) {
            throw std::invalid_argument("Cannot open the file " + filename + ".");
        }

        char c = 0;
        while (file.get(c)) {
            fileContent.append(1, c);
        }

        file.close();

        return fileContent;
    }

    void ShaderBuilder::checkShaderFile(unsigned int shaderFileId, const std::string& shaderFilename) {
        int infoLogLength = 0;
        glGetShaderiv(shaderFileId, GL_INFO_LOG_LENGTH, &infoLogLength);

        if (infoLogLength > 1) {
            auto* infoLog = new char[(unsigned long)infoLogLength];
            glGetShaderInfoLog(shaderFileId, infoLogLength, nullptr, infoLog);

            std::ostringstream buffer;
            buffer<<"Error in shader file: "<< shaderFilename << "." << std::endl << infoLog;

            delete[] infoLog;
            throw std::runtime_error(buffer.str());
        }
    }

    void ShaderBuilder::checkShader(unsigned int programID, const std::string& shaderFilenames) {
        int infoLogLength = 0;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);

        if (infoLogLength > 1) {
            auto* infoLog = new char[(unsigned long)infoLogLength];
            glGetProgramInfoLog(programID, infoLogLength, nullptr, infoLog);

            std::ostringstream buffer;
            buffer<<"Error in shader file(s): " << shaderFilenames << "." << std::endl << infoLog;

            delete[] infoLog;
            throw std::runtime_error(buffer.str());
        }
    }
}

#include "graphic/render/shader/builder/TokenReplacerShader.h"

namespace urchin {

    std::string TokenReplacerShader::replaceTokens(const std::string& shaderSource, const std::map<std::string, std::string>& tokens) {
        std::string result = shaderSource;
        for (const auto& token : tokens) {
            std::string startLineToken = "#define " + token.first;
            std::string newLineValue = "#define " + token.first + " " + token.second;

            StringUtil::replaceLine(result, startLineToken, newLineValue);
        }

        return result;
    }
}

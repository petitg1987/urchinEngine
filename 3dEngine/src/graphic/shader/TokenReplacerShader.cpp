#include "graphic/shader/TokenReplacerShader.h"

namespace urchin {

    std::string TokenReplacerShader::replaceTokens(const std::string &shaderSource, const std::map<std::string, std::string> &tokens) {
        std::string result = shaderSource;
        for (const auto &token : tokens) {
            std::ostringstream startLineToken;
            startLineToken << "#define " << token.first;

            std::ostringstream newLineValue;
            newLineValue << "#define " << token.first << " " << token.second;

            StringUtil::replaceLine(result, startLineToken.str(), newLineValue.str());
        }

        return result;
    }
}

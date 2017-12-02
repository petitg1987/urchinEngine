#include "utils/shader/TokenReplacerShader.h"

namespace urchin
{

	std::string TokenReplacerShader::replaceTokens(const std::string &shaderSource, const std::map<std::string, std::string> &tokens)
	{
		std::string result = shaderSource;
		for (const auto &token : tokens)
		{
			std::ostringstream tokenString;
			tokenString << "#" << token.first << "#";
			StringUtil::replaceAll(result, tokenString.str(), token.second);
		}

		return result;
	}
}

#include <boost/algorithm/string/replace.hpp>

#include "utils/shader/TokenReplacerShader.h"

namespace urchin
{

	TokenReplacerShader::TokenReplacerShader()
	{

	}

	std::string TokenReplacerShader::replaceTokens(const std::string &shaderSource, const std::map<std::string, std::string> &tokens)
	{
		std::string result = shaderSource;
		for(std::map<std::string, std::string>::const_iterator it = tokens.begin(); it!=tokens.end(); ++it)
		{
			std::ostringstream tokenString;
			tokenString << "#" << it->first << "#";
			boost::replace_all(result, tokenString.str(), it->second);
		}

		return result;
	}
}

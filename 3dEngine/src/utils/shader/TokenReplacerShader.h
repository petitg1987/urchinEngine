#ifndef ENGINE_TOKENREPLACERSHADER_H
#define ENGINE_TOKENREPLACERSHADER_H

#include <string>
#include "UrchinCommon.h"

namespace urchin
{

	/**
	* Replace tokens in shader source.
	* Token identified by #TOKENx# will be replaced by given value where 'x' is the token number.
	*/
	class TokenReplacerShader
	{
		public:
			enum ShaderToken
			{
				TOKEN0 = 0,
				TOKEN1,
				TOKEN2,
				TOKEN3,
				TOKEN4,
				TOKEN5,
				TOKEN6,
				TOKEN7,
				TOKEN8,
				TOKEN9
			};

			TokenReplacerShader();

			std::string replaceTokens(const std::string &, const std::map<ShaderToken, std::string> &);
	};

}

#endif

#ifndef URCHINENGINE_TOKENREPLACERSHADER_H
#define URCHINENGINE_TOKENREPLACERSHADER_H

#include <string>
#include "UrchinCommon.h"

namespace urchin {

    /**
    * Replace tokens in shader source.
    * Token identified by #TOKENx# will be replaced by given value where 'x' is the token number.
    */
    class TokenReplacerShader {
        public:
            std::string replaceTokens(const std::string&, const std::map<std::string, std::string>&);
    };

}

#endif

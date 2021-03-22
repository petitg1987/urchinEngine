#ifndef URCHINENGINE_TOKENREPLACERSHADER_H
#define URCHINENGINE_TOKENREPLACERSHADER_H

#include <string>
#include "UrchinCommon.h"

namespace urchin {

    class TokenReplacerShader {
        public:
            static std::string replaceTokens(const std::string&, const std::map<std::string, std::string>&);
    };

}

#endif

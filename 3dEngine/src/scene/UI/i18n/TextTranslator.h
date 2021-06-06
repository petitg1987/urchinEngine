#pragma once

#include <string>

namespace urchin {

    class TextTranslator {
        public:
            TextTranslator();

            std::string translate(const std::string&, const std::string&);

        private:
            std::string textDirectoryName;
    };

}

#pragma once

#include <string>

namespace urchin {

    class TranslatableText {
        public:
            virtual const std::string& getTextKey() const = 0;
            virtual void updateText(const std::string&) = 0;
    };

}

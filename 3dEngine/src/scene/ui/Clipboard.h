#pragma once

#include <string>

namespace urchin{

    class Clipboard {
        public:
            virtual ~Clipboard() = default;

            virtual void setText(std::string) = 0;
            virtual const std::string& getText() const = 0;
    };
}

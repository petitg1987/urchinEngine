#pragma once

#include <string>

namespace urchin{

    class Clipboard {
        public:
            virtual ~Clipboard() = default;

            virtual void setText(const std::string&) = 0;
            virtual std::string getText() const = 0;
    };
}

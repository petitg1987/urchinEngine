#pragma once

#include <string>

namespace urchin {

    class TranslatableLabel {
        public:
            virtual ~TranslatableLabel() = default;

            virtual const std::string& getLabelKey() const = 0;
            virtual void updateLabel(const std::string&) = 0;
    };

}

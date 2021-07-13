#pragma once

#include <string>

namespace urchin {

    class UdaAttribute {
        public:
            UdaAttribute() = default;
            UdaAttribute(std::string, std::string);

            const std::string& getAttributeName() const;
            const std::string& getAttributeValue() const;

        private:
            std::string attributeName;
            std::string attributeValue;

    };

}

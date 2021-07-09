#pragma once

#include <string>

namespace urchin {

    class DataAttribute {
        public:
            DataAttribute() = default;
            DataAttribute(std::string , std::string );

            const std::string& getAttributeName() const;
            const std::string& getAttributeValue() const;

        private:
            std::string attributeName;
            std::string attributeValue;

    };

}

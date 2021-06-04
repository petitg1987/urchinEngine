#pragma once

#include <string>

namespace urchin {

    /**
     * Represents a XML attribute
     */
    class XmlAttribute {
        public:
            XmlAttribute() = default;
            XmlAttribute(std::string , std::string );

            const std::string& getAttributeName() const;
            const std::string& getAttributeValue() const;

        private:
            std::string attributeName;
            std::string attributeValue;

    };

}

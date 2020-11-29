#ifndef URCHINENGINE_XMLATTRIBUTE_H
#define URCHINENGINE_XMLATTRIBUTE_H

#include <string>

namespace urchin {

    /**
     * Represents a XML attribute
     */
    class XmlAttribute {
        public:
            XmlAttribute();
            XmlAttribute(std::string , std::string );

            const std::string& getAttributeName() const;
            const std::string& getAttributeValue() const;

        private:
            std::string attributeName;
            std::string attributeValue;

    };

}

#endif

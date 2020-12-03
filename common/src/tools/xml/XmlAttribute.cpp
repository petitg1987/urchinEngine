#include "tools/xml/XmlAttribute.h"

#include <utility>

namespace urchin {

    XmlAttribute::XmlAttribute(std::string attributeName, std::string attributeValue) :
        attributeName(std::move(attributeName)),
        attributeValue(std::move(attributeValue)) {

    }

    const std::string& XmlAttribute::getAttributeName() const {
        return attributeName;
    }

    const std::string& XmlAttribute::getAttributeValue() const {
        return attributeValue;
    }

}

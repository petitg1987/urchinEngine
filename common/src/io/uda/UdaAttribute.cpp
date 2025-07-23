#include <utility>

#include "io/uda/UdaAttribute.h"

namespace urchin {

    UdaAttribute::UdaAttribute(std::string attributeName, std::string attributeValue) :
            attributeName(std::move(attributeName)),
            attributeValue(std::move(attributeValue)) {

    }

    const std::string& UdaAttribute::getAttributeName() const {
        return attributeName;
    }

    const std::string& UdaAttribute::getAttributeValue() const {
        return attributeValue;
    }

}

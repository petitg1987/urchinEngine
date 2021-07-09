#include <io/data/DataAttribute.h>
#include <utility>

namespace urchin {

    DataAttribute::DataAttribute(std::string attributeName, std::string attributeValue) :
            attributeName(std::move(attributeName)),
            attributeValue(std::move(attributeValue)) {

    }

    const std::string& DataAttribute::getAttributeName() const {
        return attributeName;
    }

    const std::string& DataAttribute::getAttributeValue() const {
        return attributeValue;
    }

}

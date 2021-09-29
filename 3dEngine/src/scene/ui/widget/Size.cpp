#include <stdexcept>

#include <scene/ui/widget/Size.h>

namespace urchin {

    Size::Size(float width, LengthType widthType, float height, LengthType heightType) :
            width(width),
            widthType(widthType),
            height(height),
            heightType(heightType) {
        if (widthType == LengthType::RELATIVE_LENGTH && heightType == LengthType::RELATIVE_LENGTH) {
            throw std::invalid_argument("Both length can not be relative to each other");
        }
    }

    Size::Size(float width, float height, LengthType sizeType) :
            Size(width, sizeType, height, sizeType) {

    }

    float Size::getWidth() const {
        return width;
    }

    LengthType Size::getWidthType() const {
        return widthType;
    }

    float Size::getHeight() const {
        return height;
    }

    LengthType Size::getHeightType() const {
        return heightType;
    }

}

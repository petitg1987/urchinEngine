#include <stdexcept>

#include "scene/ui/widget/Size.h"

namespace urchin {

    Size::Size(float width, LengthType widthType, float height, LengthType heightType) :
            width(width),
            widthType(widthType),
            height(height),
            heightType(heightType) {
        if (widthType == RATIO_TO_WIDTH) {
            throw std::invalid_argument("Width length can not be relative to itself");
        } else if (heightType == RATIO_TO_HEIGHT) {
            throw std::invalid_argument("Height length can not be relative to itself");
        } else if (widthType == RATIO_TO_HEIGHT && heightType == RATIO_TO_WIDTH) {
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

#include <stdexcept>

#include <scene/ui/widget/Size.h>

namespace urchin {

    Size::Size(float width, LengthType widthSizeType, float height, LengthType heightSizeType) :
            width(Length(width, widthSizeType)),
            height(Length(height, heightSizeType)) {
        if (widthSizeType == LengthType::RELATIVE_LENGTH && heightSizeType == LengthType::RELATIVE_LENGTH) {
            throw std::invalid_argument("Both length can not be relative to each other");
        }
    }

    Size::Size(float width, float height, LengthType sizeType) :
        Size(width, sizeType, height, sizeType) {

    }

    float Size::getWidth() const {
        return width.getValue();
    }

    LengthType Size::getWidthSizeType() const {
        return width.getType();
    }

    float Size::getHeight() const {
        return height.getValue();
    }

    LengthType Size::getHeightSizeType() const {
        return height.getType();
    }

}

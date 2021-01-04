#include "scene/UI/widget/Size.h"

namespace urchin {

    Size::Size(float width, LengthType widthSizeType, float height, LengthType heightSizeType) :
            width(Length(width, widthSizeType)),
            height(Length(height, heightSizeType)) {

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

#include "scene/GUI/widget/Size.h"

namespace urchin {

    Size::Size(float width, SizeType widthSizeType, float height, SizeType heightSizeType) :
            width(width),
            height(height),
            widthSizeType(widthSizeType),
            heightSizeType(heightSizeType) {

    }

    Size::Size(float width, float height, SizeType sizeType) :
        Size(width, sizeType, height, sizeType) {

    }

    float Size::getWidth() const {
        return width;
    }

    Size::SizeType Size::getWidthSizeType() const {
        return widthSizeType;
    }

    float Size::getHeight() const {
        return height;
    }

    Size::SizeType Size::getHeightSizeType() const {
        return heightSizeType;
    }

}

#include "scene/ui/widget/staticbitmap/WidthSize.h"

namespace urchin {

    WidthSize::WidthSize(float width, LengthType widthType) :
            width(width),
            widthType(widthType) {

    }

    float WidthSize::getWidth() const {
        return width;
    }

    LengthType WidthSize::getWidthType() const {
        return widthType;
    }

}

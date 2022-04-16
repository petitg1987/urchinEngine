#include <scene/ui/widget/staticbitmap/HeightSize.h>

namespace urchin {

    HeightSize::HeightSize(float height, LengthType heightType) :
            height(height),
            heightType(heightType) {

    }

    float HeightSize::getHeight() const {
        return height;
    }

    LengthType HeightSize::getHeightType() const {
        return heightType;
    }

}

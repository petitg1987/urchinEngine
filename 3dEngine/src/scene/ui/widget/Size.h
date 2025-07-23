#pragma once

#include "scene/ui/widget/LengthType.h"

namespace urchin {

    class Size {
        public:
            Size(float, LengthType, float, LengthType);
            Size(float, float, LengthType);

            float getWidth() const;
            LengthType getWidthType() const;

            float getHeight() const;
            LengthType getHeightType() const;

        private:
            float width;
            LengthType widthType;

            float height;
            LengthType heightType;
    };

}

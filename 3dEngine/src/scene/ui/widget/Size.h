#pragma once

#include <scene/ui/widget/LengthType.h>
#include <scene/ui/widget/Length.h>

namespace urchin {

    class Size {
        public:
            Size(float, LengthType, float, LengthType);
            Size(float, float, LengthType);

            float getWidth() const;
            LengthType getWidthSizeType() const;

            float getHeight() const;
            LengthType getHeightSizeType() const;

        private:
            Length width;
            Length height;
    };

}

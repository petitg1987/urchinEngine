#pragma once

#include <scene/ui/widget/LengthType.h>

namespace urchin {

    class WidthSize {
        public:
            WidthSize(float, LengthType);

            float getWidth() const;
            LengthType getWidthType() const;

        private:
            float width;
            LengthType widthType;
    };

}
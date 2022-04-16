#pragma once

#include <scene/ui/widget/LengthType.h>

namespace urchin {

    class HeightSize {
        public:
            HeightSize(float, LengthType);

            float getHeight() const;
            LengthType getHeightType() const;

        private:
            float height;
            LengthType heightType;
    };

}
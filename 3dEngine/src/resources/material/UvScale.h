#pragma once

#include <resources/material/UvScaleType.h>

namespace urchin {

    class UvScale {
        public:
            UvScale(UvScaleType, UvScaleType);

            bool hasScaling() const;
            UvScaleType getUScaleType() const;
            UvScaleType getVScaleType() const;

            float scaleU(float, const Vector3<float>&) const;
            float scaleV(float, const Vector3<float>&) const;

        private:
            UvScaleType uScaleType;
            UvScaleType vScaleType;
    };

}

#pragma once

#include <resources/material/UvScaleType.h>

namespace urchin {

    class UvScale {
        public:
            explicit UvScale(UvScaleType);

            bool hasScaling() const;
            UvScaleType getScaleType() const;

            Point2<float> scaleUV(const Point2<float>&, const Vector3<float>&, const Vector3<float>&) const;

            bool operator ==(const UvScale&) const = default;
            std::partial_ordering operator <=>(const UvScale&) const;

        private:
            UvScaleType scaleType;
    };

}

#pragma once

#include <limits>
#include <UrchinCommon.h>

namespace urchin {

    class Fog {
        public:
            Fog(float, float, const Vector3<float>&, float maxHeight = std::numeric_limits<float>::max());

            float getDensity() const;
            float getGradient() const;
            const Vector3<float>& getColor() const;
            float getMaxHeight() const;

        private:
            float density;
            float gradient;
            Vector3<float> color;
            float maxHeight;
    };

}

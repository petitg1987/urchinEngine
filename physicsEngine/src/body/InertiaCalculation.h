#pragma once

#include <UrchinCommon.h>

#include "utils/math/PhysicsTransform.h"

namespace urchin {

    class InertiaCalculation {
        public:
            static Matrix3<float> computeInverseWorldInertia(const Vector3<float>&, const PhysicsTransform&);

        private:
            InertiaCalculation() = default;
            ~InertiaCalculation() = default;
    };

}

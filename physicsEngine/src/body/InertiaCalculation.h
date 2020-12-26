#ifndef URCHINENGINE_INERTIACALCULATION_H
#define URCHINENGINE_INERTIACALCULATION_H

#include "UrchinCommon.h"

#include "utils/math/PhysicsTransform.h"

namespace urchin {

    class InertiaCalculation {
        public:
            Matrix3<float> computeInverseWorldInertia(const Vector3<float>&, const PhysicsTransform&);
    };

}

#endif

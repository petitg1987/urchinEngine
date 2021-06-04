#pragma once

#include <UrchinCommon.h>

#include <collision/narrowphase/algorithm/util/AlgorithmResult.h>

namespace urchin {

    template<class T> class EPAResult : public AlgorithmResult {
        public:
            virtual ~EPAResult() = default;

            virtual bool isValidResult() const = 0;

            virtual bool isCollide() const = 0;
            virtual const Point3<T>& getContactPointA() const = 0;
            virtual const Point3<T>& getContactPointB() const = 0;
            virtual const Vector3<T>& getNormal() const = 0;
            virtual T getPenetrationDepth() const = 0;
    };

}

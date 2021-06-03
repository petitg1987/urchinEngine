#ifndef URCHINENGINE_EPARESULTNOCOLLIDE_H
#define URCHINENGINE_EPARESULTNOCOLLIDE_H

#include <stdexcept>
#include <UrchinCommon.h>

#include <collision/narrowphase/algorithm/epa/result/EPAResult.h>

namespace urchin {

    template<class T> class EPAResultNoCollide : public EPAResult<T> {
        public:
            bool isValidResult() const override;

            bool isCollide() const override;
            const Point3<T>& getContactPointA() const override;
            const Point3<T>& getContactPointB() const override;
            const Vector3<T>& getNormal() const override;
            T getPenetrationDepth() const override;
    };

}

#endif

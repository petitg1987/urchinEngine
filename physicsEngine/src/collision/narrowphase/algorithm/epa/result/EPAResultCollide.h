#pragma once

#include <UrchinCommon.h>

#include <collision/narrowphase/algorithm/epa/result/EPAResult.h>

namespace urchin {

    template<class T> class EPAResultCollide : public EPAResult<T> {
        public:
            EPAResultCollide(const Point3<T>&, const Point3<T>&, const Vector3<T>&, T);

            bool isValidResult() const override;

            bool isCollide() const override;
            const Point3<T>& getContactPointA() const override;
            const Point3<T>& getContactPointB() const override;
            const Vector3<T>& getNormal() const override;
            T getPenetrationDepth() const override;

        private:
            Point3<T> contactPointA, contactPointB;
            Vector3<T> normal;
            T depth;
    };

}

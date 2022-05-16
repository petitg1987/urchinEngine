#pragma once

#include <UrchinCommon.h>

namespace urchin {

    template<class T> class EPAResult {
        public:
            static EPAResult<T> newInvalidResult();
            static EPAResult<T> newCollideResult(const Point3<T>&, const Point3<T>&, const Vector3<T>&, T);
            static EPAResult<T> newNoCollideResult();

            bool isValidResult() const;

            bool isCollide() const;
            const Point3<T>& getContactPointA() const;
            const Point3<T>& getContactPointB() const;
            const Vector3<T>& getNormal() const;
            T getPenetrationDepth() const;

        private:
            EPAResult(bool, const Point3<T>&, const Point3<T>&, const Vector3<T>&, T);

            bool validResult;

            Point3<T> contactPointA;
            Point3<T> contactPointB;
            Vector3<T> normal;
            T depth;
    };

}

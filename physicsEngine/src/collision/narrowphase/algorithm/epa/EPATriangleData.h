#pragma once

#include <UrchinCommon.h>

namespace urchin {

    template<class T> class EPATriangleData {
        public:
            EPATriangleData(T, const Vector3<T>&, const Point3<T>&, const std::array<T, 3>&);

            T getDistanceToOrigin() const;
            const Vector3<T>& getNormal() const;
            const Point3<T>& getClosestPointToOrigin() const;
            T getBarycentric(unsigned int index) const;

        private:
            T distanceToOrigin; //minimum distance between the triangle and the origin
            Vector3<T> normal; //normal of triangle
            Point3<T> closestPointToOrigin; //point on the triangle nearest to origin
            std::array<T, 3> barycentrics; //barycentrics of closest point to origin

    };

}

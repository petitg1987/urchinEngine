#pragma once

#include <math/algebra/point/Point3.h>

namespace urchin {

    template<class T> class Line3D {
        public:
            Line3D(const Point3<T>&, const Point3<T>&);

            const Point3<T>& getA() const;
            const Point3<T>& getB() const;

            Point3<T> orthogonalProjection(const Point3<T>&) const;
            T squareDistance(const Point3<T>&) const;
            Line3D<T> parallelLine(const Point3<T>&) const;

            T minDistance(const Line3D<T>&) const;

        private:
            Point3<T> a, b;
    };

    template<class T> std::ostream& operator <<(std::ostream&, const Line3D<T>&);

}

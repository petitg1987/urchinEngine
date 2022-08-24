#pragma once

#include <limits>

#include <math/geometry/3d/object/ConvexObject3D.h>
#include <math/algebra/point/Point3.h>
#include <math/algebra/vector/Vector3.h>

namespace urchin {

    /**
    * Represents a tetrahedron
    */
    template<class T> class Tetrahedron final : public ConvexObject3D<T> {
        public:
            Tetrahedron(const Point3<T>&, const Point3<T>&, const Point3<T>&, const Point3<T>&);

            const Point3<T>& getPointA() const;
            const Point3<T>& getPointB() const;
            const Point3<T>& getPointC() const;
            const Point3<T>& getPointD() const;

            Point3<T> getSupportPoint(const Vector3<T>&) const override;
            Point3<T> closestPoint(const Point3<T>&, std::array<T, 4>&, unsigned short voronoiRegionMask = 15u) const;

            bool collideWithPoint(const Point3<T>&) const override;

        private:
            bool pointOutsidePlane(const Point3<T>&, const Point3<T>&, const Point3<T>&, const Point3<T>&, const Point3<T>&, bool) const;

            Point3<T> a;
            Point3<T> b;
            Point3<T> c;
            Point3<T> d;
    };

    template<class T> std::ostream& operator <<(std::ostream&, const Tetrahedron<T>&);

}

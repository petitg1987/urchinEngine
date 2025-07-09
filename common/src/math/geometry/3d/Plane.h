#pragma once

#include <math/geometry/3d/Line3D.h>
#include <math/geometry/3d/object/LineSegment3D.h>
#include <math/algebra/point/Point3.h>
#include <math/algebra/point/Point4.h>
#include <math/algebra/vector/Vector3.h>

namespace urchin {

    template<class T> class Plane {
        public:
            Plane();
            Plane(const Vector3<T>&, T);
            Plane(const Point3<T>&, const Point3<T>&, const Point3<T>&);
            Plane(const Vector3<T>&, const Point3<T>&);

            void buildFrom3Points(const Point3<T>&, const Point3<T>&, const Point3<T>&);
            void buildFromNormalAndPoint(const Vector3<T>&, const Point3<T>&);
            void reverse();

            const Vector3<T>& getNormal() const;
            void setNormal(const Vector3<T>&);
            T getDistanceToOrigin() const;
            void setDistanceToOrigin(T);

            T distance(const Point3<T>&) const;
            T distance(const Point4<T>&) const;
            T verticalDistance(const Point3<T>&) const;
            T horizontalDistance(const Point3<T>&) const;
            Point3<T> orthogonalProjection(const Point3<T>&) const;

            Point3<T> intersectPoint(const Line3D<T>&, bool&) const;
            Point3<T> intersectPoint(const LineSegment3D<T>&, bool&) const;

        private:
            T intersectDistance(const Point3<T>&, const Vector3<T>&, bool&) const;

            Vector3<T> normal; //plane normal normalized
            T d; //Distance to the origin. Positive if dot product between a vector from plane to origin and the normal is positive
    };

    template<class T> std::ostream& operator <<(std::ostream&, const Plane<T>&);

}

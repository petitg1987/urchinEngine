#pragma once

#include <sstream>
#include <vector>

#include "math/geometry/3d/Line3D.h"
#include "math/geometry/3d/object/ConvexObject3D.h"
#include "math/geometry/3d/object/AABBox.h"
#include "math/geometry/3d/object/Sphere.h"
#include "math/algebra/matrix/Matrix4.h"
#include "math/algebra/point/Point3.h"
#include "math/algebra/vector/Vector3.h"
#include "math/algebra/Quaternion.h"

namespace urchin {

    /**
    * Represents an oriented bounding box in space
    */
    template<class T> class OBBox final : public ConvexObject3D<T> {
        public:
            OBBox();
            explicit OBBox(const Vector3<T>&, const Point3<T>&, const Quaternion<T>&);
            explicit OBBox(const AABBox<T>&);
            explicit OBBox(const Sphere<T>&);

            T getHalfSize(unsigned int) const;
            const Vector3<T>& getHalfSizes() const;
            const Point3<T>& getCenterOfMass() const;
            const Vector3<T>& getAxis(unsigned int) const;
            const Vector3<T>& getNormalizedAxis(unsigned int) const;
            const Quaternion<T>& getOrientation() const;
            std::vector<Point3<T>> getPoints() const;
            Point3<T> getPoint(unsigned int) const;
            Vector3<T> getVectorPoint(unsigned int) const;

            Point3<T> getSupportPoint(const Vector3<T>&) const override;
            AABBox<T> toAABBox() const;

            bool collideWithOBBox(const OBBox<T>&) const override;
            bool collideWithAABBox(const AABBox<T>&) const override;
            Point3<T> intersectPoint(const Line3D<T>&, bool&) const override;

        private:
            template<class U> bool separatedAxisTheoremCollision(const U&) const;
            bool pointInsideSquare(const Point3<T>&, const std::array<Point3<T>, 4>&, const Vector3<T>&) const;

            Vector3<T> halfSizes;
            Point3<T> centerOfMass;
            Quaternion<T> orientation;

            std::array<Vector3<T>, 3> axis;
            std::array<Vector3<T>, 3> normalizedAxis;
    };

    template<class T> OBBox<T> operator *(const Matrix4<T>&, const OBBox<T>&);
    template<class T> OBBox<T> operator *(const OBBox<T>&, const Matrix4<T>&);

    template<class T> std::ostream& operator <<(std::ostream&, const OBBox<T>&);

    #include "OBBox.inl"

}

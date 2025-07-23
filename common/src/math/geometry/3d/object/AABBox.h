#pragma once

#include <vector>
#include <span>

#include "math/geometry/3d/object/ConvexObject3D.h"
#include "math/geometry/3d/shape/BoxShape.h"
#include "math/geometry/3d/Ray.h"
#include "math/algebra/matrix/Matrix4.h"
#include "math/algebra/point/Point3.h"
#include "math/algebra/vector/Vector3.h"
#include "math/algebra/Transform.h"

namespace urchin {

    /**
    * Represents an axis aligned bounding box
    */
    template<class T> class AABBox final : public ConvexObject3D<T> {
        public:
            AABBox();
            AABBox(const Point3<T>&, const Point3<T>&);
            AABBox(const Point3<T>&, const Vector3<T>&);
            explicit AABBox(const std::vector<Point3<T>>&);
            explicit AABBox(std::span<Point3<T>>);

            T getHalfSize(unsigned int) const;
            const Vector3<T>& getHalfSizes() const;
            T getMaxHalfSize() const;
            unsigned int getMaxHalfSizeIndex() const;
            T getMinHalfSize() const;
            unsigned int getMinHalfSizeIndex() const;
            const Point3<T>& getMin() const;
            const Point3<T>& getMax() const;
            Point3<T> getCenterOfMass() const;
            const Vector3<T>& getNormalizedAxis(unsigned int) const;

            Point3<T> getSupportPoint(const Vector3<T>&) const override;
            std::array<Point3<T>, 8> getPoints() const;
            T getVolume() const;

            [[nodiscard]] AABBox<T> moveAABBox(const Transform<T>&) const;
            Matrix4<T> toProjectionMatrix() const;
            bool include(const AABBox<T>&) const;
            static AABBox<T> initMergeableAABBox();
            [[nodiscard]] AABBox<T> merge(const AABBox<T>&) const;
            [[nodiscard]] AABBox<T> cutTo(const AABBox<T>&) const;
            [[nodiscard]] AABBox<T> enlarge(const Vector3<T>&, const Vector3<T>&) const;
            [[nodiscard]] AABBox<T> enlarge(T, T) const;

            bool collideWithPoint(const Point3<T>&) const override;
            bool collideWithAABBox(const AABBox<T>&) const override;
            bool collideWithRay(const Ray<T>&) const override;

            const Point3<T>& operator [](std::size_t) const;
            Point3<T>& operator [](std::size_t);

        private:
            BoxShape<T> boxShape;
            Point3<T> min;
            Point3<T> max;
    };

    template<class T> AABBox<T> operator *(const Matrix4<T>&, const AABBox<T>&);
    template<class T> AABBox<T> operator *(const AABBox<T>&, const Matrix4<T>&);

    template<class T> std::ostream& operator <<(std::ostream&, const AABBox<T>&);

}

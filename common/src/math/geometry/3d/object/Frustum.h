#pragma once

#include <math/geometry/3d/object/ConvexObject3D.h>
#include <math/geometry/3d/object/Sphere.h>
#include <math/geometry/3d/Plane.h>
#include <math/algebra/matrix/Matrix4.h>
#include <math/algebra/vector/Vector3.h>
#include <math/algebra/point/Point3.h>

namespace urchin {

    /**
    * Represents a frustum (truncated pyramid) in space
    */
    template<class T> class Frustum final : public ConvexObject3D<T> {
        public:
            Frustum();
            Frustum(T, T, T, T);
            Frustum(const Point3<T>&, const Point3<T>&, const Point3<T>&, const Point3<T>&, const Point3<T>&, const Point3<T>&, const Point3<T>&, const Point3<T>&);

            enum FrustumPoint {
                NTL = 0, //Near top left
                NTR, //Near top right
                NBL, //Near bottom left
                NBR, //Near bottom right
                FTL, //Far top left
                FTR, //Far top right
                FBL, //Far bottom left
                FBR //Far bottom right
            };

            void buildFrustum(T, T, T, T);
            void buildFrustum(const Point3<T>&, const Point3<T>&, const Point3<T>&, const Point3<T>&, const Point3<T>&, const Point3<T>&, const Point3<T>&, const Point3<T>&);

            const std::array<Point3<T>, 8>& getFrustumPoints() const;
            const Point3<T>& getFrustumPoint(FrustumPoint frustumPoint) const;
            const Point3<T>& getEyePosition() const;
            Point3<T> computeCenterPosition() const;
            Sphere<T> computeBoundingSphere() const;

            Point3<T> getSupportPoint(const Vector3<T>&) const override;
            T computeNearDistance() const;
            T computeFarDistance() const;
            Frustum<T> splitFrustum(T, T) const;
            Frustum<T> cutFrustum(T) const;

            bool collideWithPoint(const Point3<T>&) const override;
            bool collideWithAABBox(const AABBox<T>&) const override;
            bool collideWithSphere(const Sphere<T>&) const override;
            void planesIntersectPoints(const Line3D<T>&, Point3<T>&, Point3<T>&, bool&) const;

        private:
            void buildData();

            std::array<Point3<T>, 8> frustumPoints;
            Point3<T> eyePosition;
            std::array<Plane<T>, 6> planes;
            enum {
                TOP = 0,
                BOTTOM,
                LEFT,
                RIGHT,
                NEARP,
                FARP
            };
    };

    template<class T> Frustum<T> operator *(const Matrix4<T>&, const Frustum<T>&);
    template<class T> Frustum<T> operator *(const Frustum<T>&, const Matrix4<T>&);

    template<class T> std::ostream& operator <<(std::ostream&, const Frustum<T>&);

}

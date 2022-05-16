#pragma once

#include <vector>
#include <map>
#include <sstream>

#include <math/geometry/3d/object/ConvexObject3D.h>
#include <math/geometry/3d/shape/ConvexHullShape3D.h>
#include <math/geometry/3d/IndexedTriangle3D.h>
#include <math/algebra/point/Point3.h>

namespace urchin {

    template<class T> class ConvexHull3D : public ConvexObject3D<T> {
        public:
            template<class U> friend class ResizeConvexHull3DService;
            template<class V> friend std::ostream& operator<< (std::ostream&, const ConvexHull3D<V>&);

            explicit ConvexHull3D(const std::vector<Point3<T>>&);
            explicit ConvexHull3D(const ConvexHullShape3D<T>&);

            const typename std::map<std::size_t, ConvexHullPoint<T>>& getConvexHullPoints() const;
            std::vector<Point3<T>> getPoints() const;
            const typename std::map<std::size_t, IndexedTriangle3D<T>>& getIndexedTriangles() const;

            std::size_t addNewPoint(const Point3<T>&);
            std::size_t addNewPoint(const Point3<T>&, std::vector<std::size_t>&);

            Point3<T> getSupportPoint(const Vector3<T>&) const override;

            std::unique_ptr<ConvexHull3D<T>> resize(T) const;

            bool collideWithAABBox(const AABBox<T>&) const override;
            Point3<T> intersectPoint(const Line3D<T>&, bool&) const override;

        private:
            ConvexHullShape3D<T> localizedConvexHullShape;
    };

    template<class T> std::ostream& operator <<(std::ostream&, const ConvexHull3D<T>&);

}

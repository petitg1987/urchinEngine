#include <math/geometry/3d/object/AABBox.h>
#include <math/geometry/3d/object/ConvexHull3D.h>
#include <math/geometry/3d/object/Triangle3D.h>
#include <math/geometry/3d/util/ResizeConvexHull3DService.h>
#include <collision/GJKAlgorithm.h>

namespace urchin {

    /**
     * @param points Points used to construct the convex hull. Points inside the convex hull are accepted but will unused.
     */
    template<class T> ConvexHull3D<T>::ConvexHull3D(const std::vector<Point3<T>>& points) :
            ConvexObject3D<T>(ConvexObjectType::CONVEX_HULL),
            localizedConvexHullShape(ConvexHullShape3D<T>(points)) {

    }

    template<class T> ConvexHull3D<T>::ConvexHull3D(const ConvexHullShape3D<T>& localizedConvexHullShape) :
            ConvexObject3D<T>(ConvexObjectType::CONVEX_HULL),
            localizedConvexHullShape(localizedConvexHullShape) {

    }

    /**
     * Points of convex hull indexed to be used with indexed triangles.
     */
    template<class T> const std::map<std::size_t, ConvexHullPoint<T>> &ConvexHull3D<T>::getConvexHullPoints() const {
        return localizedConvexHullShape.getConvexHullPoints();
    }

    /**
     * Points of convex hull. Order of points is undetermined.
     */
    template<class T> std::vector<Point3<T>> ConvexHull3D<T>::getPoints() const {
        return localizedConvexHullShape.getPoints();
    }

    /**
     * Triangles of convex hull where points are sorted in counterclockwise direction in a right hand coordinate system (Z+ directed to the observer).
     */
    template<class T> const std::map<std::size_t, IndexedTriangle3D<T>> &ConvexHull3D<T>::getIndexedTriangles() const {
        return localizedConvexHullShape.getIndexedTriangles();
    }

    /**
     * @return Returns index of point added. If point doesn't make part of convex, result is zero.
     */
    template<class T> std::size_t ConvexHull3D<T>::addNewPoint(const Point3<T>& newPoint) {
        return localizedConvexHullShape.addNewPoint(newPoint);
    }

    /**
    * @param removedTriangleIndices [out] Indices of removed triangles from convex hull
    * @return Returns index of point added. If point doesn't make part of convex, result is zero.
    */
    template<class T> std::size_t ConvexHull3D<T>::addNewPoint(const Point3<T>& newPoint, std::vector<std::size_t>& removedTriangleIndices) {
        return localizedConvexHullShape.addNewPoint(newPoint, removedTriangleIndices);
    }

    template<class T> Point3<T> ConvexHull3D<T>::getSupportPoint(const Vector3<T>& direction) const {
        return localizedConvexHullShape.getSupportPoint(direction);
    }

    template<class T> std::unique_ptr<ConvexHull3D<T>> ConvexHull3D<T>::resize(T distance) const {
        return ResizeConvexHull3DService<T>::resizeConvexHull(*this, distance);
    }

    template<class T> bool ConvexHull3D<T>::collideWithAABBox(const AABBox<T>& bbox) const {
        GJKResult<T> gjkResult = GJKAlgorithm<T>().processGJK(*this, bbox);
        return gjkResult.isValidResult() && gjkResult.isCollide();
    }

    template<class T> Point3<T> ConvexHull3D<T>::intersectPoint(const Line3D<T>& line, bool& hasIntersection) const {
        Point3<T> bestIntersectionPoint(0.0f, 0.0f, 0.0f);
        hasIntersection = false;

        for (const auto& [triangleIndex, indexedTriangle] : getIndexedTriangles()) {
            Point3<T> p1 = getPoints()[indexedTriangle.getIndex(0)];
            Point3<T> p2 = getPoints()[indexedTriangle.getIndex(1)];
            Point3<T> p3 = getPoints()[indexedTriangle.getIndex(2)];

            bool hasPlaneInteraction;
            Point3<T> intersectionPoint = Plane<T>(p1, p2, p3).intersectPoint(line, hasPlaneInteraction);
            if (hasPlaneInteraction) {
                bool hasTriangleIntersection = Triangle3D<T>(p1, p2, p3).projectedPointInsideTriangle(intersectionPoint);
                if (hasTriangleIntersection && (!hasIntersection || line.getA().squareDistance(intersectionPoint) < line.getA().squareDistance(bestIntersectionPoint))) {
                    bestIntersectionPoint = intersectionPoint;
                    hasIntersection = true;
                }
            }
        }

        return bestIntersectionPoint;
    }

    template<class T> std::ostream& operator <<(std::ostream& stream, const ConvexHull3D<T>& ch) {
        stream << ch.localizedConvexHullShape;
        return stream;
    }

    //explicit template
    template class ConvexHull3D<float>;
    template std::ostream& operator <<<float>(std::ostream&, const ConvexHull3D<float>&);

    template class ConvexHull3D<double>;
    template std::ostream& operator <<<double>(std::ostream&, const ConvexHull3D<double>&);

}

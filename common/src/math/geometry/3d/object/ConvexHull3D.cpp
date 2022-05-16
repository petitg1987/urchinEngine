#include <math/geometry/3d/object/ConvexHull3D.h>
#include <math/geometry/3d/util/ResizeConvexHull3DService.h>

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
    template<class T> const typename std::map<std::size_t, ConvexHullPoint<T>> &ConvexHull3D<T>::getConvexHullPoints() const {
        return localizedConvexHullShape.getConvexHullPoints();
    }

    /**
     * Points of convex hull. Order of points is undetermined.
     */
    template<class T> std::vector<Point3<T>> ConvexHull3D<T>::getPoints() const {
        return localizedConvexHullShape.getPoints();
    }

    /**
     * Triangles of convex hull where points are sorted in counter clockwise direction in a right hand coordinate system (Z+ directed to the observer).
     */
    template<class T> const typename std::map<std::size_t, IndexedTriangle3D<T>> &ConvexHull3D<T>::getIndexedTriangles() const {
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

    template<class T> bool ConvexHull3D<T>::collideWithAABBox(const AABBox<T>& /*bbox*/) const {
        //TODO use GJK ? axis theorem ?
        return false;
    }

    template<class T> Point3<T> ConvexHull3D<T>::intersectPoint(const Line3D<T>& /*line*/, bool& /*hasIntersection*/) const {
        //TODO ...
        return Point3<T>();
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

#include <limits>

#include "math/geometry/3d/Rectangle3D.h"

namespace urchin {

    template<class T> Rectangle3D<T>::Rectangle3D(const std::array<Point3<T>, 4>& orientedPoints) :
            orientedPoints(orientedPoints) {
        Vector3<T> aux = orientedPoints[1].vector(orientedPoints[2]);
        normal = aux.crossProduct(orientedPoints[1].vector(orientedPoints[0])).normalize();
    }

    template<class T> Point3<T> Rectangle3D<T>::intersectPoint(const Line3D<T>& line, bool& hasIntersection, bool& hasPlaneIntersection) const {
        Point3<T> intersectionPoint = planeIntersectionPoint(line, hasPlaneIntersection);
        if (hasPlaneIntersection && pointInsideSquare(intersectionPoint)) {
            hasIntersection = true;
        } else {
            hasIntersection = false;
        }
        return intersectionPoint;
    }

    template<class T> Point3<T> Rectangle3D<T>::planeIntersectionPoint(const Line3D<T>& line, bool& hasIntersection) const {
        //similar to Plane#intersectPoint()
        Vector3<T> lineVector = line.getA().vector(line.getB());
        T denominator = normal.dotProduct(lineVector);

        if (denominator == 0.0) [[unlikely]] {
            hasIntersection = false;
            return Point3<T>(std::numeric_limits<T>::max(), std::numeric_limits<T>::max(), std::numeric_limits<T>::max());
        }

        Vector3<T> lineAToPlanePoint = line.getA().vector(orientedPoints[0]);
        T t = normal.dotProduct(lineAToPlanePoint) / denominator;
        hasIntersection = true;
        return line.getA().translate(t * lineVector);
    }

    template<class T> bool Rectangle3D<T>::pointInsideSquare(const Point3<T>& testPoint) const {
        T orient1 = testPoint.vector(orientedPoints[0]).crossProduct(testPoint.vector(orientedPoints[1])).dotProduct(normal);
        T orient2 = testPoint.vector(orientedPoints[1]).crossProduct(testPoint.vector(orientedPoints[2])).dotProduct(normal);
        T orient3 = testPoint.vector(orientedPoints[2]).crossProduct(testPoint.vector(orientedPoints[3])).dotProduct(normal);
        T orient4 = testPoint.vector(orientedPoints[3]).crossProduct(testPoint.vector(orientedPoints[0])).dotProduct(normal);
        return (orient1 < 0.0 && orient2 < 0.0 && orient3 < 0.0 && orient4 < 0.0) || (orient1 > 0.0 && orient2 > 0.0 && orient3 > 0.0 && orient4 > 0.0);
    }

    //explicit template
    template class Rectangle3D<float>;

    template class Rectangle3D<double>;
}

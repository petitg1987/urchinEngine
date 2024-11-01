#include <sstream>
#include <cassert>
#include <limits>

#include <math/geometry/3d/Plane.h>

namespace urchin {

    template<class T> Plane<T>::Plane() :
            normal(Vector3<T>(0.0, 1.0, 0.0)),
            d(0.0) {

    }

    /**
     * @param normalizedNormal Plane normal normalized
     * @param distanceToOrigin Distance to the origin. Positive if dot product between a vector from plane to origin and the normal is positive.
     */
    template<class T> Plane<T>::Plane(const Vector3<T>& normalizedNormal, T distanceToOrigin) :
            normal(normalizedNormal),
            d(distanceToOrigin) {
        #ifdef URCHIN_DEBUG
            T normalSquareLength = normal.squareLength();
            assert(normalSquareLength > (T)0.9996 && normalSquareLength < (T)1.0004);
        #endif
    }

    template<class T> Plane<T>::Plane(const Point3<T>& p1, const Point3<T>& p2, const Point3<T>& p3) {
        buildFrom3Points(p1, p2, p3);
    }

    template<class T> Plane<T>::Plane(const Vector3<T>& normalizedNormal, const Point3<T>& point) {
        buildFromNormalAndPoint(normalizedNormal, point);
    }

    template<class T> void Plane<T>::buildFrom3Points(const Point3<T>& p1, const Point3<T>& p2, const Point3<T>& p3) {
        const Vector3<T>& aux = p2.vector(p3);
        normal = aux.crossProduct(p2.vector(p1)).normalize();
        d = normal.dotProduct(-Vector3<T>(p2.X, p2.Y, p2.Z));
    }

    template<class T> void Plane<T>::buildFromNormalAndPoint(const Vector3<T>& normalizedNormal, const Point3<T>& point) {
        #ifdef URCHIN_DEBUG
            T normalSquareLength = normal.squareLength();
            assert(normalSquareLength > (T)0.9996 || normalSquareLength < (T)1.0004);
        #endif

        this->normal = normalizedNormal;
        this->d = this->normal.dotProduct(-Vector3<T>(point.X, point.Y, point.Z));
    }

    template<class T> void Plane<T>::reverse() {
        normal = -normal;
        d = -d;
    }

    template<class T> const Vector3<T> &Plane<T>::getNormal() const {
        return normal;
    }

    template<class T> void Plane<T>::setNormal(const Vector3<T>& normal) {
        this->normal = normal;
    }

    template<class T> T Plane<T>::getDistanceToOrigin() const {
        return d;
    }

    template<class T> void Plane<T>::setDistanceToOrigin(T d) {
        this->d = d;
    }

    /**
     * @return Distance between the plane and the point 3D. If positive result: the point is on the same side as the normal.
     */
    template<class T> T Plane<T>::distance(const Point3<T>& p) const { //see http://fr.wikipedia.org/wiki/Distance_d%27un_point_%C3%A0_un_plan

        //division by sqrt(a * a + b * b + c * c) is useless because normal is normalized and so result is 1.0.
        return normal.X * p.X + normal.Y * p.Y + normal.Z * p.Z + d;
    }

    template<class T> T Plane<T>::distance(const Point4<T>& p) const {
        return normal.X * p.X + normal.Y * p.Y + normal.Z * p.Z + d * p.W;
    }

    /**
     * @return Vertical distance between the plane and the point 3D. If negative result: the point is on the same side as the normal.
     */
    template<class T> T Plane<T>::verticalDistance(const Point3<T>& p) const {
        Point3<T> planePoint = Point3<T>(normal * -d);
        return normal.dotProduct(p.vector(planePoint)) / normal.Y;
    }

    /**
      * @return Horizontal distance between the plane and the point 3D. If negative result: the point is on the same side as the normal.
      */
    template<class T> T Plane<T>::horizontalDistance(const Point3<T>& p) const {
        Point3<T> planePoint = Point3<T>(normal * -d);
        return normal.dotProduct(p.vector(planePoint)) / normal.X;
    }

    template<class T> Point3<T> Plane<T>::orthogonalProjection(const Point3<T>& p) const {
        Point3<T> planePoint = Point3<T>(normal * -d);
        Vector3<T> planePointToP = planePoint.vector(p);
        T distanceFromPlane = planePointToP.dotProduct(normal);
        return p.translate(-distanceFromPlane * normal);
    }

    template<class T> Point3<T> Plane<T>::intersectPoint(const Line3D<T>& line, bool& hasIntersection) const {
        Vector3<T> lineVector = line.getA().vector(line.getB());
        T denominator = normal.dotProduct(lineVector);

        if (denominator == 0.0) [[unlikely]] {
            hasIntersection = false;
            return Point3<T>(std::numeric_limits<T>::max(), std::numeric_limits<T>::max(), std::numeric_limits<T>::max());
        }

        Point3<T> planePoint(normal * -d);
        Vector3<T> lineAToPlanePoint = line.getA().vector(planePoint);
        T t = normal.dotProduct(lineAToPlanePoint) / denominator;
        hasIntersection = true;
        return line.getA().translate(t * lineVector);
    }

    template<class T> std::ostream& operator <<(std::ostream& stream, const Plane<T>& p) {
        return stream << "Plane normal: " << p.getNormal() << " | Distance to origin: " << p.getDistanceToOrigin();
    }

    //explicit template
    template class Plane<float>;
    template std::ostream& operator <<<float>(std::ostream&, const Plane<float>&);

    template class Plane<double>;
    template std::ostream& operator <<<double>(std::ostream&, const Plane<double>&);

}

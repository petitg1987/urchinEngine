#include <iomanip>

#include <math/geometry/3d/object/Triangle3D.h>
#include <math/geometry/3d/Ray.h>

namespace urchin {

    template<class T> Triangle3D<T>::Triangle3D(const std::array<Point3<T>, 3>& points) :
            ConvexObject3D<T>(ConvexObjectType::TRIANGLE),
            triangleShape(TriangleShape3D<T>(points)) {

    }

    template<class T> Triangle3D<T>::Triangle3D(const Point3<T>& point1, const Point3<T>& point2, const Point3<T>& point3) :
            ConvexObject3D<T>(ConvexObjectType::TRIANGLE),
            triangleShape(TriangleShape3D<T>(point1, point2, point3)) {

    }

    template<class T> const std::array<Point3<T>, 3>& Triangle3D<T>::getPoints() const {
        return triangleShape.getPoints();
    }

    template<class T> Point3<T> Triangle3D<T>::getSupportPoint(const Vector3<T>& direction) const {
        T maxPointDotDirection = getPoints()[0].toVector().dotProduct(direction);
        Point3<T> maxPoint = getPoints()[0];

        for (unsigned int i = 1; i < 3; ++i) {
            T currentPointDotDirection = getPoints()[i].toVector().dotProduct(direction);
            if (currentPointDotDirection > maxPointDotDirection) {
                maxPointDotDirection = currentPointDotDirection;
                maxPoint = getPoints()[i];
            }
        }

        return maxPoint;
    }

    template<class T> Vector3<T> Triangle3D<T>::computeNormal() const {
        Vector3<T> aux = this->getPoints()[1].vector(this->getPoints()[2]);
        Vector3<T> normal = aux.crossProduct(this->getPoints()[1].vector(this->getPoints()[0]));

        return normal.normalize();
    }

    /**
     * @param barycentrics [out] Returns barycentric coordinates for closest point
     * @return Point on triangle closest to point p
     */
    template<class T> Point3<T> Triangle3D<T>::closestPoint(const Point3<T>& p, std::array<T, 3>& barycentrics) const {
        const Point3<T>& a = this->getPoints()[0];
        const Point3<T>& b = this->getPoints()[1];
        const Point3<T>& c = this->getPoints()[2];

        //check if P is in voronoi region of point A
        Vector3<T> ab = a.vector(b);
        Vector3<T> ac = a.vector(c);
        Vector3<T> ap = a.vector(p);
        T abDotAp = ab.dotProduct(ap);
        T acDotAp = ac.dotProduct(ap);
        if (abDotAp <= (T)0.0 && acDotAp <= (T)0.0) {
            barycentrics[0] = 1.0;
            barycentrics[1] = 0.0;
            barycentrics[2] = 0.0;
            return a;
        }

        //check if P is in voronoi region of point B
        Vector3<T> bp = b.vector(p);
        T abDotBp = ab.dotProduct(bp);
        T acDotBp = ac.dotProduct(bp);
        if (abDotBp >= (T)0.0 && acDotBp <= abDotBp) {
            barycentrics[0] = 0.0;
            barycentrics[1] = 1.0;
            barycentrics[2] = 0.0;
            return b;
        }

        //check if P is in voronoi region of edge AB
        T vc = abDotAp * acDotBp - abDotBp * acDotAp;
        if (vc <= (T)0.0 && abDotAp >= (T)0.0 && abDotBp <= (T)0.0) {
            T v = abDotAp / (abDotAp - abDotBp);

            barycentrics[0] = (T)1.0 - v;
            barycentrics[1] = v;
            barycentrics[2] = 0.0;
            return a.translate(v * ab);
        }

        //check if P is in voronoi region of point C
        Vector3<T> cp = c.vector(p);
        T abDotCp = ab.dotProduct(cp);
        T acDotCp = ac.dotProduct(cp);
        if (acDotCp >= (T)0.0 && abDotCp <= acDotCp) {
            barycentrics[0] = 0.0;
            barycentrics[1] = 0.0;
            barycentrics[2] = 1.0;
            return c;
        }

        //check if P is in voronoi region of edge AC
        T vb = abDotCp * acDotAp - abDotAp * acDotCp;
        if (vb <= (T)0.0 && acDotAp >= (T)0.0 && acDotCp <= (T)0.0) {
            T w = acDotAp / (acDotAp - acDotCp);

            barycentrics[0] = (T)1.0 - w;
            barycentrics[1] = 0.0;
            barycentrics[2] = w;
            return a.translate(w * ac);
        }

        //check if P is in voronoi region of BC
        T va = abDotBp * acDotCp - abDotCp * acDotBp;
        if (va <= (T)0.0 && (acDotBp - abDotBp) >= (T)0.0 && (abDotCp - acDotCp) >= (T)0.0) {
            T w = (acDotBp - abDotBp) / ((acDotBp - abDotBp) + (abDotCp - acDotCp));

            barycentrics[0] = 0.0;
            barycentrics[1] = (T)1.0 - w;
            barycentrics[2] = w;
            return b + w * (c - b);
        }

        //P is inside face region
        T denom = (T)1.0 / (va + vb + vc);
        T v = vb * denom;
        T w = vc * denom;

        barycentrics[0] = (T)1.0 - v - w;
        barycentrics[1] = v;
        barycentrics[2] = w;
        return a.translate(ab * v + ac * w);
    }

    /**
     * Project a point on the triangle plane along the triangle normal and check if point lies on triangle
     */
    //see https://stackoverflow.com/questions/25512037/how-to-determine-if-a-point-lies-over-a-triangle-in-3d
    template<class T> bool Triangle3D<T>::projectedPointInsideTriangle(const Point3<T>& point) const {
        Vector3<T> normal = computeNormal();

        for (unsigned int i = 0; i < 3; ++i) {
            Vector3<T> triangleVector = triangleShape.getPoints()[(i + 1) % 3].vector(triangleShape.getPoints()[i]);
            Vector3<T> pointToTriangle = point.vector(triangleShape.getPoints()[i]);
            Vector3<T> tetrahedronFaceNormal = triangleVector.crossProduct(pointToTriangle);
            if (tetrahedronFaceNormal.dotProduct(normal) < 0.0) {
                return false;
            }
        }

        return true;
    }

    template<class T> bool Triangle3D<T>::collideWithRay(const Ray<T>& ray) const {
        bool hasIntersection = false;
        intersectPoint(ray, hasIntersection);
        return hasIntersection;
    }

    template<class T> Point3<T> Triangle3D<T>::intersectPoint(const Ray<T>& ray, bool& hasIntersection) const {
        //Möller-Trumbore algorithm
        constexpr T EPSILON = (T)0.0000001;

        Point3 vertex0 = triangleShape.getPoints()[0];
        Point3 vertex1 = triangleShape.getPoints()[1];
        Point3 vertex2 = triangleShape.getPoints()[2];

        Vector3 edge1 = vertex0.vector(vertex1);
        Vector3 edge2 = vertex0.vector(vertex2);
        Vector3 rayCrossEdge2 = ray.getDirection().crossProduct(edge2);
        T determinant = edge1.dotProduct(rayCrossEdge2);
        if (determinant > -EPSILON && determinant < EPSILON) {
            hasIntersection = false; //ray is parallel to the triangle
            return Point3<T>(0.0, 0.0, 0.0);
        }

        T invDeterminant = (T)1.0 / determinant;
        Vector3 s = vertex0.vector(ray.getOrigin());
        T u = invDeterminant * (s.dotProduct(rayCrossEdge2));
        if (u < 0.0 || u > 1.0) { //TODO no test for here
            hasIntersection = false;
            return Point3<T>(0.0, 0.0, 0.0);
        }

        Vector3 q = s.crossProduct(edge1);
        T v = invDeterminant * ray.getDirection().dotProduct(q);
        if (v < 0.0 || u + v > 1.0) {
            hasIntersection = false;
            return Point3<T>(0.0, 0.0, 0.0);
        }

        T t = invDeterminant * edge2.dotProduct(q);
        if (t > EPSILON && t < ray.getLength()) {
            hasIntersection = true;
            return ray.getOrigin().translate(ray.getDirection() * t);
        } else {
            hasIntersection = false; //there is a line intersection but no ray intersection
            return Point3<T>(0.0, 0.0, 0.0);
        }
    }

    template<class T> std::ostream& operator <<(std::ostream& stream, const Triangle3D<T>& triangle) {
        stream.setf(std::ios::left);
        stream << std::setw(20) << "Point 1: " << triangle.getPoints()[0] << std::endl;
        stream << std::setw(20) << "Point 2: " << triangle.getPoints()[1] << std::endl;
        stream << std::setw(20) << "Point 3: " << triangle.getPoints()[2] << std::endl;

        return stream;
    }

    //explicit template
    template class Triangle3D<float>;
    template std::ostream& operator <<<float>(std::ostream&, const Triangle3D<float>&);

    template class Triangle3D<double>;
    template std::ostream& operator <<<double>(std::ostream&, const Triangle3D<double>&);

}

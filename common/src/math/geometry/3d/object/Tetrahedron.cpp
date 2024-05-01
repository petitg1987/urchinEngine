#include <math/geometry/3d/object/Tetrahedron.h>
#include <math/geometry/3d/object/Triangle3D.h>

namespace urchin {

    template<class T> Tetrahedron<T>::Tetrahedron(const Point3<T>& a, const Point3<T>& b, const Point3<T>& c, const Point3<T>& d) :
            ConvexObject3D<T>(ConvexObjectType::TETRAHEDRON),
            a(a), b(b), c(c), d(d) {

    }

    template<class T> const Point3<T>& Tetrahedron<T>::getPointA() const {
        return a;
    }

    template<class T> const Point3<T>& Tetrahedron<T>::getPointB() const {
        return b;
    }

    template<class T> const Point3<T>& Tetrahedron<T>::getPointC() const {
        return c;
    }

    template<class T> const Point3<T>& Tetrahedron<T>::getPointD() const {
        return d;
    }

    template<class T> Point3<T> Tetrahedron<T>::getSupportPoint(const Vector3<T>& direction) const {
        const T pointADotDirection = a.toVector().dotProduct(direction);
        const T pointBDotDirection = b.toVector().dotProduct(direction);
        const T pointCDotDirection = c.toVector().dotProduct(direction);
        const T pointDDotDirection = d.toVector().dotProduct(direction);

        T maxPointDotDirection = pointADotDirection;
        Point3<T> supportPoint = a;
        if (pointBDotDirection > maxPointDotDirection) {
            maxPointDotDirection = pointBDotDirection;
            supportPoint = b;
        }

        if (pointCDotDirection > maxPointDotDirection) {
            maxPointDotDirection = pointCDotDirection;
            supportPoint = c;
        }

        if (pointDDotDirection > maxPointDotDirection) {
            supportPoint = d;
        }

        return supportPoint;
    }

    /**
     * @param barycentrics [out] Returns barycentric coordinates for closestPoint only when closestPoint != p
     * @param voronoiRegionMask Mask allowing to include/exclude some voronoi regions to test. The bits from right to left represent
     * respectively the following voronoi regions: ABC, ACD, ADB, BDC.
     * @return Point on tetrahedron closest to point p
     */
    template<class T> Point3<T> Tetrahedron<T>::closestPoint(const Point3<T>& p, std::array<T, 4>& barycentrics, unsigned short voronoiRegionMask) const {
        Point3<T> closestPoint;
        std::array<T, 3> triangleBarycentrics{};
        T bestSquareDist = std::numeric_limits<T>::max();

        bool pointOutsideOrInPlaneABC = (voronoiRegionMask & 1u) && pointOutsidePlane(p, a, b, c, d, true);
        bool pointOutsideOrInPlaneACD = (voronoiRegionMask & 2u) && pointOutsidePlane(p, a, c, d, b, true);
        bool pointOutsideOrInPlaneADB = (voronoiRegionMask & 4u) && pointOutsidePlane(p, a, d, b, c, true);
        bool pointOutsideOrInPlaneBDC = (voronoiRegionMask & 8u) && pointOutsidePlane(p, b, d, c, a, true);

        if (pointOutsideOrInPlaneABC) { //point outside face ABC: compute closest point on ABC
            Point3<T> q = Triangle3D<T>(a, b, c).closestPoint(p, triangleBarycentrics);

            Vector3<T> pq = p.vector(q);
            T squareDist = pq.dotProduct(pq);
            if (squareDist < bestSquareDist) {
                bestSquareDist = squareDist;

                closestPoint = q;
                barycentrics[0] = triangleBarycentrics[0];
                barycentrics[1] = triangleBarycentrics[1];
                barycentrics[2] = triangleBarycentrics[2];
                barycentrics[3] = 0.0;
            }
        }

        if (pointOutsideOrInPlaneACD) { //point outside face ACD: compute closest point on ACD
            Point3<T> q = Triangle3D<T>(a, c, d).closestPoint(p, triangleBarycentrics);

            Vector3<T> pq = p.vector(q);
            T squareDist = pq.dotProduct(pq);
            if (squareDist < bestSquareDist) {
                bestSquareDist = squareDist;

                closestPoint = q;
                barycentrics[0] = triangleBarycentrics[0];
                barycentrics[1] = 0.0;
                barycentrics[2] = triangleBarycentrics[1];
                barycentrics[3] = triangleBarycentrics[2];
            }
        }

        if (pointOutsideOrInPlaneADB) { //point outside face ADB: compute closest point on ADB
            Point3<T> q = Triangle3D<T>(a, d, b).closestPoint(p, triangleBarycentrics);

            Vector3<T> pq = p.vector(q);
            T squareDist = pq.dotProduct(pq);
            if (squareDist < bestSquareDist) {
                bestSquareDist = squareDist;

                closestPoint = q;
                barycentrics[0] = triangleBarycentrics[0];
                barycentrics[1] = triangleBarycentrics[2];
                barycentrics[2] = 0.0;
                barycentrics[3] = triangleBarycentrics[1];
            }
        }

        if (pointOutsideOrInPlaneBDC) { //point outside face BDC: compute closest point on BDC
            Point3<T> q = Triangle3D<T>(b, d, c).closestPoint(p, triangleBarycentrics);

            Vector3<T> pq = p.vector(q);
            T squareDist = pq.dotProduct(pq);
            if (squareDist < bestSquareDist) {
                closestPoint = q;
                barycentrics[0] = 0.0;
                barycentrics[1] = triangleBarycentrics[0];
                barycentrics[2] = triangleBarycentrics[2];
                barycentrics[3] = triangleBarycentrics[1];
            }
        }

        if (!pointOutsideOrInPlaneABC && !pointOutsideOrInPlaneACD && !pointOutsideOrInPlaneADB && !pointOutsideOrInPlaneBDC) { //point inside tetrahedron
            closestPoint = p;

            Matrix4<T> d0(a.X, a.Y, a.Z, 1.0,
                b.X, b.Y, b.Z, 1.0,
                c.X, c.Y, c.Z, 1.0,
                d.X, d.Y, d.Z, 1.0);

            Matrix4<T> d1(p.X, p.Y, p.Z, 1.0,
                b.X, b.Y, b.Z, 1.0,
                c.X, c.Y, c.Z, 1.0,
                d.X, d.Y, d.Z, 1.0);

            Matrix4<T> d2(a.X, a.Y, a.Z, 1.0,
                p.X, p.Y, p.Z, 1.0,
                c.X, c.Y, c.Z, 1.0,
                d.X, d.Y, d.Z, 1.0);

            Matrix4<T> d3(a.X, a.Y, a.Z, 1.0,
                b.X, b.Y, b.Z, 1.0,
                p.X, p.Y, p.Z, 1.0,
                d.X, d.Y, d.Z, 1.0);

            T d0Determinant = d0.determinant();
            barycentrics[0] = d1.determinant() / d0Determinant;
            barycentrics[1] = d2.determinant() / d0Determinant;
            barycentrics[2] = d3.determinant() / d0Determinant;
            barycentrics[3] = (T)1.0 - (barycentrics[0] + barycentrics[1] + barycentrics[2]);
        }

        return closestPoint;
    }

    template<class T> bool Tetrahedron<T>::collideWithPoint(const Point3<T>& p) const {
        return !pointOutsidePlane(p, a, b, c, d, false) &&
                !pointOutsidePlane(p, a, c, d, b, false) &&
                !pointOutsidePlane(p, a, d, b, c, false) &&
                !pointOutsidePlane(p, b, d, c, a, false);
    }

    /**
     * @return True if point p is outside the plane. Direction of plane normal is determinate by d.
     */
    template<class T> bool Tetrahedron<T>::pointOutsidePlane(const Point3<T>& p, const Point3<T>& planePointA, const Point3<T>& planePointB,
                                                             const Point3<T>& planePointC, const Point3<T>& d, bool onPlaneIsOutside) const {
        Vector3<T> ap = planePointA.vector(p);
        Vector3<T> ad = planePointA.vector(d);
        Vector3<T> ab = planePointA.vector(planePointB);
        Vector3<T> ac = planePointA.vector(planePointC);

        T signp = ap.dotProduct(ab.crossProduct(ac));
        T signd = ad.dotProduct(ab.crossProduct(ac));

        if (onPlaneIsOutside) {
            return (signp * signd) <= (T) 0.0;
        }
        return (signp * signd) < (T) 0.0;
    }

    template<class T> std::ostream& operator <<(std::ostream& stream, const Tetrahedron<T>& tetrahedron) {
        stream.setf(std::ios::left);
        stream << std::setw(20) << "Point A: " << tetrahedron.getPointA() << std::endl;
        stream << std::setw(20) << "Point B: " << tetrahedron.getPointB() << std::endl;
        stream << std::setw(20) << "Point C: " << tetrahedron.getPointC() << std::endl;
        stream << std::setw(20) << "Point D: " << tetrahedron.getPointD() << std::endl;

        return stream;
    }

    //explicit template
    template class Tetrahedron<float>;
    template std::ostream& operator <<<float>(std::ostream&, const Tetrahedron<float>&);

    template class Tetrahedron<double>;
    template std::ostream& operator <<<double>(std::ostream&, const Tetrahedron<double>&);

}

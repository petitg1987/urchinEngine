#include <limits>
#include <stdexcept>
#include <cassert>

#include <math/geometry/3d/Simplex.h>
#include <math/geometry/3d/object/LineSegment3D.h>
#include <math/geometry/3d/object/Tetrahedron.h>
#include <math/geometry/3d/object/Triangle3D.h>

namespace urchin {

    template<class T> Simplex<T>::Simplex() :
            simplexPointsSize(0) {
    }

    /**
     * Add a point to the end of the simplex
     * @return Point added (supportPointA - supportPointB)
     */
    template<class T> void Simplex<T>::addPoint(const Point3<T>& supportPointA, const Point3<T>& supportPointB) {
        assert(simplexPointsSize < 4);

        if (simplexPointsSize < 4) {
            SupportMapping<T> supportMapping;
            supportMapping.supportPointA = supportPointA;
            supportMapping.supportPointB = supportPointB;
            supportMapping.point = supportPointA - supportPointB;
            supportMapping.barycentric = std::numeric_limits<T>::max();

            simplexPoints[simplexPointsSize++] = supportMapping;

            updateSimplex();
        }
    }

    template<class T> std::size_t Simplex<T>::getSize() const {
        return simplexPointsSize;
    }

    template<class T> const Point3<T> &Simplex<T>::getPoint(std::size_t index) const {
        return simplexPoints[index].point;
    }

    template<class T> const Point3<T> &Simplex<T>::getSupportPointA(std::size_t index) const {
        return simplexPoints[index].supportPointA;
    }

    template<class T> const Point3<T> &Simplex<T>::getSupportPointB(std::size_t index) const {
        return simplexPoints[index].supportPointB;
    }

    template<class T> T Simplex<T>::getBarycentric(std::size_t index) const {
        return simplexPoints[index].barycentric;
    }

    /**
     * @return Closest point to origin belonging to simplex
     */
    template<class T> const Point3<T> &Simplex<T>::getClosestPointToOrigin() const {
        return closestPointToOrigin;
    }

    template<class T> bool Simplex<T>::isPointInSimplex(const Point3<T>& p) const {
        for (unsigned int i = 0; i < simplexPointsSize; ++i) {
            const Point3<T>& simplexPoint = simplexPoints[i].point;
            if (simplexPoint.X == p.X && simplexPoint.Y == p.Y && simplexPoint.Z == p.Z) {
                return true;
            }
        }

        return false;
    }

    /**
     * Computes closest points on objects A and B. If two objects collides, the behavior is undefined
     * @param closestPointA [out] Computed closest point of object A
     * @param closestPointB [out] Computed closest point of object B
     */
    template<class T> void Simplex<T>::computeClosestPoints(Point3<T>& closestPointA, Point3<T>& closestPointB) const {
        if (getSize() == 1) { //simplex is a point

            closestPointA = simplexPoints[0].barycentric * simplexPoints[0].supportPointA;
            closestPointB = simplexPoints[0].barycentric * simplexPoints[0].supportPointB;
        } else if (getSize() == 2) { //simplex is a line (1D)

            closestPointA = simplexPoints[0].barycentric * simplexPoints[0].supportPointA + simplexPoints[1].barycentric * simplexPoints[1].supportPointA;
            closestPointB = simplexPoints[0].barycentric * simplexPoints[0].supportPointB + simplexPoints[1].barycentric * simplexPoints[1].supportPointB;
        } else if (getSize() == 3) { //simplex is a triangle (2D)

            closestPointA = simplexPoints[0].barycentric * simplexPoints[0].supportPointA + simplexPoints[1].barycentric * simplexPoints[1].supportPointA
                    + simplexPoints[2].barycentric * simplexPoints[2].supportPointA;
            closestPointB = simplexPoints[0].barycentric * simplexPoints[0].supportPointB + simplexPoints[1].barycentric * simplexPoints[1].supportPointB
                    + simplexPoints[2].barycentric * simplexPoints[2].supportPointB;
        } else if (getSize() == 4) { //simplex is a tetrahedron (3D)
            closestPointA = simplexPoints[0].barycentric * simplexPoints[0].supportPointA + simplexPoints[1].barycentric * simplexPoints[1].supportPointA
                    + simplexPoints[2].barycentric * simplexPoints[2].supportPointA + simplexPoints[3].barycentric * simplexPoints[3].supportPointA;
            closestPointB = simplexPoints[0].barycentric * simplexPoints[0].supportPointB + simplexPoints[1].barycentric * simplexPoints[1].supportPointB
                    + simplexPoints[2].barycentric * simplexPoints[2].supportPointB + simplexPoints[3].barycentric * simplexPoints[3].supportPointB;
        } else {
            throw std::invalid_argument("Size of simplex unsupported to compute closest points: " + std::to_string(simplexPointsSize) + ".");
        }
    }

    /**
     * Update the simplex: remove useless point, find closest point to origin and barycentrics.
     */
    template<class T> void Simplex<T>::updateSimplex() {
        if (getSize() == 1) { //simplex is a point
            closestPointToOrigin = getPoint(0);

            setBarycentric(0, 1.0);
        } else if (getSize() == 2) { //simplex is a line (1D)

            const Point3<T>& pointA = getPoint(0);
            const Point3<T>& pointB = getPoint(1); //pointB is the last point added to the simplex

            std::array<T, 2> barycentrics = {0.0, 0.0};
            closestPointToOrigin = LineSegment3D<T>(pointA, pointB).closestPoint(Point3<T>(0.0, 0.0, 0.0), barycentrics);
            setBarycentric(0, barycentrics[0]);
            setBarycentric(1, barycentrics[1]);
        } else if (getSize() == 3) { //simplex is a triangle (2D)

            const Point3<T>& pointA = getPoint(0);
            const Point3<T>& pointB = getPoint(1);
            const Point3<T>& pointC = getPoint(2); //pointC is the last point added to the simplex

            const Vector3<T> co = pointC.vector(Point3<T>(0.0, 0.0, 0.0));
            const Vector3<T> cb = pointC.vector(pointB);
            const Vector3<T> ca = pointC.vector(pointA);
            const Vector3<T> normalAbc = cb.crossProduct(ca);

            std::array<T, 3> barycentrics = {0.0, 0.0, 0.0};
            closestPointToOrigin = Triangle3D<T>(pointA, pointB, pointC).closestPoint(Point3<T>(0.0, 0.0, 0.0), barycentrics);
            setBarycentric(0, barycentrics[0]);
            setBarycentric(1, barycentrics[1]);
            setBarycentric(2, barycentrics[2]);

            if (barycentrics[1] == 0.0) { //remove pointB
                removePoint(1);
            }
            if (barycentrics[0] == 0.0) { //remove pointA
                removePoint(0);
            }

            if (normalAbc.dotProduct(co) <= 0.0) { //voronoi region -ABC => ABC
                std::swap(simplexPoints[0], simplexPoints[1]); //swap pointA and pointB
            }
        } else if (getSize() == 4) { //simplex is a tetrahedron (3D)

            const Point3<T>& pointA = getPoint(0);
            const Point3<T>& pointB = getPoint(1);
            const Point3<T>& pointC = getPoint(2);
            const Point3<T>& pointD = getPoint(3); //pointD is the last point added to the simplex

            std::array<T, 4> barycentrics = {0.0, 0.0, 0.0, 0.0};
            constexpr unsigned short voronoiRegionMask = 14u; //test all voronoi regions except the one which doesn't include the new point added (pointD)
            closestPointToOrigin = Tetrahedron<T>(pointA, pointB, pointC, pointD).closestPoint(Point3<T>(0.0, 0.0, 0.0), barycentrics, voronoiRegionMask);
            setBarycentric(0, barycentrics[0]);
            setBarycentric(1, barycentrics[1]);
            setBarycentric(2, barycentrics[2]);
            setBarycentric(3, barycentrics[3]);

            if (barycentrics[2] == 0.0) { //remove pointC
                removePoint(2);
            }
            if (barycentrics[1] == 0.0) { //remove pointB
                removePoint(1);
            }
            if (barycentrics[0] == 0.0) { //remove pointA
                removePoint(0);
            }
        } else {
            throw std::invalid_argument("Size of simplex unsupported: " + std::to_string(getSize()) + ".");
        }
    }

    template<class T> void Simplex<T>::removePoint(std::size_t index) {
        assert(simplexPointsSize > 0);

        for (std::size_t i = index; i < simplexPointsSize - 1; ++i) {
            simplexPoints[i] = simplexPoints[i + 1];
        }
        simplexPointsSize--;
    }

    template<class T> void Simplex<T>::setBarycentric(std::size_t index, T barycentric) {
        simplexPoints[index].barycentric = barycentric;
    }

    template<class T> std::ostream& operator <<(std::ostream& stream, const Simplex<T>& simplex) {
        for (std::size_t i = 0; i < simplex.getSize(); ++i) {
            stream << "Simplex point " << i << " details:" << std::endl;
            stream << " - Point: " << simplex.getPoint(i) << std::endl;
            stream << " - Support point A: " << simplex.getSupportPointA(i) << std::endl;
            stream << " - Support point B: " << simplex.getSupportPointB(i) << std::endl;
            stream << " - Barycentric: " << simplex.getBarycentric(i) << std::endl;
        }
        stream << "Closest point to origin: " << simplex.getClosestPointToOrigin();
        return stream;
    }

    //explicit template
    template class Simplex<float>;
    template std::ostream& operator <<<float>(std::ostream&, const Simplex<float>&);

    template class Simplex<double>;
    template std::ostream& operator <<<double>(std::ostream&, const Simplex<double>&);

}

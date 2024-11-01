#pragma once

#include <array>

#include <math/algebra/point/Point3.h>

namespace urchin {

    template<class T> struct SupportMapping {
        Point3<T> supportPointA;
        Point3<T> supportPointB;
        Point3<T> point; //supportPointA - supportPointB
        T barycentric; //barycentric/weight of point 'point'. Value undefined when simplex contain the origin.
    };

    template<class T> class Simplex {
        public:
            Simplex();

            void addPoint(const Point3<T>&, const Point3<T>&);

            std::size_t getSize() const;
            const Point3<T>& getPoint(std::size_t) const;
            const Point3<T>& getSupportPointA(std::size_t) const;
            const Point3<T>& getSupportPointB(std::size_t) const;
            T getBarycentric(std::size_t) const;
            const Point3<T>& getClosestPointToOrigin() const;
            bool isPointInSimplex(const Point3<T>&) const;

            void computeClosestPoints(Point3<T>&, Point3<T>&) const;

        private:
            void updateSimplex();
            void removePoint(std::size_t);
            void setBarycentric(std::size_t, T);

            std::array<SupportMapping<T>, 4> simplexPoints;
            unsigned int simplexPointsSize;
            Point3<T> closestPointToOrigin;
    };

    template<class T> std::ostream& operator <<(std::ostream&, const Simplex<T>&);

}

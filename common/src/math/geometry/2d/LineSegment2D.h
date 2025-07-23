#pragma once

#include <array>

#include "math/geometry/2d/Line2D.h"
#include "math/algebra/point/Point2.h"
#include "math/algebra/vector/Vector2.h"

namespace urchin {

    template<class T> class LineSegment2D {
        public:
            LineSegment2D(const Point2<T>&, const Point2<T>&);

            const Point2<T>& getA() const;
            const Point2<T>& getB() const;

            Point2<T> closestPoint(const Point2<T>&, std::array<T, 2>&) const;
            T squareDistance(const Point2<T>&) const;
            bool onSegment(const Point2<T>&) const;

            Vector2<T> toVector() const;
            Line2D<T> toLine() const;

            Point2<T> intersectPoint(const LineSegment2D<T>&, bool&) const;
            Point2<T> intersectPoint(const LineSegment2D<T>&, bool&, Point2<T>&, bool&) const;
            bool hasProperIntersection(const LineSegment2D<T>&) const;

            template<class NEW_TYPE> LineSegment2D<NEW_TYPE> cast() const;

        private:
            T ccw(const Point2<T>&, const Point2<T>&, const Point2<T>&) const;

            Point2<T> a;
            Point2<T> b;
    };

    template<class T> std::ostream& operator <<(std::ostream&, const LineSegment2D<T>&);

}

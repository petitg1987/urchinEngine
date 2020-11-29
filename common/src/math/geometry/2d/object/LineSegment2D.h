#ifndef URCHINENGINE_LINESEGMENT2D_H
#define URCHINENGINE_LINESEGMENT2D_H

#include "math/geometry/2d/object/ConvexObject2D.h"
#include "math/geometry/2d/Line2D.h"
#include "math/algebra/point/Point2.h"
#include "math/algebra/vector/Vector2.h"

namespace urchin {

    template<class T> class LineSegment2D : public ConvexObject2D<T> {
        public:
            LineSegment2D(const Point2<T>&, const Point2<T>&);

            const Point2<T>& getA() const;
            const Point2<T>& getB() const;

            Point2<T> getSupportPoint(const Vector2<T>&) const;
            Point2<T> closestPoint(const Point2<T>&, T[2]) const;
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

            Point2<T> a, b;
    };

    template<class T> std::ostream& operator <<(std::ostream&, const LineSegment2D<T>&);

}

#endif

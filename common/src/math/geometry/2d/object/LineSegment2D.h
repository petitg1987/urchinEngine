#ifndef ENGINE_LINESEGMENT2D_H
#define ENGINE_LINESEGMENT2D_H

#include "math/geometry/2d/object/ConvexObject2D.h"
#include "math/algebra/point/Point2.h"
#include "math/algebra/vector/Vector2.h"

namespace urchin
{

	template<class T> class LineSegment2D : public ConvexObject2D<T>
	{
		public:
			LineSegment2D(const Point2<T> &,  const Point2<T> &);

			const Point2<T> &getA() const;
			const Point2<T> &getB() const;

			Point2<T> getSupportPoint(const Vector2<T> &) const;
			Point2<T> closestPoint(const Point2<T> &, T[2]) const;
			T squareDistance(const Point2<T> &) const;

			Point2<T> intersectPoint(const LineSegment2D<T> &) const;

			template<class NEW_TYPE> LineSegment2D<NEW_TYPE> cast() const;

		private:
			Point2<T> a, b;
	};

	template<class T> std::ostream& operator <<(std::ostream &, const LineSegment2D<T> &);

}

#endif

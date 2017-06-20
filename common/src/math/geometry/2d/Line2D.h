#ifndef ENGINE_LINE2D_H
#define ENGINE_LINE2D_H

#include "math/algebra/point/Point2.h"

namespace urchin
{

	template<class T> class Line2D
	{
		public:
			Line2D(const Point2<T> &,  const Point2<T> &);

			const Point2<T> &getA() const;
			const Point2<T> &getB() const;

			Vector2<T> computeNormal() const;
			Point2<T> orthogonalProjection(const Point2<T> &) const;
			T squareDistance(const Point2<T> &) const;
			T verticalDistance(const Point2<T> &p) const;
			T horizontalDistance(const Point2<T> &p) const;
			Line2D<T> orthogonalLine() const;
			Line2D<T> parallelLine(const Point2<T> &) const;
			T ccw(const Point2<T> &) const;

			Point2<T> intersectPoint(const Line2D<T> &, bool &) const;

		private:
			Point2<T> a, b;
	};

	template<class T> std::ostream& operator <<(std::ostream &, const Line2D<T> &);

}

#endif

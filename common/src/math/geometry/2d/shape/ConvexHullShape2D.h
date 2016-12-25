#ifndef ENGINE_CONVEXHULLSHAPE2D_H
#define ENGINE_CONVEXHULLSHAPE2D_H

#include <vector>

#include "math/geometry/2d/shape/ConvexShape2D.h"
#include "math/algebra/point/Point2.h"

namespace urchin
{

	template<class T> class ConvexHullShape2D : public ConvexShape2D
	{
		public:
			ConvexHullShape2D(const std::vector<Point2<T>> &);
			~ConvexHullShape2D();

			const std::vector<Point2<T>> &getPoints() const;

			Point2<T> getSupportPoint(const Vector2<T> &) const;
			T area() const;

		private:
			std::vector<Point2<T>> convexHullPoints;

	};

}

#endif

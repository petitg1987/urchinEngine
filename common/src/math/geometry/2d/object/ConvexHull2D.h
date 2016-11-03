#ifndef ENGINE_CONVEXHULL2D_H
#define ENGINE_CONVEXHULL2D_H

#include <vector>

#include "math/geometry/2d/object/ConvexObject2D.h"
#include "math/algebra/point/Point2.h"

namespace urchin
{

	/**
	* Represents a convex hull 2D
	*/
	template<class T> class ConvexHull2D : public ConvexObject2D<T>
	{
		public:
			ConvexHull2D(const std::vector<Point2<T>> &);
			~ConvexHull2D();

			const std::vector<Point2<T>> &getPoints() const;

			Point2<T> getSupportPoint(const Vector2<T> &) const;
			T area() const;

		private:
			std::vector<Point2<T>> convexHullPoints;
	};

}

#endif

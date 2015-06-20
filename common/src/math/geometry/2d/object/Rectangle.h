#ifndef ENGINE_RECTANGLE_H
#define ENGINE_RECTANGLE_H

#include "math/geometry/2d/object/ConvexObject2D.h"
#include "math/algebra/point/Point2.h"
#include "math/geometry/2d/shape/RectangleShape.h"

namespace urchin
{

	/**
	* Represents a rectangle aligned on axis in space
	*/
	template<class T> class Rectangle : public ConvexObject2D<T>
	{
		public:
			Rectangle(const Point2<T> &, const Vector2<T> &);
			Rectangle(const Point2<T> &, const Point2<T> &);

			Point2<T> getSupportPoint(const Vector2<T> &) const;
			bool collideWithPoint(const Point2<T> &) const;

		private:
			RectangleShape<T> rectangleShape;
			Point2<T> origin; //origin of rectangle (minimum point on X and Y axis)
	};

}

#endif

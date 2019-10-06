#ifndef URCHINENGINE_RECTANGLE_H
#define URCHINENGINE_RECTANGLE_H

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
	        Rectangle();
			Rectangle(const Point2<T> &, const Vector2<T> &);
			Rectangle(const Point2<T> &, const Point2<T> &);

			Point2<T> getMin() const;
			Point2<T> getMax() const;
			Vector2<T> getDiagonal() const;
			Point2<T> getSupportPoint(const Vector2<T> &) const;

			Rectangle<T> merge(const Rectangle<T> &) const;

			bool collideWithPoint(const Point2<T> &) const;

		private:
			RectangleShape<T> rectangleShape;
			Point2<T> origin; //origin of rectangle (minimum point on X and Y axis)
	};

}

#endif

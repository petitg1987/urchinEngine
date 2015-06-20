#include "math/geometry/2d/object/Rectangle.h"

namespace urchin
{

	/**
	 * @param origin Origin of the rectangle (minimum point on X and Y axis)
	 */
	template<class T> Rectangle<T>::Rectangle(const Point2<T> &origin, const Vector2<T> &diagonal) :
		rectangleShape(RectangleShape<T>(diagonal)),
		origin(origin)
	{

	}

	/**
	* @param minPoint Lower point of rectangle
	* @param maxPoint Upper point of rectangle
	*/
	template<class T> Rectangle<T>::Rectangle(const Point2<T> &minPoint, const Point2<T> &maxPoint) :
		rectangleShape(RectangleShape<T>(minPoint.vector(maxPoint))),
		origin(minPoint)
	{

	}

	template<class T> Point2<T> Rectangle<T>::getSupportPoint(const Vector2<T> &direction) const
	{
		const unsigned int signX = direction.X < (T)0.0 ? 0 : 1;
		const unsigned int signY = direction.Y < (T)0.0 ? 0 : 1;

		return origin + Point2<T>(signX * rectangleShape.getDiagonal().X, signY * rectangleShape.getDiagonal().Y);
	}

	template<class T> bool Rectangle<T>::collideWithPoint(const Point2<T> &point) const
	{
		Point2<T> maxPoint = origin.translate(rectangleShape.getDiagonal());
		return point.X>origin.X && point.X<maxPoint.X && point.Y>origin.Y && point.Y<maxPoint.Y;
	}

	//explicit template
	template class Rectangle<int>;
}

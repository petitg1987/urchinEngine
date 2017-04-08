#include <limits>
#include <cmath>

#include "math/geometry/2d/object/LineSegment2D.h"

namespace urchin
{

	template<class T> LineSegment2D<T>::LineSegment2D(const Point2<T> &a, const Point2<T> &b) :
		a(a), b(b)
	{

	}

	template<class T> const Point2<T> &LineSegment2D<T>::getA() const
	{
		return a;
	}

	template<class T> const Point2<T> &LineSegment2D<T>::getB() const
	{
		return b;
	}

	template<class T> Point2<T> LineSegment2D<T>::getSupportPoint(const Vector2<T> &direction) const
	{
		const T pointADotDirection = Point2<T>(0.0, 0.0).vector(a).dotProduct(direction);
		const T pointBDotDirection = Point2<T>(0.0, 0.0).vector(b).dotProduct(direction);

		if(pointADotDirection > pointBDotDirection)
		{
			return a;
		}

		return b;
	}

	/**
	 * @param barycentrics [out] Returns barycentric coordinates for closest point
	 * @return Point on segment AB closest to point p
	 */
	template<class T> Point2<T> LineSegment2D<T>::closestPoint(const Point2<T> &p, T barycentrics[2]) const
	{
		Vector2<T> ab = a.vector(b);

		T abSquareLength = ab.dotProduct(ab);
		if(abSquareLength==(T)0.0)
		{
			barycentrics[0] = 1.0;
			barycentrics[1] = 0.0;
			return a;
		}

		Vector2<T> ap = a.vector(p);
		T t = ap.dotProduct(ab) / abSquareLength;
		t = std::min((T)1.0, std::max((T)0.0, t));

		barycentrics[0] = 1.0-t;
		barycentrics[1] = t;
		return ((b-a)*t) + a;
	}

	/**
	 * @return Minimum square distance between segment AB and point p
	 */
	template<class T> T LineSegment2D<T>::squareDistance(const Point2<T> &p) const
	{
		Vector2<T> ab = a.vector(b);
		Vector2<T> ap = a.vector(p);

		T e = ap.dotProduct(ab);
		if (e <= 0.0f)
		{
			return ap.dotProduct(ap);
		}

		T f = ab.dotProduct(ab);
		if (e >= f)
		{
			Vector2<T> bp = b.vector(p);
			return bp.dotProduct(bp);
		}

		return ap.dotProduct(ap) - ((e * e) / f);
	}

	template<class T> Point2<T> LineSegment2D<T>::intersectPoint(const LineSegment2D<T> &other) const
	{
		T subX = b.X - a.X;
		T subY = b.Y - a.Y;
		T subXOther = other.getB().X - other.getA().X;
		T subYOther = other.getB().Y - other.getA().Y;

		T s = (-subY * (a.X - other.getA().X) + subX * (a.Y - other.getA().Y)) / (-subXOther * subY + subX * subYOther);
		T t = (subXOther * (a.Y - other.getA().Y) - subYOther * (a.X - other.getA().X)) / (-subXOther * subY + subX * subYOther);

		if(s>=0.0 && s<=1.0 && t>=0.0 && t<=1.0)
		{ //intersection
			return Point2<float>(a.X+(t*subX), a.Y+(t*subY));
		}

		return Point2<float>(std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN());
	}

	template<class T> std::ostream& operator <<(std::ostream &stream, const LineSegment2D<T> &l)
	{
		return stream << l.getA().X << " " << l.getA().Y << " - " << l.getB().X << " " << l.getB().Y;
	}

	//explicit template
	template class LineSegment2D<float>;
	template std::ostream& operator <<<float>(std::ostream & , const LineSegment2D<float> &);

}

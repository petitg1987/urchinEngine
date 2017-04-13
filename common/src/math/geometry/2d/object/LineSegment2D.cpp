#include <limits>
#include <cmath>
#include <cassert>

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

	/**
	 * Returns the intersection point of the two lines segment. If intersection doesn't exist: return a Point2<T> of NAN.
	 * When line segments are collinear and intersect: returns the nearest intersection point between this->getA() and this->getB().
	 */
	template<class T> Point2<T> LineSegment2D<T>::intersectPoint(const LineSegment2D<T> &other) const
	{ //see http://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect
		Vector2<T> r(b.X - a.X, b.Y - a.Y); //note: a+1.0*r = b;
		Vector2<T> s(other.getB().X - other.getA().X, other.getB().Y - other.getA().Y);

		T rCrossS = r.crossProduct(s);
		Vector2<T> thisToOther = a.vector(other.getA());
		T startPointsCrossR = thisToOther.crossProduct(r);

		if(rCrossS==0.0)
		{ //line segments are parallel
			if(startPointsCrossR==0.0)
			{ //line segments are collinear
				T t0 = thisToOther.dotProduct(r) / r.dotProduct(r);
				T t1 = t0 + s.dotProduct(r) / r.dotProduct(r);

				if(s.dotProduct(r) < 0.0)
				{
					std::swap(t0, t1);
				}
				#ifdef _DEBUG
					assert(t0 <= t1);
				#endif

				if(t0>=0.0 && t0<=1.0 && t1>=1.0)
				{ //collinear with intersection
					return a.translate(t0*r);
				}else if(t0<=0.0 && t1>=0.0 && t1<=1.0)
				{ //collinear with intersection
					return a.translate(t1*r);
				}
			}

			return Point2<T>(std::numeric_limits<T>::quiet_NaN(), std::numeric_limits<T>::quiet_NaN());
		}

		//line segments not parallel
		T t = thisToOther.crossProduct(s) / rCrossS;
		T u = startPointsCrossR / rCrossS;
		if(t>=0.0 && t<=1.0 && u>=0.0 && u<=1.0)
		{ //intersection
			return a.translate(t*r);
		}

		//no intersection
		return Point2<T>(std::numeric_limits<T>::quiet_NaN(), std::numeric_limits<T>::quiet_NaN());
	}

	template<class T> std::ostream& operator <<(std::ostream &stream, const LineSegment2D<T> &l)
	{
		return stream << l.getA().X << " " << l.getA().Y << " - " << l.getB().X << " " << l.getB().Y;
	}

	//explicit template
	template class LineSegment2D<float>;
	template std::ostream& operator <<<float>(std::ostream & , const LineSegment2D<float> &);

}

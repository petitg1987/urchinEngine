#include <cmath>
#include <cassert>
#include <limits>

#include "math/geometry/2d/Line2D.h"
#include "math/algebra/vector/Vector2.h"

namespace urchin
{

	template<class T> Line2D<T>::Line2D(const Point2<T> &a,  const Point2<T> &b) :
		a(a), b(b)
	{
		#ifdef _DEBUG
			assert(a.X!=b.X || a.Y!=b.Y);
		#endif
	}

	template<class T> const Point2<T> &Line2D<T>::getA() const
	{
		return a;
	}

	template<class T> const Point2<T> &Line2D<T>::getB() const
	{
		return b;
	}

	template<class T> Point2<T> Line2D<T>::orthogonalProjection(const Point2<T> &p) const
	{
		Vector2<T> ab = a.vector(b);

		T abSquareLength = ab.dotProduct(ab);
		Vector2<T> ap = a.vector(p);
		const T t = ap.dotProduct(ab) / abSquareLength;
		return ((b-a)*t) + a;
	}

	/**
	 * @return Minimum square distance between line and point p
	 */
	template<class T> T Line2D<T>::squareDistance(const Point2<T> &p) const
	{
		Vector2<T> ab = a.vector(b);
		Vector2<T> ap = a.vector(p);

		T e = ap.dotProduct(ab);
		T f = ab.dotProduct(ab);

		return ap.dotProduct(ap) - ((e * e) / f);
	}

	template<class T> Line2D<T> Line2D<T>::orthogonalLine() const
	{
		Vector2<T> ab = a.vector(b);
		Vector2<T> orthoVector(-ab[1], ab[0]);
		return Line2D<T>(a, a.translate(orthoVector));
	}

	template<class T> Line2D<T> Line2D<T>::parallelLine(const Point2<T> &p) const
	{
		Vector2<T> ab = a.vector(b);
		return Line2D<T>(p, p.translate(ab));
	}

	/**
	 * @return positive value, if ABC points makes a counter-clockwise turn, negative for clockwise turn, and zero if the points are collinear.
	 */
	template<class T> T Line2D<T>::ccw(const Point2<T> &c) const
	{
		return (b.X - a.X) * (c.Y - a.Y) - (b.Y - a.Y) * (c.X - a.X);
	}

	/**
	 * Returns the intersection point of the two lines. If intersection doesn't exist: return a Point2<T> of NAN.
	 * When lines are collinear and intersect: returns this->getA().
	 */
	template<class T> Point2<T> Line2D<T>::intersectPoint(const Line2D<T> &other, bool &hasIntersection) const
	{ //see LineSegment2D.intersectPoint()
		Vector2<T> r(b.X - a.X, b.Y - a.Y);
		Vector2<T> s(other.getB().X - other.getA().X, other.getB().Y - other.getA().Y);

		T rCrossS = r.crossProduct(s);
		T startPointsCrossR = a.vector(other.getA()).crossProduct(r);

		if(rCrossS==0.0)
		{ //lines are parallel
			if(startPointsCrossR==0.0)
			{ //lines are collinear
				hasIntersection = true;
				return a;
			}

			hasIntersection = false;
			return Point2<T>(0, 0);
		}

		//lines not parallel
		hasIntersection = true;
		return other.getA().translate((startPointsCrossR*s) / rCrossS);
	}

	template<class T> std::ostream& operator <<(std::ostream &stream, const Line2D<T> &l)
	{
		return stream << l.getA().X << " " << l.getA().Y << " - " << l.getB().X << " " << l.getB().Y;
	}

	//explicit template
	template class Line2D<float>;
	template std::ostream& operator <<<float>(std::ostream &, const Line2D<float> &);

	template class Line2D<double>;
	template std::ostream& operator <<<double>(std::ostream &, const Line2D<double> &);

	template class Line2D<int>;
	template std::ostream& operator <<<int>(std::ostream &, const Line2D<int> &);

	template class Line2D<long long>;
	template std::ostream& operator <<<long long>(std::ostream &, const Line2D<long long> &);

}

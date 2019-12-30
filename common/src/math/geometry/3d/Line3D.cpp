#include <cassert>

#include "Line3D.h"

namespace urchin
{

	template<class T> Line3D<T>::Line3D(const Point3<T> &a, const Point3<T> &b) :
		a(a), b(b)
	{
		assert(a.X!=b.X || a.Y!=b.Y || a.Z!=b.Z);
	}

	template<class T> const Point3<T> &Line3D<T>::getA() const
	{
		return a;
	}

	template<class T> const Point3<T> &Line3D<T>::getB() const
	{
		return b;
	}

	template<class T> Point3<T> Line3D<T>::orthogonalProjection(const Point3<T> &p) const
	{
		Vector3<T> ab = a.vector(b);
		Vector3<T> ap = a.vector(p);

		const T t = ap.dotProduct(ab) / ab.squareLength();
		return ((b-a)*t) + a;
	}

	/**
	 * @return Minimum square distance between line and point p
	 */
	template<class T> T Line3D<T>::squareDistance(const Point3<T> &p) const
	{
		Vector3<T> ab = a.vector(b);
		Vector3<T> ap = a.vector(p);

		T e = ap.dotProduct(ab);
		T f = ab.dotProduct(ab);

		return ap.dotProduct(ap) - ((e * e) / f);
	}

	template<class T> Line3D<T> Line3D<T>::parallelLine(const Point3<T> &p) const
	{
		Vector3<T> ab = a.vector(b);
		return Line3D<T>(p, p.translate(ab));
	}

    template<class T> T Line3D<T>::minDistance(const Line3D<T> &line) const
    {
        Vector3<T> linesVectorCrossProduction = a.vector(b).crossProduct(line.getA().vector(line.getB()));
        return line.getA().vector(a).dotProduct(linesVectorCrossProduction) / linesVectorCrossProduction.length();
    }

	template<class T> std::ostream& operator <<(std::ostream &stream, const Line3D<T> &l)
	{
		return stream << l.getA().X << " " << l.getA().Y << " " << l.getA().Z << " - " << l.getB().X << " " << l.getB().Y << " " << l.getB().Z ;
	}

	//explicit template
	template class Line3D<float>;
	template std::ostream& operator <<<float>(std::ostream &, const Line3D<float> &);

	template class Line3D<double>;
	template std::ostream& operator <<<double>(std::ostream &, const Line3D<double> &);

}

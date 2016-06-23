#include <stdexcept>
#include <sstream>

#include "Plane.h"

namespace urchin
{

	/**
	 * @param planeNormal Plane normal normalized
	 * @param distanceToOrigin Distance to the origin. Positive if dot product between a vector from plane to origin and the normal is positive.
	 */
	template<class T> Plane<T>::Plane(const Vector3<T> &normal, T distanceToOrigin) :
		normal(normal),
		d(distanceToOrigin)
	{
		const T normalLength = normal.length();
		if(normalLength < 0.9999f || normalLength > (T)1.0001)
		{
			std::ostringstream normalLengthStr;
			normalLengthStr << normal.length();
			throw std::invalid_argument("Impossible to build a plane. Wrong normal length: " + normalLengthStr.str() + ".");
		}
	}

	template<class T> Plane<T>::Plane(const Point3<T> &p1, const Point3<T> &p2, const Point3<T> &p3)
	{
		buildFrom3Points(p1, p2, p3);
	}

	template<class T> Plane<T>::Plane(const Vector3<T> &normal, const Point3<T> &point)
	{
		buildFromNormalAndPoint(normal, point);
	}

	template<class T> Plane<T>::Plane(T a, T b, T c, T d)
	{
		buildFromCoefficients(a, b, c, d);
	}

	template<class T> void Plane<T>::buildFrom3Points(const Point3<T> &p1, const Point3<T> &p2, const Point3<T> &p3)
	{
		const Vector3<T> &aux = p2.vector(p3);
		normal = aux.crossProduct(p2.vector(p1)).normalize();

		d = normal.dotProduct(-Vector3<T>(p2.X, p2.Y, p2.Z));
	}

	template<class T> void Plane<T>::buildFromNormalAndPoint(const Vector3<T> &normal, const Point3<T> &point)
	{
		this->normal = normal.normalize();
		d = this->normal.dotProduct(-Vector3<T>(point.X, point.Y, point.Z));
	}

	/**
	 * Built a plane from coefficients
	 * @param a Coefficient A of the equation plan
	 * @param b Coefficient B of the equation plan
	 * @param c Coefficient C of the equation plan
	 * @param d Coefficient D of the equation plan
	 */
	template<class T> void Plane<T>::buildFromCoefficients(T a, T b, T c, T d)
	{
		normal.setValues(a, b, c);
		normal = normal.normalize();
		this->d = d;
	}

	template<class T> void Plane<T>::reverse()
	{
		normal = -normal;
		d = -d;
	}

	template<class T> const Vector3<T> &Plane<T>::getNormal() const
	{
		return normal;
	}

	template<class T> T Plane<T>::getDistanceToOrigin() const
	{
		return d;
	}

	/**
	 * @return Distance between the plane and the point 3D. If positive result: the point is on the same side as the normal.
	 */
	template<class T> T Plane<T>::distance(const Point3<T> &p) const
	{ //see http://fr.wikipedia.org/wiki/Distance_d%27un_point_%C3%A0_un_plan

		//division by sqrt(a*a+b*b+c*c) is useless because normal is normalized and so result is 1.0.
		return normal.X*p.X + normal.Y*p.Y + normal.Z*p.Z + d;
	}
	
	template<class T> T Plane<T>::distance(const Point4<T> &p) const
	{
		return normal.X*p.X + normal.Y*p.Y + normal.Z*p.Z + d*p.W;
	}

	template<class T> Point3<T> Plane<T>::orthogonalProjection(const Point3<T> &p) const
	{
		Point3<T> planePoint = Point3<T>(normal * -d);
		Vector3<T> planePointToP = planePoint.vector(p);

		return p.translate(-(planePointToP.dotProduct(normal) * normal));
	}

	template<class T> Point3<T> Plane<T>::intersectPoint(const Line3D<T> &line) const
	{
		Point3<T> planePoint = Point3<T>(normal * -d);
		Vector3<T> lineVector = line.getA().vector(line.getB());

		T numerator = normal.dotProduct(lineVector);
		Vector3<T> lineAToPlanePoint = line.getA().vector(planePoint);
		T t = normal.dotProduct(lineAToPlanePoint) / numerator;
		return (line.getA() + Point3<T>(t * lineVector));
	}

	template<class T> std::ostream& operator <<(std::ostream &stream, const Plane<T> &p)
	{
		return stream << "Plane normal: "<<p.getNormal()<<" | Distance to origin: "<<p.getDistanceToOrigin();
	}

	//explicit template
	template class Plane<float>;
	template std::ostream& operator <<<float>(std::ostream &, const Plane<float> &);

	template class Plane<double>;
	template std::ostream& operator <<<double>(std::ostream &, const Plane<double> &);

}

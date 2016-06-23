#include "Sphere.h"

namespace urchin
{

	template<class T> Sphere<T>::Sphere() :
		sphereShape(SphereShape<T>(0.0)),
		position(Point3<T>(0.0, 0.0, 0.0))
	{

	}

	template<class T> Sphere<T>::Sphere(T radius, const Point3<T> &position) :
		sphereShape(SphereShape<T>(radius)),
		position(position)
	{

	}

	template<class T> T Sphere<T>::getRadius() const
	{
		return sphereShape.getRadius();
	}

	template<class T> const Point3<T> &Sphere<T>::getPosition() const
	{
		return position;
	}

	template<class T> Point3<T> Sphere<T>::getSupportPoint(const Vector3<T> &direction) const
	{
		if(direction.X==0.0 && direction.Y==0.0 && direction.Z==0.0)
		{
			return position + Point3<T>(getRadius(), (T)0.0, (T)0.0);
		}

		return position.translate(direction.normalize() * getRadius());
	}

	template<class T> bool Sphere<T>::collideWithPoint(const Point3<T> &point) const
	{
		if(position.squareDistance(point) > this->getRadius() * this->getRadius())
		{
			return false;
		}

		return true;
	}

	/**
	* @return True if the sphere collides or is inside this sphere
	*/
	template<class T> bool Sphere<T>::collideWithSphere(const Sphere<T> &sphere) const
	{
		float sumRadius = this->getRadius()+sphere.getRadius();
		if(position.squareDistance(sphere.getPosition()) > (sumRadius*sumRadius))
		{
			return false;
		}

		return true;
	}

	//explicit template
	template class Sphere<float>;
	template class Sphere<double>;

}

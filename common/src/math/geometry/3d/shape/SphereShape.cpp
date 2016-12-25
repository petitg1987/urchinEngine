#include "SphereShape.h"

namespace urchin
{

	template<class T> SphereShape<T>::SphereShape(T radius) :
		radius(radius)
	{

	}

	template<class T> T SphereShape<T>::getRadius() const
	{
		return radius;
	}

	//explicit template
	template class SphereShape<float>;
	template class SphereShape<double>;

}

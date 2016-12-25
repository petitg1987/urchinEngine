#include "CapsuleShape.h"

namespace urchin
{

	template<class T> CapsuleShape<T>::CapsuleShape(T radius, T cylinderHeight, CapsuleOrientation capsuleOrientation) :
		radius(radius),
		cylinderHeight(cylinderHeight),
		capsuleOrientation(capsuleOrientation)
	{

	}

	template<class T> T CapsuleShape<T>::getRadius() const
	{
		return radius;
	}

	template<class T> T CapsuleShape<T>::getCylinderHeight() const
	{
		return cylinderHeight;
	}

	template<class T> typename CapsuleShape<T>::CapsuleOrientation CapsuleShape<T>::getCapsuleOrientation() const
	{
		return capsuleOrientation;
	}

	template<class T> T CapsuleShape<T>::computeHeight() const
	{
		return cylinderHeight + 2.0 * radius;
	}

	//explicit template
	template class CapsuleShape<float>;
	template class CapsuleShape<double>;
}

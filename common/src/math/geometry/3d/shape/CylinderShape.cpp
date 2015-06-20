#include "CylinderShape.h"

namespace urchin
{

	template<class T> CylinderShape<T>::CylinderShape(T radius, T height, CylinderOrientation cylinderOrientation) :
		Shape3D(),
		radius(radius),
		height(height),
		cylinderOrientation(cylinderOrientation)
	{

	}

	template<class T> T CylinderShape<T>::getRadius() const
	{
		return radius;
	}

	template<class T> T CylinderShape<T>::getHeight() const
	{
		return height;
	}

	template<class T> typename CylinderShape<T>::CylinderOrientation CylinderShape<T>::getCylinderOrientation() const
	{
		return cylinderOrientation;
	}

	//explicit template
	template class CylinderShape<float>;

}

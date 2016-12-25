#include "CylinderShape.h"
#include "math/geometry/3d/object/Cylinder.h"

namespace urchin
{

	template<class T> CylinderShape<T>::CylinderShape(T radius, T height, CylinderOrientation cylinderOrientation) :
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

	template<class T> std::unique_ptr<ConvexObject3D<T>> CylinderShape<T>::toConvexObject(const Transform<T> &transform) const
	{
		return std::make_unique<Cylinder<T>>(radius*transform.getScale(), height*transform.getScale(),
				cylinderOrientation, transform.getPosition(), transform.getOrientation());
	}

	//explicit template
	template class CylinderShape<float>;
	template class CylinderShape<double>;

}

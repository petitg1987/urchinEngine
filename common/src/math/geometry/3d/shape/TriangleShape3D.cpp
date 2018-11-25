#include "TriangleShape3D.h"
#include "math/geometry/3d/object/Triangle3D.h"
#include "math/algebra/point/Point4.h"

namespace urchin
{

	template<class T> TriangleShape3D<T>::TriangleShape3D(const Point3<T> *points)
	{
		this->points[0] = points[0];
		this->points[1] = points[1];
		this->points[2] = points[2];
	}

	template<class T> TriangleShape3D<T>::TriangleShape3D(const Point3<T> &point1, const Point3<T> &point2, const Point3<T> &point3)
	{
		this->points[0] = point1;
		this->points[1] = point2;
		this->points[2] = point3;
	}

	template<class T> const Point3<T> *TriangleShape3D<T>::getPoints() const
	{
		return points;
	}

	template<class T> ConvexShape3D<T> *TriangleShape3D<T>::clone() const
	{
		return new TriangleShape3D<T>(*this);
	}

	template<class T> std::unique_ptr<ConvexObject3D<T>> TriangleShape3D<T>::toConvexObject(const Transform<T> &transform) const
	{
		return std::make_unique<Triangle3D<T>>(
				(transform.getTransformMatrix() * Point4<T>(points[0])).toPoint3(),
				(transform.getTransformMatrix() * Point4<T>(points[1])).toPoint3(),
				(transform.getTransformMatrix() * Point4<T>(points[2])).toPoint3());
	}

	//explicit template
	template class TriangleShape3D<float>;
	template class TriangleShape3D<double>;

}

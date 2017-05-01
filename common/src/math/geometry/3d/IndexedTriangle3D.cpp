#include "IndexedTriangle3D.h"

namespace urchin
{

	template<class T> IndexedTriangle3D<T>::IndexedTriangle3D(const unsigned int *indexes)
	{
		this->indexes[0] = indexes[0];
		this->indexes[1] = indexes[1];
		this->indexes[2] = indexes[2];
	}

	template<class T> IndexedTriangle3D<T>::IndexedTriangle3D(unsigned int index1, unsigned int index2, unsigned int index3)
	{
		this->indexes[0] = index1;
		this->indexes[1] = index2;
		this->indexes[2] = index3;
	}

	template<class T> const unsigned int *IndexedTriangle3D<T>::getIndexes() const
	{
		return indexes;
	}

	template<class T> Vector3<T> IndexedTriangle3D<T>::computeNormal(const std::vector<Point3<T>> &trianglePoints) const
	{
		const Vector3<T> &aux = trianglePoints[indexes[1]].vector(trianglePoints[indexes[2]]);
		return aux.crossProduct(trianglePoints[indexes[1]].vector(trianglePoints[indexes[0]])).normalize();
	}

	template<class T> Vector3<T> IndexedTriangle3D<T>::computeNormal(const Point3<T> &point0, const Point3<T> &point1, const Point3<T> &point2) const
	{
		const Vector3<T> &aux = point1.vector(point2);
		return aux.crossProduct(point1.vector(point0)).normalize();
	}

	//explicit template
	template class IndexedTriangle3D<float>;
	template class IndexedTriangle3D<double>;

}

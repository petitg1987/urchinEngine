#include "ConvexHull3D.h"
#include "math/geometry/3d/util/ResizeConvexHullService.h"

namespace urchin
{

	/**
	 * @param points Points used to construct the convex hull. Points inside the convex hull are accepted but will unused.
	 */
	template<class T> ConvexHull3D<T>::ConvexHull3D(const std::vector<Point3<T>> &points) :
		localizedConvexHullShape(ConvexHullShape3D<T>(points))
	{

	}

	/**
	 * @param points Points of the convex hull (all points must belong to the convex hull and indexes should start from 0 to points size - 1)
	 * @param indexedTriangles Triangles of the convex hull (the triangles must form a convex and indexes should start from 0 to triangle size - 1)
	 */
	template<class T> ConvexHull3D<T>::ConvexHull3D(const std::map<unsigned int, Point3<T>> &points, const std::map<unsigned int, IndexedTriangle3D<T>> &indexedTriangles) :
		localizedConvexHullShape(ConvexHullShape3D<T>(points, indexedTriangles))
	{

	}

	template<class T> ConvexHull3D<T>::~ConvexHull3D()
	{

	}

	template<class T> const typename std::map<unsigned int, IndexedTriangle3D<T>> &ConvexHull3D<T>::getIndexedTriangles() const
	{
		return localizedConvexHullShape.getIndexedTriangles();
	}

	template<class T> const typename std::map<unsigned int, Point3<T>> &ConvexHull3D<T>::getIndexedPoints() const
	{
		return localizedConvexHullShape.getIndexedPoints();
	}

	template<class T> std::vector<Point3<T>> ConvexHull3D<T>::getPoints() const
	{
		return localizedConvexHullShape.getPoints();
	}

	/**
	 * @return Returns index of point added. If point doesn't make part of convex, result is zero.
	 */
	template<class T> unsigned int ConvexHull3D<T>::addNewPoint(const Point3<T> &newPoint)
	{
		return localizedConvexHullShape.addNewPoint(newPoint);
	}

	/**
	* @param newTriangleIndexes [out] Indexes of new triangles added to convex hull
	* @param removedTriangleIndexes [out] Indexes of removed triangles from convex hull
	* @return Returns index of point added. If point doesn't make part of convex, result is zero.
	*/
	template<class T> unsigned int ConvexHull3D<T>::addNewPoint(const Point3<T> &newPoint,
			std::vector<unsigned int> &newTriangleIndexes, std::vector<unsigned int> &removedTriangleIndexes)
	{
		return localizedConvexHullShape.addNewPoint(newPoint, newTriangleIndexes, removedTriangleIndexes);
	}

	template<class T> Point3<T> ConvexHull3D<T>::getSupportPoint(const Vector3<T> &direction) const
	{
		return localizedConvexHullShape.getSupportPoint(direction);
	}

	template<class T> std::unique_ptr<ConvexHull3D<T>> ConvexHull3D<T>::resize(T distance) const
	{
		return ResizeConvexHullService<T>::instance()->resizeConvexHull(*this, distance);
	}

	template<class T> std::ostream& operator <<(std::ostream &stream, const ConvexHull3D<T> &ch)
	{
		for(typename std::map<unsigned int, IndexedTriangle3D<T>>::const_iterator it = ch.getIndexedTriangles().begin(); it!=ch.getIndexedTriangles().end(); ++it)
		{
			stream << "Triangle "<< it->first <<": "
				<<"("<<ch.getIndexedPoints().find(it->second.getIndexes()[0])->second<<") "
				<<"("<<ch.getIndexedPoints().find(it->second.getIndexes()[1])->second<<") "
				<<"("<<ch.getIndexedPoints().find(it->second.getIndexes()[2])->second<<") "
				<<std::endl;
		}

		return stream;
	}

	//explicit template
	template class ConvexHull3D<float>;
	template std::ostream& operator <<<float>(std::ostream &, const ConvexHull3D<float> &);

	template class ConvexHull3D<double>;
	template std::ostream& operator <<<double>(std::ostream &, const ConvexHull3D<double> &);

}

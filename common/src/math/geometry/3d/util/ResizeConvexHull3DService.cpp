#include <limits>
#include <map>

#include "ResizeConvexHull3DService.h"
#include "math/geometry/3d/IndexedTriangle3D.h"

namespace urchin
{

	template<class T> ResizeConvexHull3DService<T>::ResizeConvexHull3DService() :
			Singleton<ResizeConvexHull3DService<T>>()
	{

	}

	template<class T> ResizeConvexHull3DService<T>::~ResizeConvexHull3DService()
	{

	}

	/**
	 * @param distance All planes of convex hull shape will be moved along their normal to the specified distance.
	 * Positive distance will extend convex hull shape and negative distance will shrink the convex hull shape.
	 * @return Convex hull shape resized. If resize is impossible to keep plane normal outside convex hull shape: nullptr is returned.
	 */
	template<class T> std::unique_ptr<ConvexHullShape3D<T>> ResizeConvexHull3DService<T>::resizeConvexHullShape(const ConvexHullShape3D<T> &originalConvexHullShape, T distance) const
	{
		const T SQUARE_EPSILON = std::numeric_limits<T>::epsilon() * std::numeric_limits<T>::epsilon();

		//1. create Plane object from convex hull
		std::vector<Plane<T>> planes;
		buildPlanesFromConvexHullShape(originalConvexHullShape, planes);

		//2. shift planes
		std::vector<Plane<T>> shiftedPlanes;
		shiftPlanes(planes, shiftedPlanes, distance);

		//3. brute force algorithm: find intersection point of three planes not parallel
		std::vector<Point3<T>> newVertices;
		newVertices.reserve(originalConvexHullShape.getIndexedPoints().size());

		for(unsigned int i=0; i<shiftedPlanes.size(); ++i)
		{
			const Vector3<T> &normal1 = shiftedPlanes[i].getNormal();

			for(unsigned int j=i+1; j<shiftedPlanes.size(); ++j)
			{
				const Vector3<T> &normal2 = shiftedPlanes[j].getNormal();
				const Vector3<T> n1CrossN2 = normal1.crossProduct(normal2);

				if(n1CrossN2.squareLength() < SQUARE_EPSILON)
				{ //planes are parallel: continue on next plane
					continue;
				}

				for(unsigned int k=j+1; k<shiftedPlanes.size(); ++k)
				{
					const Vector3<T> &normal3 = shiftedPlanes[k].getNormal();
					const Vector3<T> n2CrossN3 = normal2.crossProduct(normal3);
					const Vector3<T> n3CrossN1 = normal3.crossProduct(normal1);

					if(n2CrossN3.squareLength() < SQUARE_EPSILON || n3CrossN1.squareLength() < SQUARE_EPSILON)
					{ //planes are parallel: continue on next plane
						continue;
					}

					//three planes found not parallel between them
					T quotient = normal1.dotProduct(n2CrossN3);
					if (std::fabs(quotient) > std::numeric_limits<T>::epsilon())
					{
						quotient = -1.0 / quotient;
						Point3<T> potentialVertex(n2CrossN3 * shiftedPlanes[i].getDistanceToOrigin());
						potentialVertex += Point3<T>(n3CrossN1 * shiftedPlanes[j].getDistanceToOrigin());
						potentialVertex += Point3<T>(n1CrossN2 * shiftedPlanes[k].getDistanceToOrigin());
						potentialVertex *= quotient;

						if (isPointInsidePlanes(shiftedPlanes, potentialVertex))
						{
							newVertices.push_back(potentialVertex);
						}
					}
				}
			}
		}

		if(newVertices.size() < originalConvexHullShape.getPoints().size())
		{ //impossible to resize convex hull.
			return std::unique_ptr<ConvexHullShape3D<T>>(nullptr);
		}

		try
		{
			return std::make_unique<ConvexHullShape3D<T>>(newVertices);
		}catch(std::invalid_argument &ia)
		{ //due to resize and float precision, points doesn't form anymore a convex hull but a plane, line or point.
			return std::unique_ptr<ConvexHullShape3D<T>>(nullptr);
		}
	}

	template<class T> std::unique_ptr<ConvexHull3D<T>> ResizeConvexHull3DService<T>::resizeConvexHull(const ConvexHull3D<T> &originalConvexHull, T distance) const
	{
		std::unique_ptr<ConvexHullShape3D<T>> convexHullShapeResized = resizeConvexHullShape(originalConvexHull.localizedConvexHullShape, distance);
		if(convexHullShapeResized)
		{
			return std::make_unique<ConvexHull3D<T>>(convexHullShapeResized->getIndexedPoints(), convexHullShapeResized->getIndexedTriangles());
		}

		return std::unique_ptr<ConvexHull3D<T>>(nullptr);
	}

	template<class T> void ResizeConvexHull3DService<T>::buildPlanesFromConvexHullShape(const ConvexHullShape3D<T> &convexHull, std::vector<Plane<T>> &planes) const
	{
		const std::map<unsigned int, IndexedTriangle3D<T>> &indexedTriangles = convexHull.getIndexedTriangles();
		const std::map<unsigned int, Point3<T>> &indexedPoints = convexHull.getIndexedPoints();

		planes.reserve(indexedTriangles.size());
		for(typename std::map<unsigned int, IndexedTriangle3D<T>>::const_iterator it = indexedTriangles.begin(); it!=indexedTriangles.end(); ++it)
		{
			const IndexedTriangle3D<T> &indexedTriangle = it->second;
			const Point3<T> &point1 = indexedPoints.find(indexedTriangle.getIndexes()[0])->second;
			const Point3<T> &point2 = indexedPoints.find(indexedTriangle.getIndexes()[1])->second;
			const Point3<T> &point3 = indexedPoints.find(indexedTriangle.getIndexes()[2])->second;

			Plane<T> plane(point1, point2, point3); //build plane with normal outside convex hull
			if(!isPlaneAlreadyExist(planes, plane))
			{
				planes.push_back(plane);
			}
		}
	}

	template<class T> bool ResizeConvexHull3DService<T>::isPlaneAlreadyExist(const std::vector<Plane<T>> &planes, const Plane<T> &plane) const
	{
		const T EPSILON = 0.001;
		for(unsigned int i=0; i<planes.size(); ++i)
		{
			//1. test distances to origin
			T d1 = planes[i].getDistanceToOrigin();
			T d2 = plane.getDistanceToOrigin();
			if( (d1 - EPSILON) > d2 || (d1 + EPSILON) < d2)
			{ //distances to origin are different
				continue;
			}

			//2. test normals
			Vector3<T> normal1 = planes[i].getNormal();
			Vector3<T> normal2 = plane.getNormal();
			if( (normal1[0] - EPSILON) > normal2[0] || (normal1[0] + EPSILON) < normal2[0] //axis X
					|| (normal1[1] - EPSILON) > normal2[1] || (normal1[1] + EPSILON) < normal2[1] //axis Y
					|| (normal1[2] - EPSILON) > normal2[2] || (normal1[2] + EPSILON) < normal2[2]) //axis Z
			{ //normals are different
				continue;
			}

			return true; //same planes
		}

		return false;
	}

	/**
	 * Shift planes to given distance
	 */
	template<class T> void ResizeConvexHull3DService<T>::shiftPlanes(const std::vector<Plane<T>> &planes, std::vector<Plane<T>> &shiftedPlanes, T distance) const
	{
		shiftedPlanes.reserve(planes.size());
		for(unsigned int i=0; i<planes.size(); ++i)
		{
			T shiftedDistanceToOrigin = planes[i].getDistanceToOrigin() - distance;
			shiftedPlanes.push_back(Plane<T>(planes[i].getNormal(), shiftedDistanceToOrigin));
		}
	}

	/**
	 * @return True if point is inside all the planes. An error of '0.01' is allowed.
	 */
	template<class T> bool ResizeConvexHull3DService<T>::isPointInsidePlanes(const std::vector<Plane<T>> &planes, const Point3<T> &point) const
	{
		const T EPSILON = 0.01;
		for(unsigned int i=0; i<planes.size(); ++i)
		{
			T distToPlane = planes[i].distance(point);
			if((distToPlane - EPSILON) > 0.0)
			{
				return false;
			}
		}

		return true;
	}

	//explicit template
	template class ResizeConvexHull3DService<float>;

	template class ResizeConvexHull3DService<double>;

}

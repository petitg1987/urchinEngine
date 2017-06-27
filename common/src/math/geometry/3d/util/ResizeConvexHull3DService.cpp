#include <limits>

#include "ResizeConvexHull3DService.h"
#include "math/geometry/3d/IndexedTriangle3D.h"
#include "tools/logger/Logger.h"

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
		std::map<unsigned int, Plane<T>> planes = buildPlanesFromConvexHullShape(originalConvexHullShape);
		shiftPlanes(planes, distance);

		std::map<unsigned int, ConvexHullPoint<T>> newConvexHullPoints;
		for(const auto itPoint : originalConvexHullShape.getConvexHullPoints())
		{
			std::vector<Plane<T>> threePlanes = findThreeNonParallelPlanes(itPoint.second.triangleIndices, planes);
			if(threePlanes.size()==3)
			{
				Vector3<T> n1CrossN2 = threePlanes[0].getNormal().crossProduct(threePlanes[1].getNormal());
				Vector3<T> n2CrossN3 = threePlanes[1].getNormal().crossProduct(threePlanes[2].getNormal());
				Vector3<T> n3CrossN1 = threePlanes[2].getNormal().crossProduct(threePlanes[0].getNormal());

				Point3<T> newPoint = Point3<T>(n2CrossN3 * threePlanes[0].getDistanceToOrigin());
				newPoint += Point3<T>(n3CrossN1 * threePlanes[1].getDistanceToOrigin());
				newPoint += Point3<T>(n1CrossN2 * threePlanes[2].getDistanceToOrigin());
				newPoint *= -1.0 / threePlanes[0].getNormal().dotProduct(n2CrossN3);

				if(distance<0.0 && !isPointInsidePlanes(planes, newPoint))
				{ //too big negative distance induced wrong resize of convex hull
					return std::unique_ptr<ConvexHullShape3D<T>>(nullptr);
				}

				ConvexHullPoint<T> convexHullPoint;
				convexHullPoint.point = newPoint;
				convexHullPoint.triangleIndices = itPoint.second.triangleIndices;
				newConvexHullPoints.insert(std::pair<unsigned int, ConvexHullPoint<T>>(itPoint.first, convexHullPoint));
			}else
			{ //useless point found on convex hull (could be removed from convex hull without impact)
				std::stringstream logStream;
				logStream.precision(std::numeric_limits<T>::max_digits10);
				logStream<<"Impossible to resize convex hull because of useless point (distance: "<<std::to_string(distance)<<")."<<std::endl;
				logStream<<" - Convex hull: "<<std::endl<<originalConvexHullShape<<std::endl;
				Logger::logger().logError(logStream.str());

				return std::unique_ptr<ConvexHullShape3D<T>>(nullptr);
			}
		}

		return std::make_unique<ConvexHullShape3D<T>>(newConvexHullPoints, originalConvexHullShape.getIndexedTriangles());
	}

	template<class T> std::unique_ptr<ConvexHull3D<T>> ResizeConvexHull3DService<T>::resizeConvexHull(const ConvexHull3D<T> &originalConvexHull, T distance) const
	{
		std::unique_ptr<ConvexHullShape3D<T>> convexHullShapeResized = resizeConvexHullShape(originalConvexHull.localizedConvexHullShape, distance);
		if(convexHullShapeResized)
		{
			return std::make_unique<ConvexHull3D<T>>(*convexHullShapeResized);
		}

		return std::unique_ptr<ConvexHull3D<T>>(nullptr);
	}

	template<class T> std::map<unsigned int, Plane<T>> ResizeConvexHull3DService<T>::buildPlanesFromConvexHullShape(const ConvexHullShape3D<T> &convexHull) const
	{
		std::map<unsigned int, Plane<T>> planes;

		for(const auto &itTriangles : convexHull.getIndexedTriangles())
		{
			const IndexedTriangle3D<T> &indexedTriangle = itTriangles.second;
			const Point3<T> &point1 = convexHull.getConvexHullPoints().at(indexedTriangle.getIndex(0)).point;
			const Point3<T> &point2 = convexHull.getConvexHullPoints().at(indexedTriangle.getIndex(1)).point;
			const Point3<T> &point3 = convexHull.getConvexHullPoints().at(indexedTriangle.getIndex(2)).point;

			planes.insert(std::pair<unsigned int, Plane<T>>(itTriangles.first, Plane<T>(point1, point2, point3))); //plane is built with normal outside convex hull
		}

		return planes;
	}

	template<class T> void ResizeConvexHull3DService<T>::shiftPlanes(std::map<unsigned int, Plane<T>> &planes, T distance) const
	{
		for(auto &itPlanes : planes)
		{
			itPlanes.second.setDistanceToOrigin(itPlanes.second.getDistanceToOrigin() - distance);
		}
	}

	template<class T> std::vector<Plane<T>> ResizeConvexHull3DService<T>::findThreeNonParallelPlanes(const std::vector<unsigned int> &planeIndices, const std::map<unsigned int, Plane<T>> &allPlanes) const
	{
		constexpr float PARALLEL_COMPARISON_TOLERANCE = 0.01f;

		std::vector<Plane<T>> nonParallelPlanes;
		nonParallelPlanes.reserve(3);

		Plane<T> plane1 = allPlanes.at(planeIndices[0]);
		for(unsigned int i=1; i<planeIndices.size(); ++i)
		{
			Plane<T> plane2 = allPlanes.at(planeIndices[i]);
			if(plane1.getNormal().crossProduct(plane2.getNormal()).squareLength() < PARALLEL_COMPARISON_TOLERANCE)
			{ //planes are parallel: continue on next plane
				continue;
			}

			for(unsigned int j=i+1; j<planeIndices.size(); ++j)
			{
				Plane<T> plane3 = allPlanes.at(planeIndices[j]);

				Vector3<T> n2CrossN3 = plane2.getNormal().crossProduct(plane3.getNormal());
				if(n2CrossN3.squareLength() < 0.0
						|| plane3.getNormal().crossProduct(plane1.getNormal()).squareLength() < PARALLEL_COMPARISON_TOLERANCE
						|| plane1.getNormal().dotProduct(n2CrossN3)==0.0) //additional check due to float imprecision
				{ //planes are parallel: continue on next plane
					continue;
				}

				nonParallelPlanes = {plane1, plane2, plane3};
				return nonParallelPlanes;
			}
		}

		return nonParallelPlanes;
	}

	/**
	 * @return True if point is inside all the planes.
	 */
	template<class T> bool ResizeConvexHull3DService<T>::isPointInsidePlanes(const std::map<unsigned int, Plane<T>> &planes, const Point3<T> &point) const
	{
		constexpr T EPSILON = std::numeric_limits<T>::epsilon();
		for(const auto itPlane : planes)
		{
			if(itPlane.second.distance(point) > EPSILON)
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

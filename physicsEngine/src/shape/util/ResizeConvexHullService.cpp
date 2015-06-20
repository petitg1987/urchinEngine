#include <limits>

#include "shape/util/ResizeConvexHullService.h"

namespace urchin
{

	ResizeConvexHullService::ResizeConvexHullService() : Singleton<ResizeConvexHullService>()
	{

	}

	ResizeConvexHullService::~ResizeConvexHullService()
	{

	}

	/**
	 * @param distance All planes of convex hull will be moved along their normal to the specified distance.
	 * Positive distance will extend convex hull and negative distance will shrink the convex hull.
	 * @return Convex hull resized. If resize is impossible to keep plane normal outside convex hull: nullptr is returned.
	 */
	std::unique_ptr<ConvexHull3D<float>> ResizeConvexHullService::resizeConvexHull(const ConvexHull3D<float> &originalConvexHull, float distance) const
	{
		const float SQUARE_EPSILON = std::numeric_limits<float>::epsilon() * std::numeric_limits<float>::epsilon();

		//1. create Plane object from convex hull
		std::vector<Plane<float>> planes;
		buildPlanesFromConvexHull(originalConvexHull, planes);

		//2. shift planes
		std::vector<Plane<float>> shiftedPlanes;
		shiftPlanes(planes, shiftedPlanes, distance);

		//3. brute force algorithm: find intersection point of three planes not parallel
		std::vector<Point3<float>> newVertices;
		newVertices.reserve(originalConvexHull.getIndexedPoints().size());

		for(unsigned int i=0; i<shiftedPlanes.size(); ++i)
		{
			const Vector3<float> &normal1 = shiftedPlanes[i].getNormal();

			for(unsigned int j=i+1; j<shiftedPlanes.size(); ++j)
			{
				const Vector3<float> &normal2 = shiftedPlanes[j].getNormal();
				const Vector3<float> n1CrossN2 = normal1.crossProduct(normal2);

				if(n1CrossN2.squareLength() < SQUARE_EPSILON)
				{ //planes are parallel: continue on next plane
					continue;
				}

				for(unsigned int k=j+1; k<shiftedPlanes.size(); ++k)
				{
					const Vector3<float> &normal3 = shiftedPlanes[k].getNormal();
					const Vector3<float> n2CrossN3 = normal2.crossProduct(normal3);
					const Vector3<float> n3CrossN1 = normal3.crossProduct(normal1);

					if(n2CrossN3.squareLength() < SQUARE_EPSILON || n3CrossN1.squareLength() < SQUARE_EPSILON)
					{ //planes are parallel: continue on next plane
						continue;
					}

					//three planes found not parallel between them
					float quotient = normal1.dotProduct(n2CrossN3);
					if (std::fabs(quotient) > std::numeric_limits<float>::epsilon())
					{
						quotient = -1.0 / quotient;
						Point3<float> potentialVertex(n2CrossN3 * shiftedPlanes[i].getDistanceToOrigin());
						potentialVertex += Point3<float>(n3CrossN1 * shiftedPlanes[j].getDistanceToOrigin());
						potentialVertex += Point3<float>(n1CrossN2 * shiftedPlanes[k].getDistanceToOrigin());
						potentialVertex *= quotient;

						if (isPointInsidePlanes(shiftedPlanes, potentialVertex))
						{
							newVertices.push_back(potentialVertex);
						}
					}
				}
			}
		}

		if(newVertices.size() < originalConvexHull.getPoints().size())
		{ //impossible to resize convex hull.
			return std::unique_ptr<ConvexHull3D<float>>(nullptr);
		}

		try
		{
			return std::unique_ptr<ConvexHull3D<float>>(new ConvexHull3D<float>(newVertices));
		}catch(std::invalid_argument &ia)
		{ //due to resize and float precision, points doesn't form anymore a convex hull but a plane, line or point.
			return std::unique_ptr<ConvexHull3D<float>>(nullptr);
		}
	}

	void ResizeConvexHullService::buildPlanesFromConvexHull(const ConvexHull3D<float> &convexHull, std::vector<Plane<float>> &planes) const
	{
		const std::map<unsigned int, IndexedTriangle3D<float>> &indexedTriangles = convexHull.getIndexedTriangles();
		const std::map<unsigned int, Point3<float>> &indexedPoints = convexHull.getIndexedPoints();

		planes.reserve(indexedTriangles.size());
		for(std::map<unsigned int, IndexedTriangle3D<float>>::const_iterator it = indexedTriangles.begin(); it!=indexedTriangles.end(); ++it)
		{
			const IndexedTriangle3D<float> &indexedTriangle = it->second;
			const Point3<float> &point1 = indexedPoints.find(indexedTriangle.getIndexes()[0])->second;
			const Point3<float> &point2 = indexedPoints.find(indexedTriangle.getIndexes()[1])->second;
			const Point3<float> &point3 = indexedPoints.find(indexedTriangle.getIndexes()[2])->second;

			Plane<float> plane(point1, point2, point3); //build plane with normal outside convex hull
			if(!isPlaneAlreadyExist(planes, plane))
			{
				planes.push_back(plane);
			}
		}
	}

	bool ResizeConvexHullService::isPlaneAlreadyExist(const std::vector<Plane<float>> &planes, const Plane<float> &plane) const
	{
		const float EPSILON = 0.001f;
		for(unsigned int i=0; i<planes.size(); ++i)
		{
			//1. test distances to origin
			float d1 = planes[i].getDistanceToOrigin();
			float d2 = plane.getDistanceToOrigin();
			if( (d1 - EPSILON) > d2 || (d1 + EPSILON) < d2)
			{ //distances to origin are different
				continue;
			}

			//2. test normals
			Vector3<float> normal1 = planes[i].getNormal();
			Vector3<float> normal2 = plane.getNormal();
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
	void ResizeConvexHullService::shiftPlanes(const std::vector<Plane<float>> &planes, std::vector<Plane<float>> &shiftedPlanes, float distance) const
	{
		shiftedPlanes.reserve(planes.size());
		for(unsigned int i=0; i<planes.size(); ++i)
		{
			float shiftedDistanceToOrigin = planes[i].getDistanceToOrigin() - distance;
			shiftedPlanes.push_back(Plane<float>(planes[i].getNormal(), shiftedDistanceToOrigin));
		}
	}

	/**
	 * @return True if point is inside all the planes. An error of '0.01' is allowed.
	 */
	bool ResizeConvexHullService::isPointInsidePlanes(const std::vector<Plane<float>> &planes, const Point3<float> &point) const
	{
		const float EPSILON = 0.01f;
		for(unsigned int i=0; i<planes.size(); ++i)
		{
			float distToPlane = planes[i].distance(point);
			if((distToPlane - EPSILON) > 0.0f)
			{
				return false;
			}
		}

		return true;
	}

}

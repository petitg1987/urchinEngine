#include <stdexcept>
#include <algorithm>
#include <cmath>

#include "PolytopePlaneSurface.h"

namespace urchin
{

	/**
	 * @param ccwPointIndices Point indices of the plane which must be coplanar and counter clockwise oriented
	 */
	PolytopePlaneSurface::PolytopePlaneSurface(const std::vector<unsigned int> &ccwPointIndices, const std::vector<Point3<float>> &points)
	{
		if(ccwPointIndices.size()<3)
		{
			throw std::runtime_error("A indexed face must be composed of at least three points. Number of points: " + ccwPointIndices.size());
		}

		ccwPoints.reserve(ccwPointIndices.size());
		for(unsigned int ccwPointIndex : ccwPointIndices)
		{
			ccwPoints.push_back(points[ccwPointIndex]);
		}

		Vector3<float> v1 = ccwPoints[0].vector(ccwPoints[2]);
		Vector3<float> v2 = ccwPoints[1].vector(ccwPoints[0]);
		normal = v1.crossProduct(v2).normalize();

		Vector3<float> upVector(0.0, 1.0, 0.0);
		angleToHorizontalInRadian = std::acos(normal.dotProduct(upVector));
	}

	bool PolytopePlaneSurface::isWalkable(float maxSlopeInRadian) const
	{
		return isWalkableCandidate() && std::fabs(getAngleToHorizontal()) < maxSlopeInRadian;
	}

	Rectangle<float> PolytopePlaneSurface::computeXZRectangle() const
	{
		Point2<float> minPoint(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
		Point2<float> maxPoint(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
		for(auto point : ccwPoints)
		{
			minPoint.X = minPoint.X > point.X ? point.X : minPoint.X;
			minPoint.Y = minPoint.Y > -point.Z ? -point.Z : minPoint.Y;

			maxPoint.X = maxPoint.X < point.X ? point.X : minPoint.X;
			maxPoint.Y = maxPoint.Y < -point.Z ? -point.Z : minPoint.Y;
		}
		return Rectangle<float>(minPoint, maxPoint);
	}

    std::vector<Point2<float>> PolytopePlaneSurface::getOutlineCwPoints() const
    {
        std::vector<Point2<float>> reverseFlatPoints;
        reverseFlatPoints.reserve(ccwPoints.size());

        for(auto it = ccwPoints.rbegin(); it!=ccwPoints.rend(); ++it)
        {
            reverseFlatPoints.emplace_back(Point2<float>(it->X, -it->Z));
        }

        return reverseFlatPoints;
    }

	Plane<float> PolytopePlaneSurface::getPlane(const Rectangle<float> &box, const NavMeshAgent &agent) const
	{
		return Plane<float>(ccwPoints[0], ccwPoints[1], ccwPoints[2]);
	}

	std::vector<CSGPolygon<float>> PolytopePlaneSurface::getSelfObstacles() const
	{
		return {};
	}

	/**
 	 * Return point on un-expanded surface
 	 */
	Point3<float> PolytopePlaneSurface::computeRealPoint(const Point2<float> &point, const NavMeshAgent &agent) const
	{
		Point3<float> point3D(point.X, 0.0, -point.Y);
		float shortestFaceDistance = normal.dotProduct(point3D.vector(ccwPoints[0]));
		float t = shortestFaceDistance / normal.Y;
		Point3<float> pointOnExpandedSurface = point3D.translate(t * Vector3<float>(0.0, 1.0, 0.0));

        float reduceDistance =  - agent.computeExpandDistance(normal);
        return pointOnExpandedSurface.translate(normal * reduceDistance);
	}

	const std::vector<Point3<float>> &PolytopePlaneSurface::getCcwPoints() const
	{
		return ccwPoints;
	}

	const Vector3<float> &PolytopePlaneSurface::getNormal() const
	{
		return normal;
	}

	float PolytopePlaneSurface::getAngleToHorizontal() const
	{
		return angleToHorizontalInRadian;
	}
}

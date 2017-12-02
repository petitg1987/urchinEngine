#include <stdexcept>
#include <algorithm>
#include <cmath>

#include "PolytopePlaneSurface.h"

namespace urchin
{

	/**
	 * @param ccwPointIndices Point indices of the plane which must be coplanar and counter clockwise oriented
	 */
	PolytopePlaneSurface::PolytopePlaneSurface(const std::vector<unsigned int> &ccwPointIndices) :
		isInitialized(false),
		ccwPointIndices(ccwPointIndices),
		angleToHorizontalInRadian(std::numeric_limits<float>::max())
	{
		if(ccwPointIndices.size()<3)
		{
			throw std::runtime_error("A indexed face must be composed of at least three points. Number of points: " + ccwPointIndices.size());
		}

		ccwPoints.reserve(ccwPointIndices.size());
	}

	void PolytopePlaneSurface::refreshWith(const std::vector<PolytopePoint> &polytopePoints)
	{
		ccwPoints.clear();
		for(unsigned int ccwPointIndex : ccwPointIndices)
		{
			ccwPoints.push_back(polytopePoints[ccwPointIndex].getPoint());
		}

		Vector3<float> v1 = ccwPoints[0].vector(ccwPoints[2]);
		Vector3<float> v2 = ccwPoints[1].vector(ccwPoints[0]);
		normal = v1.crossProduct(v2).normalize();

		Vector3<float> upVector(0.0, 1.0, 0.0);
		angleToHorizontalInRadian = std::acos(normal.dotProduct(upVector));

		isInitialized = true;
	}

	bool PolytopePlaneSurface::isWalkable(float maxSlopeInRadian) const
	{
		return isWalkableCandidate() && std::fabs(getAngleToHorizontal()) < maxSlopeInRadian;
	}

    std::vector<Point2<float>> PolytopePlaneSurface::getOutlineCwPoints() const
    {
        checkInitialization();

        std::vector<Point2<float>> reverseFlatPoints;
        reverseFlatPoints.reserve(ccwPoints.size());

        for(auto it = ccwPoints.rbegin(); it!=ccwPoints.rend(); ++it)
        {
            reverseFlatPoints.emplace_back(Point2<float>(it->X, -it->Z));
        }

        return reverseFlatPoints;
    }

	void PolytopePlaneSurface::checkInitialization() const
	{
		#ifdef _DEBUG
			if(!isInitialized)
			{
				throw std::runtime_error("Impossible to access to this data because polytope face not initialized.");
			}
		#endif
	}

	const std::vector<unsigned int> &PolytopePlaneSurface::getCcwPointIndices() const
	{
		return ccwPointIndices;
	}

	const std::vector<Point3<float>> &PolytopePlaneSurface::getCcwPoints() const
	{
		checkInitialization();
		return ccwPoints;
	}

	const Vector3<float> &PolytopePlaneSurface::getNormal() const
	{
		checkInitialization();
		return normal;
	}

	float PolytopePlaneSurface::getAngleToHorizontal() const
	{
		checkInitialization();
		return angleToHorizontalInRadian;
	}
}

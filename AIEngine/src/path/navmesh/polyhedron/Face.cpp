#include <stdexcept>
#include <algorithm>
#include <cmath>

#include "Face.h"

namespace urchin
{

	/**
	 * @param points Points of the plane which must be coplanar and counter clockwise oriented
	 */
	Face::Face(const std::vector<Point3<float>> &ccwPoints) :
		ccwPoints(ccwPoints)
	{
		if(ccwPoints.size()<3)
		{
			throw std::runtime_error("A face must be composed of at least three points. Number of points: " + ccwPoints.size());
		}

		Vector3<float> v1 = ccwPoints[0].vector(ccwPoints[1]);
		Vector3<float> v2 = ccwPoints[0].vector(ccwPoints[2]);
		normal = v1.crossProduct(v2).normalize();

		Vector3<float> upVector(0.0, 1.0, 0.0);
		angleToHorizontalInRadian = std::acos(normal.dotProduct(upVector));
	}

	const std::vector<Point3<float>> &Face::getCcwPoints() const
	{
		return ccwPoints;
	}

	const std::vector<Point3<float>> Face::getCwPoints() const
	{
		std::vector<Point3<float>> cwPoints(ccwPoints);
		std::reverse(cwPoints.begin(), cwPoints.end());

		return cwPoints;
	}

	const Vector3<float> &Face::getNormal() const
	{
		return normal;
	}

	float Face::getAngleToHorizontal() const
	{
		return angleToHorizontalInRadian;
	}

}

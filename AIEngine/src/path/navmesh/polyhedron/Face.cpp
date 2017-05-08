#include <stdexcept>
#include <algorithm>
#include <cmath>

#include "Face.h"
#include "Polyhedron.h"

namespace urchin
{

	/**
	 * @param ccwPointIndices Point indices of the plane which must be coplanar and counter clockwise oriented
	 */
	Face::Face(const std::vector<unsigned int> &ccwPointIndices) :
		ccwPointIndices(ccwPointIndices),
		angleToHorizontalInRadian(0.0),
		walkableCandidate(true)
	{
		#ifdef _DEBUG
			valuesPrecomputed = false;
		#endif

		if(ccwPointIndices.size()<3)
		{
			throw std::runtime_error("A face must be composed of at least three points. Number of points: " + ccwPointIndices.size());
		}
	}

	void Face::preComputeValues(const std::vector<PolyhedronPoint> &allPoints)
	{
		ccwPoints.reserve(ccwPointIndices.size());
		for(unsigned int ccwPointIndex : ccwPointIndices)
		{
			ccwPoints.push_back(allPoints[ccwPointIndex].point);
		}

		Vector3<float> v1 = ccwPoints[0].vector(ccwPoints[2]);
		Vector3<float> v2 = ccwPoints[1].vector(ccwPoints[0]);
		normal = v1.crossProduct(v2).normalize();

		Vector3<float> upVector(0.0, 1.0, 0.0);
		angleToHorizontalInRadian = std::acos(normal.dotProduct(upVector));

		#ifdef _DEBUG
			valuesPrecomputed = true;
		#endif
	}

	const std::vector<Point3<float>> &Face::getCcwPoints() const
	{
		#ifdef _DEBUG
			if(!valuesPrecomputed)
			{
				throw std::runtime_error("Face values are not pre-computed. Call 'preComputeValues' before.");
			}
		#endif

		return ccwPoints;
	}

	const Vector3<float> &Face::getNormal() const
	{
		#ifdef _DEBUG
			if(!valuesPrecomputed)
			{
				throw std::runtime_error("Face values are not pre-computed. Call 'preComputeValues' before.");
			}
		#endif

		return normal;
	}

	float Face::getAngleToHorizontal() const
	{
		#ifdef _DEBUG
			if(!valuesPrecomputed)
			{
				throw std::runtime_error("Face values are not pre-computed. Call 'preComputeValues' before.");
			}
		#endif

		return angleToHorizontalInRadian;
	}

	void Face::setWalkableCandidate(bool walkableCandidate)
	{
		this->walkableCandidate = walkableCandidate;
	}

	bool Face::isWalkableCandidate() const
	{
		return walkableCandidate;
	}
}

#include <stdexcept>
#include <algorithm>
#include <cmath>

#include "PolyhedronFace.h"

namespace urchin
{

	/**
	 * @param ccwPointIndices Point indices of the plane which must be coplanar and counter clockwise oriented
	 */
	PolyhedronFace::PolyhedronFace(const std::vector<unsigned int> &ccwPointIndices) :
		isInitialized(false),
		ccwPointIndices(ccwPointIndices),
		angleToHorizontalInRadian(std::numeric_limits<float>::max()),
		walkableCandidate(true)
	{
		if(ccwPointIndices.size()<3)
		{
			throw std::runtime_error("A indexed face must be composed of at least three points. Number of points: " + ccwPointIndices.size());
		}

		ccwPoints.reserve(ccwPointIndices.size());
	}

	void PolyhedronFace::refreshWith(const std::vector<PolyhedronPoint> &polyhedronPoints)
	{
		ccwPoints.clear();
		for(unsigned int ccwPointIndex : ccwPointIndices)
		{
			ccwPoints.push_back(polyhedronPoints[ccwPointIndex].getPoint());
		}

		Vector3<float> v1 = ccwPoints[0].vector(ccwPoints[2]);
		Vector3<float> v2 = ccwPoints[1].vector(ccwPoints[0]);
		normal = v1.crossProduct(v2).normalize();

		Vector3<float> upVector(0.0, 1.0, 0.0);
		angleToHorizontalInRadian = std::acos(normal.dotProduct(upVector));

		isInitialized = true;
	}

	void PolyhedronFace::checkInitialization() const
	{
		#ifdef _DEBUG
			if(!isInitialized)
			{
				throw std::runtime_error("Impossible to access to this data because polyhedron face not initialized.");
			}
		#endif
	}

	const std::vector<unsigned int> &PolyhedronFace::getCcwPointIndices() const
	{
		return ccwPointIndices;
	}

	const std::vector<Point3<float>> &PolyhedronFace::getCcwPoints() const
	{
		checkInitialization();
		return ccwPoints;
	}

	const Vector3<float> &PolyhedronFace::getNormal() const
	{
		checkInitialization();
		return normal;
	}

	float PolyhedronFace::getAngleToHorizontal() const
	{
		checkInitialization();
		return angleToHorizontalInRadian;
	}

	void PolyhedronFace::setWalkableCandidate(bool walkableCandidate)
	{
		this->walkableCandidate = walkableCandidate;
	}

	bool PolyhedronFace::isWalkableCandidate() const
	{
		return walkableCandidate;
	}
}

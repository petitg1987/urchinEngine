#include <stdexcept>

#include "Face.h"

namespace urchin
{

	/**
	 * @param points Points of the plane which must be coplanar
	 */
	Face::Face(const std::vector<Point3<float>> &points) :
		points(points)
	{
		if(points.size()<3)
		{
			throw std::runtime_error("A face must be composed of at least three points. Number of points: " + points.size());
		}

		Vector3<float> v1 = points[0].vector(points[1]);
		Vector3<float> v2 = points[0].vector(points[2]);
		normal = v1.crossProduct(v2).normalize();
	}

	const std::vector<Point3<float>> &Face::getPoints() const
	{
		return points;
	}

	const Vector3<float> &Face::getNormal() const
	{
		return normal;
	}

}
